/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_SETTINGS_H
#define HBNST_SETTINGS_H

#include <QObject>
#include <QTimeZone>

namespace Settings {
Q_NAMESPACE

    enum class StaticPlugin : int {
        None = 0,
        Simple,
        Compressed
    };
    Q_ENUM_NS(StaticPlugin);

    enum class Cache : int {
        None = 0,
        Memcached
    };
    Q_ENUM_NS(Cache);

    void loadSupportedLocales(const QVector<QLocale> &locales);
    bool load(const QVariantMap &core, const QVariantMap &defaults);

    QString siteName();

    StaticPlugin staticPlugin();

    QTimeZone defTimeZone();
    QLocale defLocale();
}

#endif // HBNST_SETTINGS_H
