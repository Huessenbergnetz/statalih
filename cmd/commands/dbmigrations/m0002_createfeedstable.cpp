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
                slug VARCHAR(255) NOT NULL,
                title VARCHAR(255) NOT NULL,
                description TEXT,
                source VARCHAR(2048),
                link VARCHAR(2048),
                etag VARCHAR(255),
                "lastBuildDate" TIMESTAMP,
                "lastFetch" TIMESTAMP,
                coords POINT,
                data JSONB,
                CONSTRAINT slug_unique UNIQUE(slug)
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
