/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_CLI_H
#define HBNST_CLI_H

#include <QObject>
#include <QString>
#include <QStringList>

class QSqlError;
class QSqlQuery;
class QSqlDatabase;

/*!
 * \brief Basic class for command line interface operations.
 */
class CLI : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructs a new %CLI object with given \a parent.
     */
    explicit CLI(QObject *parent = nullptr);

    /*!
     * \brief Destroys the %CLI object.
     */
    ~CLI() override = default;

    /*!
     * \brief Return code values for command line operations.
     */
    enum class RC : int {
        OK = 0,             /**< Everything is fine */
        InvalidOption = 1,  /**< An invalid option has been given */
        InputError = 2,     /**< An input error has occured */
        ConfigError = 3,    /**< Invalid configuration */
        FileError = 4,      /**< Error while accessing a file */
        DbError = 5,        /**< Database error */
        InternalError =6    /**< Internal error */
    };

protected:
    /*!
     * \brief Prints the \a error message to to \c stderr, closes with a new line.
     */
    void printError(const QString &error) const;

    /*!
     * \brief Prints all \a errors to \c stderr, closes every message with a new line.
     */
    void printError(const QStringList &errors) const;

    /*!
     * \brief Prints the \a error to \c stderr and returns the error \a code.
     */
    [[nodiscard]] RC error(const QString &error, RC code) const;

    /*!
     * \brief Prints the \a error to \c stderr and returns the error code for an input error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC inputError(const QString &error) const;

    /*!
     * \brief Prints the \a error to \c stderr and returns the error code for a configuration error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC configError(const QString &error) const;

    /*!
     * \brief Prints the \a error to \c stderr and returns the error code for a file access error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC fileError(const QString &error) const;

    /*!
     * \brief Prints the \a error to \c stderr and returns the error code for a database error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC dbError(const QString &error) const;

    /*!
     * \brief Prints the text of the SQL \a error to \c stderr and returns the error code for a database error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC dbError(const QSqlError &error) const;

    /*!
     * \brief Prints the error text from the SQL \a query to \c stderr and returns the error code for a database error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC dbError(const QSqlQuery &query) const;

    /*!
     * \brief Prints the error text from the SQL \a db to \c stderr and returns the error code for a database error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC dbError(const QSqlDatabase &db) const;

    /*!
     * \brief Prints the \a error to \c stderr and return the error code for an internal error.
     * \sa error(), printError()
     */
    [[nodiscard]] RC internalError(const QString &error) const;

    /*!
     * \brief Prints the \a warning to \c stdout and closes with a new line.
     */
    void printWarning(const QString &warning) const;

    /*!
     * \brief Prints the \a status with a width of 100 to \c stdout.
     *
     * This will not add a new line. Use it together with printDone() and printFailed().
     */
    void printStatus(const QString &status) const;

    /*!
     * \brief Prints “Done” to \c stdout and closes with a new line.
     *
     * Use this together with printStatus().
     */
    void printDone() const;

    /*!
     * \brief Prints the \a done message to \c stdout and closes with a new line.
     *
     * Use this together with printStatus()
     */
    void printDone(const QString &done) const;

    /*!
     * \brief Prints “Failed” to \c stdout and closes with a new line.
     */
    void printFailed() const;

    /*!
     * \brief Prints the \a failed message to \c stdout and closes with a new line.
     */
    void printFailed(const QString &failed) const;

    /*!
     * \brief Prints a generic \a message to \c stdout and closes with a new line.
     */
    void printMessage(const QByteArray &message) const;
    void printMessage(const QString &message) const;

    /*!
     * \brief Prints a success \a message to \c stdout and closes with a new line.
     */
    void printSuccess(const QString &message) const;

    /*!
     * \brief Prints the \a description to \c stdout and breaks it at 95 chars into a new line.
     */
    void printDesc(const QString &description) const;

    /*!
     * \brief Prints the \a descriptions to \c stdout and breakt them at 95 char into new lines.
     */
    void printDesc(const QStringList &description) const;

    /*!
     * \brief Prints a table with the given \a headers and \a data to \c stdout.
     *
     * \a headers and \a data are not allowed to be empty. The size of \a headers has
     * to fit the size of the list elements in \a data.
     */
    void printTable(const QStringList &headers, const QList<QStringList> &data) const;

    /*!
     * \brief Set \a quiet to \c true tu suppress output.
     */
    void setQuiet(bool quiet);

private:
    bool m_quiet{false};

    static constexpr qsizetype terminalWidth{95};

    Q_DISABLE_COPY(CLI)
};

#endif // HBNST_CLI_H
