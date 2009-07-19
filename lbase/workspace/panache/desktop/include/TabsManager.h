/*
 * TabsManager.h
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

#ifndef __TABSMANAGER_H__
#define __TABSMANAGER_H__

#include <QObject>
#include <QList>

#include <IPanacheDesktopPlugin.h>

class TabPluginContainer;
class App;

class TabsManager : public QObject
{
        Q_OBJECT

public:
        TabsManager(App *mapp);

        void addPlugin(IPanacheDesktopPlugin *plugin, QString title, QString icon, QString id);

signals:
        void toDesktop(TabPluginContainer *sender);

public slots:
        void mtoDesktop();
        void mclosed();

private:
        App *app;
        QList<TabPluginContainer *> plugins;
};

#endif
