/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controller.h"

#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QTimer>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(u"statalih"_s);
    app.setOrganizationDomain(u"huessenbergnetz.de"_s);
    app.setOrganizationName(u"Huessenbergnetz"_s);
    app.setApplicationVersion(QStringLiteral(HBNST_VERSION));

    {
        QLocale locale;
        auto trans = new QTranslator(&app); // NOLINT(cppcoreguidelines-owning-memory)
        if (Q_LIKELY(trans->load(locale, u"statalihcmd"_s, u"."_s, QStringLiteral(HBNST_TRANSLATIONSDIR)))) {
            if (Q_UNLIKELY(!app.installTranslator(trans))) {
                qWarning() << "Failed to install translator for" << locale;
            }
        } else {
            qWarning() << "Failed to load translations for" << locale << "from" << HBNST_TRANSLATIONSDIR;
        }
    }

    auto c = new Controller(&app); // NOLINT(cppcoreguidelines-owning-memory)
    QTimer::singleShot(0, c, &Controller::exec);

    return app.exec();
}
