/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_DATABASEREFRESHCOMMAND_H
#define HBNZMC_DATABASEREFRESHCOMMAND_H

#include "databasecommand.h"

class DatabaseRefreshCommand final : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseRefreshCommand(QObject *parent = nullptr);
    ~DatabaseRefreshCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(DatabaseRefreshCommand)
};

#endif // HBNZMC_DATABASEREFRESHCOMMAND_H
