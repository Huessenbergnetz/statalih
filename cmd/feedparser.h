/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDPARSER_H
#define HBNST_FEEDPARSER_H

#include "feed.h"

#include <QObject>

class QDomDocument;

class FeedParser : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FeedParser)
public:
    explicit FeedParser(QObject *parent = nullptr);
    ~FeedParser() override = default;

public slots:
    void parse(const QDomDocument &xml);

private slots:
    void parseRss2(const QDomDocument &xml);

signals:
    void feedParsed(const Feed &feed);
};

#endif // HBNST_FEEDPARSER_H
