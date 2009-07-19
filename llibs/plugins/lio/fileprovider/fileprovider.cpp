/*
 * localprovider.cpp
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

#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include "fileprovider.h"

#include <QtDebug>

bool LIOFileProvider::rconnect(QString address, LIOJob *job)
{
        //Pas de serveurs en local
        if (!qdirs.contains(job))
        {
                QDir *dir = new QDir();
                
                qdirs.insert(job, dir);
        }
        
        return true;

        (void)address;
}

QDir *LIOFileProvider::qdir(LIOJob *job)
{
        return qdirs.value(job);
}

bool LIOFileProvider::cd(QString& directory, LIOJob *job)
{
        return qdir(job)->cd(directory);
}

bool LIOFileProvider::cdUp(LIOJob *job)
{
        return qdir(job)->cdUp();
}

QString LIOFileProvider::pwd(LIOJob *job)
{
        return qdir(job)->absolutePath();
}

bool LIOFileProvider::download(QString& remoteName, QString& tempName, LIOJob *job, bool toTemp)
{
        QString path;

        if (toTemp)
        {
                path = QString("/tmp/") + tempName;
        }
        else
        {
                path = tempName;
        }

        return QFile::copy(qdir(job)->filePath(remoteName), path);
}

bool LIOFileProvider::upload(QString& tempName, QString& remoteName, LIOJob *job, bool fromTemp)
{
        QString path;

        if (fromTemp)
        {
                path = QString("/tmp/") + tempName;
        }
        else
        {
                path = tempName;
        }

        return QFile::copy(path, pwd(job) + "/" + remoteName);
}

bool LIOFileProvider::rename(QString& old, QString& mnew, LIOJob *job)
{
        return QFile::rename(qdir(job)->filePath(old), qdir(job)->filePath(mnew));
}

bool LIOFileProvider::mkdir(QString& name, LIOJob *job)
{
        return qdir(job)->mkdir(name);
}

bool LIOFileProvider::rm(QString& name, LIOJob *job, bool recursive)
{
        if (recursive)
        {
                return rremove(qdir(job)->filePath(name), job);
        }
        else
        {
                QFileInfo fi(qdir(job)->filePath(name));

                if (!fi.isDir())
                {
                        return qdir(job)->remove(name);
                }
                else
                {
                        return qdir(job)->rmdir(name);
                }
        }
}

bool LIOFileProvider::rremove(QString path, LIOJob *job)
{
        QStringList dirs, files;
        QDir        mdir(path);

        dirs  = mdir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
        files = mdir.entryList(QDir::Files);

        foreach(QString file, files)
        {
                if (!mdir.remove(file))
                {
                        return false;
                }
        }

        foreach(QString dir, dirs)
        {
                if (!rremove(mdir.filePath(dir), job))
                {
                        return false;
                }
                if (!mdir.rmdir(dir))
                {
                        return false;
                }
        }

        return true;
}

bool LIOFileProvider::exists(QString& name, LIOJob *job)
{
        return qdir(job)->exists(name);
}

bool LIOFileProvider::chmod(QString& name, QFile::Permission permissions, LIOJob *job)
{
        QFile fl(qdir(job)->filePath(name));

        return fl.setPermissions(permissions);
}

bool LIOFileProvider::chown(QString& file, QString& owner, QString& group, LIOJob *job)
{
        //TODO: Implémenter cette fonction
        return false;

        (void)file;
        (void)owner;
        (void)group;
}

bool LIOFileProvider::list(QList<QUrlInfo *> *mreturn, LIOJob *job, bool showHidden)
{
        QStringList entries = qdir(job)->entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | (showHidden ? QDir::Hidden : QDir::Dirs));

        foreach(QString entry, entries)
        {
                QFileInfo fi(qdir(job)->filePath(entry));
                QUrlInfo  *ui;

                ui = new QUrlInfo(
                        entry,
                        (int)fi.permissions(),
                        fi.owner(),
                        fi.group(),
                        fi.size(),
                        fi.lastModified(),
                        fi.lastRead(),
                        fi.isDir(),
                        fi.isFile(),
                        fi.isSymLink(),
                        fi.isReadable(),
                        fi.isWritable(),
                        fi.isExecutable());

                mreturn->append(ui);
        }

        return true;
}

bool LIOFileProvider::copy(QString& from, QString& to, LIOJob *job)
{
        return QFile::copy(from, to);
}

bool LIOFileProvider::move(QString& from, QString& to, LIOJob *job)
{
        return QFile::rename(from, to);
}

Q_EXPORT_PLUGIN2(liolocalprovider, LIOFileProvider)
