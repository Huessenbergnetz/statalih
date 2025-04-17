/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_ITEMIMAGEEXTRACTOR_H
#define HBNST_ITEMIMAGEEXTRACTOR_H

#include "feed.h"

#include <QObject>
#include <QQueue>

class QNetworkAccessManager;
class QNetworkReply;

class ItemImageExtractor : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ItemImageExtractor)
public:
    explicit ItemImageExtractor(QObject *parent = nullptr);
    ~ItemImageExtractor() override = default;

public:
    void start(const QList<FeedItem> &items);

private slots:
    void extract();
    void itemDataFetched(QNetworkReply *reply);

signals:
    void finished(const QVariantMap &itemImages, const QMap<QString,QString> &errors);

private:
    FeedItem m_currentItem;
    QQueue<FeedItem> m_items;
    QVariantMap m_itemImages;
    QMap<QString,QString> m_errors;
    QNetworkAccessManager *m_nam{nullptr};
    static const QRegularExpression ogImgRegex;
};

#endif // HBNST_ITEMIMAGEEXTRACTOR_H
