# SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.20)

project(statalih
    VERSION 0.0.1
    DESCRIPTION "Cutelyst based web feed aggregator"
    HOMEPAGE_URL https://github.com/Huessenbergnetz/statalih
    LANGUAGES CXX
)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()

set(CUTELYST_VERSION_MAJOR 5)

find_package(Qt6 6.5.0 COMPONENTS Core Network Sql Xml LinguistTools REQUIRED)
find_package(Cutelyst${CUTELYST_VERSION_MAJOR}Qt6 REQUIRED)
find_package(Cutelee6Qt6 REQUIRED)
find_package(Cutelyst${CUTELYST_VERSION_MAJOR}Qt6Botan REQUIRED)
find_package(Cutelyst${CUTELYST_VERSION_MAJOR}Qt6Forms REQUIRED)
find_package(FirfuoridaQt6 REQUIRED)
find_package(ASqlQt6)
find_package(PkgConfig REQUIRED)
pkg_search_module(Pwquality REQUIRED IMPORTED_TARGET pwquality>=1.2.2)

# Auto generate moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

set(HBNST_APPDIR ${CMAKE_INSTALL_DATADIR}/statalih CACHE PATH "Directory to install statalih data files into")
set(HBNST_TEMPLATESDIR ${HBNST_APPDIR}/templates CACHE PATH "Directory to install statalih template files into")
set(HBNST_TRANSLATIONSDIR ${HBNST_APPDIR}/translations CACHE PATH "Directory to install statalih translation files into")
set(HBNST_FORMSDIR ${HBNST_APPDIR}/forms CACHE PATH "Directory to install statalih form files into")
set(HBNST_CONFFILE ${CMAKE_INSTALL_SYSCONFDIR}/statalih.ini CACHE FILEPATH "Path to the main configuration file")
set(HBNST_TEST_LOG_RULES "*.debug=false;asql.*.debug=true;cutelyst.*.debug=true;hbnzm.*.debug=true;simplemail.*.debug=true" CACHE STRING "Logging rules for test script")

option(ENABLE_MAINTAINER_CFLAGS "Enable maintainer CFlags" OFF)

GNUInstallDirs_get_absolute_install_dir(HBNST_FULL_TRANSLATIONSDIR HBNST_TRANSLATIONSDIR DATADIR)
GNUInstallDirs_get_absolute_install_dir(HBNST_FULL_TEMPLATESDIR HBNST_TEMPLATESDIR DATADIR)
GNUInstallDirs_get_absolute_install_dir(HBNST_FULL_FORMSDIR HBNST_FORMSDIR DATADIR)
GNUInstallDirs_get_absolute_install_dir(HBNST_FULL_CONFFILE HBNST_CONFFILE SYSCONFDIR)

if(ENABLE_MAINTAINER_CFLAGS)
    add_compile_definitions(QT_DEPRECATED_WARNINGS)

    if(NOT MSVC)
        add_compile_options(
            -Wall
            -Wextra
            -Wcast-align
            -Wno-uninitialized
            -Wempty-body
            -Wformat-security
            -Wformat
            -Winit-self
            -Wunused-variable
        )
    endif()

    if(APPLE)
        add_compile_options(-Wweak-vtables)
    endif()
endif()

# Set configuration key names and default values

set(HBNST_CONF_DB "database")
set(HBNST_CONF_DB_TYPE "type")
set(HBNST_CONF_DB_TYPE_DEFVAL "QPSQL")
set(HBNST_CONF_DB_HOST "host")
set(HBNST_CONF_DB_HOST_DEFVAL "localhost")
set(HBNST_CONF_DB_USER "user")
set(HBNST_CONF_DB_USER_DEFVAL "statalih")
set(HBNST_CONF_DB_PASS "password")
set(HBNST_CONF_DB_NAME "name")
set(HBNST_CONF_DB_NAME_DEFVAL "statalihdb")
set(HBNST_CONF_DB_PORT "port")
set(HBNST_CONF_DB_PORT_DEFVAL 5432)

configure_file(
  ${CMAKE_SOURCE_DIR}/common/confignames.h.in
  ${CMAKE_BINARY_DIR}/common/confignames.h
  @ONLY
)

add_subdirectory(cmd)
