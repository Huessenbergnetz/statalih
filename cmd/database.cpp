/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "database.h"
#include "confignames.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

using namespace Qt::Literals::StringLiterals;

Database::Database(QObject *parent)
    : Configuration{parent}
{

}

CLI::RC Database::openDb(const QString &connectionName)
{
    const RC rc = loadConfig();
    if (rc != RC::OK) {
        return rc;
    }

    //% "Establishing database connection"
    printStatus(qtTrId("stcmd-status-open-db"));

    const QString confSec   = QStringLiteral(HBNST_CONF_DB);
    const QString dbType    = value(confSec, QStringLiteral(HBNST_CONF_DB_TYPE), QStringLiteral(HBNST_CONF_DB_TYPE_DEFVAL)).toString().toUpper();
    const QString dbHost    = value(confSec, QStringLiteral(HBNST_CONF_DB_HOST)).toString();
    const QString dbUser    = value(confSec, QStringLiteral(HBNST_CONF_DB_USER)).toString();
    const QString dbPass    = value(confSec, QStringLiteral(HBNST_CONF_DB_PASS)).toString();
    const QString dbName    = value(confSec, QStringLiteral(HBNST_CONF_DB_NAME), QStringLiteral(HBNST_CONF_DB_NAME_DEFVAL)).toString();
    const int     dbPort    = value(confSec, QStringLiteral(HBNST_CONF_DB_PORT), HBNST_CONF_DB_PORT_DEFVAL).toInt();

    auto db = QSqlDatabase::addDatabase(dbType, connectionName);

    if (Q_UNLIKELY(!db.isValid())) {
        printFailed();
        //: Error message
        //% "Failed to obtain database object."
        return dbError(qtTrId("stcmd-error-db-invalid-object"));
    }

    if (dbType == u"QPSQL") {
        db.setDatabaseName(dbName);
        if (!dbUser.isEmpty()) {
            db.setUserName(dbUser);
        }
        if (!dbPass.isEmpty()) {
            db.setPassword(dbPass);
        }
        if (!dbHost.isEmpty()) {
            db.setHostName(dbHost);
            if (dbHost[0] != '/'_L1) {
                db.setPort(dbPort);
            }
        }
    // } else if (dbType == u"QMYSQL") {
    //     db.setDatabaseName(dbName);
    //     if (!dbUser.isEmpty()) {
    //         db.setUserName(dbUser);
    //     }
    //     if (!dbPass.isEmpty()) {
    //         db.setPassword(dbPass);
    //     }
    //     if (dbHost[0] == '/'_L1) {
    //         db.setConnectOptions(u"UNIX_SOCKET=%1;MYSQL_OPT_RECONNECT=1;CLIENT_INTERACTIVE=1"_s.arg(dbHost));
    //     } else {
    //         db.setConnectOptions(u"MYSQL_OPT_RECONNECT=1;CLIENT_INTERACTIVE=1"_s);
    //         db.setHostName(dbHost);
    //         db.setPort(dbPort);
    //     }
    } else {
        printFailed();
        //: Error message, %1 will be the invalid DB type
        //% "Can not open database connection for not supported database type “%1”."
        return configError(qtTrId("stcmd-error-config-invalid-dbtype").arg(dbType));
    }

    if (Q_UNLIKELY(!db.open())) {
        printFailed();
        return dbError(db);
    }

    printDone();

    if (dbType == u"QMYSQL") {
        QSqlQuery q(db);
        if (Q_UNLIKELY(!q.exec(u"SET time_zone = '+00:00'"_s))) {
            //: CLI warning message, %1 will be the database error
            //% "Failed to set database connection time zone to UTC: %1"
            printWarning(qtTrId("stcmd-warn-db-set-con-tz").arg(q.lastError().text()));
        }
    } else if (dbType == u"QPSQL") {
        QSqlQuery q(db);
        if (Q_UNLIKELY(!q.exec(u"SET TIME ZONE 'UTC'"_s))) {
            printWarning(qtTrId("stcmd-warn-db-set-con-tz").arg(q.lastError().text()));
        }
    }

    return RC::OK;
}

#include "moc_database.cpp"
