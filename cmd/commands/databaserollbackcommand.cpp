/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaserollbackcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

DatabaseRollbackCommand::DatabaseRollbackCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(u"rollback"_s);
}

void DatabaseRollbackCommand::init()
{
    const QString defVal = u"1"_s;
    m_cliOptions.append(QCommandLineOption({u"s"_s, u"steps"_s},
                                           //: CLI option description, %1 will be replaced by the default value
                                           //% "Number of migration steps to roll back. Default: %1."
                                           qtTrId("statalihcmd-opt-database-rollback-steps-desc").arg(defVal),
                                           //: CLI option value name
                                           //% "steps"
                                           qtTrId("statalihcmd-opt-database-rollback-steps-value"),
                                           defVal));
}

void DatabaseRollbackCommand::exec(QCommandLineParser *parser)
{
    CLI::RC rc = RC::OK;

    init();

    parser->addOptions(m_cliOptions);
    parser->parse(QCoreApplication::arguments());

    if (checkShowHelp(parser)) {
        exit(rc);
        return;
    }

    const QString stepsString = parser->value(u"steps"_s);
    bool ok = false;
    uint steps = stepsString.toUInt(&ok);

    if (!ok) {
        //: CLI error message
        //% "Can not perform database migration rollback. “%1” is not a valid integer value for steps."
        exit(inputError(qtTrId("statalihcmd-error-invalid-rollback-steps-integer").arg(stepsString)));
        return;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    initMigrations();

    if (!m_migrator->rollback(steps)) {
        exit(dbError(m_migrator->lastError().text()));
        return;
    }

    exit(rc);
}

QString DatabaseRollbackCommand::summary() const
{
    //: CLI command summary
    //% "Roll back database migrations"
    return qtTrId("statalihcmd-command-database-rollback-summary");
}

QString DatabaseRollbackCommand::description() const
{
    //: CLI command description
    //% "Rolls back the specified number of migration steps. Note that this will result in data loss. This command is for development purposes only."
    return qtTrId("statalihcmd-command-database-rollback-description");
}

#include "moc_databaserollbackcommand.cpp"
