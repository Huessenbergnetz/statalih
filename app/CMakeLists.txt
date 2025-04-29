# SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: AGPL-3.0-or-later

add_library(statalih SHARED)

target_sources(statalih
    PRIVATE
        statalih.cpp
        statalih.h
        logging.h
        settings.cpp
        settings.h
)

add_subdirectory(controllers)

target_include_directories(statalih
    PRIVATE
        ${CMAKE_BINARY_DIR}/common
        ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(statalih
    PRIVATE
        Qt6::Core
        Cutelyst::Core
        Cutelyst::Session
        Cutelyst::View::Cutelee
        Cutelyst::StaticSimple
        Cutelyst::StaticCompressed
        Cutelyst::CSRFProtection
        Cutelyst::Utils::LangSelect
        Cutelyst::Utils::Validator
        Cutelyst::StatusMessage
        CutelystForms::Core
        ASql::Core
        ASql::Pg
)

target_compile_definitions(statalih
    PRIVATE
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_STRICT_ITERATORS
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_CAST_FROM_BYTEARRAY
        QT_USE_QSTRINGBUILDER
        HBNST_VERSION="${PROJECT_VERSION}"
        HBNST_TRANSLATIONSDIR="${HBNST_FULL_TRANSLATIONSDIR}"
        HBNST_TEMPLATESDIR="${HBNST_FULL_TEMPLATESDIR}"
        HBNST_FORMSDIR="${HBNST_FULL_FORMSDIR}"
)

install(TARGETS statalih DESTINATION ${CMAKE_INSTALL_LIBDIR}/cutelyst${Cutelyst3Qt6_VERSION_MAJOR}-qt6-apps)

set(statalih_TS_FILES
    translations/statalih.en.ts
    translations/statalih.de.ts
)

set_source_files_properties(${statalih_TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations"
)

qt_add_lupdate(statalih
    TS_FILES ${statalih_TS_FILES}
    OPTIONS -no-obsolete -locations none -source-language en -tr-function-alias qsTrId=cTrId
)

qt_add_lrelease(statalhi
    TS_FILES ${statalih_TS_FILES}
    QM_FILES_OUTPUT_VARIABLE statalih_QM_FILES
    OPTIONS -idbased
)

install(FILES ${statalih_QM_FILES} DESTINATION ${HBNST_TRANSLATIONSDIR})
