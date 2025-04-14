/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "configuration.h"

#include <QSettings>

Configuration::Configuration(QObject *parent)
    : CLI{parent}
{

}

QVariant Configuration::value(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    Q_ASSERT_X(m_loaded, "get config value", "configuration not loaded");

    const auto map = m_config.value(group).toMap();

    return map.value(key, defaultValue);
}

CLI::RC Configuration::loadConfig(const QString &iniPath)
{
    //% "Reading configuration file"
    printStatus(qtTrId("stcmd-status-reading-config"));

    if (!iniPath.isEmpty()) {
        m_iniPath = iniPath;
    }

    QSettings conf(m_iniPath, QSettings::IniFormat);

    switch(conf.status()) {
    case QSettings::NoError:
        break;
    case QSettings::AccessError:
    {
        printFailed();
        //: Error message, %1 will be replaced by the file path
        //% "Can not read configuration file at %1"
        return fileError(qtTrId("stcmd-error-config-file-not-readable").arg(m_iniPath));
    }
    case QSettings::FormatError:
    {
        printFailed();
        //: Error message, %1 will be replaced by the file path
        //% "Failed to parse configuration file at %1"
        return configError(qtTrId("stcmd-error-config-file-malformed").arg(m_iniPath));
    }
    }

    const QStringList confGroups = conf.childGroups();

    for (const QString &group : confGroups) {
        auto map = m_config.value(group, QVariantMap()).toMap();
        conf.beginGroup(group);
        const auto keys = conf.childKeys();
        for (const QString &key : keys) {
            map.insert(key, conf.value(key));
        }
        conf.endGroup();
        m_config.insert(group, map);
    }

    printDone();

    m_loaded = true;

    return RC::OK;
}

void Configuration::setIniPath(const QString &iniPath)
{
    m_iniPath = iniPath;
}

#include "moc_configuration.cpp"
