/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedscommand.h"
#include "feedsaddcommand.h"
#include "feedsupdatecommand.h"
#include "feedslistcommand.h"
#include "feedslistitemscommand.h"

#include <QCoreApplication>
#include <QCommandLineParser>

using namespace Qt::Literals::StringLiterals;

FeedsCommand::FeedsCommand(QObject *parent)
    : Command{parent}
{
    setObjectName(u"feeds"_s);
}

void FeedsCommand::exec(QCommandLineParser *parser)
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

QString FeedsCommand::summary() const
{
    //: CLI command summary
    //% "Manage web feeds"
    return qtTrId("statalihcmd-command-feeds-summary");
}

QString FeedsCommand::description() const
{
    //: CLI command description
    //% "Add, list, remove, update and fetch web feeds."
    return qtTrId("statalihcmd-command-feeds-description");
}

void FeedsCommand::init()
{
    new FeedsAddCommand(this);
    new FeedsUpdateCommand(this);
    new FeedsListCommand(this);
    new FeedsListItemsCommand(this);
}

#include "moc_feedscommand.cpp"
