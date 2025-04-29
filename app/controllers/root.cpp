/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "root.h"
#include "settings.h"

#include <Cutelyst/Plugins/StatusMessage>

using namespace Qt::StringLiterals;

Root::Root(QObject *parent)
    : Controller{parent}
{
}

void Root::index(Context *c)
{
    c->res()->setBody(u"Hello World!"_s);
    c->res()->setContentType("text/plain; charset=utf-8");
}

void Root::pageNotFound(Context *c)
{
    c->res()->setBody(u"Page not found!");
    c->res()->setContentType("text/plain; charset=utf-8"_ba);
    c->res()->setStatus(Response::NotFound);
}

bool Root::Auto(Context *c)
{
    StatusMessage::load(c);

    c->stash({
                 {u"site_name"_s, Settings::siteName()},
                 {u"default_timezone"_s, QString::fromLatin1(Settings::defTimeZone().id())}
             });

    return true;
}
