/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_CONFIGURATION_H
#define HBNST_CONFIGURATION_H

#include "cli.h"

#include <QVariantMap>

/*!
 * \brief Base class to access configuration options.
 */
class Configuration : public CLI
{
    Q_OBJECT
    Q_DISABLE_COPY(Configuration)
public:
    /*!
     * \brief Constructs a new %Configuration object with the given \a parent.
     */
    explicit Configuration(QObject *parent = nullptr);

    /*!
     * \brief Destroys the %Configuration object.
     */
    ~Configuration() override = default;

    /*!
     * \brief Returns the value for the config option stored in \a group and identified by \a key.
     *
     * If there could be nothing found, the \a defaultValue will be returned.
     */
    [[nodiscard]] QVariant value(const QString &group, const QString &key, const QVariant &defaultValue = QVariant()) const;

protected:
    /*!
     * \brief Loads the configuration INI file from \a iniPath and return CLI::RC:OK on success.
     */
    CLI::RC loadConfig(const QString &iniPath = QString());

    /*!
     * \brief Sets the path to the INI configuration file to \a iniPath.
     */
    void setIniPath(const QString &iniPath);

private:
    QString m_iniPath;
    QVariantMap m_config;
    bool m_loaded = false;
};

#endif // HBNST_CONFIGURATION_H
