/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedsupdatecommand.h"
#include "feedparser.h"
#include "itemimageextractor.h"
#include "utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace Qt::Literals::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

FeedsUpdateCommand::FeedsUpdateCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("update");
}

void FeedsUpdateCommand::init()
{
    m_cliOptions.emplace_back(QStringList({u"a"_s, u"all"_s}),
                              //: CLI option description
                              //% "Update all feeds"
                              qtTrId("statalihcmd-opt-feeds-update-all-desc"));

    m_cliOptions.emplace_back(QStringList({u"p"_s, u"place"_s}),
                              //: CLI option description
                              //% "Only update feeds that belong to place identified by ID."
                              qtTrId("statalihcmd-opt-feeds-udpate-place-desc"),
                              // source string defined in placesaddcommand.cpp
                              qtTrId("statlihcmd-opt-value-dbid"));

    m_cliOptions.emplace_back(QStringList({u"s"_s, u"slug"_s}),
                              //: CLI option description
                              //% "Only update feeds that belong to place identified by slug."
                              qtTrId("statalihcmd-opt-feeds-update-slug-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"id"_s}),
                              //: CLI option description
                              //% "Only update feed with ID."
                              qtTrId("statalihcmd-opt-feeds-update-id-desc"),
                              // source string defined in placesaddcommand.cpp
                              qtTrId("statlihcmd-opt-value-dbid"));
}

void FeedsUpdateCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(RC::OK);
        return;
    }

    QLocale locale;

    // source string defined in feedsaddcommand.cpp
    printStatus(qtTrId("statalihcmd-status-parsing-input"));

    if (!parser->isSet(u"a"_s) && !parser->isSet(u"p"_s) && !parser->isSet(u"s"_s) && !parser->isSet(u"id"_s)) {
        printFailed();
        const QStringList opts{u"--all"_s, u"--place"_s, u"--slug"_s, u"--id"_s};
        //% "Use one of %1 to select the feeds you want to update."
        exit(inputError(qtTrId("statalihcmd-err-feeds-update-invalid-feed-selection")));
        return;
    }

    if (!parser->isSet(u"all"_s)) {
        if (parser->isSet(u"id"_s)) {
            bool ok = false;
            const auto id = parser->value(u"id"_s).toInt(&ok);
            if (!ok) {
                printFailed();
                //% "Invalid feed ID."
                exit(inputError(qtTrId("statalihcmd-err-feeds-update-invalid-feed-id")));
                return;
            }
        } else if (parser->isSet(u"p"_s)) {
            bool ok = false;
            const auto id = parser->value(u"place"_s).toInt(&ok);
            if (!ok) {
                printFailed();
                //% "Invalid place ID."
                exit(inputError(qtTrId("statalihcmd-err-feeds-update-invalid-place-id")));
                return;
            }
        }
    }

    printDone();

    CLI::RC rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    //% "Query feeds to update from database"
    printStatus(qtTrId("statalihcmd-status-feeds-update-query-feeds-db"));

    QString qs = uR"-(SELECT f.id, f.title, f.source, f.etag, f."lastBuildDate", f."lastFetch" FROM feeds f)-"_s;

    if (!parser->isSet(u"all"_s) && !parser->isSet(u"id"_s)) {
        qs += uR"-( JOIN places p ON p.id = f."placeId")-"_s;
    }

    qs += uR"-( WHERE f.enabled = true)-"_s;

    if (!parser->isSet(u"all"_s)) {
        if (parser->isSet(u"id"_s)) {
            qs += u" AND f.id = :id";
        } else if (parser->isSet(u"p"_s)) {
            qs += u" AND p.id = :id";
        } else if (parser->isSet(u"s"_s)) {
            qs += u" AND p.slug = :slug";
        }
    }

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    if (Q_UNLIKELY(!q.prepare(qs))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (!parser->isSet(u"all"_s)) {
        if (parser->isSet(u"id"_s)) {
            q.bindValue(u":id"_s, parser->value(u"id"_s).toInt());
        } else if (parser->isSet(u"p"_s)) {
            q.bindValue(u":id"_s, parser->value(u"place"_s).toInt());
        } else if (parser->isSet(u"s"_s)) {
            q.bindValue(u":slug"_s, parser->value(u"slug"_s).toInt());
        }
    }

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    printDone();

    while (q.next()) {
        m_feedsToUpdate.enqueue({
                                    q.value(0).toInt(),
                                    q.value(1).toString(),
                                    QUrl(q.value(2).toString()),
                                    q.value(3).toString(),
                                    q.value(4).toDateTime(),
                                    q.value(5).toDateTime()
                                });
    }

    if (m_feedsToUpdate.isEmpty()) {
        //% "No feeds found for update."
        printWarning(qtTrId("statalihcmd-warn-feeds-update-no-feeds"));
        exit(RC::OK);
        return;
    }

    m_nam = new QNetworkAccessManager(this);
    m_nam->setTransferTimeout(10'000);
    connect(m_nam, &QNetworkAccessManager::finished, this, &FeedsUpdateCommand::feedFetched);

    QMetaObject::invokeMethod(this, "updateFeed");
}

void FeedsUpdateCommand::updateFeed()
{
    if (m_feedsToUpdate.empty()) {
        exit(RC::OK);
        return;
    }

    m_current = m_feedsToUpdate.dequeue();

    QLocale locale;
    //% "Fetching feed %1 (ID: %2)"
    printStatus(qtTrId("statalihcmd-status-feeds-update-fetching-feed").arg(locale.quoteString(m_current.title), QString::number(m_current.id)));

    QNetworkRequest req{m_current.source};
    if (!m_current.etag.isEmpty()) {
        req.setHeader(QNetworkRequest::IfNoneMatchHeader, m_current.etag);
    }
    m_nam->get(req);
}

void FeedsUpdateCommand::feedFetched(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        printFailed();
        //% "Failed to fetch feed from %1: %2"
        printWarning(qtTrId("statalihcmd-warn-feeds-update-fetch-failed").arg(m_current.source.toString(), reply->errorString()));
        updateFeed();
    } else {
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 304) {
            printDone();
            //% "Feed has not been modified since last update."
            printMessage(qtTrId("statlihcmd-info-feeds-update-not-modified"));
            updateFeed();
        } else {
            m_current.etag = reply->header(QNetworkRequest::ETagHeader).toString();

            QDomDocument doc;

#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
            auto parseResult = doc.setContent(reply, QDomDocument::ParseOption::UseNamespaceProcessing);
            if (Q_UNLIKELY(!parseResult)) {
                printFailed();
                // source string defined in feedsaddcommand.cpp
                printWarning(qtTrId("statlihcmd-err-feeds-add-parsing").arg(QString::number(parseResult.errorLine), QString::number(parseResult.errorColumn), parseResult.errorMessage));
                QMetaObject::invokeMethod(this, "updateFeed");
                return;
            }
#else
            QString errorMsg;
            int errorLine{-1};
            int errorColumn{-1};
            if (Q_UNLIKELY(!doc.setContent(reply, true, &errorMsg, &errorLine, &errorColumn))) {
                printFailed();
                // source string defined in feedsaddcommand.cpp
                printWarning(qtTrId("statlihcmd-err-feeds-add-parsing").arg(QString::number(errorLine), QString::number(errorColumn), errorMsg));
                QMetaObject::invokeMethod(this, "updateFeed");
                return;
            }
#endif

            auto parser = new FeedParser(this);
            connect(parser, &FeedParser::feedParsed, this, &FeedsUpdateCommand::feedParsed);
            connect(parser, &FeedParser::feedParsed, parser, &QObject::deleteLater);
            parser->parse(doc);
        }
    }
}

