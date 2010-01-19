/*
 * App.cpp
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

#include <QSettings>
#include <QPluginLoader>
#include <QDir>
#include <QtDBus>

#include <TabPluginContainer.h>
#include <DesktopPluginContainer.h>
#include <IPanacheDesktopPlugin.h>

#include <App.h>
#include <DesktopManager.h>
#include <TabsManager.h>
#include <MenuManager.h>
#include <TaskBar.h>

App::App(int& argc, char *argv[]) : LApplication(argc, argv)
{
        cfg = new LConfig(QString("Panache-desktop"), this);

        dma = new DesktopManager(this);
        connect(dma, SIGNAL(toTab(DesktopPluginContainer *)), this, SLOT(toTab(DesktopPluginContainer *)));

        tma = new TabsManager(this);
        connect(tma, SIGNAL(toDesktop(TabPluginContainer *)), this, SLOT(toDesktop(TabPluginContainer *)));

        mma = new MenuManager(this);
        
        //Connect D-Bus signal wich allow reloading on the fly plugins' list
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.connect(QString(), QString(), "org.logram-project.panache", "updateDesktopPlugins", this, SLOT(loadPlugins()));

        loadPlugins();

        //Application is loaded
        appLoaded("panache-desktop");
}

void App::loadPlugins()
{
        QSettings settings(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat, this);

        //Browse file and add required plugins
        QStringList groups = settings.childGroups();

        foreach(QString plugin, groups)
        {
                //Add unknow plugins

                if (!knowPlugins.contains(plugin))
                {
                        settings.beginGroup(plugin);

                        bool isTab = settings.value("IsTab", true).toBool();

                        //Load plugin
                        QPluginLoader *loader  = new QPluginLoader(QString("/usr/lib/") + settings.value("FileName").toString());
                        QObject       *mplugin = 0;
                        mplugin = loader->instance();
                        IPanacheDesktopPlugin *nplugin = 0;

                        if (mplugin == 0)
                        {
                                return;
                        }

                        nplugin = qobject_cast<IPanacheDesktopPlugin *>(mplugin);

                        //Properly load plugin
                        nplugin->load();

                        QString title = settings.value("Title").toString();
                        QString icon  = settings.value("Icon").toString();

                        if (isTab)
                        {
                                tma->addPlugin(nplugin, title, icon, plugin);
                        }
                        else
                        {
                                if (dma->exists)
                                {
                                        dma->addPlugin(nplugin, title, icon, plugin);
                                }
                                else
                                {
                                        qWarning("Add a plugin on a non-existant desktop ?");
                                }
                        }

                        knowPlugins.insert(plugin, loader);

                        settings.endGroup();
                }
        }

        //Delete useless plugins
        foreach(QString pluginName, knowPlugins.keys())
        {
                if (!groups.contains(pluginName))
                {
                        //Delete plugin
                        QPluginLoader *loader = knowPlugins.take(pluginName);

                        loader->unload();
                }
        }
}

void App::toDesktop(TabPluginContainer *sender)
{
        if (!dma->exists)
        {
                qWarning("Add a plugin on a non-existant desktop ?");
                return;
        }
        
        //Fetch plugin
        IPanacheDesktopPlugin *plugin = sender->getPlugin();

        //Delete plugin's widgets
        plugin->unlinkParent();

        //Add to DesktopManager
        dma->addPlugin(plugin, sender->mtitle, sender->micon, sender->tabName);
}

void App::toTab(DesktopPluginContainer *sender)
{
        //Fetch Plugin
        IPanacheDesktopPlugin *plugin = sender->getPlugin();

        //Delete plugin's widgets
        plugin->unlinkParent();

        //Add to TabsManager
        tma->addPlugin(plugin, sender->mtitle, sender->micon, sender->tabName);
}

DesktopManager *App::desktopManager()
{
        return dma;
}

TabsManager *App::tabsManager()
{
        return tma;
}

MenuManager *App::menuManager()
{
        return mma;
}

bool App::x11EventFilter(XEvent *event)
{
        mma->appEvent(event);
        
        return false;
}
