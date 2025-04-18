/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0001_CREATEPLACESTABLE_H
#define M0001_CREATEPLACESTABLE_H

#include <Firfuorida/Migration>

class M0001_CreatePlacesTable final : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0001_CreatePlacesTable)
public:
    explicit M0001_CreatePlacesTable(Firfuorida::Migrator *parent);
    ~M0001_CreatePlacesTable() override = default;

    void up() final;
    void down() final;
};

#endif // M0001_CREATEPLACESTABLE_H
