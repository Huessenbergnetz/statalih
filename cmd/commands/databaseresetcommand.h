/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_DATABASERESETCOMMAND_H
#define HBNZMC_DATABASERESETCOMMAND_H

#include "databasecommand.h"

class DatabaseResetCommand final : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseResetCommand(QObject *parent = nullptr);
    ~DatabaseResetCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    Q_DISABLE_COPY(DatabaseResetCommand)
};

#endif // HBNZMC_DATABASERESETCOMMAND_H
