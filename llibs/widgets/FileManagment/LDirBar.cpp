/*
 * LDirBar.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#include <QStringList>
#include <QUrl>
#include <QApplication>
#include <QtDebug>

#include <LDirBar.h>

LDirBar::LDirBar(LDirView *view, QWidget *parent) : QToolBar(parent)
{
        _view      = 0;
        rootAction = 0;

        if (view != 0)
        {
                setView(view);
        }
}

void LDirBar::setView(LDirView *view)
{
        if (_view != 0)
        {
                //Déconnecter l'ancien View
                disconnect(_view, SIGNAL(dirChanged(const QString &)), this, SLOT(setDir(const QString &)));
                disconnect(this, SIGNAL(dirChanged(const QString &)), _view, SLOT(setDir(const QString &)));
        }

        _view = view;

        connect(_view, SIGNAL(dirChanged(const QString &)), this, SLOT(setDir(const QString &)));
        connect(this, SIGNAL(dirChanged(const QString &)), _view, SLOT(setDir(const QString &)));

        //Réactualiser les boutons
        setDir(_view->dir());
}

LDirView *LDirBar::view() const
{
        return _view;
}

void LDirBar::setDir(const QString& url)
{
        QStringList elements;
        QString     mhost, element;
        QPushButton *btn;

        if (url.startsWith('/') || url.startsWith("file://"))
        {
                mhost    = "file://";
                elements = url.split('/', QString::SkipEmptyParts);

                if (elements.count() > 0)
                {
                        if (elements.at(0) == "file:")
                        {
                                elements.removeAt(0);
                        }
                }
        }
        else
        {
                QUrl murl(url);

                mhost = murl.scheme() + "://" + murl.authority();

                elements = murl.path().split('/', QString::SkipEmptyParts);
        }

        //On doit afficher l'élément «/»
        //elements.prepend("/");

        //Changer l'icône (ou la créer) de l'élément root
        if (mhost != host)
        {
                if (rootAction == 0)
                {
                        rootAction = new QPushButton(this);
                        rootAction->setCheckable(true);

                        connect(rootAction, SIGNAL(clicked(bool)), this, SLOT(rootClicked(bool)));

                        addWidget(rootAction)->setVisible(true);
                }

                //Adapter l'icône en fonction du host
                if (mhost == "file://")
                {
                        rootAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon));
                }
                else
                {
                        rootAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveNetIcon));
                }
                //TODO: Également gérer des icônes pour les archives, les CD, les pages man, etc

                rootAction->setText(mhost);
                host = mhost;
        }

        //Explorer les éléments et actualiser les boutons
        for (int i = 0; i < elements.count(); i++)
        {
                element = elements.at(i);

                if (i >= buttons.count())
                {
                        //Il faut ajouter un bouton
                        btn = new QPushButton(element, this);

                        buttons.append(btn);
                        addWidget(btn)->setVisible(true);
                        btn->setCheckable(true);
                        btn->show();

                        //Connecter les signaux du bouton
                        connect(btn, SIGNAL(clicked(bool)), this, SLOT(elementClicked(bool)));
                }
                else
                {
                        //S'assurer que le bouton a le bon texte
                        btn = buttons.at(i);

                        btn->setText(element);

                        if (i != elements.count() - 1)
                        {
                                btn->setChecked(false);
                        }
                }
        }

        //Supprimer les boutons en trop
        while (buttons.count() > elements.count())
        {
                delete buttons.takeAt(buttons.count() - 1);
        }

        //Le dernier bouton doit être checké
        if (buttons.count() != 0)
        {
                buttons.at(buttons.count() - 1)->setChecked(true);
                rootAction->setChecked(false);
        }
        else
        {
                rootAction->setChecked(true);
        }
}

void LDirBar::elementClicked(bool toggled)
{
        QPushButton *btn  = static_cast<QPushButton *>(sender());
        int         index = buttons.indexOf(btn);

        QString path;

        //Trouver les boutons avant celui cliqué, pour reconstituer le chemin
        for (int i = 0; i <= index; i++)
        {
                path += "/" + buttons.at(i)->text();
        }

        _view->setDir(host + path);
}

void LDirBar::rootClicked(bool toggled)
{
        _view->setDir(host + "/");
}
