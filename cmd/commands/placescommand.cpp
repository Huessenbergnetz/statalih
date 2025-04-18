/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "placescommand.h"
#include "placesaddcommand.h"

#include <QCommandLineParser>
#include <QCoreApplication>

using namespace Qt::StringLiterals;

PlacesCommand::PlacesCommand(QObject *parent)
    : Command{parent}
{
    setObjectName("places");
}

void PlacesCommand::exec(QCommandLineParser *parser)
{
    init();

    parser->addPositionalArgument(u"subcommands"_s, QString());
    parser->parse(QCoreApplication::arguments());

    setGlobalOptions(parser);

    QStringList args = parser->positionalArguments();
    if (args.takeFirst() != objectName()) {
        qFatal() << "Invalid command name! Aborting…";
    }

    const QString command = args.empty() ? u""_s : args.takeFirst().toLower();

    runSubCommand(command, parser);
}

QString PlacesCommand::summary() const
{
    //: CLI command summary
    //% "Manage places"
    return qtTrId("statalihcmd-command-places-summary");
}

QString PlacesCommand::description() const
{
    //: CLI command description
    //% "Add, list, remove and change places."
    return qtTrId("statalihcmd-command-places-summary");
}

void PlacesCommand::init()
{
    new PlacesAddCommand(this);
}

#include "moc_placescommand.cpp"
