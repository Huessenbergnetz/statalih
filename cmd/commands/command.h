/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_COMMAND_H
#define HBNZMC_COMMAND_H

#include <database.h>
#include <QCommandLineOption>

class QCommandLineParser;
class QTextStream;

/*!
 * \brief Base class for all %CLI commands.
 */
class Command : public Database
{
    Q_OBJECT
public:
    /*!
     * \brief Constructs a new %Command object with the given \a parent.
     */
    explicit Command(QObject *parent = nullptr);

    ~Command() override = default;

    virtual void exec(QCommandLineParser *parser) = 0;

    [[nodiscard]] virtual QString summary() const = 0;

    [[nodiscard]] virtual QString description() const = 0;

    void showHelp() const;

protected:
    void showGlobalOptions(QTextStream *out) const;
    void showUsage(QTextStream *out) const;
    void showSubCommands(QTextStream *out) const;
    void showOptions(QTextStream *out) const;
    void setGlobalOptions(QCommandLineParser *parser);
    bool checkShowHelp(QCommandLineParser *parser) const;
    void runSubCommand(const QString &command, QCommandLineParser *parser);
    void showInvalidCommand(const QString &command) const;
    void exit(CLI::RC rc) const;
    void addOutputFormatOption();

    QList<QCommandLineOption> m_cliOptions;

private:
    [[nodiscard]] int getLongestOptionName(const QList<QCommandLineOption> &options) const;
    void showOptions(QTextStream *out, const QList<QCommandLineOption> &options) const;

    Q_DISABLE_COPY(Command)
};

#endif // HBNZMC_COMMAND_H
