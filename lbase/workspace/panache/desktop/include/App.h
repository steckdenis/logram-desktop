/*
 * App.h
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

#ifndef __APP_H__
#define __APP_H__

#include <QApplication>
#include <QStringList>
#include <QPluginLoader>
#include <QHash>

#include <LConfig.h>
#include <LApplication.h>

class DesktopManager;
class TabsManager;
class MenuManager;
class TaskBar;

class TabPluginContainer;
class DesktopPluginContainer;

class QPluginLoader;

class App : public LApplication
{
        Q_OBJECT

public:
        App(int& argc, char *argv[]);

        DesktopManager *desktopManager();
        TabsManager *tabsManager();
        MenuManager *menuManager();

        LConfig *cfg;

protected:
        virtual bool x11EventFilter(XEvent *event);

public slots:
        void toDesktop(TabPluginContainer *sender);
        void toTab(DesktopPluginContainer *sender);

        void loadPlugins();

private:
        DesktopManager *dma;
        TabsManager    *tma;
        MenuManager    *mma;
        TaskBar        *tbr;

        QHash<QString, QPluginLoader *> knowPlugins;
};

#endif
