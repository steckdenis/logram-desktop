/*
 * LMimeData.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#include <QRegExp>
#include <QDomDocument>
#include <QDomElement>
#include <QLocale>
#include <QDir>

#include <LMimeData.h>
#include <LDesktopFile.h>

#include <QtDebug>

LMimeData::LMimeData(QObject *parent) : QObject(parent)
{
        //Rien à faire ici
}

LMimeData::LMimeData(const QString &fileName, QObject *parent) : QObject(parent)
{
        setFileName(fileName);
}
                
void LMimeData::setFileName(const QString &fileName)
{
        _fileName = fileName;
        
        mimeCache = QString();  //On n'a plus le type MIME en cache
}

QString LMimeData::fileName() const
{
        return _fileName;
}
                
QString LMimeData::mime()
{
        if (!mimeCache.isNull())
        {
                //On a déjà trouvé le type MIME
                return mimeCache;
        }
        
        //Voir si l'extension n'est pas déjà connue
        QString ext = _fileName.section('.', 1, -1);
        
        if (knownExts.contains(ext))
        {
                return knownExts.value(ext);
        }
        
        //Explorer /usr/share/mime/globs2 à la recherche du MIME (TODO: Le lire aux endroits spécifiés par la norme)
        QFile fl("/usr/share/mime/globs2");
        
        if (!fl.open(QIODevice::ReadOnly))
        {
                return QString();
        }
        
        QRegExp pattern;
        QString ln;
        QStringList parts;
        QString mext;
        
        pattern.setPatternSyntax(QRegExp::Wildcard);
        
        QString fname = _fileName.toLower();
        
        //Lire les lignes du fichier
        while (!fl.atEnd())
        {
                ln = fl.readLine();
                
                if (ln.startsWith('#'))
                {
                        continue;
                }
                
                //Splitter la ligne pour obtenir les enregistrements qu'on veut
                parts = ln.trimmed().split(':');      //Format : priorité:type_mime:pattern
                
                //Mettre en cache l'extension
                mext = parts.at(2).section('.', 1, -1);
                
                knownExts.insert(mext, parts.at(1));
                
                //Vérifier que le pattern convient
                pattern.setPattern(parts.at(2));
                
                //Si le pattern convient, on a trouvé
                if (pattern.exactMatch(fname))
                {
                        mimeCache = parts.at(1);
                        return mimeCache;
                }
        }
        
        return QString();       //On n'a pas trouvé (TODO: lire le contenu du fichier)
}

QString LMimeData::comment()
{
        QString mmime = mime();
        
        //Le commentaire est stocké dans un fichier XML : /usr/share/mime/${mmime}.xml
        QDomDocument doc;
        
        if (!doc.setContent(new QFile("/usr/share/mime/" + mmime + ".xml")))
        {
                return QString();
        }
        
        QDomElement element = doc.documentElement();
        
        //Explorer les enfants
        QDomElement el = element.firstChildElement();
        
        QString locale = QLocale::system().name().section('_', 0, 0);
        QString rs;
        
        while (!el.isNull())
        {
                if (el.tagName() == "comment")
                {
                        //On cherche aussi le commentaire dans la bonne langue
                        if (el.attribute("xml:lang") == locale)
                        {
                                //On a trouvé la bonne langue
                                return el.text();
                        }
                        
                        //Si l'élément n'a pas de langue, on place son contenu dans rs (pour le retourner si
                        //on ne trouve pas la version traduite
                        
                        if (!el.hasAttribute("xml:lang"))
                        {
                                rs = el.text();
                        }
                }
        
                el = el.nextSiblingElement();
        }
        
        return rs;
}

QString LMimeData::iconName()
{
        QString mmime = mime();
        
        //Le fichier /usr/share/mime/generic-icons contient une association "mime:icône", le lire (TODO: Lire icons aussi)
        QFile fl("/usr/share/mime/generic-icons");
        
        if (!fl.open(QIODevice::ReadOnly))
        {
                return QString();
        }
        
        QString ln;
        QStringList parts;
        
        while (!fl.atEnd())
        {
                ln = fl.readLine();
                
                parts = ln.trimmed().split(':');
                
                if (parts.at(0) == mmime)
                {
                        return parts.at(1);
                }
        }
        
        // On n'a pas trouvé, retourner l'icône du type générique
        return mmime.section('/', 0, 0) + "-x-generic";
}

QList<LDesktopFile *> LMimeData::openWith()
{
        QList<LDesktopFile *> rs;
        
        QString mmime = mime();
        
        if (mmime.isEmpty()) return rs;
        
        //Mettre en cache la liste des fichiers .desktop disponibles
        if (desktopFiles.count() == 0)
        {
                findDesktopFiles("/usr/share/applications");
        }
        
        LDesktopFile *fl;
        
        //Explorer les fichiers .desktop
        foreach(QString file, desktopFiles)
        {
                fl = new LDesktopFile(file);
                
                //Si le fichier .desktop peut ouvrir le fichier, l'ajouter à la liste
                QStringList mimes = fl->value("MimeType", false).split(';');
                
                if (mimes.contains(mmime))
                {
                        rs << fl;
                }
                else
                {
                        delete fl;
                }
        }
        
        return rs;
}

void LMimeData::findDesktopFiles(QString path)
{
        QDir dir(path);
        
        QStringList filters;
        filters << "*.desktop";
        
        QStringList files = dir.entryList(filters, QDir::Files);
        QStringList dirs  = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        
        foreach(QString file, files)
        {
                desktopFiles << dir.absoluteFilePath(file);
        }
        
        foreach(QString mdir, dirs)
        {
                findDesktopFiles(dir.absoluteFilePath(mdir));
        }
}