void FeedsUpdateCommand::feedParsed(const Feed &feed)
{
    m_feed = feed;
    if (!feed.isValid()) {
        printFailed();
        //% "Failed to parse feed."
        printWarning(qtTrId("statalihcmd-warn-feeds-update-parsing-failed"));
        QMetaObject::invokeMethod(this, "updateFeed");
        return;
    }

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    if (Q_UNLIKELY(!q.prepare(uR"-(UPDATE feeds SET etag = :etag, "lastBuildDate" = :lastBuildDate, "lastFetch" = :lastFetch WHERE id = :id)-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":etag"_s, m_current.etag);
    q.bindValue(u":lastBuildDate"_s, m_feed.lastBuildDate());
    q.bindValue(u":lastFetch"_s, QDateTime::currentDateTimeUtc());
    q.bindValue(u":id"_s, m_current.id);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    QList<FeedItem> newItems;
    QList<FeedItem> updatedItems;

    const QList<FeedItem> items = m_feed.items();
    for (const auto &item : items) {

        q.prepare(uR"-(SELECT pubDate FROM items WHERE guid = :guid)-"_s);
        q.bindValue(u":guid"_s, item.guid());
        q.exec();
        if (q.next()) {
            // item already exists, check if update is needed
            if (item.pubDate() > q.value(0).toDateTime()) {
                updatedItems << item;
                q.prepare(uR"-(UPDATE items SET title = :title, description = :description, author = :author, link = :link, "pubDate" = :pubDate
                               WHERE guid = :guid)-"_s);
                q.bindValue(u":title"_s, item.title());
                q.bindValue(u":description"_s, Utils::cleanDescription(item.description()));
                q.bindValue(u":author"_s, item.author());
                q.bindValue(u":link"_s, item.link());
                q.bindValue(u":pubDate"_s, item.pubDate());
                q.exec();
            }
        } else {
            // new item
            newItems << item;
            q.prepare(uR"-(INSERT INTO items ("feedId", guid, title, description, author, link, "pubDate")
                           VALUES (:feedId, :guid, :title, :description, :author, :link, :pubDate))-"_s);
            q.bindValue(u":feedId"_s, m_current.id);
            q.bindValue(u":guid"_s, item.guid());
            q.bindValue(u":title"_s, item.title());
            q.bindValue(u":description"_s, Utils::cleanDescription(item.description()));
            q.bindValue(u":author"_s, item.author());
            q.bindValue(u":link"_s, item.link());
            q.bindValue(u":pubDate"_s, item.pubDate());
            q.exec();
        }
    }

    if (newItems.empty() && updatedItems.empty()) {
        printDone();
        QMetaObject::invokeMethod(this, "updateFeed");
    } else {
        QList<FeedItem> _items = newItems;
        _items.append(updatedItems);

        auto iie = new ItemImageExtractor(this);
        connect(iie, &ItemImageExtractor::finished, this, &FeedsUpdateCommand::imagesFetched);
        iie->start(_items);
    }
}

void FeedsUpdateCommand::imagesFetched(const QVariantMap &itemImages, const QMap<QString,QString> &errors)
{
    if (!itemImages.empty()) {

        QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

        for (auto i = itemImages.constBegin(), end = itemImages.constEnd(); i != end; ++i) {
            const QString guid = i.key();
            q.prepare(uR"-(SELECT data FROM items WHERE guid = :guid)-"_s);
            q.bindValue(u":guid"_s, guid);
            if (q.exec() && q.next()) {
                auto data = q.value(0).toJsonObject();
                data.insert("image"_L1, QJsonObject::fromVariantMap(i.value().toMap()));

                q.prepare(uR"-(UPDATE items SET data = :data WHERE guid = :guid)-"_s);
                q.bindValue(u":data"_s, QString::fromUtf8(QJsonDocument(data).toJson(QJsonDocument::Compact)));
                q.exec();
            }
        }
    }

    printDone();
    QMetaObject::invokeMethod(this, "updateFeed");
}

QString FeedsUpdateCommand::summary() const
{
    //: CLI command summary
    //% "Fetch feed updates"
    return qtTrId("statalihcmd-command-feeds-update-summary");
}

QString FeedsUpdateCommand::description() const
{
    //: CLI command description
    //% "Fetches feed updates and adds new items to the database."
    return qtTrId("statalihcmd-command-feeds-update-description");
}

#include "moc_feedsupdatecommand.cpp"
