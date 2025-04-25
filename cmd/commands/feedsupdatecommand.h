/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDSUPDATECOMMAND_H
#define HBNST_FEEDSUPDATECOMMAND_H

#include "command.h"
#include "feed.h"

#include <QDateTime>
#include <QQueue>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class FeedsUpdateCommand : public Command
{
    Q_OBJECT
public:
    explicit FeedsUpdateCommand(QObject *parent = nullptr);
    ~FeedsUpdateCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private slots:
    void updateFeed();
    void feedFetched(QNetworkReply *reply);
    void feedParsed(const Feed &feed);
    void imagesFetched(const QVariantMap &itemImages, const QMap<QString,QString> &errors);

private:
    void init();

    struct FeedStruct {
        int id;
        QString title;
        QUrl source;
        QString etag;
        QDateTime lastBuildDate;
        QDateTime lastFetch;

        [[nodiscard]] QString logInfo() const noexcept
        {
            return QStringLiteral("“%1” (ID: %2)").arg(title, QString::number(id));
        }
    };

    QQueue<FeedStruct> m_feedsToUpdate;
    FeedStruct m_current;
    Feed m_feed;
    QNetworkAccessManager *m_nam{nullptr};

    Q_DISABLE_COPY(FeedsUpdateCommand);
};

#endif // HBNST_FEEDSUPDATECOMMAND_H
