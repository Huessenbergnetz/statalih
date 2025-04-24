/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "command.h"
#include "controller.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <QLocale>
#include <algorithm>

using namespace Qt::Literals::StringLiterals;

Command::Command(QObject *parent)
    : Database{parent}
{

}

void Command::showHelp() const
{
    QTextStream out(stdout, QIODeviceBase::WriteOnly);

    showUsage(&out);

    out << description() << '\n';
    out << '\n';

    showGlobalOptions(&out);
    showSubCommands(&out);
    showOptions(&out);
}

void Command::showGlobalOptions(QTextStream *out) const
{
    Controller *c = nullptr;
    auto p = parent();
    while (p) {
        if (p->objectName() == u"controller") {
            c = qobject_cast<Controller*>(p);
            break;
        } else {
            p = p->parent();
        }
    }

    const QList<QCommandLineOption> options = c ? c->globalOptions() : QList<QCommandLineOption>();

    if (!options.empty()) {
        //% "Global options:"
        *out << qtTrId("statalihcmd-help-header-global-options") << '\n';
        showOptions(out, options);
        *out << '\n';
    }
}

void Command::showUsage(QTextStream *out) const
{
    const QStringList args = QCoreApplication::arguments();
    const QString exeName = args.first();

    //% "Usage:"
    *out << qtTrId("statalihcmd-help-usage") << ' ' << exeName << ' ';
    //% "[global options]"
    *out << qtTrId("statalihcmd-help-usage-global-options");
    QStringList commands;
    QObject *p = parent();
    while (p->inherits("Command")) {
        commands << p->objectName();
        p = p->parent();
    }

    std::reverse(commands.begin(), commands.end());

    commands << objectName();

    *out << ' ' << commands.join(QChar(QChar::Space));

    if (!findChildren<Command*>(QString(), Qt::FindDirectChildrenOnly).empty()) {
        //% "<command>"
        *out << ' ' << qtTrId("statalihcmd-help-usage-command");
    }

    if (!m_cliOptions.empty()) {
        //% "[options]"
        *out << ' ' << qtTrId("statalihcmd-help-usage-options");
    }

    *out << '\n';
    *out << '\n';
}

void Command::showSubCommands(QTextStream *out) const
{
    const QList<Command *> coms = findChildren<Command *>(QString(), Qt::FindDirectChildrenOnly);
    if (!coms.empty()) {
        //: CLI help header
        //% "Commands:"
        *out << qtTrId("statalihcmd-help-header-commands") << '\n';

        int maxCommandNameLength = 0;
        for (const auto com : coms) {
            maxCommandNameLength = std::max(maxCommandNameLength, static_cast<int>(com->objectName().length()));
        }
        for (const auto com : coms) {
            *out << "  " << qSetFieldWidth(maxCommandNameLength + 4) << Qt::left << com->objectName() << qSetFieldWidth(0) << " " << com->summary() << '\n';
        }
        *out << '\n';
    }
}

void Command::showOptions(QTextStream *out) const
{
    if (!m_cliOptions.empty()) {
        //: CLI help header
        //% "Options:"
        *out << qtTrId("statalihcmd-help-header-options") << '\n';
        showOptions(out, m_cliOptions);
        *out << '\n';
    }
}

void Command::setGlobalOptions(QCommandLineParser *parser)
{
    setIniPath(parser->value(u"ini"_s));
    setQuiet(parser->isSet(u"quiet"_s));
}

bool Command::checkShowHelp(QCommandLineParser *parser) const
{
    if (parser->isSet(u"help"_s) || parser->isSet(u"h"_s)) {
        showHelp();
        return true;
    } else {
        return false;
    }
}

int Command::getLongestOptionName(const QList<QCommandLineOption> &options) const
{
    int longest = 0;

    for (const QCommandLineOption &opt : options) {
        const QStringList names = opt.names();
        QString longName;
        for (const QString &name : names) {
            if (name.length() > 1) {
                longName = name;
            }
        }
        longest = std::max(longest, static_cast<int>(longName.length() + opt.valueName().length()));
    }

    return longest;
}

void Command::showOptions(QTextStream *out, const QList<QCommandLineOption> &options) const
{
    int longestLongNameAndValue = getLongestOptionName(options);

    constexpr int additionalFieldWith = 10;
    for (const QCommandLineOption &opt : options) {
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

        *out << "  " << shortName << qSetFieldWidth(longestLongNameAndValue + additionalFieldWith) << Qt::left << longNameAndValue << qSetFieldWidth(0) << opt.description() << '\n';
    }
}

void Command::runSubCommand(const QString &command, QCommandLineParser *parser)
{
    auto com = findChild<Command *>(command, Qt::FindDirectChildrenOnly);

    if (com) {
        com->exec(parser);
    } else {
        if (checkShowHelp(parser)) {
            exit(CLI::RC::OK);
        } else {
            showInvalidCommand(command);
        }
    }
}

void Command::showInvalidCommand(const QString &command) const
{
    // QTextStream out(stdout, QIODeviceBase::WriteOnly);
    // out << qtTrId("statalihcmd-invalid-command").arg(command) << '\n';
    showHelp();
    exit(CLI::RC::InvalidOption);
}

void Command::exit(CLI::RC rc) const
{
    qApp->exit(static_cast<int>(rc));
}

void Command::addOutputFormatOption()
{
    QLocale locale;
    const QStringList formats({u"table"_s, u"json"_s, u"json-pretty"_s});
    m_cliOptions.emplace_back(QStringList({u"f"_s, u"format"_s}),
                                  //: CLI option description
                                  //% "Render output in a particular format. Available: %1. Default: %2."
                                  qtTrId("statalihcmd-opt-format-desc").arg(locale.createSeparatedList(formats), formats.first()),
                                  //: CLI option value name
                                  //% "format"
                                  qtTrId("statalihcmd-opt-format-value"),
                                  formats.first());
}


#include "moc_command.cpp"
