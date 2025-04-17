/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "feedparser.h"
#include "feed_p.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QVersionNumber>
#include <QDebug>

using namespace Qt::StringLiterals;

FeedParser::FeedParser(QObject *parent)
    : QObject{parent}
{

}

void FeedParser::parse(const QDomDocument &xml)
{
    const auto docEl = xml.documentElement();
    const QString tagName = docEl.tagName();
    if (tagName == "rss"_L1) {
        auto version = QVersionNumber::fromString(docEl.attribute(u"version"_s));
        if (version == QVersionNumber(2, 0)) {
            parseRss2(xml);
        } else {

        }
    } else if (tagName == "feed"_L1) {

    }
}

void FeedParser::parseRss2(const QDomDocument &xml)
{
    const QDomNodeList channels = xml.elementsByTagName(u"channel"_s);
    if (channels.size() != 1) {
        return;
    }

    const auto channel = channels.at(0).toElement();
    if (!channel.hasChildNodes()) {
        return;
    }

    Feed feed;
    feed.data->type = Feed::Type::RSS_2_0;
    const auto nodes = channel.childNodes();
    for (const auto &node : nodes) {
        const auto e = node.toElement();
        const QString tn = e.tagName();
        if (tn == "title"_L1) {
            feed.data->title = e.text();
        } else if (tn == "description"_L1) {
            feed.data->description = e.text();
        } else if (tn == "link"_L1) {
            if (e.attribute(u"rel"_s) == "self"_L1) {
                feed.data->source = QUrl(e.attribute(u"href"_s));
            } else if (e.attribute(u"rel"_s).isEmpty()) {
                feed.data->link = QUrl(e.text());
            }
        } else if (tn == "lastBuildDate"_L1) {
            feed.data->lastBuildDate = QDateTime::fromString(e.text(), Qt::RFC2822Date).toUTC();
        } else if (tn == "generator"_L1) {
            feed.data->generator = e.text();
        } else if (tn == "langauge"_L1) {
            feed.data->language = e.text();
        } else if (tn == "managingDirector"_L1) {
            feed.data->publisher = e.text();
        } else if (tn == "item"_L1) {
            if (e.hasChildNodes()) {
                FeedItem item;
                const auto itemNodes = e.childNodes();
                for (const auto &iNode : itemNodes) {
                    const auto ie = iNode.toElement();
                    const QString itn = ie.tagName();
                    if (itn == "title"_L1) {
                        item.data->title = ie.text();
                    } else if (itn == "link"_L1) {
                        item.data->link = QUrl(ie.text());
                    } else if (itn == "guid"_L1) {
                        item.data->guid = ie.text();
                    } else if (itn == "description"_L1) {
                        item.data->description = ie.text();
                    } else if (itn == "author"_L1 || itn == "creator"_L1) {
                        item.data->author = ie.text();
                    } else if (itn == "pubDate"_L1) {
                        item.data->pubDate = QDateTime::fromString(ie.text(), Qt::RFC2822Date).toUTC();
                    }
                }
                feed.data->items.emplace_back(std::move(item));
            }
        }
    }

    emit feedParsed(feed);
}
