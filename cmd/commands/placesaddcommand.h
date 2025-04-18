/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_PLACESADDCOMMAND_H
#define HBNST_PLACESADDCOMMAND_H

#include "command.h"

class PlacesAddCommand : public Command
{
    Q_OBJECT
public:
    explicit PlacesAddCommand(QObject *parent = nullptr);
    ~PlacesAddCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private:
    void init();

    Q_DISABLE_COPY(PlacesAddCommand);
};

#endif // HBNST_PLACESADDCOMMAND_H
