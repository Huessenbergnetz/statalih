/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaseresetcommand.h"

#include <Firfuorida/Migrator>

#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

DatabaseResetCommand::DatabaseResetCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(u"reset"_s);
}

void DatabaseResetCommand::exec(QCommandLineParser *parser)
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

    if (!m_migrator->reset()) {
        exit(dbError(m_migrator->lastError().text()));
        return;
    }

    exit(rc);
}

QString DatabaseResetCommand::summary() const
{
    //: CLI command summary
    //% "Reset database migrations"
    return qtTrId("statalihcmd-command-database-reset-summary");
}

QString DatabaseResetCommand::description() const
{
    //: CLI command description
    //% "Resets all database migrations. A reset will roll back all migrations. "
    //% "Note that this will result in data loss. This command is for development purposes only."
    return qtTrId("statalihcmd-command-database-reset-description");
}

#include "moc_databaseresetcommand.cpp"
