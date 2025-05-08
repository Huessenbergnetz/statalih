/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_UTILS_H
#define HBNST_UTILS_H

#include <QJsonArray>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

#include <optional>
#include <utility>

namespace Utils {

QString slugify(const QString &str);
QString cleanDescription(const QString &desc);

QString coordsToDb(float latitude, float longitude);
std::optional<std::pair<float,float>> coordsFromDb(const QVariant &v);
QString humanCoords(float latitude, float longitude);
inline QString humanCoords(std::pair<float,float> coords) {
    return Utils::humanCoords(coords.first, coords.second);
}

QJsonArray queryToJsonObjectArray(QSqlQuery &query);

}

#endif // HBNST_UTILS_H
