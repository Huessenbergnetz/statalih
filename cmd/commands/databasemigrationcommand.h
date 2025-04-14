/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_DATABASEMIGRATIONCOMMAND_H
#define HBNZMC_DATABASEMIGRATIONCOMMAND_H

#include "databasecommand.h"

class DatabaseMigrationCommand final : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseMigrationCommand(QObject *parent = nullptr);

    ~DatabaseMigrationCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    Q_DISABLE_COPY(DatabaseMigrationCommand)
};

#endif // HBNZMC_DATABASEMIGRATIONCOMMAND_H
