/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "databasecommand.h"
#include "databasemigrationcommand.h"
#include "databaserefreshcommand.h"
#include "databaseresetcommand.h"
#include "databaserollbackcommand.h"

#include "dbmigrations/m0002_createfeedstable.h"

#include <Firfuorida/Migrator>

#include <QCoreApplication>
#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

DatabaseCommand::DatabaseCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(u"database"_s);
}

void DatabaseCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addPositionalArgument(u"subcommand"_s, QString());
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst() != objectName()) {
        qFatal() << "Invalid command name! Aborting…";
    }

    const QString command = args.empty() ? u""_s : args.takeFirst().toLower();

    runSubCommand(command, parser);
}

QString DatabaseCommand::summary() const
{
    //: CLI command summary
    //% "Manage database migrations"
    return qtTrId("statalihcmd-command-database-summary");
}

QString DatabaseCommand::description() const
{
    //: CLI command description
    //% "Run, roll back, reset and refresh database migrations. Be careful, these commands can easily lead to data loss! Make sure that you have made a backup of your database beforehand."
    return qtTrId("statalihcmd-command-database-description");
}

QString DatabaseCommand::dbConName() const
{
    return u"dbmigrations"_s;
}

void DatabaseCommand::initMigrations()
{
    m_migrator = std::make_unique<Firfuorida::Migrator>(dbConName(), u"migrations"_s);
    new M0002_CreateFeedsTable(m_migrator.get());
}

void DatabaseCommand::init()
{
    new DatabaseMigrationCommand(this);
    new DatabaseRefreshCommand(this);
    new DatabaseResetCommand(this);
    new DatabaseRollbackCommand(this);
}

#include "moc_databasecommand.cpp"
