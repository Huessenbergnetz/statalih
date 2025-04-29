/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "statalih.h"
#include "confignames.h"
#include "logging.h"
#include "settings.h"

#include "controllers/root.h"

#include <Cutelyst/Engine>
#include <Cutelyst/Plugins/CSRFProtection>
#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Session/sessionstorefile.h>
#include <Cutelyst/Plugins/StaticCompressed/StaticCompressed>
#include <Cutelyst/Plugins/StaticSimple>
#include <Cutelyst/Plugins/StatusMessage>
#include <Cutelyst/Plugins/Utils/LangSelect>
#include <Cutelyst/Plugins/Utils/Validator>
#include <CutelystForms/forms.h>

#include <asql-qt6/ASql/apool.h>
#include <asql-qt6/ASql/adatabase.h>
#include <asql-qt6/ASql/apg.h>
#include <asql-qt6/ASql/aresult.h>

#include <QCoreApplication>
#include <QUrl>
#include <QUrlQuery>

using namespace Qt::StringLiterals;

#if defined(QT_DEBUG)
Q_LOGGING_CATEGORY(HBNST_CORE, "hbnst.core")
#else
Q_LOGGING_CATEGORY(HBNST_CORE, "hbnst.core", QtInfoMsg)
#endif

Statalih::Statalih(QObject *parent)
    : Application{parent}
{
    QCoreApplication::setApplicationName(u"Statalih"_s);
    QCoreApplication::setApplicationVersion(QStringLiteral(HBNST_VERSION));
}

bool Statalih::init()
{
    const auto supportedLocales = loadTranslationsFromDir(u"statalih"_s, QStringLiteral(HBNST_TRANSLATIONSDIR));

    Settings::loadSupportedLocales(supportedLocales);

    if (Q_UNLIKELY(!Settings::load(engine()->config(QStringLiteral(HBNST_CONF_CORE)),
                                   engine()->config(QStringLiteral(HBNST_CONF_DEFAULTS))))) {
        return false;
    }

#if defined(QT_DEBUG)
    constexpr bool viewCache{false};
#else
    constexpr bool viewCache{true};
#endif

    new Root(this);

    qCDebug(HBNST_CORE) << "Static plugin:" << Settings::staticPlugin();
    if (Settings::staticPlugin() != Settings::StaticPlugin::None) {
        const QStringList statIncPaths; // TODO
        const QStringList staticDirs; // TODO
        if (Settings::staticPlugin() == Settings::StaticPlugin::Simple) {
            auto statPlugin = new StaticSimple(this); // NOLINT(cppcoreguidelines-owning-memory)
            statPlugin->setIncludePaths(statIncPaths);
            statPlugin->setDirs(staticDirs);
            statPlugin->setServeDirsOnly(true);
        } else if (Settings::staticPlugin() == Settings::StaticPlugin::Compressed) {
            auto statPlugin = new StaticCompressed(this); // NOLINT(cppcoreguidelines-owning-memory)
            statPlugin->setIncludePaths(statIncPaths);
            statPlugin->setDirs(staticDirs);
            statPlugin->setServeDirsOnly(true);
        }
    }

    auto sess = new Session(this); // NOLINT(cppcoreguidelines-owning-memory)
    sess->setStorage(std::make_unique<SessionStoreFile>(sess));

    auto lsp = new LangSelect(this, LangSelect::Session); // NOLINT(cppcoreguidelines-owning-memory)
    lsp->setFallbackLocale(Settings::defLocale());
    lsp->setSupportedLocales(supportedLocales);
    lsp->setSessionKey(u"locale"_s);

    auto csrf = new CSRFProtection(this); // NOLINT(cppcoreguidelines-owning-memory)
    csrf->setCookieName("hbnst_csrftoken");

    new StatusMessage(this);

    auto forms = new CutelystForms::Forms(this); // NOLINT(cppcoreguidelines-owning-memory)
    forms->setIncludePaths({QStringLiteral(HBNST_FORMSDIR)});

    Validator::loadTranslations(this);

    return true;
}

bool Statalih::postFork()
{
    return initDb();
}

bool Statalih::initDb() const
{
    QUrl dbUrl{engine()->config(QStringLiteral(HBNST_CONF_CORE)).value(QStringLiteral(HBNST_CONF_CORE_DATABASE), QStringLiteral(HBNST_CONF_CORE_DATABASE_DEFVAL)).toString()};
    if (Q_UNLIKELY(!dbUrl.isValid())) {
        qCCritical(HBNST_CORE) << "Invalid database connection info URL";
        return false;
    }

    if (dbUrl.scheme().compare("postgres"_L1) == 0 || dbUrl.scheme().compare("postgresql"_L1) == 0) {

        QUrlQuery dbUrlQuery;
        dbUrlQuery.addQueryItem(u"application_name"_s, QCoreApplication::applicationName());
        dbUrlQuery.addQueryItem(u"target_session_attrs"_s, u"read-write"_s);

        dbUrl.setQuery(dbUrlQuery);

        ASql::APool::create(ASql::APg::factory(dbUrl));
        ASql::APool::setMaxIdleConnections(10);
        ASql::APool::setSetupCallback([](ASql::ADatabase db) {
            db.exec(u"SET TIMEZONE 'UTC'"_s, nullptr, [](ASql::AResult &result) {
                if (result.hasError()) {
                    qCWarning(HBNST_CORE) << "Failed to set database connection time zone to UTC:" << result.errorString();
                }
            });
        });

    } else {
        qCCritical(HBNST_CORE) << "Not supported database type:" << dbUrl.scheme();
        return false;
    }

    return true;
}
