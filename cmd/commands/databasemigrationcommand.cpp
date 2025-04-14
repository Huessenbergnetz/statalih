/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databasemigrationcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

DatabaseMigrationCommand::DatabaseMigrationCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(u"migrate"_s);
}

void DatabaseMigrationCommand::exec(QCommandLineParser *parser)
{
    CLI::RC rc = RC::OK;

    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(rc);
        return;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    initMigrations();

    if (!m_migrator->migrate()) {
        rc = dbError(m_migrator->lastError().text());
        exit(rc);
        return;
    }

    exit(rc);
}

QString DatabaseMigrationCommand::summary() const
{
    //: CLI command summary
    //% "Run database migrations"
    return qtTrId("statalihcmd-command-database-migration-summary");
}

QString DatabaseMigrationCommand::description() const
{
    //: CLI command description
    //% "Performs all database migrations that have not already be performed before. You should run this command after upgrading Zumftmeistar."
    return qtTrId("statalihcmd-command-database-migration-description");
}

#include "moc_databasemigrationcommand.cpp"
