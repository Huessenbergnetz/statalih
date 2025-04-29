/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "settings.h"
#include "logging.h"
#include "confignames.h"

#include <QFileInfo>
#include <QGlobalStatic>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QWriteLocker>

using namespace Qt::StringLiterals;

#if defined(QT_DEBUG)
Q_LOGGING_CATEGORY(HBNST_SETTINGS, "hbnst.settings")
#else
Q_LOGGING_CATEGORY(HBNST_SETTINGS, "hbnst.settings", QtInfoMsg)
#endif

struct SettingsVals {
    mutable QReadWriteLock lock{QReadWriteLock::Recursive};

    QTimeZone defTimeZone{HBNST_CONF_DEFAULTS_TZ_DEFVAL};
    QVector<QLocale> supportedLocales;
    QStringList supportedLocaleNames;

    QString siteName = QStringLiteral(HBNST_CONF_CORE_SITENAME_DEFVAL);
    QString tmpl = QStringLiteral(HBNST_CONF_CORE_TEMPLATE_DEFVAL);
    QString tmplDir = QStringLiteral(HBNST_TEMPLATESDIR);

    QLocale defLocale{QStringLiteral(HBNST_CONF_DEFAULTS_LOCALE_DEFVAL)};

    Settings::StaticPlugin staticPlugin{Settings::StaticPlugin::Simple};

    bool loaded{false};
    bool localesLoaded{false};
};

Q_GLOBAL_STATIC(SettingsVals, cfg) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void Settings::loadSupportedLocales(const QVector<QLocale> &locales)
{
    QWriteLocker locker{&cfg->lock};

    if (cfg->localesLoaded) {
        return;
    }

    qCDebug(HBNST_SETTINGS) << "Loading supported locales";

    cfg->supportedLocales = locales;

    cfg->supportedLocaleNames.reserve(locales.size());
    for (const QLocale &locale : locales) {
        cfg->supportedLocaleNames << locale.name();
    }

    cfg->localesLoaded = true;
}

bool loadCore(const QVariantMap &core)
{
    cfg->siteName = core.value(QStringLiteral(HBNST_CONF_CORE_SITENAME),
                               QStringLiteral(HBNST_CONF_CORE_SITENAME_DEFVAL)).toString();

    cfg->tmpl = core.value(QStringLiteral(HBNST_CONF_CORE_TEMPLATE),
                           QStringLiteral(HBNST_CONF_CORE_TEMPLATE_DEFVAL)).toString();
    if (cfg->tmpl.startsWith('/'_L1)) {
        auto fullPathParts = cfg->tmpl.split('/'_L1, Qt::SkipEmptyParts);
        cfg->tmpl = fullPathParts.takeLast();
        cfg->tmplDir = '/'_L1 + fullPathParts.join('/'_L1);
    }

    const QFileInfo tmplDirFi{cfg->tmplDir + '/'_L1 + cfg->tmpl};
    if (Q_UNLIKELY(!tmplDirFi.exists())) {
        qCCritical(HBNST_SETTINGS)
                << "Template directory" << tmplDirFi.absoluteFilePath() << "does not exist";
        return false;
    }

    if (Q_UNLIKELY(!tmplDirFi.isDir())) {
        qCCritical(HBNST_SETTINGS)
                << "Template directory path" << tmplDirFi.absoluteFilePath() << "does not point to a directory";
        return false;
    }

    const QString _statPlugin = core.value(QStringLiteral(HBNST_CONF_CORE_STATICPLUGIN),
                                           QStringLiteral(HBNST_CONF_CORE_STATICPLUGIN_DEFVAL)).toString();
    if (_statPlugin.compare("none"_L1, Qt::CaseInsensitive) == 0) {
        cfg->staticPlugin = Settings::StaticPlugin::None;
    } else if (_statPlugin.compare("simple"_L1, Qt::CaseInsensitive) == 0) {
        cfg->staticPlugin = Settings::StaticPlugin::Simple;
    } else if (_statPlugin.compare("compressed"_L1, Qt::CaseInsensitive) == 0) {
        cfg->staticPlugin = Settings::StaticPlugin::Compressed;
    } else {
        qCWarning(HBNST_SETTINGS)
                << "Invalid value for" << HBNST_CONF_CORE_STATICPLUGIN << "in section"
                << HBNST_CONF_CORE << ", using default value:" << HBNST_CONF_CORE_STATICPLUGIN_DEFVAL;
    }

    return true;
}

bool loadDefaults(const QVariantMap &defaults)
{
    if (!cfg->localesLoaded) {
        qCCritical(HBNST_SETTINGS)
                << "Can not load defaults while supported languages are not loaded";
        return false;
    }

    const QTimeZone tz{defaults.value(QStringLiteral(HBNST_CONF_DEFAULTS_TZ),
                                      QStringLiteral(HBNST_CONF_DEFAULTS_TZ_DEFVAL)).toString().toLatin1()};
    if (tz.isValid()) {
        cfg->defTimeZone = tz;
    } else {
        qCWarning(HBNST_SETTINGS)
                << "Invalid value for" << HBNST_CONF_DEFAULTS_TZ << "in section"
                << HBNST_CONF_DEFAULTS << ", using default value:" << HBNST_CONF_DEFAULTS_TZ_DEFVAL;
    }

    QLocale defLocale{defaults.value(QStringLiteral(HBNST_CONF_DEFAULTS_LOCALE),
                                     QStringLiteral(HBNST_CONF_DEFAULTS_LOCALE_DEFVAL)).toString()};
    if (defLocale == QLocale::c()) {
        qCWarning(HBNST_SETTINGS)
                << "Invalid value for" << HBNST_CONF_DEFAULTS_LOCALE << "in section"
                << HBNST_CONF_DEFAULTS << ", using default value:" << HBNST_CONF_DEFAULTS_LOCALE_DEFVAL;
        defLocale = QLocale{QStringLiteral(HBNST_CONF_DEFAULTS_LOCALE_DEFVAL)};
    }

    bool foundLocale{false};
    for (const QLocale &l : std::as_const(cfg->supportedLocales)) {
        if (defLocale == l) {
            foundLocale = true;
            break;
        }
    }

    if (foundLocale) {
        cfg->defLocale = defLocale;
    } else {
        qCWarning(HBNST_SETTINGS)
                << defLocale << "set to" << HBNST_CONF_DEFAULTS_LOCALE << "in section" << HBNST_CONF_DEFAULTS
                << "is not supported, using default value:" << HBNST_CONF_DEFAULTS_LOCALE_DEFVAL;
        cfg->defLocale = QLocale{QStringLiteral(HBNST_CONF_DEFAULTS_LOCALE_DEFVAL)};
    }

    return true;
}

bool Settings::load(const QVariantMap &core, const QVariantMap &defaults)
{
    QWriteLocker locker(&cfg->lock);

    if (cfg->loaded) {
        return true;
    }

    qCDebug(HBNST_SETTINGS) << "Loading settings";

    if (!loadCore(core)) {
        return false;
    }

    if (!loadDefaults(defaults)) {
        return false;
    }

    cfg->loaded = true;

    return true;
}

QString Settings::siteName()
{
    QReadLocker locker{&cfg->lock};
    return cfg->siteName;
}

Settings::StaticPlugin Settings::staticPlugin()
{
    QReadLocker locker{&cfg->lock};
    return cfg->staticPlugin;
}

QTimeZone Settings::defTimeZone()
{
    QReadLocker locker{&cfg->lock};
    return cfg->defTimeZone;
}

QLocale Settings::defLocale()
{
    QReadLocker locker{&cfg->lock};
    return cfg->defLocale;
}
