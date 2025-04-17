/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "itemimageextractor.h"

#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QUrl>

using namespace Qt::StringLiterals;

const QRegularExpression ItemImageExtractor::ogImgRegex{uR"-(<meta\s+property=["'](og:image[^"']*)["']\s+content=["']([^"']+))-"_s};

ItemImageExtractor::ItemImageExtractor(QObject *parent)
    : QObject{parent}
{

}

void ItemImageExtractor::start(const QList<FeedItem> &items)
{
    if (items.empty()) {
        emit finished(m_itemImages, {});
    }

    for (const auto &item : items) {
        m_items.enqueue(item);
    }

    m_nam = new QNetworkAccessManager(this);
    m_nam->setTransferTimeout(10'000);
    connect(m_nam, &QNetworkAccessManager::finished, this, &ItemImageExtractor::itemDataFetched);

    QMetaObject::invokeMethod(this, "extract");
}

void ItemImageExtractor::extract()
{
    if (m_items.empty()) {
        deleteLater();
        emit finished(m_itemImages, m_errors);
        return;
    }

    m_currentItem = m_items.dequeue();

    QNetworkRequest req{m_currentItem.link()};
    m_nam->get(req);
}

void ItemImageExtractor::itemDataFetched(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_errors.insert(m_currentItem.guid(), reply->errorString());
    } else {
        QVariantMap map;
        const QString data = QString::fromUtf8(reply->readAll());
        for (const auto &match : ogImgRegex.globalMatch(data)) {
            const QString property = match.captured(1);
            const QString content = match.captured(2);
            if (property == "og:image"_L1) {
                QUrl url{content};
                if (url.isValid()) {
                    map.insert(u"url"_s, url);
                }
            } else if (property == "og:image:secure_url"_L1) {
                QUrl url{content};
                if (url.isValid()) {
                    map.insert(u"secure_url"_s, url);
                }
            } else if (property == "og:image:width"_L1) {
                bool ok{false};
                const int width = content.toInt(&ok);
                if (ok) {
                    map.insert(u"width"_s, width);
                }
            } else if (property == "og:image:height"_L1) {
                bool ok{false};
                const int height = content.toInt(&ok);
                if (ok) {
                    map.insert(u"height"_s, height);
                }
            } else if (property == "og:image:alt"_L1) {
                map.insert(u"alt"_s, content);
            } else if (property == "og:image:type"_L1) {
                map.insert(u"type"_s, content);
            }
        }
        m_itemImages.insert(m_currentItem.guid(), map);
    }
    extract();
}

#include "moc_itemimageextractor.cpp"
