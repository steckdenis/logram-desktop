/*
 * DesktopManager.h
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

#ifndef __DESKTOPMANAGER_H__
#define __DESKTOPMANAGER_H__

#include <QObject>
#include <QWidget>
#include <LDirView.h>
#include <IPanacheDesktopDecoratorPlugin.h>
#include <DesktopPluginContainer.h>
#include <ToolBar.h>


class App;

class DesktopManager : public QObject
{
        Q_OBJECT

public:
        DesktopManager(App *mapp);
        void addPlugin(IPanacheDesktopPlugin *plugin, QString title, QString icon, QString id);
        //void updateUndoButtons();
        bool exists;

signals:
        void toTab(DesktopPluginContainer *sender);

public slots:
        void mtoTab();
        void mclosed();
        void openDir(QString path, bool *cancel);
	void openParentDir(QString path, bool *cancel);
	void go(QString);
	void changeAdress(QString path);
	void home();
	void reload();
	void back();
	void next();
	void cdup();
	
private:
        App *app;
        IPanacheDesktopDecoratorPlugin *plugin;
        QWidget *desktopWidget;
	LDirView *view;
        QList<DesktopPluginContainer *> plugins;
	ToolBar *toolBar;
};

#endif
