/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedslistitemscommand.h"
#include "utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

using namespace Qt::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

FeedsListItemsCommand::FeedsListItemsCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("list-items");
}

void FeedsListItemsCommand::init()
{
    m_cliOptions.emplaceBack(QStringList({u"feed"_s}),
                             //: CLI option description
                             //% "Only show items for the feed specified by feed ID. Can not be used together with --place."
                             qtTrId("statalihcmd-opt-feeds-itemlist-feed-desc"),
                             // source string defined in placesaddcommand.cpp
                             qtTrId("statlihcmd-opt-value-dbid"));

    m_cliOptions.emplaceBack(QStringList({u"p"_s, u"place"_s}),
                             //: CLI option description
                             //% "Only show items for feeds belonging to the place identified by place ID. Can not be used together with --feed."
                             qtTrId("statalihcmd-opt-feeds-itemlist-place-desc"),
                             // source string defined in placesaddcommand.cpp
                             qtTrId("statlihcmd-opt-value-dbid"));

    m_cliOptions.emplaceBack(QStringList({u"s"_s, u"search"_s}),
                             //: CLI option description
                             //% "Search for text in item title or description."
                             qtTrId("statalihcmd-opt-feeds-itemlist-search-desc"),
                             // source string defined in feedsaddcommand.cpp
                             qtTrId("statalihcmd-opt-value-text"));

    addOutputFormatOption();
}

void FeedsListItemsCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(RC::OK);
        return;
    }

    setGlobalOptions(parser);

    // source string defined in feedsaddcommand.cpp
    printStatus(qtTrId("statalihcmd-status-parsing-input"));

    if (parser->isSet(u"feed"_s) && parser->isSet(u"place"_s)) {
        printFailed();
        //% "Either --place or --feed is supported but not both."
        exit(inputError(qtTrId("statalihcmd-err-feeds-listitems-feedandplace")));
        return;
    }

    int placeId = -1;
    const QString placeIdStr = parser->value(u"place"_s);
    if (!placeIdStr.isEmpty()) {
        bool ok = false;
        placeId = placeIdStr.toInt(&ok);
        if (!ok)  {
            printFailed();
            // source string defined in feedslistcommand.cpp
            exit(inputError(qtTrId("statalihcmd-err-feeds-list-invalid-placeid")));
            return;
        }
    }

    int feedId = -1;
    const QString feedIdStr = parser->value(u"feed"_s);
    if (!feedIdStr.isEmpty()) {
        bool ok = false;
        feedId = feedIdStr.toInt(&ok);
        if (!ok) {
            printFailed();
            //% "Invalid feed ID."
            exit(inputError(qtTrId("statalihcmd-err-feeds-listitmes-invaid-feedid")));
            return;
        }
    }

    QString search = parser->value(u"search"_s);

    const QString outputFormat = parser->value(u"format"_s).toLower();

    printDone();

    CLI::RC rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    // source string defined in placeslistcommand.cpp
    printStatus(qtTrId("statalihcmd-status-query-db"));

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    QString qs;
    if (outputFormat == "json"_L1 || outputFormat == "json-pretty"_L1) {
        qs = uR"-(SELECT i.id, i."feedId", p.id AS "placeId", i.guid, i.title, i.description, i.author, i."pubDate", i.link, i.data FROM items i JOIN feeds f ON i."feedId" = f.id JOIN places p ON f."placeId" = p.id)-"_s;
    } else {
        qs = uR"-(SELECT i.id, i."feedId", p.id AS "placeId", i.title, i."pubDate", i.link FROM items i JOIN feeds f ON i."feedId" = f.id JOIN places p ON f."placeId" = p.id)-"_s;
    }

    QStringList where;

    if (!search.isEmpty()) {
        where << u"(i.title ILIKE :search OR i.description ILIKE :search)"_s;
    }

    if (placeId > -1) {
        where << u"p.id = :placeId"_s;
    }

    if (feedId > -1) {
        where << uR"-(i."feedId" = :feedId)-"_s;
    }

    if (!where.empty()) {
        qs += " WHERE "_L1;
        qs += where.join(" AND "_L1);
    }

    qs += uR"-( ORDER BY i."pubDate" DESC)-"_s;

    if (Q_UNLIKELY(!q.prepare(qs))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (!search.isEmpty()) {
        search.prepend('%'_L1);
        search.append('%'_L1);
        q.bindValue(u":search"_s, search);
    }

    if (placeId > -1) {
        q.bindValue(u":placeId"_s, placeId);
    }

    if (feedId > -1) {
        q.bindValue(u":feedId"_s, feedId);
    }

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    printDone();

    if (q.size() == 0) {
        // source string defined in feedslistcommand.cpp
        printWarning(qtTrId("statlihcmd-warn-feeds-list-nothing-found"));
        exit(RC::OK);
        return;
    }

    if (outputFormat == "json"_L1 || outputFormat == "json-pretty"_L1) {

        const auto jsonArray = Utils::queryToJsonObjectArray(q);
        const QJsonDocument json{jsonArray};
        QTextStream out(stdout, QIODeviceBase::WriteOnly);
        out << json.toJson(outputFormat == "json"_L1 ? QJsonDocument::Compact : QJsonDocument::Indented);

    } else {
        const QStringList headers{
            // source string defined in placeslistcommand.cpp
            qtTrId("statalihcmd-table-header-id"),
            //: CLI table header
            //% "Feed ID"
            qtTrId("statalihcmd-table-header-feedid"),
            // source string defined in feedslistcommand.cpp
            qtTrId("statalihcmd-table-header-placeid"),
            // source string defined in feedslistcommand.cpp
            qtTrId("statalihcmd-table-header-title"),
            //: CLI table header
            //% "Publication Date"
            qtTrId("statalihcmd-table-header-pubdate"),
            // source string defined in feedslistcommand.cpp
            qtTrId("statalihcmd-table-header-link")
        };

        QLocale locale;
        QList<QStringList> data;
        while (q.next()) {
            QStringList row;
            row << QString::number(q.value(0).toLongLong());
            row << QString::number(q.value(1).toInt());
            row << QString::number(q.value(2).toInt());
            row << q.value(3).toString();
            const auto pubDate = q.value(4).toDateTime();
            if (pubDate.isValid()) {
                row << locale.toString(pubDate.toLocalTime(), QLocale::ShortFormat);
            } else {
                row << QString();
            }
            row << q.value(5).toString();

            data << row;
        }

        printTable(headers, data);
    }

    exit(RC::OK);
}

QString FeedsListItemsCommand::summary() const
{
    //: CLI command summary
    //% "List feed items"
    return qtTrId("statalihcmd-command-feeds-listitems-summary");
}

QString FeedsListItemsCommand::description() const
{
    //: CLI command description
    //% "List feed items data from the database."
    return qtTrId("statalihcmd-command-feeds-listitems-description");
}

#include "moc_feedslistitemscommand.cpp"
