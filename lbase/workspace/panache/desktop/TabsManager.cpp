/*
 * TabsManager.cpp
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

#include <QDir>

#include <TabsManager.h>
#include <TabPluginContainer.h>
#include <SubTab.h>

#include <IPanacheDesktopPlugin.h>
#include <App.h>

TabsManager::TabsManager(App *mapp) : QObject(mapp)
{
        app = mapp;
}

void TabsManager::addPlugin(IPanacheDesktopPlugin *plugin, QString title, QString icon, QString id)
{
        TabPluginContainer *mplugin = new TabPluginContainer(title, icon, id, plugin, app);

        connect(mplugin, SIGNAL(toDesktop()), this, SLOT(mtoDesktop()), Qt::QueuedConnection);
        connect(mplugin, SIGNAL(closed()), this, SLOT(mclosed()), Qt::QueuedConnection);

        plugins.append(mplugin);
}

void TabsManager::mtoDesktop()
{
        TabPluginContainer *mplugin = qobject_cast<TabPluginContainer *>(sender());

        emit toDesktop(mplugin);

        plugins.removeOne(mplugin);

        delete mplugin;
}

void TabsManager::mclosed()
{
        TabPluginContainer *mplugin = qobject_cast<TabPluginContainer *>(sender());

        //Delete plugin from loading list
        QSettings tabInfos(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat);

        tabInfos.remove(mplugin->tabName);

        //Delete tab and plugin
        IPanacheDesktopPlugin *nplugin = mplugin->getPlugin();
        nplugin->unlinkParent();

        plugins.removeOne(mplugin);
        delete mplugin;
}
