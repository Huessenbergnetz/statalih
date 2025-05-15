/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedslistcommand.h"
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

#include <QDebug>

using namespace Qt::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

FeedsListCommand::FeedsListCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("list");
}

void FeedsListCommand::init()
{
    m_cliOptions.emplace_back(QStringList({u"s"_s, u"search"_s}),
                              //: CLI option description
                              //% "Search for the text in title or slug."
                              qtTrId("statalihcmd-opt-feeds-list-search-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"p"_s, u"place"_s}),
                              //: CLI option description
                              //% "Database ID of the place the feeds to show belong to."
                              qtTrId("statalihcmd-opt-feeds-list-place-desc"),
                              // source string defined in placesaddcommand.cpp
                              qtTrId("statlihcmd-opt-value-dbid"));

    addOutputFormatOption();
}

void FeedsListCommand::exec(QCommandLineParser *parser)
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

    int placeId = -1;
    const QString placeIdStr = parser->value(u"place"_s);
    if (!placeIdStr.isEmpty()) {
        bool ok = false;
        placeId = placeIdStr.toInt(&ok);
        if (!ok) {
            printFailed();
            //% "Invalid place ID."
            exit(inputError(qtTrId("statalihcmd-err-feeds-list-invalid-placeid")));
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
        qs = u"SELECT * FROM feeds"_s;
    } else {
        qs = uR"-(SELECT id, "placeId", slug, title, link, "lastFetch", enabled FROM feeds)-"_s;
    }

    QStringList where;

    if (!search.isEmpty()) {
        where << u"(slug ILIKE :search OR title ILIKE :search)"_s;
    }

    if (placeId > -1) {
        where << uR"-("placeId" = :placeId)-"_s;
    }

    if (!where.empty()) {
        qs += " WHERE "_L1;
        qs += where.join(" AND "_L1);
    }

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

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    printDone();

    if (q.size() == 0) {
        //% "Your query has not returned any result."
        printWarning(qtTrId("statlihcmd-warn-feeds-list-nothing-found"));
        exit(RC::OK);
        return;
    }


    if (outputFormat == "json"_L1 || outputFormat == "json-pretty"_L1) {

        const auto jsonArray = Utils::queryToJsonObjectArray(q);
        const QJsonDocument json(jsonArray);
        QTextStream out(stdout, QIODeviceBase::WriteOnly);
        out << json.toJson(outputFormat == "json"_L1 ? QJsonDocument::Compact : QJsonDocument::Indented);

    } else {
        const QStringList headers{
            // source string defined in placeslistcommand.cpp
            qtTrId("statalihcmd-table-header-id"),
            //: CLI table header
            //% "Place ID"
            qtTrId("statalihcmd-table-header-placeid"),
            // source string defined in placeslistcommand.cpp
            qtTrId("statalihcmd-table-header-slug"),
            //: CLI table header
            //% "Title"
            qtTrId("statalihcmd-table-header-title"),
            //: CLI table header
            //% "Link"
            qtTrId("statalihcmd-table-header-link"),
            //: CLI table header
            //% "Last Fetch"
            qtTrId("statalihcmd-feeds-list-table-header-lastFetch"),
            //: CLI table header
            //% "Enabled"
            qtTrId("statalihcmd-feeds-list-table-header-enabled")
        };

        QLocale locale;
        QList<QStringList> data;
        while (q.next()) {
            QStringList row;
            row << QString::number(q.value(0).toInt());
            row << QString::number(q.value(1).toInt());
            row << q.value(2).toString();
            row << q.value(3).toString();
            row << q.value(4).toString();

            const auto lastFetch = q.value(5).toDateTime();
            if (lastFetch.isValid()) {
                row << locale.toString(lastFetch.toLocalTime(), QLocale::ShortFormat);
            } else {
                row << QString();
            }

            QString isEnabled = q.value(6).toBool() ?
                        //% "yes"
                        qtTrId("statalihcmd-yes") :
                        //% "no"
                        qtTrId("statalihcmd-no");
            row << isEnabled;

            data << row;
        }

        printTable(headers, data);
    }

    exit(RC::OK);
}

QString FeedsListCommand::summary() const
{
    //: CLI command summary
    //% "List feeds"
    return qtTrId("statalihcmd-command-feeds-list-summary");
}

QString FeedsListCommand::description() const
{
    //: CLI command description
    //% "List feeds available in the database."
    return qtTrId("statalihcmd-command-feeds-list-description");
}

#include "moc_feedslistcommand.cpp"
