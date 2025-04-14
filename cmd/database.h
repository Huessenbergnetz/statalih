/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_DATABASE_H
#define HBNST_DATABASE_H

#include "configuration.h"

class Database : public Configuration
{
    Q_OBJECT
    Q_DISABLE_COPY(Database)
public:
    explicit Database(QObject *parent = nullptr);

    ~Database() override = default;

protected:
    CLI::RC openDb(const QString &connectionName);
};

#endif // HBNST_DATABASE_H
