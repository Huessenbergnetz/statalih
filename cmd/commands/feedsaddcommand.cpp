/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedsaddcommand.h"
#include "feedparser.h"
#include "itemimageextractor.h"
#include "utils.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDomDocument>
#include <QDomElement>
#include <QHttpHeaders>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QUrl>

#include <QDebug>

#include <chrono>

using namespace Qt::Literals::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

FeedsAddCommand::FeedsAddCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("add");
}

void FeedsAddCommand::init()
{
    m_cliOptions.emplace_back(QStringList({u"p"_s, u"place"_s}),
                              //: CLI option description
                              //% "Database ID of the place this feed belongs to."
                              qtTrId("statalihcmd-opt-feeds-add-place-desc"),
                              // source string defined in placesaddcommand.cpp
                              qtTrId("statlihcmd-opt-value-dbid"));

    m_cliOptions.emplace_back(QStringList({u"u"_s, u"url"_s}),
                              //: CLI option description
                              //% "URL of the web feed to add."
                              qtTrId("statalihcmd-opt-feeds-add-url-desc"),
                              //: CLI option value name
                              //% "URL"
                              qtTrId("statalihcmd-opt-value-url"));

    m_cliOptions.emplace_back(QStringList({u"t"_s, u"title"_s}),
                              //: CLI option description
                              //% "Overrides the web feed title extracted from the feed data."
                              qtTrId("statalihcmd-opt-feeds-add-title-desc"),
                              //: CLI option value name
                              //% "text"
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"s"_s, u"slug"_s}),
                              //: CLI option description
                              //% "Overrides the slug generated from the title extracted form the feed data."
                              qtTrId("statalihcmd-opt-feeds-add-slug-desc"),
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"d"_s, u"description"_s}),
                              //: CLI option description
                              //% "Overrides the web feed description extracted from the feed data."
                              qtTrId("statalihcmd-opt-feeds-add-description-desc"),
                              qtTrId("statalihcmd-opt-value-text"));

    addOutputFormatOption();
}

void FeedsAddCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(RC::OK);
        return;
    }

    //: Status message
    //% "Parsing input values"
    printStatus(qtTrId("statalihcmd-status-parsing-input"));

    const QString placeStr = parser->value(u"place"_s).trimmed();
    if (placeStr.isEmpty()) {
        printFailed();
        //: Error message
        //% "Invalid place database ID."
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-place-id")));
        return;
    }

    bool ok{false};
    m_placeId = placeStr.toInt(&ok);
    if (!ok) {
        printFailed();
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-place-id")));
        return;
    }

    const QString urlStr = parser->value(u"url"_s).trimmed();
    if (urlStr.isEmpty()) {
        printFailed();
        //: Error message
        //% "Please use the -u parameter to specify the URL of the web feed."
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-missing-url")));
        return;
    }

    const QUrl url{parser->value(u"url"_s)};
    if (Q_UNLIKELY(!url.isValid())) {
        printFailed();
        //: Error message
        //% "The web feed url is not valid."
        exit(parsingError(qtTrId("statalihcmd-err-feeds-add-invalid-url")));
        return;
    }

    QLocale locale;

    const QStringList supportedSchemes({u"http"_s, u"https"_s});
    if (Q_UNLIKELY(!supportedSchemes.contains(url.scheme()))) {
        printFailed();
        //: Error message, %1 will be replaced by a list of supported schemes
        //% "Not supported URL scheme. Only the following schemes are supported: %1"
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-url-scheme").arg(locale.createSeparatedList(supportedSchemes))));
        return;
    }

    m_overrideTitle = parser->value(u"title"_s).simplified();
    m_overrideSlug = parser->value(u"slug"_s).simplified();
    m_overrideDescription = parser->value(u"description"_s).simplified();
    m_format = parser->value(u"format"_s).trimmed().toLower();

    printDone();

    CLI::RC rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    //% "Checking database"
    printStatus(qtTrId("statalihcmd-status-feeds-add-checking-db"));

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    if (Q_UNLIKELY(!q.prepare(u"SELECT * FROM places WHERE id = :id"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":id"_s, m_placeId);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (!q.next()) {
        printFailed();
        //: Error message
        //% "Can not find a place with ID %1 in the database."
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-unknown-place").arg(m_placeId)));
        return;
    }

    printDone();

    //: Status message
    //% "Fetching feed"
    printStatus(qtTrId("statlihcmd-status-feed-add-fetch"));

    auto nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &FeedsAddCommand::feedFetched);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    nam->setTransferTimeout(std::chrono::seconds{10});
#else
    nam->setTransferTimeout(10'000);
#endif
    QNetworkRequest request;
    const QString userAgent = QCoreApplication::applicationName() + '/'_L1 + QCoreApplication::applicationVersion();
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    request.setUrl(url);
    nam->get(request);
}

void FeedsAddCommand::feedFetched(QNetworkReply *reply)
{
    reply->manager()->deleteLater();
    reply->deleteLater();
    if (reply->error() == QNetworkReply::NoError) {
        printDone();

        if (reply->hasRawHeader("etag")) {
            const auto headers = reply->headers();
            m_etag = QString::fromLatin1(headers.value("etag"));
        }

        //: Satus message
        //% "Parsing XML"
        printStatus(qtTrId("statlihcmd-status-feed-add-parse-xml"));

        QDomDocument doc;
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        auto parseResult = doc.setContent(reply, QDomDocument::ParseOption::UseNamespaceProcessing);
        if (Q_UNLIKELY(!parseResult)) {
            //: Error message, %1 and %2 will be replaced by line and column,
            //: %3 by the error message of the parser
            //% "Failed to parse feed XML data at line %1 and column %2: %3"
            exit(parsingError(qtTrId("statlihcmd-err-feeds-add-parsing").arg(QString::number(parseResult.errorLine), QString::number(parseResult.errorColumn), parseResult.errorMessage)));
            return;
        }
#else
        QString errorMsg;
        int errorLine{-1};
        int errorColumn{-1};
        if (Q_UNLIKELY(!doc.setContent(reply, true, &errorMsg, &errorLine, &errorColumn))) {
            printFailed();
            exit(parsingError(qtTrId("statlihcmd-err-feeds-add-parsing").arg(QString::number(errorLine), QString::number(errorColumn), errorMsg)));
            return;
        }
#endif

        auto parser = new FeedParser(this);
        connect(parser, &FeedParser::feedParsed, this, &FeedsAddCommand::feedParsed);
        connect(parser, &FeedParser::feedParsed, parser, &QObject::deleteLater);
        parser->parse(doc);
    } else {
        printFailed();
        exit(networkError(reply));
    }
}

void FeedsAddCommand::feedParsed(const Feed &feed)
{
    printDone();

    m_feed = feed;

    //% "Adding new web feed"
    printStatus(qtTrId("statalihcmd-status-feeds-add-db-add"));

    QSqlQuery q(QSqlDatabase::database(HBNST_DBCONNAME));

    if (Q_UNLIKELY(!q.prepare(u"SELECT id FROM feeds WHERE source = :source"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":source"_s, m_feed.source());

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (Q_UNLIKELY(q.next())) {
        printFailed();
        //% "This web feed has already been added to the database with ID %1."
        exit(inputError(qtTrId("statalihcmd-err-feeds-add-already-added").arg(q.value(0).toString())));
        return;
    }

    if (Q_UNLIKELY(!q.prepare(uR"-(INSERT INTO feeds ("placeId", slug, title, description, source, link, etag, "lastBuildDate", "lastFetch", created, data)
                              VALUES (:placeId, :slug, :title, :description, :source, :link, :etag, :lastBuildDate, :lastFetch, :created, :data)
                              RETURNING id)-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    m_title = m_overrideTitle.isEmpty() ? m_feed.title() : m_overrideTitle;
    m_slug = m_overrideSlug.isEmpty() ? Utils::slugify(m_title) : Utils::slugify(m_overrideSlug);
    m_description = m_overrideDescription.isEmpty() ? Utils::cleanDescription(m_feed.description()) : m_overrideDescription;

    q.bindValue(u":placeId"_s, m_placeId);
    q.bindValue(u":slug"_s, m_slug);
    q.bindValue(u":title"_s, m_title);
    q.bindValue(u":description"_s, m_description);
    q.bindValue(u":source"_s, m_feed.source());
    q.bindValue(u":link"_s, m_feed.link());
    q.bindValue(u":etag"_s, m_etag);
    q.bindValue(u":lastBuildDate"_s, m_feed.lastBuildDate());
    q.bindValue(u":lastFetch"_s, QDateTime::currentDateTimeUtc());
    q.bindValue(u":created"_s, QDateTime::currentDateTimeUtc());
    q.bindValue(u":data"_s, QJsonObject());

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (Q_UNLIKELY(!q.next())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    m_feedId = q.value(0).toInt();

    if (Q_UNLIKELY(!q.prepare(uR"-(INSERT INTO items ("feedId", guid, title, description, author, link, "pubDate")
                              VALUES (:feedId, :guid, :title, :description, :author, :link, :pubDate))-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    const QList<FeedItem> items = m_feed.items();
    for (const auto &item : items) {
        q.bindValue(u":feedId"_s, m_feedId);
        q.bindValue(u":guid"_s, item.guid());
        q.bindValue(u":title"_s, item.title());
        q.bindValue(u":description"_s, Utils::cleanDescription(item.description()));
        q.bindValue(u":author"_s, item.author());
        q.bindValue(u":link"_s, item.link());
        q.bindValue(u":pubDate"_s, item.pubDate());

        q.exec();
    }

    printDone();

    //% "Fetching item images"
    printStatus(qtTrId("statalihcmd-status-feeds-add-fetch-imgs"));

    auto iie = new ItemImageExtractor(this);
    connect(iie, &ItemImageExtractor::finished, this, &FeedsAddCommand::imagesFetched);
    iie->start(m_feed.items());

}

void FeedsAddCommand::imagesFetched(const QVariantMap &itemImages, const QMap<QString,QString> &errors)
{

    printDone();

    if (!errors.empty()) {
        //% "While fetching item images the following errors occured:"
        printMessage(qtTrId("statalih-msg-feeds-add-fetch-imgs-errors"));
        for (auto i = errors.constBegin(), end = errors.constEnd(); i != end; ++i) {
            const QString msg = i.key() + u": "_s + i.value();
            printWarning(msg);
        }
    }

    //: Status messages
    //% "Adding item image information to database"
    printStatus(qtTrId("statalih-status-feeds-add-images-add-db"));

    if (!itemImages.empty()) {

        QSqlQuery q(QSqlDatabase::database(HBNST_DBCONNAME));

        bool ok = q.prepare(uR"-(UPDATE items SET data = :data WHERE guid = :guid)-"_s);

        if (ok) {
            QStringList queryErrors;
            for (auto i = itemImages.constBegin(), end = itemImages.constEnd(); i != end; ++i) {
                const QString guid = i.key();
                const QJsonObject imgData({
                                              {u"image"_s, QJsonObject::fromVariantMap(i.value().toMap())}
                                          });
                q.bindValue(u":guid"_s, guid);
                q.bindValue(u":data"_s, QString::fromUtf8(QJsonDocument(imgData).toJson(QJsonDocument::Compact)));

                if (!q.exec()) {
                    qDebug() << q.lastError().text();
                    queryErrors << q.lastError().text();
                }
            }

            if (queryErrors.empty()) {
                printDone();
            } else {
                printFailed();
                printError(queryErrors);
            }
        } else {
            printFailed();
            printError(q.lastError().text());
        }
    }

    if (m_format == "table"_L1) {
        const QStringList headers{
            //: CLI table header
            //% "Key"
            qtTrId("statalihcmd-table-header-key"),
            //: CLI table header
            //% "Value"
            qtTrId("statalihcmd-table-header-value")
        };

        QList<QStringList> data;
        data << QStringList({u"ID"_s, QString::number(m_feedId)});
        data << QStringList({u"Title"_s, m_title});
        data << QStringList({u"Slug"_s, m_slug});
        data << QStringList({u"Source"_s, m_feed.source().toString()});
        data << QStringList({u"Link"_s, m_feed.link().toString()});
        data << QStringList({u"Description"_s, m_description});
        data << QStringList({u"Items"_s, QString::number(m_feed.items().size())});

        printTable(headers, data);
    } else if (m_format == "json"_L1 || m_format == "json-pretty"_L1) {

        QJsonObject o{
            {u"id"_s, m_feedId},
            {u"title"_s, m_title},
            {u"slug"_s, m_slug},
            {u"source"_s, m_feed.source().toString()},
            {u"link"_s, m_feed.link().toString()},
            {u"description"_s, m_description},
            {u"items"_s, m_feed.items().size()}
        };

        const QJsonDocument json(o);
        QTextStream out(stdout, QIODevice::WriteOnly);
        out << json.toJson(m_format == "json"_L1 ? QJsonDocument::Compact : QJsonDocument::Indented);

    }

    exit(RC::OK);
}

QString FeedsAddCommand::summary() const
{
    //: CLI command summary
    //% "Add a new web feed"
    return qtTrId("statalihcmd-command-feeds-add-summary");
}

QString FeedsAddCommand::description() const
{
    //: CLI command description
    //% "Adds a new web feed to the database. Will download the feed, parse it and store the information and items to the system."
    return qtTrId("statalihcmd-command-feeds-add-description");
}

#include "moc_feedsaddcommand.cpp"
