/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0003_CREATEITEMSTABLE_H
#define M0003_CREATEITEMSTABLE_H

#include <Firfuorida/Migration>

class M0003_CreateItemsTable final : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0003_CreateItemsTable)
public:
    explicit M0003_CreateItemsTable(Firfuorida::Migrator *parent);
    ~M0003_CreateItemsTable() override = default;

    void up() final;
    void down() final;
};

#endif // M0003_CREATEITEMSTABLE_H
