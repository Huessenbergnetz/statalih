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
#include <QLoggingCategory>
#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace Qt::Literals::StringLiterals;

#if defined(QT_DEBUG)
Q_LOGGING_CATEGORY(ST_UPDATER, "statalih.updater");
#else
Q_LOGGING_CATEGORY(ST_UPDATER, "statalih.updater", QtInfoMsg);
#endif

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
        qCCritical(ST_UPDATER) << "No feed selected for update.";
        const QStringList opts{u"--all"_s, u"--place"_s, u"--slug"_s, u"--id"_s};
        //% "Use one of %1 to select the feeds you want to update."
        exit(inputError(qtTrId("statalihcmd-err-feeds-update-invalid-feed-selection").arg(locale.createSeparatedList(opts))));
        return;
    }

    if (!parser->isSet(u"all"_s)) {
        if (parser->isSet(u"id"_s)) {
            bool ok = false;
            const auto id = parser->value(u"id"_s).toInt(&ok);
            if (!ok) {
                printFailed();
                qCCritical(ST_UPDATER) << "Invalid feed ID.";
                //% "Invalid feed ID."
                exit(inputError(qtTrId("statalihcmd-err-feeds-update-invalid-feed-id")));
                return;
            }
        } else if (parser->isSet(u"p"_s)) {
            bool ok = false;
            const auto id = parser->value(u"place"_s).toInt(&ok);
            if (!ok) {
                printFailed();
                qCCritical(ST_UPDATER) << "Invalid place ID.";
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

    qCDebug(ST_UPDATER) << "Query feeds to update from database.";
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
        qCCritical(ST_UPDATER) << "Failed to prepare query to get feeds to update from database:" << q.lastError().text();
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
        qCCritical(ST_UPDATER) << "Failed to execute query to get feeds to update from database:" << q.lastError().text();
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
        qCInfo(ST_UPDATER) << "No feeds found for update.";
        //% "No feeds found for update."
        printMessage(qtTrId("statalihcmd-warn-feeds-update-no-feeds"));
        exit(RC::OK);
        return;
    }

    qCInfo(ST_UPDATER) << m_feedsToUpdate.size() << "feeds found for update.";

    m_nam = new QNetworkAccessManager(this);
    m_nam->setTransferTimeout(10'000);
    connect(m_nam, &QNetworkAccessManager::finished, this, &FeedsUpdateCommand::feedFetched);

    QMetaObject::invokeMethod(this, &FeedsUpdateCommand::updateFeed);
}

void FeedsUpdateCommand::updateFeed()
{
    if (m_feedsToUpdate.empty()) {
        exit(RC::OK);
        return;
    }

    m_current = m_feedsToUpdate.dequeue();
    qCInfo(ST_UPDATER).noquote() << "Start updating feed" << m_current.logInfo();

    QLocale locale;
    //% "Fetching feed %1 (ID: %2)"
    printStatus(qtTrId("statalihcmd-status-feeds-update-fetching-feed").arg(locale.quoteString(m_current.title), QString::number(m_current.id)));

    QNetworkRequest req{m_current.source};
    if (!m_current.etag.isEmpty()) {
        req.setHeader(QNetworkRequest::IfNoneMatchHeader, m_current.etag);
    }
    qCInfo(ST_UPDATER).noquote() << "Fetching feed" << m_current.logInfo() << "from" << m_current.source.toString();
    m_nam->get(req);
}

void FeedsUpdateCommand::feedFetched(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        printFailed();
        qCWarning(ST_UPDATER).noquote().nospace() << "Failed to fetch feed " << m_current.logInfo() << " from "
                                                  << m_current.source.toString() << ": " << reply->errorString();
        //% "Failed to fetch feed from %1: %2"
        printWarning(qtTrId("statalihcmd-warn-feeds-update-fetch-failed").arg(m_current.source.toString(), reply->errorString()));
        updateFeed();
    } else {
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 304) {
            printDone();
            qCInfo(ST_UPDATER).noquote() << "Feed" << m_current.logInfo() << "has not been modified since last update.";
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
                qCWarning(ST_UPDATER).noquote().nospace() << "Failed to parse XML of feed " << m_current.logInfo()
                                                          << " at line " << parseResult.errorLine << " and column "
                                                          << parseResult.errorColumn << ": " << parseResult.errorMessage;
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
                qCWarning(ST_UPDATER).noquote().nospace() << "Failed to parse XML of feed " << m_current.logInfo()
                                                          << " at line " << errorLine << " and column "
                                                          << errorColumn << ": " << errorMsg;
                // source string defined in feedsaddcommand.cpp
                printWarning(qtTrId("statlihcmd-err-feeds-add-parsing").arg(QString::number(errorLine), QString::number(errorColumn), errorMsg));
                QMetaObject::invokeMethod(this, "updateFeed");
                return;
            }
#endif

            qCInfo(ST_UPDATER).noquote() << "Successfully fetched feed" << m_current.logInfo()
                                         << "from" << m_current.source.toString();
            qCInfo(ST_UPDATER).noquote() << "Start parsing feed" << m_current.logInfo();

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
        qCWarning(ST_UPDATER).noquote() << "Failed to parse feed" << m_current.logInfo();
        //% "Failed to parse feed."
        printWarning(qtTrId("statalihcmd-warn-feeds-update-parsing-failed"));
        QMetaObject::invokeMethod(this, "updateFeed");
        return;
    }

    qCInfo(ST_UPDATER).noquote() << "Successfully parsed feed" << m_current.logInfo();

    qCInfo(ST_UPDATER).noquote() << "Start updating feed" << m_current.logInfo() << "in the database.";

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    if (Q_UNLIKELY(!q.prepare(uR"-(UPDATE feeds SET etag = :etag, "lastBuildDate" = :lastBuildDate, "lastFetch" = :lastFetch WHERE id = :id)-"_s))) {
        printFailed();
        qCCritical(ST_UPDATER).noquote() << "Failed to prepare query to update feed" << m_current.logInfo()
                                         << "in the database:" << q.lastError().text();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":etag"_s, m_current.etag);
    q.bindValue(u":lastBuildDate"_s, m_feed.lastBuildDate());
    q.bindValue(u":lastFetch"_s, QDateTime::currentDateTimeUtc());
    q.bindValue(u":id"_s, m_current.id);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        qCCritical(ST_UPDATER).noquote() << "Failed to execute query to update feed" << m_current.logInfo()
                                         << "in the database:" << q.lastError().text();
        exit(dbError(q));
        return;
    }

    QList<FeedItem> newItems;
    QList<FeedItem> updatedItems;

    const QList<FeedItem> items = m_feed.items();
    for (const auto &item : items) {

        if (Q_LIKELY(q.prepare(uR"-(SELECT "pubDate" FROM items WHERE guid = :guid)-"_s))) {
            q.bindValue(u":guid"_s, item.guid());
            if (Q_LIKELY(q.exec())) {
                if (q.next()) {
                    if (item.pubDate() > q.value(0).toDateTime()) {
                        qCDebug(ST_UPDATER).noquote().nospace() << "Found item with guid “" << item.guid() << "“ in the database. "
                                                                << "Updating it.";

                        if (Q_LIKELY(q.prepare(uR"-(UPDATE items SET title = :title, description = :description, author = :author, link = :link, "pubDate" = :pubDate
                                                    WHERE guid = :guid)-"_s))) {
                            q.bindValue(u":title"_s, item.title());
                            q.bindValue(u":description"_s, Utils::cleanDescription(item.description()));
                            q.bindValue(u":author"_s, item.author());
                            q.bindValue(u":link"_s, item.link());
                            q.bindValue(u":pubDate"_s, item.pubDate());

                            if (Q_LIKELY(q.exec())) {
                                updatedItems << item;
                            } else {
                                qCWarning(ST_UPDATER).noquote().nospace() << "Failed to execute query to update item with guid “" << item.guid()
                                                                          << "” in the database: " << q.lastError().text();
                            }

                        } else {
                            qCWarning(ST_UPDATER).noquote().nospace() << "Failed to prepare query to update item with guid “" << item.guid()
                                                                      << "” in the database: " << q.lastError().text();
                        }
                    }

                } else {
                    qCDebug(ST_UPDATER).noquote().nospace() << "Can not find item with guid “" << item.guid() << "“ in the database. "
                                                            << "Must be new. Will insert it.";

                    if (Q_LIKELY(q.prepare(uR"-(INSERT INTO items ("feedId", guid, title, description, author, link, "pubDate")
                                                VALUES (:feedId, :guid, :title, :description, :author, :link, :pubDate))-"_s))) {

                        q.bindValue(u":feedId"_s, m_current.id);
                        q.bindValue(u":guid"_s, item.guid());
                        q.bindValue(u":title"_s, item.title());
                        q.bindValue(u":description"_s, Utils::cleanDescription(item.description()));
                        q.bindValue(u":author"_s, item.author());
                        q.bindValue(u":link"_s, item.link());
                        q.bindValue(u":pubDate"_s, item.pubDate());

                        if (Q_LIKELY(q.exec())) {
                            newItems << item;
                        } else {
                            qCWarning(ST_UPDATER).noquote().nospace() << "Failed to exceute query to insert item with guid “" << item.guid()
                                                                      << "” into the database: " << q.lastError().text();
                        }

                    } else {
                        qCWarning(ST_UPDATER).noquote().nospace() << "Failed to prepare query to insert item with guid “" << item.guid()
                                                                  << "” into the database: " << q.lastError().text();
                    }

                }
            } else {
                qCWarning(ST_UPDATER).noquote().nospace() << "Failed to execute query to select item with guid “"
                                                          << item.guid() << "“ from the database: " << q.lastError().text();
            }
        } else {
            qCWarning(ST_UPDATER).noquote().nospace() << "Failed to prepare query to select item with guid “"
                                                      << item.guid() << "“ from the database: " << q.lastError().text();
        }
    }

    if (newItems.empty() && updatedItems.empty()) {
        printDone();
        qCInfo(ST_UPDATER).noquote().nospace() << "Finished updating feed " << m_current.logInfo()
                                               << ". No new or updated items.";
        QMetaObject::invokeMethod(this, "updateFeed");
    } else {
        qCInfo(ST_UPDATER).noquote().nospace() << "Finished updating feed " << m_current.logInfo()
                                               << " in the database. " << newItems.size() << " new items "
                                               << "and " << updatedItems.size() << " updated items.";
        QList<FeedItem> _items = newItems;
        _items.append(updatedItems);

        qCInfo(ST_UPDATER).noquote() << "Start fetching images for new and updated items of feed" << m_current.logInfo();
        auto iie = new ItemImageExtractor(this);
        connect(iie, &ItemImageExtractor::finished, this, &FeedsUpdateCommand::imagesFetched);
        iie->start(_items);
    }
}

void FeedsUpdateCommand::imagesFetched(const QVariantMap &itemImages, const QMap<QString,QString> &errors)
{
    if (!itemImages.empty()) {
        qCInfo(ST_UPDATER).noquote().nospace()
                << "Finished fetching images for items of feed" << m_current.logInfo()
                << ": found " << itemImages.size() << " images";

        QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

        for (auto i = itemImages.constBegin(), end = itemImages.constEnd(); i != end; ++i) {
            const QString guid = i.key();

            if (Q_UNLIKELY(!q.prepare(uR"-(SELECT data FROM items WHERE guid = :guid)-"_s))) {
                qCWarning(ST_UPDATER).nospace().noquote()
                        << "Failed to prepare database query to get data "
                        << "for item with guid “" << guid << "”: " << q.lastError().text();
                continue;
            }
            q.bindValue(u":guid"_s, guid);
            if (Q_UNLIKELY(!q.exec())) {
                qCWarning(ST_UPDATER).nospace().noquote()
                        << "Failed to execute database query to get data "
                        << "for item with guid “" << guid << "”: " << q.lastError().text();
                continue;
            }
            if (Q_UNLIKELY(!q.next())) {
                qCWarning(ST_UPDATER).nospace().noquote()
                        << "Failed to find item with guid “" << guid << "”.";
            } else {
                auto data = q.value(0).toJsonObject();
                data.insert("image"_L1, QJsonObject::fromVariantMap(i.value().toMap()));

                if (Q_UNLIKELY(!q.prepare(uR"-(UPDATE items SET data = :data WHERE guid = :guid)-"_s))) {
                    qCWarning(ST_UPDATER).noquote().nospace()
                            << "Failed to prepare database query to update data "
                            << "for item with guid “" << guid << "”: " << q.lastError().text();
                    continue;
                }

                q.bindValue(u":data"_s, QString::fromUtf8(QJsonDocument(data).toJson(QJsonDocument::Compact)));

                if (Q_UNLIKELY(!q.exec())) {
                    qCWarning(ST_UPDATER).noquote().nospace()
                            << "Failed to execute database query to update data "
                            << "for item with guid “" << guid << "”: " << q.lastError().text();
                }
            }
        }
    } else {
        qCInfo(ST_UPDATER).noquote().nospace() << "Finished fetching images for items of feed" << m_current.logInfo()
                                               << ": Nothing to do.";
    }

    printDone();
    qCInfo(ST_UPDATER) << "Finished updating feed" << m_current.logInfo();
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
