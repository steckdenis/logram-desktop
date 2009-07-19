/*
 * LDirTree.cpp
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

#include <QUrl>
#include <QModelIndex>
#include <QItemSelectionModel>

#include <LDirTree.h>

LDirTree::LDirTree(LDirView *view, QWidget *parent) : QTreeView(parent)
{
        _job   = 0;
        _model = 0;
        _view  = 0;

        setHeaderHidden(true);
        setEditTriggers(SelectedClicked);

        setView(view);

        //Cacher toutes les colonnes sauf le nom
        for (int i = 1; i < _model->columnCount(QModelIndex()); i++)
        {
                setColumnHidden(i, true);
        }

        //Connecter un signal
        connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemActivated(const QModelIndex &)));

        //Créer le menu contextuel
        actRoot = new QAction(tr("Élément principal"), this);
        connect(actRoot, SIGNAL(triggered(bool)), this, SLOT(rootClicked(bool)));

        actUnroot = new QAction(tr("Afficher tout l'arbre"), this);
        connect(actUnroot, SIGNAL(triggered(bool)), this, SLOT(unrootClicked(bool)));
        actUnroot->setVisible(false);

        addAction(actRoot);
        addAction(actUnroot);

        setContextMenuPolicy(Qt::ActionsContextMenu);
}

void LDirTree::rootClicked(bool)
{
        QModelIndex item = selectionModel()->selectedIndexes()[0];

        setRootIndex(item);
        actUnroot->setVisible(true);
}

void LDirTree::unrootClicked(bool)
{
        setRootIndex(QModelIndex());
        actUnroot->setVisible(false);
}

LIOJob *LDirTree::job() const
{
        return _job;
}

LDirModel *LDirTree::model() const
{
        return _model;
}

LDirView *LDirTree::view() const
{
        return _view;
}

void LDirTree::setView(LDirView *view)
{
        if (_view != 0)
        {
                //Déconnecter proprement les signaux
                disconnect(_view, SIGNAL(dirChanged(const QString &)), this, SLOT(setDir(const QString &)));
                disconnect(this, SIGNAL(dirChanged(const QString &)), _view, SLOT(setDir(const QString &)));
        }

        _view = view;
        _job  = view->job();

        //Reconnecter les signaux
        connect(_view, SIGNAL(dirChanged(const QString &)), this, SLOT(setDir(const QString &)));
        connect(this, SIGNAL(dirChanged(const QString &)), _view, SLOT(setDir(const QString &)));

        //Charger le modèle, etc
        setDir(view->dir());
}

void LDirTree::setDir(const QString& dir)
{
        QString mhost;

        if (dir.startsWith('/') || dir.startsWith("file://"))
        {
                mhost = "file://";
        }
        else
        {
                QUrl url(dir);

                mhost = url.scheme() + "://" + url.authority();
        }

        //Si on a changé d'host, prendre le bon Job (celui de la vue), et reconstruire le modèle
        if (mhost != _host)
        {
                _job = _view->job();

                QString rootPath("/");

                _model = new LDirModel(_job, rootPath, false, false, this);

                //Assigner le modèle à la vue
                setModel(_model);
        }

        _host = mhost;
}

void LDirTree::itemActivated(const QModelIndex& index)
{
        QString dir  = _model->itemPath(index);
        QString mdir = _host + dir;

        emit dirChanged(mdir);
}
