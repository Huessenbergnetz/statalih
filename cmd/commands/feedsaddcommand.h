/*
 * SPDX-FileCopyrightText: (C) 2025 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HBNST_FEEDSADDCOMMAND_H
#define HBNST_FEEDSADDCOMMAND_H

#include "command.h"
#include "feed.h"

class QUrl;
class QNetworkReply;

class FeedsAddCommand : public Command
{
    Q_OBJECT
public:
    explicit FeedsAddCommand(QObject *parent = nullptr);
    ~FeedsAddCommand() override = default;

    void exec(QCommandLineParser *parser) override;

    [[nodiscard]] QString summary() const override;

    [[nodiscard]] QString description() const override;

private slots:
    void feedFetched(QNetworkReply *reply);
    void feedParsed(const Feed &feed);

private:
    void init();

    QString m_title;
    QString m_slug;
    QString m_description;
    QString m_format;
    float m_latitude{0.0};
    float m_longitude{0.0};
    bool m_coordsSet{false};

    Q_DISABLE_COPY(FeedsAddCommand);
};

#endif // HBNST_FEEDSADDCOMMAND_H
