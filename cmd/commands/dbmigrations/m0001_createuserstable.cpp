/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0001_createuserstable.h"

using namespace Qt::Literals::StringLiterals;

M0001_CreateUsersTable::M0001_CreateUsersTable(Firfuorida::Migrator *parent)
    : Firfuorida::Migration{parent}
{

}

void M0001_CreateUsersTable::up()
{
    if (dbType() == Firfuorida::Migrator::PSQL) {
        raw(uR"-(
            CREATE TABLE users (
                id VARCHAR(64) PRIMARY KEY,
                type SMALLINT NOT NULL DEFAULT 0,
                "displayName" VARCHAR(64) NOT NULL,
                created TIMESTAMP NOT NULL,
                updated TIMESTAMP,
                "lastLogin" TIMESTAMP,
                "lockedAt" TIMESTAMP,
                "lockedBy" VARCHAR(64)
            )
        )-"_s);
    } else {
        auto t = create(u"users"_s);
    }
}

void M0001_CreateUsersTable::down()
{
    drop(u"users"_s);
}

#include "moc_m0001_createuserstable.cpp"
