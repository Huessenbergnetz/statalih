/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDSCOMMAND_H
#define HBNST_FEEDSCOMMAND_H

#include "command.h"

#include <memory>

class FeedsCommand : public Command
{
    Q_OBJECT
public:
    explicit FeedsCommand(QObject *parent = nullptr);

    ~FeedsCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(FeedsCommand)
};

#endif // HBNST_FEEDSCOMMAND_H
