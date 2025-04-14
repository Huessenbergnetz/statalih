/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_DATABASEROLLBACKCOMMAND_H
#define HBNZMC_DATABASEROLLBACKCOMMAND_H

#include "databasecommand.h"

class DatabaseRollbackCommand final : public DatabaseCommand
{
    Q_OBJECT
public:
    explicit DatabaseRollbackCommand(QObject *parent = nullptr);
    ~DatabaseRollbackCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(DatabaseRollbackCommand)
};

#endif // HBNZMC_DATABASEROLLBACKCOMMAND_H
