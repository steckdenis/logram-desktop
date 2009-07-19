/*
 * LDirView.h
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

#ifndef __LDIRVIEW_H__
#define __LDIRVIEW_H__

#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QUrl>
#include <QSize>
#include <QAbstractItemView>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QTimer>
#include <QUndoCommand>
#include <QUndoStack>
#include <QtDebug>

#include <LDirModel.h>
#include <LIOJob.h>

class IActionPlugin;

/**
 * \class LDirView
 * \brief Affiche le contenu d'un dossier
 *
 * Contrôle affichant le contenu d'un dossier, utilisant les LIOs pour l'accès aux fichiers.
 *
 * C'est le contrôle le plus important de Logram. Il permet à l'utilisateur de personnaliser
 * l'affichage (mode icône, liste, détails ou arbre), utilise des plugins pour les actions
 * sur les fichiers, permet tout un tas de choses, etc.
 *
 * Vraiment un contrôle à connaître.
 */
class LDirView : public QWidget
{
        Q_OBJECT

public:

        /**
         * \brief Construit un LDirView
         * \param baseUrl Adresse de l'élément principal, comme "ftp://truc.com/utilisateur". Si QString(), c'est le dossier local / .
         * \param showFile True pour afficher les fichiers, false pour seulement les dossiers
         * \param showHidden True pour affichier les fichiers/dossiers cachés
         * \param parent QWidget parent (utilisé comme QObject parent pour cette classe, et comme QWidget parent pour les vues)
         */
        LDirView(QString baseUrl = QString(), bool showFile = true, bool showHidden = false, QWidget *parent = 0);

        /**
         * \brief Retourne le dossier courant
         * \return Dossier courant ("ftp://machin.tld/chose" par exemple)
         */
        QString dir() const;

        /**
         * \brief Permet de savoir si les fichiers sont affichés
         * \return true si les fichiers sont affichés, false si seulement les dossiers
         */
        bool showFiles() const;

        /**
         * \brief Permet de savoir si les fichiers/dossiers cachés sont affichés
         * \return true si les fichiers/dossiers sont affichés, false sinon
         */
        bool showHidden() const;

        /**
         * \enum ViewMode
         * \brief Définis le mode d'affichage du LDirView
         */
        enum ViewMode
        {
                IconMode,            /*!< Affichage en mode icone, comme d'habitude */
                ListMode,            /*!< Affichage en liste, compact */
                DetailsMode,         /*!< Affichage en mode détaillé, avec des colonnes */
                TreeMode             /*!< Comme le mode détaillé, mais les dossiers sont dépliés */
        };


        /**
         * \brief Retourne le mode d'affichage
         */
        ViewMode viewMode() const;

        /**
         * \brief Retourne la taille des éléments
         * \return Taille des éléments en pixels (48x48 par défaut)
         */
        QSize itemSize() const;

        /**
         * \brief Retourne le chemin de fichier de l'index spécifié
         */
        QUrl filePath(const QModelIndex& index);

        /**
         * \brief Retourne la vue
         *
         * Retourne la vue, sous forme de QAbstractItemView, car LDirView utilise un QListView ou
         * un QTreeView suivant le mode d'affichage.
         *
         * Cette fonction vous permet de connecter vous-meme les signaux et les slots que vous voulez,
         * et de changer des propriétés
         */
        QAbstractItemView *view();

        /**
         * \brief Retourne le LDirModel utilisé
         *
         * Pratique pour utiliser les fonctions propres au LDirModel
         */
        LDirModel *model();

        /**
         * \brief Retourne le LIOJob utilisé
         */
        LIOJob *job();

        /**
         * \brief Retourne la pile Undo
         */
        QUndoStack *undoStack();

        /**
         * \brief Retourne la liste des plugins d'action utilisés
         */
        QList<IActionPlugin *> *actions();

public slots:

        /**
         * \brief Permet d'afficher ou non les fichiers cachés
         * \param showHidden true pour afficher les fichiers cachés, false sinon
         */
        void setShowHidden(bool showHidden);

        /**
         * \brief Permet d'afficher ou non les fichiers
         * \param showFiles true pour affichier les fichiers, false pour seulement les dossiers
         */
        void setShowFiles(bool showFiles);

