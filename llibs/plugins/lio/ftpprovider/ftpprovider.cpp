/*
 * ftpprovider.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Denis Steckelmacher <steckdenis@yahoo.fr>
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

#include <QStringList>
#include <QFile>

#include "ftpprovider.h"

#include <QtDebug>

bool LIOFtpProvider::rconnect(QString address, LIOJob *job)
{
        //Connecter au serveur
        if (!ftps.contains(job))
        {
                QFtp *mftp = new QFtp(this);
                
                connect(mftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
                connect(mftp, SIGNAL(rawCommandReply(int, const QString &)), this, SLOT(rawCommandReply(int, const QString &)));
                connect(mftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(listInfo(const QUrlInfo &)));
                connect(mftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dataTransferProgress(qint64, qint64)));
                
                ftps.insert(job, mftp);
                response.insert(mftp, 0);
                rsDetails.insert(mftp, QString());
                listInfos.insert(mftp, 0);
        }
        
        QUrl url(QString("ftp://") + address);
        int id;
        
        //Définir l'hôte
        id = ftp(job)->connectToHost(url.host(), url.port(21));
        
        //Attendre que la commande soit exécutée
        if (waitCommand(id) != 0) return false;
        
        //Se logger si le besoin est
        id = ftp(job)->login(url.userName(), url.password());

        return (waitCommand(id) == 0);
}

QFtp *LIOFtpProvider::ftp(LIOJob *job)
{
        return ftps.value(job);
}

//
// Slots
//

int LIOFtpProvider::waitCommand(int id)
{
        //Créer une nouvelle boucle d'attente et la pusher
        QEventLoop *loop = new QEventLoop(this);
        
        watchedCommands.push(id);
        loops.push(loop);
        
        //Démarrer la boucle
        int rs = loop->exec();
        
        //On n'a plus besoin de la boucle
        watchedCommands.pop();
        loops.pop();
        delete loop;
        
        return rs;
}

void LIOFtpProvider::commandFinished(int id, bool error)
{
        //Terminer la boucle qui correspond à la commande
        int index = watchedCommands.indexOf(id);
        QEventLoop *loop = loops.at(index);
        
        loop->exit(error);
}

void LIOFtpProvider::rawCommandReply(int replyCode, const QString &details)
{
        QFtp *mftp = static_cast<QFtp *>(sender());
        
        response[mftp] = replyCode;
        rsDetails[mftp] = details;
        
        qDebug() << replyCode << details;
}

void LIOFtpProvider::listInfo(const QUrlInfo &i)
{
        QFtp *mftp = static_cast<QFtp *>(sender());
        
        QUrlInfo *info = new QUrlInfo(i);
        
        qDebug() << "List : " << i.name();
        
        listInfos[mftp]->append(info);
}

void LIOFtpProvider::dataTransferProgress(qint64 done, qint64 total)
{
        QFtp *ftp = static_cast<QFtp *>(sender());
        
        QProgressDialog *pgs = pgsDialogs.value(ftp);
        
        if (pgs != 0)
        {
                pgs->setMaximum(total);
                pgs->setValue(done);
                
                if (pgs->wasCanceled())
                {
                        ftp->abort();
                }
        }
}

//
// Fonctions
//

bool LIOFtpProvider::cd(QString& directory, LIOJob *job)
{
        qDebug() << "cd" << directory;
                
        int id = ftp(job)->cd(directory);
        
        return (waitCommand(id) == 0);
}

bool LIOFtpProvider::cdUp(LIOJob *job)
{
        qDebug() << "cdup";
        
        int id = ftp(job)->rawCommand("CDUP");
        
        return (waitCommand(id) == 0);
}

QString LIOFtpProvider::pwd(LIOJob *job)
{
        qDebug() << "pwd";
        
        int id = ftp(job)->rawCommand("PWD");
        
        if (waitCommand(id) != 0) return QString();
        
        return rsDetails[ftp(job)];
}

bool LIOFtpProvider::download(QString& remoteName, QString& tempName, LIOJob *job, bool toTemp)
{
        qDebug() << "download" << remoteName << tempName << toTemp;
        
        //Créer la QProgressDialog
        QProgressDialog *pgs = new QProgressDialog(0);
        pgs->setLabelText(tr("Téléchargement de %1 ...").arg(remoteName));
        pgs->setMinimumDuration(500);
        pgsDialogs.insert(ftp(job), pgs);
        
        QString path;

        if (toTemp)
        {
                path = QString("/tmp/") + tempName;
        }
        else
        {
                path = tempName;
        }

        //Créer ou ouvrir le fichier temporaire
        QFile *fl = new QFile(path, this);
        
        if (!fl->open(QIODevice::WriteOnly)) return false;
        
        //Télécharger le fichier
        int id = ftp(job)->get(remoteName, fl);
        
        int rs = waitCommand(id);
        
        //La commande est terminée, on peut se débarrasser du fichier et de la progress dialog
        delete fl;
        pgsDialogs.remove(ftp(job));
        delete pgs;
        
        return (rs == 0);
}

bool LIOFtpProvider::upload(QString& tempName, QString& remoteName, LIOJob *job, bool fromTemp)
{
        qDebug() << "upload" << tempName << remoteName << fromTemp;
        
        //Créer la QProgressDialog
        QProgressDialog *pgs = new QProgressDialog(0);
        pgs->setLabelText(tr("Envoi de %1 ...").arg(remoteName));
        pgs->setMinimumDuration(500);
        pgsDialogs.insert(ftp(job), pgs);
        
        QString path;

        if (fromTemp)
        {
                path = QString("/tmp/") + tempName;
        }
        else
        {
                path = tempName;
        }

        //Créer ou ouvrir le fichier temporaire
        QFile *fl = new QFile(path, this);
        
        if (!fl->open(QIODevice::ReadOnly)) return false;
        
        //Télécharger le fichier
        int id = ftp(job)->put(fl, remoteName);
        
        int rs = waitCommand(id);
        
        //La commande est terminée, on peut se débarrasser du fichier et de la progress dialog
        delete fl;
        pgsDialogs.remove(ftp(job));
        delete pgs;
        
        return (rs == 0);
}

bool LIOFtpProvider::rename(QString& old, QString& mnew, LIOJob *job)
{
        qDebug() << "rename" << old << mnew;
        
        int id = ftp(job)->rename(old, mnew);
        
        return (waitCommand(id) == 0);
}

bool LIOFtpProvider::mkdir(QString& name, LIOJob *job)
{
        qDebug() << "mkdir" << name;
        
        int id = ftp(job)->mkdir(name);
        
        return (waitCommand(id) == 0);
}

bool LIOFtpProvider::rm(QString& name,  LIOJob *job, bool recursive)
{
        qDebug() << "rm" << name << recursive;
        
        if (recursive)
        {
                return rremove(name, job);
        }
        else
        {
                int id = ftp(job)->remove(name);
                
                return (waitCommand(id) == 0);
        }
}

bool LIOFtpProvider::rremove(QString path, LIOJob *job)
{
        qDebug() << "rremove" << path;
        
        if (!cd(path, job)) return false;
        
        //Liste le contenu du répertoire
        QList<QUrlInfo *> list;
        
        listInfos[ftp(job)] = &list;

        int id = ftp(job)->list();
        
        if (waitCommand(id) != 0) return false;
        
        //Explorer la liste et supprimer les fichiers
        foreach(QUrlInfo *inf, list)
        {
                if (inf->isDir())
                {
                        rremove(path + "/" + inf->name(), job);
                }
                else
                {
                        id = ftp(job)->remove(inf->name());
                        
                        if (waitCommand(id) != 0) return false;
                }
        }
        
        if (!cdUp(job)) return false;
        
        return true;
}

bool LIOFtpProvider::exists(QString& name, LIOJob *job)
{
        //FIXME: Non implémenté
        return false;
        
        (void) name;
        (void) job;
}

bool LIOFtpProvider::chmod(QString& name, QFile::Permission permissions, LIOJob *job)
{
        //FIXME: Non supporté
        return false;
        
        (void) name;
        (void) permissions;
        (void) job;
}

bool LIOFtpProvider::chown(QString& file, QString& owner, QString& group, LIOJob *job)
{
        //FIXME: Non supporté
        return false;

        (void)file;
        (void)owner;
        (void)group;
        (void) job;
}

bool LIOFtpProvider::list(QList<QUrlInfo *> *mreturn,  LIOJob *job, bool showHidden)
{
        qDebug() << "list";
        
        listInfos[ftp(job)] = mreturn;

        int id = ftp(job)->list();
        
        //La slot listInfo() a rempli la structure
        return (waitCommand(id) == 0);
        
        (void) showHidden;
}

bool LIOFtpProvider::copy(QString& from, QString& to, LIOJob *job)
{
        //TODO: Implémenter cette fonction
        
        return false;
        
        (void) from;
        (void) to;
        (void) job;
}

bool LIOFtpProvider::move(QString& from, QString& to, LIOJob *job)
{
        //TODO: Implémenter cette fonction
        
        return false;
        
        (void) from;
        (void) to;
        (void) job;
}

Q_EXPORT_PLUGIN2(liolocalprovider, LIOFtpProvider)
