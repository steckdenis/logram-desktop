/*
 * ftpprovider.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher <steckdenis@yahoo.fr>
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Logram is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Logram; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef __FTPPROVIDER_H__
#define __FTPPROVIDER_H__

#include <QObject>
#include <QFtp>
#include <QEventLoop>
#include <QUrl>
#include <QStack>
#include <QHash>
#include <QProgressDialog>

#include <ILIOProvider.h>

class LIOFtpProvider : public QObject, public ILIOProvider
{
        Q_OBJECT
        Q_INTERFACES(ILIOProvider)

        public :
                bool rconnect(QString address, LIOJob *job);
                bool cd(QString& directory, LIOJob *job);
                bool cdUp(LIOJob *job);
                QString pwd(LIOJob *job);
                bool download(QString& remoteName, QString& tempName,  LIOJob *job, bool toTemp = true);
                bool upload(QString& tempName, QString& remoteName,  LIOJob *job, bool fromTemp = true);
                bool rename(QString& old, QString& mnew, LIOJob *job);
                bool mkdir(QString& name, LIOJob *job);
                bool rm(QString& name, LIOJob *job, bool recursive = false);
                bool exists(QString& name, LIOJob *job);
                bool chmod(QString& name, QFile::Permission permissions, LIOJob *job);
                bool chown(QString& file, QString& owner, QString& group, LIOJob *job);
                bool list(QList<QUrlInfo *> *mreturn, LIOJob *job, bool showHidden = false);
                bool copy(QString& from, QString& to, LIOJob *job);
                bool move(QString& from, QString& to, LIOJob *job);
                
        private slots:
                void commandFinished(int id, bool error);
                void rawCommandReply(int replyCode, const QString &details);
                void listInfo(const QUrlInfo &i);
                void dataTransferProgress(qint64 done, qint64 total);

        private:
                QHash<LIOJob *, QFtp *> ftps;
                
                QHash<QFtp *, int> response;
                QHash<QFtp *, QString> rsDetails;
                QHash<QFtp *, QProgressDialog *> pgsDialogs;
                
                QHash<QFtp *, QList<QUrlInfo *> *> listInfos;
                
                QStack<QEventLoop *> loops;             //---- Permet de "pusher" une association entre une commande en attente et son EventLoop
                QStack<int>          watchedCommands;   //-'

                bool rremove(QString path, LIOJob *job);
                int  waitCommand(int id);
                QFtp *ftp(LIOJob *job);
};

#endif
