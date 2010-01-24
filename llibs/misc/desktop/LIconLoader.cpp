/*
 * LIconLoader.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *                      Inpiré par QtIconLoader (C) Nokia Corporation
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

#include <QFile>
#include <QDir>
#include <QString>
#include <QSettings>
#include <QList>
#include <QDebug>

#include <LIconLoader.h>

LIconLoader::LIconLoader(QString theme, QObject *parent) : QObject(parent)
{
        //Trouver les dossiers qui contiennent les thèmes
        QString dataDirs = QFile::decodeName(getenv("XDG_DATA_DIRS"));

        if (dataDirs.isEmpty())
        {
                dataDirs = "/usr/share:/usr/local/share";
        }

        dataDirs.prepend(QDir::homePath() + ":");

        dirs = dataDirs.split(':');

        //Explorer le thème
        if (theme.isNull())
        {
                theme = LConfig::logramValue("IconTheme", "logram", Theme").toString();
        }

        exploreThemes(theme);
}

QIcon LIconLoader::icon(const QString& name)
{
        if (name.startsWith('/'))
        {
                return QIcon(name);
        }
        else if (name.endsWith(".png"))
        {
                return QIcon(QString("/usr/share/pixmaps/") + name);
        }

        QList<int> sizes;

        sizes << 64 << 32 << 24 << 16;

        //Trouver l'icône la plus grande qui correspond
        foreach(int size, sizes)
        {
                foreach(QString dir, themedirs.values(size))
                {
                        QString iconName = dir + name + ".png";

                        if (QFile::exists(iconName))
                        {
                                return QIcon(QPixmap(iconName));
                        }
                }
        }

        return QIcon();
}

void LIconLoader::exploreThemes(QString& themeName)
{
        //Explorer les dossiers à la recherche du nom du thème
        QFile       theme;
        QStringList parents;
        QString     themeRoot;

        for (int i = 0; i < dirs.count() && !theme.exists(); i++)
        {
                QString dirname = (dirs[i].startsWith(QDir::homePath()) ? "/.icons/" : "/icons/");

                themeRoot = dirs[i] + dirname + themeName + "/";
                theme.setFileName(themeRoot + "index.theme");
        }

        //Lire le fichier de thème
        if (theme.exists())
        {
                //Lire la liste des dossiers
                QSettings set(theme.fileName(), QSettings::IniFormat);

                Q_FOREACH(const QString &key, set.allKeys())
                {
                        if (key.endsWith("/Size"))
                        {
                                if (int size = set.value(key).toInt())
                                {
                                        themedirs.insertMulti(size, themeRoot + key.left(key.size() - 5) + "/");
                                }
                        }
                }

                //Obtenir la liste des thèmes parents
                //Utiliser un QFile au lieu du QSettings, car le Inherits peut avoir une forme incompatible avec QSettings
                QString line;
                theme.open(QIODevice::ReadOnly | QIODevice::Text);

                while (!theme.atEnd())
                {
                        line = theme.readLine();
                        if (line.startsWith("Inherits"))
                        {
                                line.remove('"');
                                QStringList list = line.split('=');
                                parents = list.at(1).trimmed().split(',');
                        }
                }
                theme.close();
        }

        //Explorer les thèmes parents
        QString hcolor("hicolor");

        if ((themeName != hcolor) && (!parents.contains(hcolor)))
        {
                parents.append(hcolor);
        }

        foreach(QString parent, parents)
        {
                exploreThemes(parent);
        }
}
