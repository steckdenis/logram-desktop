/*
 * LDirBar.h
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

#ifndef __LDIRBAR_H__
#define __LDIRBAR_H__

#include <QToolBar>
#include <QPushButton>
#include <QString>
#include <QList>
#include <QStringList>
#include <QUrl>
#include <QAction>

#include <LDirView.h>

/**
 * \class LDirBar
 * \brief Barre d'affichage des dossiers
 *
 * Ce contrôle est une ToolBar (utilisable dans une QMainWindow), qui, une fois reliée à un LDirView,
 * permet d'intégragir avec le chemin contenu dedans.
 *
 * Cette barre affiche le chemin courant du LDirView, et permet de remonter dans l'arborescence en cliquand sur les boutons.
 * De même, avec un clic-droit sur les boutons, on peut choisir dans un menu le dossier dans lequel on veut aller.
 */
class LDirBar : public QToolBar
{
        Q_OBJECT

public:

        /**
         * \brief Construit un LDirView
         * \param view LDirView à utiliser
         * \param parent QWidget parent
         * \sa setView()
         */
        LDirBar(LDirView *view = 0, QWidget *parent = 0);

        /**
         * \brief Obtiens le LDirView utilisé
         */
        LDirView *view() const;

        /**
         * \brief Définis le LDirView
         *
         * Change le LDirView et reconnecte les signaux entre le LDirBar et le LDirView
         *
         * \param view LDirView à utiliser
         */
        void setView(LDirView *view);

public slots:

        /**
         * \brief Change le dossier courant
         * \param url url du dossier (chemin absolu, donc "ftp://truc.machin/chose" ou "/home/moi")
         */
        void setDir(const QString& url);

signals:

        /**
         * \brief Se produit quand l'utilisateur change de dossier
         * \param path Chemin du nouveau dossier
         */
        void dirChanged(const QString& path);

private slots:
        void elementClicked(bool toggled);
        void rootClicked(bool toggled);

private:
        LDirView *_view;

        QString host;

        QList<QPushButton *> buttons;
        QPushButton          *rootAction;
};

#endif /* __LDIRBAR_H__ */
