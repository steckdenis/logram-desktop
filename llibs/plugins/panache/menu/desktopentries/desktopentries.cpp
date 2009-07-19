/*
 * desktopentries.cpp
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

#include <QPixmap>
#include <QSettings>
#include <QString>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QStyle>
#include <QApplication>

#include <desktopentries.h>
#include <LIconLoader.h>
#include <LDesktopFile.h>

void DesktopEntries::init(MenuManager *parent)
{
        mparent = parent;

        iloader = new LIconLoader();

        //Lire la liste des fichiers .desktop sur le bureau
        QDir        mdir(QDir::homePath() + "/Desktop");
        QStringList nameFilters, files;

        nameFilters << "*.desktop";

        //Obtenir la liste des dossiers et des fichiers
        files = mdir.entryList(nameFilters, QDir::Files);

        //Explorer les fichiers .desktop
        foreach(QString fileName, files)
        {
                LDesktopFile df(mdir.filePath(fileName));

                //Créer l'entry
                MenuEntry *entry = new MenuEntry();

                entry->title = df.name();
                if (df.icon().startsWith('/'))
                {
                        entry->icon = QPixmap(df.icon());
                }
                else
                {
                        entry->icon = iloader->icon(df.icon()).pixmap(48, 48);
                }
                entry->togglable = false;
                entry->separator = false;

                //Ajouter à la liste
                entries.append(entry);
                fileNames.append(mdir.filePath(fileName));
        }

        (void)parent;
}

QList<MenuEntry *> *DesktopEntries::menuEntries()
{
        return &entries;
}

void DesktopEntries::appEvent(XEvent *event)
{
        (void)event;
}

/* Originairement de Cleriot Simon <malgon33@gmail.com> */
void DesktopEntries::itemClicked(int index, bool toggled)
{
        QString fileName = fileNames.at(index);
        QString process;

        QSettings settings(fileName, QSettings::IniFormat);

        if (!settings.value("Desktop Entry/Terminal", false).toBool())
        {
                process = settings.value("Desktop Entry/Exec").toString() + " ";
        }
        else
        {
                process = "xterm " + settings.value("Desktop Entry/Exec").toString();
        }

        QProcess::startDetached(process.replace("%U", ""));

        (void)toggled;
}

void DesktopEntries::itemContextMenu(int index, int x, int y)
{
        (void)index;
        (void)x;
        (void)y;
}

Q_EXPORT_PLUGIN2(desktopentries, DesktopEntries)
