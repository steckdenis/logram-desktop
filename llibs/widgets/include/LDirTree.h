/*
 * LDirTree.h
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

#ifndef __LDIRTREE_H__
#define __LDIRTREE_H__

#include <QTreeView>
#include <QMenu>
#include <QAction>

#include <LDirModel.h>
#include <LDirView.h>
#include <LIOJob.h>

/**
 * \class LDirTree
 * \brief Affiche un arbre des dossiers
 *
 * S'utilise conjointement à LDirView, pour faciliter la navigation et le repérage dans les dossiers d'un ordinateur.
 *
 * Comme LDirView, LDirTree utilise un LIOJob pour accéder aux données
 */
class LDirTree : public QTreeView
{
        Q_OBJECT

public:

        /**
         * \brief Construit un LDirTree
         * \param view LDirView à utiliser pour se connecter
         * \param parent QWidget parent
         */
        LDirTree(LDirView *view, QWidget *parent = 0);

        /**
         * \brief Assigne un LDirView au LDirTree
         * \param view le LDirView
         */
        void setView(LDirView *view);

        /**
         * \brief Retourne le LIOJob utilisé
         */
        LIOJob *job() const;

        /**
         * \brief Retourne le LDirModel utilisé
         */
        LDirModel *model() const;

        /**
         * \brief Retourne le LDirView utilisé
         */
        LDirView *view() const;

private slots:

        /**
         * \brief Définis l'url
         *
         * Définis le dossier courant. Utilisé en interne pour synchroniser le changement de Job entre le LDirView et le LDirTree
         */
        void setDir(const QString& dir);

        void itemActivated(const QModelIndex& index);
        void rootClicked(bool);
        void unrootClicked(bool);

signals:
        void dirChanged(const QString& dir);

private:
        LIOJob    *_job;
        LDirModel *_model;
        LDirView  *_view;

        QString _host;

        QMenu   *contextMenu;
        QAction *actRoot;
        QAction *actUnroot;
};

#endif /* __LDIRTREE_H__ */
