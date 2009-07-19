/*
 * LIOJob.cpp
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

#include <QString>
#include <QPluginLoader>
#include <QUrl>
#include <QCryptographicHash>
#include <QFileSystemWatcher>

#include <LIOJob.h>
#include "../plugins/lio/include/ILIOProvider.h"

#include <QtDebug>

LIOJob::LIOJob(QObject *parent) : QObject(parent)
{
        provider = 0;
        watch    = new QFileSystemWatcher(this);

        connect(watch, SIGNAL(fileChanged(const QString &)), this, SLOT(fileChanged(const QString &)));

        _host = QString();
}

bool LIOJob::setHost(QString mhost)
{
        QString _mhost;
        QString mprovider;

        if (mhost.isNull())
        {
                mprovider = "file";
        }
        else
        {
                QUrl url(mhost);

                mprovider = url.scheme();
                _mhost    = url.authority();
        }

        if (!loadProvider(mprovider))
        {
                return false;
        }
        if (!provider->rconnect(_mhost, this))
        {
                return false;
        }

        _host = mprovider + "://" + _mhost; // "ftp://truc.com" ou "file://"

        return true;
}

QString& LIOJob::host()
{
        return _host;
}

bool LIOJob::cd(QString& directory)
{
        if (_host.isNull())
        {
                return false;
        }

        provider->cd(directory, this);

        return true;
}

bool LIOJob::loadProvider(QString mprovider)
{
        if (cProvider == mprovider)
        {
                return true;
        }

        //Charger le plugin
        QPluginLoader loader(QString("/usr/lib/liblio") + mprovider + "provider.so");
        QObject       *mplugin = loader.instance();
        provider = 0;
        if (mplugin)
        {
                provider = qobject_cast<ILIOProvider *>(mplugin);
        }
        else
        {
                return false;
        }

        return true;
}

bool LIOJob::cdUp()
{
        return provider->cdUp(this);
}

QString LIOJob::pwd()
{
        return provider->pwd(this);
}

bool LIOJob::download(QString& remoteName, QString& tempName, bool toTemp)
{
        return provider->download(remoteName, tempName, this, toTemp);
}

bool LIOJob::upload(QString& tempName, QString& remoteName, bool fromTemp)
{
        return provider->upload(tempName, remoteName, this, fromTemp);
}

QString LIOJob::localPath(const QString& url)
{
        // Si le fichier est local, alors directement retourner l'adresse
        QString turl = url;

        if (url.startsWith('/'))
        {
                return url;
        }
        else if (url.startsWith("file://"))
        {
                return turl.remove(0, 7);       // file:///home ==> /home
        }

        // Voir si le fichier n'est pas déjà surveillé
        if (watchedFiles.contains(url))
        {
                return watchedFiles.value(url);
        }
        
        // Récupérer l'extension de l'url
        QString ext = url.section('/', -1).section('.', 1);

        // Générer le hash md5 de l'url pour stocker le fichier temporaire
        QString hashed   = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex();
        QString fileName = QString("/tmp/") + hashed + "." + ext;

        // Ajouter le fichier à la liste
        watchedFiles.insert(url, fileName);

        // Télécharger le fichier
        QUrl    murl(url);
        QString mhost = murl.scheme() + "://" + murl.authority();
        QString ahost;

        if (mhost != host())
        {
                //On doit se reconnecter (TODO: Rendre ça Thread Safe)
                ahost = host();
                setHost(mhost);
        }

        // Downloader le fichier
        download(turl.remove(mhost), fileName, false);
        
        
        // Créer le QFileSystemWatcher
        watch->addPath(fileName);

        // Rétablir l'host
        if (!ahost.isNull())
        {
                setHost(ahost);
        }

        // Retourner le nom de fichier temporaire
        return fileName;
}

void LIOJob::fileChanged(const QString& path)
{
        // Un fichier a été modifié, le renvoyer
        QString url = watchedFiles.key(path);

        // Prendre l'host de l'url, et le comparer avec l'host du LIOJob, pour pouvoir en changer si nécessaire
        QUrl    murl(url);
        QString mhost = murl.scheme() + "://" + murl.authority();
        QString ahost;

        if (mhost != host())
        {
                //On doit se reconnecter (TODO: Rendre ça Thread Safe)
                ahost = host();
                setHost(mhost);
        }

        // Uploader le fichier
        QString pth = path;
        upload(pth, url.remove(mhost), false);

        // Rétablir l'host
        if (!ahost.isNull())
        {
                setHost(ahost);
        }
}

bool LIOJob::rename(QString& old, QString& mnew)
{
        return provider->rename(old, mnew, this);
}

bool LIOJob::mkdir(QString& name)
{
        return provider->mkdir(name, this);
}

bool LIOJob::rm(QString& name, bool recursive)
{
        return provider->rm(name, this, recursive);
}

bool LIOJob::exists(QString& name)
{
        return provider->exists(name, this);
}

bool LIOJob::chmod(QString& name, QFile::Permission permissions)
{
        return provider->chmod(name, permissions, this);
}

bool LIOJob::chown(QString& file, QString& owner, QString& group)
{
        return provider->chown(file, owner, group, this);
}

bool LIOJob::list(QList<QUrlInfo *> *mreturn, bool showHidden)
{
        return provider->list(mreturn, this, showHidden);
}

bool LIOJob::copy(QString& from, QString& to)
{
        return provider->copy(from, to, this);
}

bool LIOJob::move(QString& from, QString& to)
{
        return provider->move(from, to, this);
}
