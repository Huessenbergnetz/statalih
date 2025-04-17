/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "cli.h"
#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTextStream>
#include <QNetworkReply>
#include <cstdio>
#include <iostream>

using namespace Qt::Literals::StringLiterals;

CLI::CLI(QObject *parent) : QObject(parent)
{

}

void CLI::printError(const QString &error) const
{
    if (!m_quiet) {
        std::fprintf(stderr, "\x1b[31m%s\x1b[0m\n", qUtf8Printable(error)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printError(const QStringList &errors) const
{
    if (!m_quiet && !errors.empty()) {
        for (const QString &error : errors) {
            printError(error);
        }
    }
}

CLI::RC CLI::error(const QString &error, CLI::RC code) const
{
    printError(error);
    return code;
}

CLI::RC CLI::inputError(const QString &error) const
{
    return this->error(error, RC::InputError);
}

CLI::RC CLI::configError(const QString &error) const
{
    return this->error(error, RC::ConfigError);
}

CLI::RC CLI::fileError(const QString &error) const
{
    return this->error(error, RC::FileError);
}

CLI::RC CLI::dbError(const QString &error) const
{
    return this->error(error, RC::DbError);
}

CLI::RC CLI::dbError(const QSqlError &error) const
{
    return dbError(error.text());
}

CLI::RC CLI::dbError(const QSqlQuery &query) const
{
    return dbError(query.lastError());
}

CLI::RC CLI::dbError(const QSqlDatabase &db) const
{
    return dbError(db.lastError());
}

CLI::RC CLI::parsingError(const QString &error) const
{
    return this->error(error, RC::ParsingError);
}

CLI::RC CLI::networkError(const QString &error) const
{
    return this->error(error, RC::NetworkError);
}

CLI::RC CLI::networkError(QNetworkReply *reply) const
{
    return networkError(reply->errorString());
}

CLI::RC CLI::internalError(const QString &error) const
{
    return this->error(error, RC::InternalError);
}

void CLI::printWarning(const QString &warning) const
{
    if (!m_quiet) {
        std::printf("\x1b[33m%s\x1b[0m\n", qUtf8Printable(warning)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printStatus(const QString &status) const
{
    if (!m_quiet) {
        std::printf("%-100s", status.toUtf8().constData()); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printDone() const
{
    //% "Done"
    printDone(qtTrId("stcmd-msg-done"));
}

void CLI::printDone(const QString &done) const
{
    if (!m_quiet) {
        std::printf("\x1b[32m%s\x1b[0m\n", qUtf8Printable(done)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printFailed() const
{
    //% "Failed"
    printFailed(qtTrId("stcmd-msg-failed"));
}

void CLI::printFailed(const QString &failed) const
{
    if (!m_quiet) {
        printf("\x1b[31m%s\x1b[0m\n", qUtf8Printable(failed)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printMessage(const QByteArray &message) const
{
    if (!m_quiet) {
        std::cout << message.constData() << "\n";
    }
}

void CLI::printMessage(const QString &message) const
{
    if (!m_quiet) {
        printMessage(message.toUtf8());
    }
}

void CLI::printSuccess(const QString &message) const
{
    if (!m_quiet) {
        std::printf("\x1b[32m%s\x1b[0m\n", qUtf8Printable(message)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
}

void CLI::printDesc(const QString &description) const
{
    if (!m_quiet) {
        if (description.length() <= CLI::terminalWidth) {
            std::cout << "# " << qUtf8Printable(description) << "\n";
        } else {
            const QStringList parts = description.split(QChar(QChar::Space));
            QStringList out;
            qsizetype outSize = 0;
            for (const QString &part : parts) {
                if ((outSize + part.length() + 1) <= CLI::terminalWidth) {
                    out << part;
                    outSize += (part.length() + 1);
                } else {
                    printDesc(out.join(QChar(QChar::Space)));
                    out.clear();
                    out << part;
                    outSize = (part.size() + 1);
                }
            }
            if (!out.empty()) {
                printDesc(out.join(QChar(QChar::Space)));
            }
        }
    }
}

void CLI::printDesc(const QStringList &description) const
{
    if (!m_quiet && !description.empty()) {
        for (const QString &desc : description) {
            printDesc(desc);
        }
    }
}

void printTableSeparator(QTextStream &stream, const QList<qsizetype> &cellWidth)
{
    stream << '+';
    for (qsizetype width : cellWidth) {
        stream << QString(width + 2, '-'_L1) << '+';
    }
    stream << '\n';
}

void printTableRow(QTextStream &stream, const QList<qsizetype> &cellWidth, const QStringList &data)
{
    stream << '|';

    const qsizetype cols = cellWidth.size();

    for (qsizetype i = 0; i < cols; ++i) {
        stream << ' ' << qSetFieldWidth(static_cast<int>(cellWidth[i] + 1)) << Qt::left << data[i] << qSetFieldWidth(0) << '|';
    }

    stream << '\n';
}

void CLI::printTable(const QStringList &headers, const QList<QStringList> &data) const
{
    Q_ASSERT_X(!headers.empty(), "CLI::printTable", "headers can can not be empty");
    Q_ASSERT_X(!data.empty(), "CLI::printTable", "data can not be empty");

    const qsizetype cols = headers.size();

    QList<qsizetype> cellWidth(cols, 0);

    for (const QStringList &list : data) {
        Q_ASSERT_X(cols == list.size(), "CLI::printTable", "header count and row count are not the same");
        for (qsizetype i = 0; i < cols; ++i) {
            cellWidth[i] = std::max(cellWidth[i], headers.at(i).size());
            cellWidth[i] = std::max(cellWidth[i], list.at(i).size());
        }
    }

    QTextStream out(stdout, QIODeviceBase::WriteOnly);
    printTableSeparator(out, cellWidth);
    printTableRow(out, cellWidth, headers);
    printTableSeparator(out, cellWidth);
    for (const QStringList &list : data) {
        printTableRow(out, cellWidth, list);
    }
    printTableSeparator(out, cellWidth);
}

void CLI::setQuiet(bool quiet)
{
    m_quiet = quiet;
}

#include "moc_cli.cpp"
