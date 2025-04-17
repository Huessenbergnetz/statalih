/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedsaddcommand.h"
#include "feedparser.h"
#include "utils.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDomDocument>
#include <QDomElement>
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
    m_cliOptions.emplace_back(QStringList({u"u"_s, u"url"_s}),
                              //: CLI option description
                              //% "URL of the web feed to add."
                              qtTrId("statalihcmd-opt-feeds-add-url-desc"),
                              //: CLI option value name
                              //% "URL"
                              qtTrId("statalihcmd-opt-feeds-add-value"));

    QLocale locale;
    m_cliOptions.emplace_back(QStringList({u"c"_s, u"coordinates"_s}),
                              //: CLI option description, %1 will be replaced by the
                              //: example latitude, %2 by the example longitude
                              //% "Coordinates of the location covered in the feed. "
                              //% "Enter them like \"%1;%2\"."
                              qtTrId("statalihcmd-opt-feeds-add-coords-desc").arg(locale.toString(51.571667), locale.toString(9.166667)),
                              //: CLI option value name
                              //% "latitude;longitude"
                              qtTrId("statalihcmd-opt-feeds-add-coords-value"));

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

    const QStringList formats({u"table"_s, u"json"_s, u"json-pretty"_s});
    m_cliOptions.emplace_back(QStringList({u"f"_s, u"format"_s}),
                              //: CLI option description
                              //% "Render output in a particular format. Available: %1. Default: %2."
                              qtTrId("statalihcmd-opt-format-desc").arg(locale.createSeparatedList(formats), formats.first()),
                              //: CLI option value name
                              //% "format"
                              qtTrId("statalihcmd-opt-format-value"),
                              formats.first());
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

    if (parser->isSet(u"coordinates"_s)) {

        const QStringList coordStr = parser->value(u"coordinates"_s).split(';'_L1);

        if (Q_UNLIKELY(coordStr.size() != 2)) {
            printFailed();
            //: Error message
            //% "Invalid coordinates"
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        bool ok = false;
        m_latitude = locale.toFloat(coordStr.at(0), &ok);
        if (Q_UNLIKELY(!ok)) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        if (m_latitude < -90.f || m_latitude > 90.f) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        m_longitude = locale.toFloat(coordStr.at(1), &ok);
        if (Q_UNLIKELY(!ok)) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        if (m_longitude < -180.f || m_longitude > 180.f) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        m_coordsSet = true;
    }

    m_title = parser->value(u"title"_s).simplified();
    m_slug = parser->value(u"slug"_s).simplified();
    m_description = parser->value(u"description"_s).simplified();
    m_format = parser->value(u"format"_s).trimmed().toLower();

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

    CLI::RC rc{RC::OK};

    rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    //% "Adding new web feed"
    printStatus(qtTrId("statalihcmd-status-feeds-add-db-add"));

    QSqlQuery q(QSqlDatabase::database(HBNST_DBCONNAME));

    if (Q_UNLIKELY(!q.prepare(u"SELECT id FROM feeds WHERE source = :source"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":source"_s, feed.source());

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

    if (Q_UNLIKELY(!q.prepare(uR"-(INSERT INTO feeds (slug, title, description, source, link, "lastBuildDate", "lastFetch", coords, data)
                              VALUES (:slug, :title, :description, :source, :link, :lastBuildDate, :lastFetch, :coords, :data)
                              RETURNING id)-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    QString title = m_title.isEmpty() ? feed.title() : m_title;
    QString slug = m_slug.isEmpty() ? Utils::slugify(title) : Utils::slugify(m_slug);
    QString desc = m_description.isEmpty() ? feed.description() : m_description;

    q.bindValue(u":slug"_s, slug);
    q.bindValue(u":title"_s, title);
    q.bindValue(u":description"_s, desc);
    q.bindValue(u":source"_s, feed.source());
    q.bindValue(u":link"_s, feed.link());
    q.bindValue(u":lastBuildDate"_s, feed.lastBuildDate());
    q.bindValue(u":lastFetch"_s, QDateTime::currentDateTimeUtc());
    if (m_coordsSet) {
        const QString coords = u"(%1,%2)"_s.arg(QString::number(m_latitude), QString::number(m_longitude));
        q.bindValue(u":coords"_s, coords);
    } else {
        q.bindValue(u":coords"_s, {});
    }
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

    const auto feedDbId = q.value(0).toInt();

    if (Q_UNLIKELY(!q.prepare(uR"-(INSERT INTO items ("feedId", guid, title, description, author, link, "pubDate")
                              VALUES (:feedId, :guid, :title, :description, :author, :link, :pubDate))-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    const QList<FeedItem> items = feed.items();
    for (const auto &item : items) {
        q.bindValue(u":feedId"_s, feedDbId);
        q.bindValue(u":guid"_s, item.guid());
        q.bindValue(u":title"_s, item.title());
        q.bindValue(u":description"_s, item.description());
        q.bindValue(u":author"_s, item.author());
        q.bindValue(u":link"_s, item.link());
        q.bindValue(u":pubDate"_s, item.pubDate());

        q.exec();
    }

    printDone();

    if (m_format == "table"_L1) {
        const QStringList headers{
            //: CLI table header
            //% "Key"
            qtTrId("statalihcmd-command-feeds-add-finish-table-header-key"),
            //: CLI table header
            //% "Value"
            qtTrId("statalihcmd-command-feeds-add-finish-table-header-value")
        };

        QList<QStringList> data;
        data << QStringList({u"ID"_s, QString::number(feedDbId)});
        data << QStringList({u"Title"_s, title});
        data << QStringList({u"Slug"_s, slug});
        data << QStringList({u"Source"_s, feed.source().toString()});
        data << QStringList({u"Link"_s, feed.link().toString()});
        data << QStringList({u"Description"_s, desc});
        data << QStringList({u"Items"_s, QString::number(items.size())});
        QString coords;
        if (m_coordsSet) {
            coords = u"N %1 E %2"_s.arg(QString::number(m_latitude), QString::number(m_longitude));
        }
        data << QStringList({u"Coordinates"_s, coords});

        printTable(headers, data);
    } else if (m_format == "json"_L1 || m_format == "json-pretty"_L1) {

        QJsonObject o{
            {u"id"_s, feedDbId},
            {u"title"_s, title},
            {u"slug"_s, slug},
            {u"source"_s, feed.source().toString()},
            {u"link"_s, feed.link().toString()},
            {u"description"_s, desc},
            {u"items"_s, items.size()}
        };

        if (m_coordsSet) {
            o.insert(u"coordinates"_s, QJsonObject({
                                                       {u"latitude"_s, m_latitude},
                                                       {u"longitude"_s, m_longitude}
                                                   }));
        } else {
            o.insert(u"coordinates"_s, {});
        }

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
    //% "Adds a new web feed to the database. Will download the feed, parse it and the information and items to the system."
    return qtTrId("statalihcmd-command-feeds-add-description");
}

#include "moc_feedsaddcommand.cpp"
