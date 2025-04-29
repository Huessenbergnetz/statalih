/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_STATALIH_H
#define HBNST_STATALIH_H

#include <Cutelyst/Application>

using namespace Cutelyst;

class Statalih final : public Application
{
    Q_OBJECT
    CUTELYST_APPLICATION(IID "Statalih")
public:
    Q_INVOKABLE explicit Statalih(QObject *parent = nullptr);
    ~Statalih() final = default;

    bool init() final;

    bool postFork() final;

private:
    [[nodiscard]] bool initDb() const;

    Q_DISABLE_COPY(Statalih)
};

#endif // HBNST_STATLIH_H
