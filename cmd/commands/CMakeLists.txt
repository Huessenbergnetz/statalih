# SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: AGPL-3.0-or-later

target_sources(statalihcmd
    PRIVATE
        command.cpp
        command.h
        databasecommand.cpp
        databasecommand.h
        databasemigrationcommand.cpp
        databasemigrationcommand.h
        databaserefreshcommand.cpp
        databaserefreshcommand.h
        databaseresetcommand.cpp
        databaseresetcommand.h
        databaserollbackcommand.cpp
        databaserollbackcommand.h
        feedscommand.cpp
        feedscommand.h
        feedsaddcommand.cpp
        feedsaddcommand.h
)

add_subdirectory(dbmigrations)
