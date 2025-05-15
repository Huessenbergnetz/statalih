/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDSLISTITEMS_H
#define HBNST_FEEDSLISTITEMS_H

#include "command.h"

class FeedsListItemsCommand final : public Command
{
    Q_OBJECT
public:
    explicit FeedsListItemsCommand(QObject *parent = nullptr);
    ~FeedsListItemsCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(FeedsListItemsCommand);
};

#endif // HBNST_FEEDSLISTITEMS_H
