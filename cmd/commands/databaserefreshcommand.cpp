/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databaserefreshcommand.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

DatabaseRefreshCommand::DatabaseRefreshCommand(QObject *parent)
    : DatabaseCommand{parent}
{
    setObjectName(u"refresh");
}

void DatabaseRefreshCommand::init()
{
    const QString defVal = u"0"_s;
    m_cliOptions.append(QCommandLineOption(QStringList({u"s"_s, u"steps"_s}),
                                           //: CLI option description, %1 will be replaced by the default value
                                           //% "Number of migration stepts to refresh. If 0, all migrations will be refreshed. Default: %1."
                                           qtTrId("statalihcmd-opt-database-refresh-steps-desc").arg(defVal),
                                           //: CLI option value name
                                           //% "steps"
                                           qtTrId("statalihcmd-opt-database-refresh-steps-value"),
                                           defVal));
}

void DatabaseRefreshCommand::exec(QCommandLineParser *parser)
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
        //: Error message, %1 will be replaced by the malformed input value.
        //% "Can not perform database migration refresh. “%1” is not a valid integer value for steps."
        exit(inputError(qtTrId("statalihcmd-error-invalid-refresh-steps-integer").arg(stepsString)));
        return;
    }

    setGlobalOptions(parser);

    rc = openDb(dbConName());
    if (rc != RC::OK) {
        exit(rc);
        return;
    }

    initMigrations();

    if (!m_migrator->refresh(steps)) {
        exit(dbError(m_migrator->lastError().text()));
        return;
    }

    exit (rc);
}

QString DatabaseRefreshCommand::summary() const
{
    //: CLI command summary
    //% "Refresh database migrations"
    return qtTrId("statalihcmd-command-database-refresh-summary");
}

QString DatabaseRefreshCommand::description() const
{
    //: CLI command description
    //% "Refreshes the specified number of migration steps. A refresh will roll back the specified number of migrations and will rerun them afterwards. Note that this will result in data loss. This command is for development purposes only."
    return qtTrId("statalihcmd-command-database-refresh-description");
}

#include "moc_databaserefreshcommand.cpp"