        /**
         * \brief Change le dossier courant
         * \param url url du dossier (chemin absolu, donc "ftp://truc.machin/chose" ou "/home/moi")
         * \param isUndoAction True si c'est une fonction Undo qui appelle la fonction. <strong>Usage interne seulement !</strong>
         */
        void setDir(const QString& url, bool isUndoAction = false);

        /**
         * \brief Définis le mode d'affichage
         * \param mode Mode d'affichage
         */
        void setViewMode(ViewMode mode);

        /**
         * \brief Définis la taille des entrées
         * \param size Taille des éléments, en pixels
         */
        void setItemSize(const QSize& size);

        /**
         * \brief Copie le fichier sélectionné dans le presse-papier
         */
        void copy();

        /**
         * \brief Colle le contenu du presse-papier dans le répertoire courant
         */
        void paste();

        /**
         * \brief Supprime le fichier ou dossier sélectionné
         */
        void remove();

private slots:
        void viewActivated(const QModelIndex& index);
        void viewClicked(const QModelIndex& index);
        void viewContextMenu(const QPoint& pos);

        void propertiesClicked();

protected:
        virtual void timerEvent(QTimerEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
        virtual bool eventFilter(QObject *watched, QEvent *event);

signals:

        /**
         * \brief Emis quand un fichier doit etre ouvert
         *
         * Informe les objets connectés que l'utilisateur a voulu ouvrir un fichier.
         *
         * Si l'objet ne touche pas à cancel, LDirView se chargera automatiquement d'ouvrir le
         * fichier pour l'application. Si cancel est placé à <strong>true</strong>, le LDirView ne
         * fera rien
         *
         * \param path nom du fichier ouvert (absolu)
         * \param cancel pointeur vers un indiquateur d'annulation, à placer à true par l'application que le LDirView ne fasse rien
         * \sa openDir
         */
        void openFile(QString path, bool *cancel);

        /**
         * \brief Emis quand un dossier doit etre ouvert
         *
         * Informe les objets connectés que l'utilisateur a voulu ouvrir un dossier.
         *
         * Si l'objet ne touche pas à cancel, LDirView se chargera automatiquement d'ouvrir le
         * dossier pour l'application, donc de se placer dedans (en appelant setDir). Si cancel est placé à <strong>true</strong>, le LDirView ne
         * fera rien
         *
         * \param path nom du dossier ouvert (absolu)
         * \param cancel pointeur vers un indiquateur d'annulation, à placer à true par l'application que le LDirView ne fasse rien
         * \sa openFile
         */
        void openDir(QString path, bool *cancel);

        /**
         * \brief Se produit quand l'utilisateur change de dossier
         * \param path Chemin du nouveau dossier
         */
        void dirChanged(const QString& path);

        /**
         * \brief Se produit quand l'utilisateur clique sur un élément
         * \param index Index de l'élément cliqué
         */
        void itemClicked(const QModelIndex& index);
        
        /**
         * \brief Mettre à jour les actions
         *
         * Utilisé par le LDirActions, ne pas utiliser sans savoir ce que vous faites
         *
         */
        void updateActions();

private:
        QString   _url;
        LIOJob    *_job;
        LDirModel *_model;
        QWidget   *_parent;

        QTreeView         *tree;
        QListView         *list;
        QAbstractItemView *_view;
        bool isTree;

        QUndoStack *undos;

        QSize _itemSize;
        bool  _showHidden, _showFiles;

        ViewMode _viewMode;

        QList<IActionPlugin *> plugins;
        QMenu *contextMenu;

        QTimer *timer;
        
        int timerId;

        void resetModel();
        void buildMenu();
};

class LDUndoAction : public QUndoCommand
{
public:
        LDUndoAction(LDirView *mdv, QString mcpath, QString mppath) : dv(mdv), cpath(mcpath), ppath(mppath)
        {
                setText("Change path");
                qDebug() << mcpath << mppath;
        }

        virtual void undo() { dv->setDir(ppath, true); }
        virtual void redo() { dv->setDir(cpath, true); }

private:
        LDirView *dv;
        QString  cpath, ppath;
};

#endif
