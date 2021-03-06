/*
 * Copyright (C) 2012 Andres Pagliano, Gabriel Miretti, Gonzalo Buteler,
 * Nestor Bustamante, Pablo Perez de Angelis
 *
 * This file is part of LVK Chatbot.
 *
 * LVK Chatbot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LVK Chatbot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LVK Chatbot.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LVK_DAS_SFTPCONTESTDATAUPLOADER_H
#define LVK_DAS_SFTPCONTESTDATAUPLOADER_H

#include <QObject>

#include "da-server/contestdata.h"
#include "da-server/contestdatauploader.h"
#include "qssh/sftpchannel.h"
#include "qssh/sshconnection.h"

class QMutex;

namespace Lvk
{

/// \addtogroup Lvk
/// @{

namespace DAS
{

/// \ingroup Lvk
/// \addtogroup DAS
/// @{

/**
 * \brief The SftpContestDataUploader class provides an SFTP uploader
 */
class SftpContestDataUploader : public ContestDataUploader
{
    Q_OBJECT

public:

    /**
     * Constructs a SftpContestDataUploader object
     */
    SftpContestDataUploader();

    /**
     * Destroys the object and closes any connection
     */
    ~SftpContestDataUploader();

    /**
     * \copydoc ContestDataUploader::upload
     */
    virtual void upload(const ContestData &data);

signals:

    /**
     * \copydoc ContestDataUploader::finished
     */
    void finished(DAS::ContestDataUploader::Status status);

private slots:
    void onConnected();
    void onConnectionError(QSsh::SshError);
    void onChannelInitialized();
    void onChannelError(const QString &err);
    void onOpfinished(QSsh::SftpJobId job, const QString & error = QString());

private:
    QMutex *m_mutex;
    bool m_inProgress;
    QString m_localFilename;
    QString m_remoteFilename;
    ContestData m_data;
    QSsh::SftpChannel::Ptr m_channel;
    QSsh::SshConnection *m_connection;

    void initFilenames();
    void getConnectionParams(QSsh::SshConnectionParameters &params);
    void parseDestination(const QString &dest);
    bool sendScore();
    void finish(Status status);
    void close();
};

/// @}

} // namespace DAS

/// @}

} // namespace Lvk


#endif // LVK_DAS_SFTPCONTESTDATAUPLOADER_H
