/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0002_createfeedstable.h"

using namespace Qt::Literals::StringLiterals;

M0002_CreateFeedsTable::M0002_CreateFeedsTable(Firfuorida::Migrator *parent)
    : Firfuorida::Migration{parent}
{

}

void M0002_CreateFeedsTable::up()
{
    if (dbType() == Firfuorida::Migrator::PSQL) {
        raw(uR"-(
            CREATE TABLE feeds (
                id SERIAL PRIMARY KEY,
                type SMALLINT NOT NULL,
                slug VARCHAR(255) NOT NULL,
                title VARCHAR(255) NOT NULL,
                description TEXT,
                link VARCHAR(2048),
                "lastBuildDate" TIMESTAMP,
                "lastEtag" VARCHAR(255),
                coords POINT,
                data JSONB
            )
        )-"_s);
    } else {
        auto t = create(u"feeds"_s);
    }
}

void M0002_CreateFeedsTable::down()
{
    drop(u"feeds"_s);
}

#include "moc_m0002_createfeedstable.cpp"
