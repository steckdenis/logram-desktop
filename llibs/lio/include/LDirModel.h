/*
 * LDirModel.h
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

#ifndef __LDIRMODEL_H__
#define __LDIRMODEL_H__

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDateTime>
#include <QIcon>
#include <QPixmap>
#include <QStringList>
#include <QMimeData>
#include <QUrl>
#include <QFile>

#include <LIOJob.h>
#include <LConfig.h>
#include <LIconLoader.h>

class Element;
class LMimeData;

/**
 * \class LDirModel
 * \brief Modèle permettant d'accéder aux dossiers, en utilisant les LIOs
 *
 * Cette classe s'utilise comme le QDirModel. Elle utilise le framework
 * Model/View de Qt. Elle peut être utilisée pour affichage dans n'importe
 * quelle vue de Qt ou de votre application, même si cette classe marche le
 * mieux avec un QTreeView ou un QListView.
 *
 * De plus, cette classe fourni quelques fonctions pratiques pour traiter
 * les actions de l'utilisateur sur le modèle, comme itemPath(), itemUrlInfo(), etc
 */
class LDirModel : public QAbstractItemModel
{
public:

        /**
         * \brief Construit un LDirModel
         * \param job LIOJob à utiliser. Sa fonction setHost() doit déjà avoir été appelée
         * \param dir Dossier de base du LDirModel. Dossier de l'élément root en quelque sorte. Le LDirModel ne pourra pas aller au dessus
         * \param showFile True pour afficher les fichiers, false sinon
         * \param showHidden True pour afficher les fichiers cachés, false sinon
         * \param parent QObject parent
         */
        LDirModel(LIOJob *job, QString& dir, bool showFile = true, bool showHidden = false, QObject *parent = 0);
        ~LDirModel();

        //Lecture
        QVariant data(const QModelIndex& index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex& index) const;

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;

        //Ecriture
        Qt::ItemFlags flags(const QModelIndex& index) const;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

        //Gestion du glisser-déposer
        Qt::DropActions supportedDropActions() const;
        QStringList mimeTypes() const;
        QMimeData *mimeData(const QModelIndexList& indexes) const;
        bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent);


        //Spécifique à LDirModel

        /**
         * \brief Retourne le chemin d'accès d'un Item
         *
         * Permet de trouver le chemin d'accès (avec nom de fichier pour les fichiers) d'un Item donné,
         * en fonction du root du model.
         *
         * C'est particulièrement utilise pour ouvrir le fichier correspondant à l'item envoyé par
         * l'événement itemClicked des vues.
         *
         * \return le nom du fichier correspondant à l'index
         * \param index l'index spécifié
         * \sa itemUrlInfo()
         */
        QString itemPath(const QModelIndex& index) const;

        /**
         * \brief Retourne le QUrlInfo correspondant à l'index
         * \param index l'index spécifié
         * \return le QUrlInfo de l'index
         * \sa itemPath()
         */
        QUrlInfo *itemUrlInfo(const QModelIndex& index) const;

        /**
         * \brief Rafraîchis l'index spécifié
         * \param index Index à rafraîchir
         */
        void refreshItem(const QModelIndex& index);

        /**
         * \brief Configure la taille des miniatures
         *
         * Permet de renseigner la fonction filePixmap() de la taille des miniatures générées
         * pour les fichiers images. La taille par défaut est 16x16 pixels
         *
         * \param width largeur
         * \param height hauteur
         */
        void setIconSize(int width, int height);

        /**
         * \brief Retourne l'icône de l'index spécifié
         *
         * Cette fonction sert à récupérer l'icône du fichier indiqué par l'index spécifié
         *
         * \return Icône du fichier/dossier
         * \param index l'index du fichier
         */
        QPixmap filePixmap(const QModelIndex& index) const;

        /**
         * \brief Retrouve un index grâce à son chemin
         *
         * Retourne l'index spécifié qui a comme chemin le chemin donné
         *
         * \param path Chemin recherché
         * \return Index qui a ce chemin
         */
        QModelIndex indexForPath(const QString& path);

        /**
         * \brief Réexplore l'arbre des fichiers
         */
        bool refresh();

        /**
         * \brief Affiche ou non les fichiers
         * \param showFile true pour affichier les fichiers, false pour seulement les dossiers
         */
        void setShowFiles(bool showFile);

        /**
         * \brief Affiche ou non les fichiers cachés
         * \param showHidden true pour les afficher, false sinon
         */
        void setShowHidden(bool showHidden);

        /**
         * \brief Parmet de savoir si les fichiers sont affichés
         */
        bool showFiles();

        /**
         * \brief Permet de savoir si les fichiers cachés sont affichés
         */
        bool showHidden();

        /**
         * \brief Colle un fichier
         *
         * Permet de coller un fichier, ou un dossier, dans un noed parent. C'est ce qui permet par
         * exemple d'implémenter le copier/coller
         *
         * \param url url du fichier (absolue : "ftp://ftp.gnu.org/pub/grub/grub-2.tar.bz2" par exemple)
         * \param parent QModelIndex parent, dans lequel copier le fichier/dossier
         * \param job Job temporaire utilisé pour garder la connexion (accélère les transferts FTP par exemple). Le job ne peut pas avoir été connecté
         * \return true si tout se passe bien, false sinon
         */
        bool pasteFile(const QUrl& url, const QModelIndex& index, Qt::DropAction action = Qt::CopyAction, LIOJob *mjob = 0);

        /**
         * \brief Retourne le job utilisé
         */
        LIOJob *job();

        enum Field
        {
                Name,
                Size,
                LastModified,
                LastRead,
                Owner,
                Group
        };
        
        protected:
                bool canFetchMore(const QModelIndex &parent) const;
                void fetchMore(const QModelIndex &parent);


private:
        Element *rootElement;
        LIOJob  *_job;
        bool    _showFile, _showHidden;
        int     _iconW, _iconH;
        QString _rootDir;

        LIconLoader *iloader;
        LMimeData *mimedata;

        Element *getElement(const QModelIndex& index) const;
};

#endif
