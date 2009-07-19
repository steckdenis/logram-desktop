/*
 * openwith.cpp
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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QProcess>
#include <QUrl>
#include <QIcon>

#include <openwith.h>

#include <LIOJob.h>

#include <QtDebug>

static bool initialized = false;

void OpenWithPlugin::init()
{
        //BLoQ gère plusieurs onglets, donc charge plusieurs fois le plugin. Malheureusement, Qt partage un même objet, donc ça pose problème.
        if (!initialized)
        {
                job = 0;
                mime = new LMimeData(this);
                iloader = new LIconLoader(QString(), this);
                
                //Construire l'information
                minfo.title       = tr("Ouvrir avec");
                minfo.description = tr("Ouvre le fichier sélectionné avec une application");
                minfo.author      = "steckdenis";
                minfo.flags       = ActionShowInMenu | ActionShowInSideBar;
                minfo.pixmap      = iloader->icon("document-open").pixmap(16, 16);

                //Créer le menu
                menu = new QMenu(tr("Ouvrir avec"));
                
                initialized = true;
        }
}

void OpenWithPlugin::triggered()
{
        LDesktopFile *desk = actionFiles.value(static_cast<QAction *>(sender()));

        openFile(desk);
}

void OpenWithPlugin::openFile(LDesktopFile *desk)
{
        //Télécharger le fichier si besoin
        QString path;
        
        if (arg.startsWith("file://"))
        {
                path = arg;
                
                path.remove(0, 7);
        }
        else
        {
                //On doit malheureusement télécharger le fichier
                if (job == 0)
                {
                        job = new LIOJob(this);
                }
                
                QUrl url(arg);
                QString host = url.scheme() + "://" + url.authority();
                
                if (job->host() != host)
                {
                        job->setHost(host);
                }
                
                path = job->localPath(arg);
        }
        
        //Ouvrir le fichier avec l'application
        QProcess::startDetached(desk->exec().replace("%u", path, Qt::CaseInsensitive));
}

ActionPluginInfo *OpenWithPlugin::info()
{
        return &minfo;
}

QAction *OpenWithPlugin::menuAction()
{
        return menu->menuAction();
}

void OpenWithPlugin::filesClicked(QList<QString>& urls, QList<QUrlInfo *>& infos)
{
        //Vérifier que c'est bien dans un dossier qu'on va ajouter les fichiers
        menu->menuAction()->setVisible(true);

        if (urls.count() != 1)
        {
                menu->menuAction()->setVisible(false);
                
                //Ne rien afficher dans la zone SideBar
                minfo.flags = minfo.flags & (!ActionShowInSideBar);
                
                return;
        }

        if (infos.at(0)->isDir())
        {
                menu->menuAction()->setVisible(false);
                
                //Ne rien afficher dans la zone SideBar
                minfo.flags = minfo.flags & (!ActionShowInSideBar);
                
                return;
        }
        
        minfo.flags |= ActionShowInSideBar;     //L'afficher

        //Télécharger le fichier si distant
        arg = urls.at(0);
        
        //En fonction du MIME du fichier, afficher les actions possibles
        mime->setFileName(urls.at(0));          //Pas besoin de DL, LMimeData ne se sert que du nom de fichier (FIXME: pour le moment)
        
        //Effacer la précédante liste de .desktop
        foreach(QAction *key, actionFiles.keys())
        {
                LDesktopFile *desk = actionFiles.take(key);
                
                delete desk;
        }
        
        actionFiles.clear();
        
        desktops = mime->openWith();
        
        //Explorer les .desktop et les ajouter au menu et à la liste des Actions
        menu->clear();
        actions.clear();
        
        foreach(LDesktopFile *desk, desktops)
        {
                //Créer l'entrée
                QAction *a = menu->addAction(iloader->icon(desk->icon()), desk->name(), this, SLOT(triggered()));
                
                actionFiles.insert(a, desk);
                
                //Créer l'action
                ActionSideBar *act = new ActionSideBar;
                
                act->pixmap = iloader->icon(desk->icon()).pixmap(16, 16);
                act->title = desk->name();
                act->description = tr("Ouvre le fichier %1 avec %2").arg(urls.at(0)).arg(desk->name());
                act->visible = true;
                act->id = actions.count();
                
                actions.append(act);
        }

        (void)urls;
        (void)infos;
}

QList<ActionSideBar *> *OpenWithPlugin::sidebarActions()
{
        return &actions;
}

void OpenWithPlugin::sidebarClicked(int id)
{
        openFile(desktops.at(id));
}

Q_EXPORT_PLUGIN2(openwith, OpenWithPlugin)
