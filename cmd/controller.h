/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNZMC_CONTROLLER_H
#define HBNZMC_CONTROLLER_H

#include "cli.h"

#include <QCommandLineOption>

class QCommandLineParser;

class Controller final : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    ~Controller() final = default;

    [[nodiscard]] QList<QCommandLineOption> globalOptions() const;

public slots:
    void exec();

private:
    void init();
    void showHelp() const;
    QList<QCommandLineOption> m_globalOptions;

    Q_DISABLE_COPY(Controller)
};

#endif // HBNZMC_CONTROLLER_H
