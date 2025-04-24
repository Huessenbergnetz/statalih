/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_PLACESLISTCOMMAND_H
#define HBNST_PLACESLISTCOMMAND_H

#include "command.h"

class PlacesListCommand : public Command
{
    Q_OBJECT
public:
    explicit PlacesListCommand(QObject *parent = nullptr);
    ~PlacesListCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const;

    [[nodiscard]] QString description() const;

private:
    void init();

    Q_DISABLE_COPY(PlacesListCommand)
};

#endif // HBNST_PLACESLISTCOMMAND_H
