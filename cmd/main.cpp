/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "controller.h"

#include <QCoreApplication>
#include <QLocale>
#include <QLoggingCategory>
#include <QTranslator>
#include <QTimer>

extern "C" {
#ifdef WITH_SYSTEMD
#include <systemd/sd-journal.h>
#endif
}

using namespace Qt::Literals::StringLiterals;

Q_LOGGING_CATEGORY(ST_CORE, "statalih.core");

#ifdef WITH_SYSTEMD
void journaldMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    int prio = LOG_INFO;
    switch(type) {
    case QtDebugMsg:
        prio = LOG_DEBUG;
        break;
    case QtInfoMsg:
        prio = LOG_INFO;
        break;
    case QtWarningMsg:
        prio = LOG_WARNING;
        break;
    case QtCriticalMsg:
        prio = LOG_CRIT;
        break;
    case QtFatalMsg:
        prio = LOG_ALERT;
        break;
    }

#ifdef QT_DEBUG
    sd_journal_send("PRIORITY=%i", prio, "SYSLOG_FACILITY=%hhu", 1, "SYSLOG_IDENTIFIER=%s", context.category, "SYSLOG_PID=%lli", QCoreApplication::applicationPid(), "MESSAGE=%s", qFormatLogMessage(type, context, msg).toUtf8().constData(), "CODE_FILE=%s", context.file, "CODE_LINE=%i", context.line, "CODE_FUNC=%s", context.function, NULL);
#else
    sd_journal_send("PRIORITY=%i", prio, "SYSLOG_FACILITY=%hhu", 1, "SYSLOG_IDENTIFIER=%s", context.category, "SYSLOG_PID=%lli", QCoreApplication::applicationPid(), "MESSAGE=%s", qFormatLogMessage(type, context, msg).toUtf8().constData(), NULL);
#endif

    if (prio == 0) {
        abort();
    }
}
#endif

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

    qSetMessagePattern(u"%{message}"_s);
    qInstallMessageHandler(journaldMessageOutput);

    auto c = new Controller(&app); // NOLINT(cppcoreguidelines-owning-memory)
    QTimer::singleShot(0, c, &Controller::exec);

    return app.exec();
}
