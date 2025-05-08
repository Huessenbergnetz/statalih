/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "utils.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QLocale>
#include <QRegularExpression>
#include <QSqlRecord>

using namespace Qt::StringLiterals;

static constexpr char16_t asciiTab{9};
static constexpr char16_t asciiSpace{32};
static constexpr char16_t ascii_dash{45};
static constexpr char16_t ascii_0{48};
static constexpr char16_t ascii_9{57};
static constexpr char16_t ascii_A{65};
static constexpr char16_t ascii_Z{90};
static constexpr char16_t ascii_underscore{95};
static constexpr char16_t ascii_a{97};
static constexpr char16_t ascii_z{122};

static QRegularExpression tagRegEx{u"<[^>]*>"_s};

QString Utils::slugify(const QString &str)
{
    QString _str = str.simplified().toLower();
    _str.replace(QChar(QChar::Space), '-'_L1)
    .replace('_'_L1, '-'_L1)
    .replace(u"ß"_s, u"ss"_s)
    .replace(u"ä"_s, u"ae"_s)
    .replace(u"ö"_s, u"oe"_s)
    .replace(u"ü"_s, u"ue"_s);

    QString _str2;
    for (const auto &ch : std::as_const(_str)) {
        const auto &uc = ch.unicode();
        if ((uc >= ascii_a && uc <= ascii_z) ||
                (uc >= ascii_0 && uc <= ascii_9) ||
                (uc == ascii_dash)) {
            _str2.append(ch);
        }
    }

    return _str2;
}

QString Utils::cleanDescription(const QString &desc)
{
    QString cleaned = desc.simplified();
    cleaned.remove(tagRegEx);
    return cleaned;
}

QString Utils::coordsToDb(float latitude, float longitude)
{
    return u"(%1,%2)"_s.arg(QString::number(latitude), QString::number(longitude));
}

std::optional<std::pair<float,float>> Utils::coordsFromDb(const QVariant &v)
{
    const auto str = v.toString();
    if (str.isEmpty()) {
        return std::nullopt;
    }

    const auto openingParaIdx = str.indexOf('('_L1);
    if (openingParaIdx < 0) {
        return std::nullopt;
    }

    const auto closingParaIdx = str.indexOf(')'_L1);
    if (closingParaIdx < 0) {
        return std::nullopt;
    }

    const auto commadIdx = str.indexOf(','_L1);
    if (commadIdx < 2) {
        return std::nullopt;
    }

    const QString latitudeStr = str.sliced(openingParaIdx + 1, commadIdx - openingParaIdx - 1);
    bool ok = false;
    const auto latitude = latitudeStr.toFloat(&ok);
    if (!ok) {
        return std::nullopt;
    }

    const QString longitudeStr = str.sliced(commadIdx + 1, closingParaIdx - commadIdx - 1);
    const auto longitude = longitudeStr.toFloat(&ok);
    if (!ok) {
        return std::nullopt;
    }

    return std::make_pair(latitude,longitude);
}

QString Utils::humanCoords(float latitude, float longitude)
{
    QLocale locale;
    return u"N %1 E %2"_s.arg(locale.toString(latitude), locale.toString(longitude));
}

QJsonArray Utils::queryToJsonObjectArray(QSqlQuery &query)
{
    QJsonArray ret;
    const QSqlRecord record = query.record();
    const int columns       = record.count();
    QStringList cols;
    cols.reserve(columns);
    for (int i = 0; i < columns; ++i) {
        cols.append(record.fieldName(i));
    }

    while (query.next()) {
        QJsonObject line;
        for (int i = 0; i < columns; ++i) {
            line.insert(cols.at(i), QJsonValue::fromVariant(query.value(i)));
        }
        ret.append(line);
    }
    return ret;
}
