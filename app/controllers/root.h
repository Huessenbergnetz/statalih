/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_ROOT_H
#define HBNST_ROOT_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class Root final : public Controller
{
    Q_OBJECT
    C_NAMESPACE("")
public:
    explicit Root(QObject *parent = nullptr);
    ~Root() final = default;

    C_ATTR(index, :Path :Args(0))
    void index(Context *c);

    C_ATTR(pageNotFound, :Path)
    void pageNotFound(Context *c);

private:
    C_ATTR(Auto, :Private)
    bool Auto(Context *c);

    Q_DISABLE_COPY(Root)
};

#endif // HBNST_ROOT_H
