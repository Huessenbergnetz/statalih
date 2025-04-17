/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "utils.h"

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
