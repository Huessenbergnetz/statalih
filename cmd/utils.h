/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_UTILS_H
#define HBNST_UTILS_H

#include <QString>

namespace Utils {

QString slugify(const QString &str);
QString cleanDescription(const QString &desc);

}

#endif // HBNST_UTILS_H
