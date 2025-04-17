# SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: AGPL-3.0-or-later

add_executable(statalihcmd)

target_sources(statalihcmd
    PRIVATE
        main.cpp
        cli.cpp
        cli.h
        configuration.cpp
        configuration.h
        database.cpp
        database.h
        controller.cpp
        controller.h
        feed.cpp
        feed_p.h
        feed.h
        feedparser.cpp
        feedparser.h
        utils.cpp
        utils.h
)

add_subdirectory(commands)

target_link_libraries(statalihcmd
    PRIVATE
        Qt6::Core
        Qt6::Sql
        Qt6::Network
        Qt6::Xml
        FirfuoridaQt6::Core
)

target_compile_definitions(statalihcmd
    PRIVATE
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_ASCII
    QT_STRICT_ITERATORS
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_USE_QSTRINGBUILDER
    HBNST_VERSION="${PROJECT_VERSION}"
    HBNST_TRANSLATIONSDIR="${HBNST_FULL_TRANSLATIONSDIR}"
    HBNST_CONFFILE="${HBNST_FULL_CONFFILE}"
    $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
)

target_include_directories(statalihcmd
    PRIVATE
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_BINARY_DIR}/common
)

set_target_properties(statalihcmd
    PROPERTIES
        OUTPUT_NAME statalih
)

install(TARGETS statalihcmd DESTINATION ${CMAKE_INSTALL_BINDIR})

set(statalihcmd_TS_FILES
    translations/statalihcmd.ts
    translations/statalihcmd.de.ts
)

set_source_files_properties(${statalihcmd_TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations"
)

qt_add_lupdate(statalihcmd
    TS_FILES ${statalihcmd_TS_FILES}
    OPTIONS -no-obsolete -locations none -source-language en
)

qt_add_lrelease(statalihcmd
    TS_FILES ${statalihcmd_TS_FILES}
    QM_FILES_OUTPUT_VARIABLE statalihcmd_QM_FILES
    OPTIONS -idbased
)

install(FILES ${statalihcmd_QM_FILES} DESTINATION ${HBNST_TRANSLATIONSDIR})
