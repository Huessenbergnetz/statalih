/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDSLISTCOMMAND_H
#define HBNST_FEEDSLISTCOMMAND_H

#include "command.h"

class FeedsListCommand final : public Command
{
    Q_OBJECT
public:
    explicit FeedsListCommand(QObject *parent = nullptr);
    ~FeedsListCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const;

    [[nodiscard]] QString description() const;

private:
    void init();

    Q_DISABLE_COPY(FeedsListCommand)
};

#endif // HBNST_FEEDSLISTCOMMAND_H
