/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_DATABASECOMMAND_H
#define HBNZMC_DATABASECOMMAND_H

#include "command.h"

#include <memory>

namespace Firfuorida {
class Migrator;
}

class DatabaseCommand : public Command
{
    Q_OBJECT
public:
    explicit DatabaseCommand(QObject *parent = nullptr);

    ~DatabaseCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

protected:
    [[nodiscard]] QString dbConName() const;
    void initMigrations();
    std::unique_ptr<Firfuorida::Migrator> m_migrator;

private:
    void init();

    Q_DISABLE_COPY(DatabaseCommand)
};

#endif // HBNZMC_DATABASECOMMAND_H
