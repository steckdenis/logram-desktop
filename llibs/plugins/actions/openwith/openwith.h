/*
 * openwith.h
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

#ifndef __OPENWITH_H__
#define __OPENWITH_H__

#include <QObject>
#include <QMenu>
#include <QHash>

#include <IActionPlugin.h>
#include <LMimeData.h>
#include <LIconLoader.h>

#include <LIOJob.h>

class OpenWithPlugin : public QObject, public IActionPlugin
{
        Q_OBJECT
        Q_INTERFACES(IActionPlugin)

        public :
                void init();
                ActionPluginInfo *info();
                QAction *menuAction();
                void filesClicked(QList<QString>& urls, QList<QUrlInfo *>& infos);

                QList<ActionSideBar *> *sidebarActions();
                void sidebarClicked(int id);
                
                void openFile(LDesktopFile *desk);

        private slots:
                void triggered();

        private:
                ActionPluginInfo minfo;
                QMenu            *menu;
                LIOJob           *job;
                LIconLoader      *iloader;
                QHash<QAction *, LDesktopFile *> actionFiles;
                
                QList<ActionSideBar *>   actions;
                QList<LDesktopFile *> desktops;
                
                QString arg;
                LMimeData *mime;
};

#endif
