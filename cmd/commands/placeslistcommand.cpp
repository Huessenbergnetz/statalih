/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "commands/placeslistcommand.h"
#include "utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace Qt::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

PlacesListCommand::PlacesListCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("list");
}

void PlacesListCommand::init()
{
    m_cliOptions.emplace_back(QStringList({u"s"_s, u"search"_s}),
                              //: CLI option description
                              //% "Search for the text in name or slug."
                              qtTrId("statalihcmd-opt-places-list-search-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    addOutputFormatOption();
}

void PlacesListCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(RC::OK);
        return;
    }

    CLI::RC rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    //% "Querying database"
    printStatus(qtTrId("statalihcmd-status-query-db"));

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    QString qs = uR"-(SELECT id, name, slug, parent, "administrativeId", coords, link, created, updated FROM places)-"_s;

    QString search = parser->value(u"search"_s);
    if (!search.isEmpty()) {
        qs += uR"-( WHERE name ILIKE :search OR slug ILIKE :search)-"_s;
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

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    printDone();

    if (q.size() == 0) {
        //% "Your query has not returned any result."
        printWarning(qtTrId("statlihcmd-warn-places-list-nothing-found"));
        exit(RC::OK);
        return;
    }

    const QString outputFormat = parser->value(u"format"_s).toLower();
    if (outputFormat == "json"_L1 || outputFormat == "json-pretty"_L1) {

    } else {
        const QStringList headers{
            //: CLI table header
            //% "ID"
            qtTrId("statalihcmd-table-header-id"),
            //: CLI table header
            //% "Name"
            qtTrId("statalihcmd-table-header-name"),
            //: CLI table header
            //% "Slug"
            qtTrId("statalihcmd-table-header-slug"),
            //: CLI table header
            //% "Parent ID"
            qtTrId("statalihcmd-places-list-table-header-parentid"),
            //: CLI table header
            //% "Administrative ID"
            qtTrId("statalihcmd-places-list-table-header-adminid"),
            //: CLI table header
            //% "Coordinates"
            qtTrId("statalihcmd-places-list-table-header-coords"),
            //: CLI table header
            //% "Link"
            qtTrId("statalihcmd-table-header-link"),
            //: CLI table header, means the numer of feeds
            //% "Feeds"
            qtTrId("statalihcmd-places-list-table-header-feeds"),
            //: CLI table header
            //% "Created"
            qtTrId("statalihcmd-table-header-created"),
            //: CLI table header
            //% "Updated"
            qtTrId("statalihcmd-table-header-updated")
        };

        QLocale locale;
        QList<QStringList> data;
        while (q.next()) {
            QStringList row;
            const auto id = q.value(0).toInt();
            row << QString::number(id);
            row << q.value(1).toString();
            row << q.value(2).toString();
            const auto parentId = q.value(3).toInt();
            row << (parentId > 0 ? QString::number(parentId) : QString());
            row << q.value(4).toString();
            const auto coords = Utils::coordsFromDb(q.value(5));
            if (coords) {
                row << Utils::humanCoords(coords.value());
            } else {
                row << QString();
            }
            row << q.value(6).toString();

            QSqlQuery qq{QSqlDatabase::database(HBNST_DBCONNAME)};
            qq.prepare(uR"-(SELECT COUNT(*) FROM feeds WHERE "placeId" = :placeId)-"_s);
            qq.bindValue(u":placeId"_s, id);
            qq.exec();
            qq.next();
            row << locale.toString(qq.value(0).toInt());

            row << locale.toString(q.value(7).toDateTime().toLocalTime(), QLocale::ShortFormat);
            const auto updated = q.value(8).toDateTime();
            if (updated.isValid()) {
                row << locale.toString(updated.toLocalTime(), QLocale::ShortFormat);
            } else {
                row << QString();
            }

            data << row;
        }

        printTable(headers, data);
    }

    exit(RC::OK);
}

QString PlacesListCommand::summary() const
{
    //: CLI command summary
    //% "List places"
    return qtTrId("statalihcmd-command-places-list-summary");
}

QString PlacesListCommand::description() const
{
    //: CLI command description
    //% "List places available in the database."
    return qtTrId("statalihcmd-command-places-list-description");
}

#include "moc_placeslistcommand.cpp"
