/*
 * newfile.cpp
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
#include <QPixmap>
#include <QStringList>
#include <QInputDialog>
#include <QUrl>

#include <newfile.h>

#include <LIOJob.h>

#include <QtDebug>

static bool initialized = false;

void NewFilePlugin::init()
{
        //BLoQ gère plusieurs onglets, donc charge plusieurs fois le plugin. Malheureusement, Qt partage un même objet, donc ça pose problème.
        if (!initialized)
        {
                //Construire l'information
                minfo.title       = tr("Créateur de fichiers");
                minfo.description = tr("Création rapide de fichiers depuis des sqelettes");
                minfo.author      = "steckdenis";
                minfo.flags       = ActionShowInMenu | ActionShowInSideBar;

                //Créer le menu
                QDir        dir("/usr/share/logram/newfiles");
                QStringList filter;
                filter << "*.info";
                QAction *act;

                menu = new QMenu(tr("Nouveau fichier"));

                QStringList files = dir.entryList(filter);

                foreach(QString file, files)
                {
                        QStringList parts = file.split('.');
                        QString     part  = parts[0];

                        //pixmap, text, parent
                        QPixmap pix = QPixmap(dir.absoluteFilePath(file) + ".png");
                        act = new QAction(pix, part, this);

                        menu->addAction(act);

                        actionFiles.insert(act, dir.absoluteFilePath(file));
                        
                        //Ajouter l'action dans les action SideBar
                        ActionSideBar *sact = new ActionSideBar;
                        sact->pixmap = pix;
                        sact->title = part;
                        sact->id = actions.count();
                        sact->visible = true;
                        
                        actions.append(sact);

                        connect(act, SIGNAL(triggered()), this, SLOT(triggered()));
                }
                
                initialized = true;
        }
}

void NewFilePlugin::triggered()
{
        QString fileName = actionFiles.value(static_cast<QAction *>(sender()));

        addFile(fileName);
}

void NewFilePlugin::addFile(const QString &fileName)
{
        //Demander à l'utilisateur le nom du fichier
        bool    ok;
        QString fname = QInputDialog::getText(0, tr("Nouveau fichier"), tr("Entrez le nom du fichier à créer"), QLineEdit::Normal, tr("Sans nom"), &ok);

        if (ok)
        {
                QUrl url(path);

                qDebug() << path;

                if (path.startsWith("file://"))
                {
                        //Juste copier le fichier source vers le fichier cible
                        QFile::copy(fileName, url.path() + "/" + fname);
                }
                else
                {
                        //Il faut uploader le fichier
                        LIOJob job;

                        job.setHost(url.scheme() + "://" + url.authority());

                        QString mpath = url.path() + "/" + fname;
                        QString flname = fileName;
                        job.upload(flname, mpath, false);
                }
        }
}

ActionPluginInfo *NewFilePlugin::info()
{
        return &minfo;
}

QAction *NewFilePlugin::menuAction()
{
        return menu->menuAction();
}

void NewFilePlugin::filesClicked(QList<QString>& urls, QList<QUrlInfo *>& infos)
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

        if (!infos.at(0)->isDir())
        {
                menu->menuAction()->setVisible(false);
                
                //Ne rien afficher dans la zone SideBar
                minfo.flags = minfo.flags & (!ActionShowInSideBar);
                
                return;
        }
        
        minfo.flags |= ActionShowInSideBar;     //L'afficher

        path = urls.at(0);

        (void)urls;
        (void)infos;
}

QList<ActionSideBar *> *NewFilePlugin::sidebarActions()
{
        return &actions;
}

void NewFilePlugin::sidebarClicked(int id)
{
        //Pas implémenté
        addFile(actionFiles.values().at(id));
}

Q_EXPORT_PLUGIN2(newfile, NewFilePlugin)
