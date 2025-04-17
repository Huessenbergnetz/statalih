/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0003_createitemstable.h"

using namespace Qt::StringLiterals;

M0003_CreateItemsTable::M0003_CreateItemsTable(Firfuorida::Migrator *parent)
    : Firfuorida::Migration{parent}
{

}

void M0003_CreateItemsTable::up()
{
    if (dbType() == Firfuorida::Migrator::PSQL) {
        raw(uR"-(
            CREATE TABLE items (
                id BIGSERIAL PRIMARY KEY,
                "feedId" INTEGER NOT NULL,
                guid VARCHAR(2048),
                title VARCHAR(255),
                description TEXT,
                author VARCHAR(255),
                link VARCHAR(2048),
                "pubDate" TIMESTAMP,
                data JSONB,
                CONSTRAINT guid_unique UNIQUE(guid),
                CONSTRAINT "feeds_feedId_idx" FOREIGN KEY ("feedId") REFERENCES feeds (id) ON DELETE CASCADE ON UPDATE CASCADE
            )
        )-"_s);
    } else {
        auto t = create(u"items"_s);
    }
}

void M0003_CreateItemsTable::down()
{
    drop(u"items"_s);
}

#include "moc_m0003_createitemstable.cpp"
