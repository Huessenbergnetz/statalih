/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_PLACESCOMMAND_H
#define HBNST_PLACESCOMMAND_H

#include "command.h"

class PlacesCommand : public Command
{
    Q_OBJECT
public:
    explicit PlacesCommand(QObject *parent = nullptr);
    ~PlacesCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(PlacesCommand)
};

#endif // HBNST_PLACESCOMMAND_H
