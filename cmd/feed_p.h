/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEED_P_H
#define HBNST_FEED_P_H

#include "feed.h"

class FeedItemData : public QSharedData
{
public:
    QString title;
    QString guid;
    QString description;
    QString author;
    QUrl link;
    QDateTime pubDate;
};

class FeedData : public QSharedData
{
public:
    QString title;
    QString description;
    QString generator;
    QString language;
    QString publisher;
    QUrl link;
    QUrl source;
    QDateTime lastBuildDate;
    QList<FeedItem> items;
    Feed::Type type{Feed::Type::Invalid};
};

#endif // HBNST_FEED_P_H
