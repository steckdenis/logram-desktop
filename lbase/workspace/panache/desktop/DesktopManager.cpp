/*
 * DesktopManager.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Denis Steckelmacher <steckdenis@yahoo.fr>
 *		 2009 - Leo Testard <leo.testard@gmail.com>
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

#include <QPluginLoader>
#include <QPoint>
#include <QDir>
#include <QSize>
#include <QProcess>
#include <QPalette>
#include <QCoreApplication>
#include <QString>
#include <DesktopManager.h>
#include <DesktopPluginContainer.h>
#include <App.h>
#include <LConfig.h>

DesktopManager::DesktopManager(App *mapp) : QObject(mapp)
{
        app = mapp;

        //Check if the user wants to show the desktop
        if (QCoreApplication::arguments().contains("nodesktop"))
        {
                exists = false;
                return;
        }
        
        exists = true;
        
        //Load designer desktop plugin
        QPluginLoader loader(LConfig::logramValue("Desktop/DecoratorPlugin", "/usr/lib/libparadisesun.so", "Theme").toString());
        QObject       *mplugin = loader.instance();
        plugin = 0;
        if (mplugin)
        {
                plugin = qobject_cast<IPanacheDesktopDecoratorPlugin *>(mplugin);

                desktopWidget = plugin->Initialize();
        }

        view  = new LDirView(QDir::homePath() + "/Bureau", true, false, desktopWidget);
        int               width  = LConfig::logramValue("Desktop/ItemWidth", 48, "Theme").toInt();
        int               height = LConfig::logramValue("Desktop/ItemHeight", 48, "Theme").toInt();
        QAbstractItemView *lv    = view->view();

        view->setItemSize(QSize(width, height));
        view->setDir(LConfig::logramValue("Desktop/homeDir", QDir::homePath() + "/Bureau", "Theme").toString());
	view->move(0, 30);

        lv->setGeometry(desktopWidget->geometry());

        QPalette p = lv->palette();
        p.setBrush(QPalette::Window, Qt::transparent);
        p.setBrush(QPalette::Base, Qt::transparent);
        p.setBrush(QPalette::Dark, Qt::transparent);
        lv->setPalette(p);
        lv->setAttribute(Qt::WA_NoSystemBackground, true);
        lv->setAutoFillBackground(false);
        
        connect(view, SIGNAL(openDir(QString, bool *)), this, SLOT(openDir(QString, bool *)));

	toolBar = new ToolBar(desktopWidget);
	connect(toolBar, SIGNAL(go(QString)), this, SLOT(go(QString)));
	connect(toolBar, SIGNAL(goHome()), this, SLOT(home()));
	connect(toolBar, SIGNAL(refresh()), this, SLOT(reload()));
	connect(toolBar, SIGNAL(goBack()), this, SLOT(back()));
	connect(toolBar, SIGNAL(goNext()), this, SLOT(next()));
	connect(toolBar, SIGNAL(goUp()), this, SLOT(cdup()));
	
	connect(view, SIGNAL(dirChanged(QString)), this, SLOT(changeAdress(QString)));

        lv->show();
        view->show();
	toolBar->show();
}

void DesktopManager::addPlugin(IPanacheDesktopPlugin *plugin, QString title, QString icon, QString id)
{
        DesktopPluginContainer *mplugin = new DesktopPluginContainer(title, icon, id, plugin, app);

        QPoint pos = mplugin->pos();

        mplugin->setParent(desktopWidget);
        mplugin->show();
        mplugin->move(pos);

        connect(mplugin, SIGNAL(toTab()), this, SLOT(mtoTab()), Qt::QueuedConnection);
        connect(mplugin, SIGNAL(closed()), this, SLOT(mclosed()), Qt::QueuedConnection);

        plugins.append(mplugin);
}

void DesktopManager::mtoTab()
{
        DesktopPluginContainer *mplugin = qobject_cast<DesktopPluginContainer *>(sender());

        emit toTab(mplugin);

        plugins.removeOne(mplugin);

        delete mplugin;
}

void DesktopManager::mclosed()
{
        DesktopPluginContainer *mplugin = qobject_cast<DesktopPluginContainer *>(sender());

        //Delete plugin from loading list
        QSettings tabInfos(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat);

#include <ToolBar.h>

        tabInfos.remove(mplugin->tabName);

        //Delete plugin and tab
        IPanacheDesktopPlugin *nplugin = mplugin->getPlugin();
        nplugin->unlinkParent();

        plugins.removeOne(mplugin);
        delete mplugin;
}

void DesktopManager::openDir(QString path, bool *cancel)
{
        //On ouvre un fichier dans le LDirView, l'ouvrir dans BLoQ
        if (path.startsWith("file://"))
        {
                QString pth = path.remove(0, 7);
		
                view->setDir(pth);
        }

        *cancel = true;
}

void DesktopManager::openParentDir(QString path, bool *cancel)
{
	while (1) {
	if (path.endsWith("/"))
	{
		QString parentPath = path;
		view->setDir(parentPath);
		break;
	}
	else
	{	
		QString parentPath = path.remove(0, 1);
	} }
}

void DesktopManager::changeAdress(QString path)
{
	toolBar->changeAdress(path);
}

void DesktopManager::go(QString adress)
{
	view->setDir(adress);
}

void DesktopManager::home()
{
	view->setDir(QDir::homePath() + "/Bureau");
}

void DesktopManager::reload()
{
	QString pth = view->dir();
	view->setDir(pth);
}

void DesktopManager::back()
{
        //Retour
        view->undoStack()->undo();

        //updateUndoButtons();
}

void DesktopManager::next()
{
        view->undoStack()->redo();

        //updateUndoButtons();
}

void DesktopManager::cdup()
{
        QStringList parts = view->dir().split('/');

        parts.removeAt(parts.count() - 1);                //Retirer le dernier élément
        view->setDir(parts.join("/"));                      //On a retiré le dossier, en envoie
}
/*
void DesktopManager::updateUndoButtons()
{
        //Mettre à jour l'état des boutons de retour en fonction de la possibilité d'annuler/refaire

        toolBar.->setEnabled(view->undoStack()->canUndo());
        mnext->setEnabled(view->undoStack()->canRedo());
}*/