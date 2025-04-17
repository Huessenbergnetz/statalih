/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feed_p.h"

FeedItem::FeedItem() : data(new FeedItemData)
{}

FeedItem::FeedItem(const FeedItem &other) = default;
FeedItem::FeedItem(FeedItem &&other) noexcept = default;
FeedItem &FeedItem::operator=(const FeedItem &other) = default;
FeedItem &FeedItem::operator=(FeedItem &&other) noexcept = default;
FeedItem::~FeedItem() noexcept = default;

QString FeedItem::title() const noexcept
{
    return data->title;
}

QString FeedItem::guid() const noexcept
{
    return data->guid;
}

QString FeedItem::description() const noexcept
{
    return data->description;
}

QString FeedItem::author() const noexcept
{
    return data->author;
}

QUrl FeedItem::link() const noexcept
{
    return data->link;
}

QDateTime FeedItem::pubDate() const noexcept
{
    return data->pubDate;
}

Feed::Feed() : data(new FeedData)
{}

Feed::Feed(const Feed &other) = default;

Feed::Feed(Feed &&other) noexcept = default;

Feed &Feed::operator=(const Feed &other) = default;

Feed &Feed::operator=(Feed &&other) noexcept = default;

Feed::~Feed() noexcept = default;

Feed::Type Feed::type() const noexcept
{
    return data->type;
}

QString Feed::title() const noexcept
{
    return data->title;
}

QString Feed::description() const noexcept
{
    return data->description;
}

QString Feed::generator() const noexcept
{
    return data->generator;
}

QString Feed::language() const noexcept
{
    return data->language;
}

QString Feed::publisher() const noexcept
{
    return data->publisher;
}

QUrl Feed::source() const noexcept
{
    return data->source;
}

QUrl Feed::link() const noexcept
{
    return data->link;
}

QDateTime Feed::lastBuildDate() const noexcept
{
    return data->lastBuildDate;
}

QList<FeedItem> Feed::items() const noexcept
{
    return data->items;
}

bool Feed::isValid() const noexcept
{
    if (data->type == Feed::Type::RSS_2_0) {
        return !data->source.isEmpty();
    } else {
        return false;
    }
}

#include "moc_feed.cpp"
