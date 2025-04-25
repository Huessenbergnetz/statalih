/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "placesaddcommand.h"
#include "utils.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLocale>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>

#include <QDebug>

using namespace Qt::StringLiterals;

#define HBNST_DBCONNAME u"dbcon"_s

PlacesAddCommand::PlacesAddCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("add");
}

void PlacesAddCommand::init()
{

    QLocale locale;

    m_cliOptions.emplace_back(QStringList({u"n"_s, u"name"_s}),
                              //: CLI option description
                              //% "Name of the place to add. (required)"
                              qtTrId("statalihcmd-opt-places-add-name-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"s"_s, u"slug"_s}),
                              //: CLI option description
                              //% "Slug for this place used in URLs."
                              qtTrId("statalihcmd-opt-places-add-slug-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"p"_s, u"parent"_s}),
                              //% "Database ID of a place this place belongs to. "
                              //% "Like a district belonging to a city. "
                              //% "Use %1 to get a list of places."
                              qtTrId("statalihcmd-opt-places-add-parent-desc").arg(locale.quoteString(u"places list")),
                              //: CLI option value name
                              //% "ID"
                              qtTrId("statlihcmd-opt-value-dbid"));

    m_cliOptions.emplace_back(QStringList({u"c"_s, u"coordinates"_s}),
                              //: CLI option description, %1 will be replaced by the
                              //: example latitude, %2 by the example longitude
                              //% "Coordinates of the place. Enter them like \"%1;%2\"."
                              qtTrId("statalihcmd-opt-places-add-coords-desc").arg(locale.toString(51.571667), locale.toString(9.166667)),
                              //: CLI option value name
                              //% "latitude;longitude"
                              qtTrId("statalihcmd-opt-places-add-coords-value"));

    m_cliOptions.emplace_back(QStringList({u"l"_s, u"link"_s}),
                              //: CLI option description
                              //% "URL of a website describing this place."
                              qtTrId("statalihcmd-opt-places-add-link-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-url"));

    m_cliOptions.emplace_back(QStringList({u"d"_s, u"description"_s}),
                              //: CLI option description
                              //% "Short description for this place."
                              qtTrId("statalihcmd-opt-places-add-description-desc"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    m_cliOptions.emplace_back(QStringList({u"a"_s, u"administrative-id"_s}),
                              //: CLI option description
                              //% "Administrative ID for this place."
                              qtTrId("statalihcmd-opt-places-add-admin-id"),
                              // source string defined in feedsaddcommand.cpp
                              qtTrId("statalihcmd-opt-value-text"));

    addOutputFormatOption();
}

void PlacesAddCommand::exec(QCommandLineParser *parser)
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

    const QString name = parser->value(u"name"_s);
    if (Q_UNLIKELY(name.isEmpty())) {
        printFailed();
        //% "Please use the -n argument to set a name for this place."
        exit(inputError(qtTrId("statalihcmd-err-places-add-missing-name")));
        return;
    }

    const QString slug = parser->isSet(u"slug"_s) ? Utils::slugify(parser->value(u"slug"_s)) : Utils::slugify(name);

    int parentId = 0;
    if (parser->isSet(u"parent"_s)) {
        bool ok{false};
        parentId = parser->value(u"parent"_s).toInt(&ok);
        if (!ok || parentId <= 0) {
            printFailed();
            //% "Invalid parent ID. Has to be an integer value bigger than 0."
            exit(inputError(qtTrId("statlihcmd-err-places-add-invalid-parent")));
            return;
        }
    }

    bool coordsSet{false};
    float latitude{0.0};
    float longitude{0.0};

    if (parser->isSet(u"coordinates"_s)) {
        const QStringList coordStr = parser->value(u"coordinates"_s).split(';'_L1, Qt::SkipEmptyParts);

        if (Q_UNLIKELY(coordStr.size() != 2)) {
            printFailed();
            //: Error message
            //% "Invalid coordinates"
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        QLocale locale;
        bool ok = false;
        latitude = locale.toFloat(coordStr.at(0), &ok);
        if (Q_UNLIKELY(!ok)) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        if (latitude < -90.f || latitude > 90.f) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        longitude = locale.toFloat(coordStr.at(1), &ok);
        if (Q_UNLIKELY(!ok)) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        if (longitude < -180.f || longitude > 180.f) {
            printFailed();
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-coords")));
            return;
        }

        coordsSet = true;
    }

    const QUrl link = parser->isSet(u"link"_s) ? QUrl(parser->value(u"link"_s)) : QUrl();
    if (!link.isEmpty()) {
        if (!link.isValid()) {
            printFailed();
            //% "Invalid URL given for link option."
            exit(inputError(qtTrId("statalihcmd-err-feeds-add-invalid-link")));
            return;
        }
        const QStringList allowedSchemes{u"http"_s, u"https"_s};
        if (!allowedSchemes.contains(link.scheme(), Qt::CaseInsensitive)) {
            printFailed();
            //% "Link has to be a HTTP or HTTPS URL."
            exit(inputError(qtTrId("statalihcmd-err-places-add-nonhttp-link")));
            return;
        }
    }

    const QString description = parser->value(u"description"_s);
    const QString adminId = parser->value(u"administrative-id"_s);

    printDone();

    CLI::RC rc = openDb(HBNST_DBCONNAME);
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    //% "Adding new place"
    printStatus(qtTrId("statalihcmd-status-places-add-db-add"));

    QSqlQuery q{QSqlDatabase::database(HBNST_DBCONNAME)};

    if (Q_UNLIKELY(!q.prepare(u"SELECT id, name FROM places WHERE slug = :slug"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":slug"_s, slug);

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    if (Q_UNLIKELY(q.next())) {
        printFailed();
        //% "This slug is already in use by another place (%1, ID: %2)."
        exit(inputError(qtTrId("statlihcmd-err-places-add-slug-in-use").arg(q.value(1).toString(), QString::number(q.value(0).toInt()))));
        return;
    }

    if (Q_UNLIKELY(!q.prepare(uR"-(INSERT INTO places (name, slug, parent, "administrativeId", coords, description, link, created, updated, data)
                              VALUES (:name, :slug, :parent, :adminId, :coords, :desc, :link, :created, :updated, :data)
                              RETURNING id)-"_s))) {
        printFailed();
        exit(dbError(q));
        return;
    }

    q.bindValue(u":name"_s, name);
    q.bindValue(u":slug"_s, slug);
    q.bindValue(u":parent"_s, parentId > 0 ? parentId : QVariant());
    q.bindValue(u":adminId"_s, !adminId.isEmpty() ? adminId : QVariant());
    if (coordsSet) {
        q.bindValue(u":coords"_s, Utils::coordsToDb(latitude, longitude));
    } else {
        q.bindValue(u":coords"_s, {});
    }
    q.bindValue(u":desc"_s, !description.isEmpty() ? description : QVariant());
    q.bindValue(u":link"_s, link.isValid() ? link : QVariant());
    q.bindValue(u":created"_s, QDateTime::currentDateTimeUtc());

    if (Q_UNLIKELY(!q.exec())) {
        printFailed();
        exit(dbError(q));
        return;
    }

    printDone();

    q.next();
    const auto id = q.value(0).toInt();

    const QString outputFormat = parser->value(u"format"_s).toLower();
    if (outputFormat == "json"_L1 || outputFormat == "json-pretty"_L1) {

        QJsonObject o{
            {u"id"_s, id},
            {u"name"_s, name},
            {u"slug"_s, slug},
            {u"parent"_s, parentId > 0 ? parentId : QJsonValue()},
            {u"administrativeId"_s, !adminId.isEmpty() ? adminId : QJsonValue()},
            {u"description"_s, !description.isEmpty() ? description : QJsonValue()},
            {u"link"_s, link.isValid() ? link.toString() : QJsonValue()}
        };

        if (coordsSet) {
            o.insert(u"coordinates"_s, QJsonObject({
                                                       {u"latitude"_s, latitude},
                                                       {u"longitude"_s, longitude}
                                                   }));
        } else {
            o.insert(u"coordinates"_s, {});
        }

        const QJsonDocument json(o);
        QTextStream out(stdout, QIODeviceBase::WriteOnly);
        out << json.toJson(outputFormat == "json"_L1 ? QJsonDocument::Compact : QJsonDocument::Indented);

    } else {
        const QStringList headers{
                    // source string defined in feedsaddcommand.cpp
                    qtTrId("statalihcmd-table-header-key"),
                    // source string defined in feedsaddcommand.cpp
                    qtTrId("statalihcmd-table-header-value")
                };

        QList<QStringList> data;
        data << QStringList({u"ID"_s, QString::number(id)});
        data << QStringList({u"Name"_s, name});
        data << QStringList({u"Slug"_s, slug});
        data << QStringList({u"Parent ID"_s, parentId > 0 ? QString::number(parentId) : QString()});
        data << QStringList({u"Administrative ID"_s, adminId});
        data << QStringList({u"Description"_s, description});
        data << QStringList({u"Link"_s, link.toString()});
        QString coords;
        if (coordsSet) {
            coords = Utils::humanCoords(latitude, longitude);
        }
        data << QStringList({u"Coordinates"_s, coords});

        printTable(headers, data);
    }

    exit(RC::OK);
}

QString PlacesAddCommand::summary() const
{
    //: CLI command summary
    //% "Add a new place"
    return qtTrId("statalihcmd-command-places-add-summary");
}

QString PlacesAddCommand::description() const
{
    //: CLI command description
    //% "Adds a new place to the database."
    return qtTrId("statalihcmd-command-places-add-description");
}
