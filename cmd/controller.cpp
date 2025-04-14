/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controller.h"

#include "commands/command.h"
#include "commands/databasecommand.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

using namespace Qt::Literals::StringLiterals;

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    setObjectName(u"controller"_s);
}

void Controller::exec()
{
    init();

    QCommandLineParser parser;

    parser.addOptions(m_globalOptions);
    parser.addPositionalArgument(u"command"_s, QString());

    parser.parse(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();
    const QString command = args.empty() ? u""_s : args.first();

    auto com = findChild<Command *>(command, Qt::FindDirectChildrenOnly);

    if (com) {
        com->exec(&parser);
    } else {
        if (parser.isSet(u"h"_s) || parser.isSet(u"help"_s)) {
            showHelp();
        } else {
            showHelp();
            // QTextStream out(stdout, QIODeviceBase::WriteOnly);
            // //: CLI error
            // //% "“%1” is not a valid command. Use -h/--help to show a list of available commands."
            // out << qtTrId("statalihcmd-invalid-command").arg(command) << '\n';
            // qApp->exit(static_cast<int>(CLI::RC::InvalidOption));
        }
    }
}

void Controller::init()
{
    m_globalOptions.append(QCommandLineOption(QStringList({u"h"_s, u"help"_s}),
                                              //: CLI option description
                                              //% "Displays help on command line options and commands."
                                              qtTrId("statalihcmd-opt-global-help-desc")));

    const QString iniFileDefVal = QStringLiteral(HBNST_CONFFILE);
    m_globalOptions.append(QCommandLineOption(QStringList({u"i"_s, u"ini"_s}),
                                              //: CLI option description, %1 will be the default path
                                              //% "Path to configuration ini file. Default: %1"
                                              qtTrId("statalihcmd-opt-global-inifile-desc").arg(iniFileDefVal),
                                              //: CLI option value name
                                              //% "ini file"
                                              qtTrId("statalihcmd-opt-global-inifile-val"),
                                              iniFileDefVal));

    m_globalOptions.append(QCommandLineOption(QStringList({u"q"_s, u"quiet"_s}),
                                              //: CLI option description
                                              //% "Be quiet and print less output."
                                              qtTrId("statalihcmd-opt-global-quiet-desc")));

    new DatabaseCommand(this);
}

void Controller::showHelp() const
{
    const QStringList args = QCoreApplication::arguments();
    const QString exeName = args.first();

    QTextStream out(stdout, QIODeviceBase::WriteOnly);

    out << qtTrId("statalihcmd-help-usage") << ' ' << exeName << ' ';
    out << qtTrId("statalihcmd-help-usage-global-options") << ' ';
    out << qtTrId("statalihcmd-help-usage-command") << '\n';
    out << '\n';

    //: General description for zmc
    //% "Zumftmeistar is a web management application for clubs and associations based on Cutelyst.\nThis command line client helps to manage several aspects of Zumftmeistar."
    out << qtTrId("statalihcmd-help-description") << '\n';
    out << '\n';

    out << qtTrId("statalihcmd-help-header-global-options") << '\n';

    int longestOptionNameAndValue = 0;
    for (const QCommandLineOption &opt : std::as_const(m_globalOptions)) {
        const QStringList names = opt.names();
        QString longName;
        for (const QString &name : names) {
            if (name.length() > 1) {
                longName = name;
            }
        }
        longestOptionNameAndValue = std::max(longestOptionNameAndValue, static_cast<int>(longName.length() + opt.valueName().length()));
    }

    constexpr int additionalFieldWith = 10;
    for (const QCommandLineOption &opt : std::as_const(m_globalOptions)) {
        QString shortName, longName;
        const QStringList names = opt.names();
        for (const QString &name : names) {
            if (name.length() == 1) {
                shortName = name;
            } else {
                longName = name;
            }
        }

        if (shortName.isEmpty()) {
            shortName = u"    "_s;
        } else {
            shortName.append(", "_L1).prepend('-'_L1);
        }

        QString longNameAndValue;
        if (opt.valueName().isEmpty()) {
            longNameAndValue = "--"_L1 + longName;
        } else {
            longNameAndValue = "--"_L1 + longName + QChar(QChar::Space) + '<'_L1 + opt.valueName() + '>'_L1;
        }

        out << "  " << shortName << qSetFieldWidth(longestOptionNameAndValue + additionalFieldWith) << Qt::left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
    }
    out << '\n';

    out << qtTrId("statalihcmd-help-header-commands") << '\n';

    const QList<Command *> coms = findChildren<Command *>(QString(), Qt::FindDirectChildrenOnly);
    int maxCommandNameLength = 0;
    for (const auto com : coms) {
        maxCommandNameLength = std::max(maxCommandNameLength, static_cast<int>(com->objectName().length()));
    }
    for (const auto com: coms) {
        out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << Qt::left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
    }

    qApp->exit(static_cast<int>(CLI::RC::OK));
}

QList<QCommandLineOption> Controller::globalOptions() const
{
    return m_globalOptions;
}

#include "moc_controller.cpp"
