/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "m0001_createplacestable.h"

using namespace Qt::StringLiterals;

M0001_CreatePlacesTable::M0001_CreatePlacesTable(Firfuorida::Migrator *parent)
    : Firfuorida::Migration{parent}
{

}

void M0001_CreatePlacesTable::up()
{
    if (dbType() == Firfuorida::Migrator::PSQL) {
        raw(uR"-(
            CREATE TABLE places (
                id SERIAL PRIMARY KEY,
                name VARCHAR(255) NOT NULL,
                slug VARCHAR(255) NOT NULL,
                parent INTEGER,
                "administrativeId" VARCHAR(32),
                coords POINT,
                description TEXT,
                link VARCHAR(255),
                created TIMESTAMP,
                updated TIMESTAMP,
                data JSONB,
                CONSTRAINT places_parent_idx FOREIGN KEY (parent) REFERENCES places (id) ON DELETE SET NULL ON UPDATE CASCADE
            )
        )-"_s);
    } else {
        auto t = create(u"places"_s);
    }
}

void M0001_CreatePlacesTable::down()
{
    drop(u"places"_s);
}

#include "moc_m0001_createplacestable.cpp"
