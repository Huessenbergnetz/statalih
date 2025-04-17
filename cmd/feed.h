/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEED_H
#define HBNST_FEED_H

#include <QDateTime>
#include <QObject>
#include <QSharedDataPointer>
#include <QUrl>

class FeedItemData;

class FeedItem
{
    Q_GADGET
public:
    FeedItem();
    FeedItem(const FeedItem &other);
    FeedItem(FeedItem &&other) noexcept;
    FeedItem &operator=(const FeedItem &other);
    FeedItem &operator=(FeedItem &&other) noexcept;
    ~FeedItem() noexcept;

    void swap(FeedItem &other) noexcept
    { data.swap(other.data); }

    [[nodiscard]] QString title() const noexcept;

    [[nodiscard]] QString guid() const noexcept;

    [[nodiscard]] QString description() const noexcept;

    [[nodiscard]] QString author() const noexcept;

    [[nodiscard]] QUrl link() const noexcept;

    [[nodiscard]] QDateTime pubDate() const noexcept;

private:
    friend class FeedParser;
    QSharedDataPointer<FeedItemData> data;
};

Q_DECLARE_SHARED(FeedItem)

class FeedData;

class Feed
{
    Q_GADGET
public:
    enum class Type {
        Invalid = 0,
        Atom_1_0 = 1,
        RSS_2_0 = 2
    };
    Q_ENUM(Type)

    Feed();
    Feed(const Feed &other);
    Feed(Feed &&other) noexcept;
    Feed &operator=(const Feed &other);
    Feed &operator=(Feed &&other) noexcept;
    ~Feed() noexcept;

    void swap(Feed &other) noexcept
    { data.swap(other.data); }

    [[nodiscard]] Type type() const noexcept;

    [[nodiscard]] QString title() const noexcept;

    [[nodiscard]] QString description() const noexcept;

    [[nodiscard]] QString generator() const noexcept;

    [[nodiscard]] QString language() const noexcept;

    [[nodiscard]] QString publisher() const noexcept;

    [[nodiscard]] QUrl link() const noexcept;

    [[nodiscard]] QUrl source() const noexcept;

    [[nodiscard]] QDateTime lastBuildDate() const noexcept;

    [[nodiscard]] QList<FeedItem> items() const noexcept;

    [[nodiscard]] bool isValid() const noexcept;

private:
    friend class FeedParser;
    QSharedDataPointer<FeedData> data;
};

Q_DECLARE_SHARED(Feed)

#endif // HBNST_FEED_H
