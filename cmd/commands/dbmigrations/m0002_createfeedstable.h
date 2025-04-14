/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef M0002_CREATEFEEDSTABLE_H
#define M0002_CREATEFEEDSTABLE_H

#include <Firfuorida/Migration>

class M0002_CreateFeedsTable final : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M0002_CreateFeedsTable)
public:
    explicit M0002_CreateFeedsTable(Firfuorida::Migrator *parent);
    ~M0002_CreateFeedsTable() override = default;

    void up() final;
    void down() final;
};

#endif // M0002_CREATEFEEDSTABLE_H
