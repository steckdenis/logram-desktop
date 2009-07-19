/*
 * ILIOProvider.h
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

#ifndef __ILIOPROVIDER_H__
#define __ILIOPROVIDER_H__

#include <QtPlugin>
#include <QFile>
#include <QtNetwork/QUrlInfo>
#include <QList>

class LIOJob;

/**
 * \class ILIOProvider
 * \brief Interface pour la communication entre les Providers LIO et le LIOJob
 */
class ILIOProvider
{
public:
        ~ILIOProvider() {}

        /**
         * \brief Se connecte à un serveur
         *
         * Est appelée par LIOJob quand on change d'adresse absolue, et donc généralement quand on charge
         * le provider
         *
         * \param address adresse du serveur (ftp.gnu.org par exemple (pour ftp://ftp.gnu.org/))
         * \param job LIOJob utilisé pour la connexion (un plugin peut être chargé par plusieurs LIOJob, merci la gestion des plugins de Qt)
         * \return true si ok, false sinon
         */
        virtual bool rconnect(QString address, LIOJob *job) = 0;

        /**
         * \brief Se place dans un dossier
         *
         * Se place dans un dossier.
         *
         * \param directory nom du dossier
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false (le provider aura affiché le message d'erreur)
         * \sa cdUp()
         */
        virtual bool cd(QString& directory, LIOJob *job) = 0;

        /**
         * \brief Se place dans le dossier parent
         *
         * Remonte d'un cran dans l'arborescence des dossiers. Si on se trouve déjà à la racine, rien n'est fait
         * et la fonction retourne false
         *
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, false sinon
         * \sa cd()
         */
        virtual bool cdUp(LIOJob *job) = 0;

        /**
         * \brief Retourne le dossier courant
         *
         * Retourne le nom du dossier dans lequel se trouve l'accès LIO. Si withServer est égal à true, la chaîne
         * contenant le nom du serveur (si distant) est retournée (http://... compris)
         *
         * \param job LIOJob utilisé pour la connexion
         * \return répertoire courant
         */
        virtual QString pwd(LIOJob *job) = 0;

        /**
         * \brief Télécharge un fichier
         *
         * Copie un fichier de son emplacement sur le réseau vers un nom dans le fichier /tmp.
         *
         * Cette fonction permet de facilement interagir avec les fichiers. Couplé à un QFileSystemWatcher,
         * elle permetterait, en conjonction avec upload(), de facilement modifier des fichiers distants en
         * toute transparence.
         *
         * \param remoteName nom du fichier distant (dans le répertoire courant)
         * \param tempnName nom du fichier temporaire (av6728.tmp par exemple, pour le fichier /tmp/av6728.tmp)
         * \param toTemp true pour /tmp/{tempName}, false si tempName est un chemin complet local
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         * \sa upload
         */
        virtual bool download(QString& remoteName, QString& tempName, LIOJob *job, bool toTemp = true) = 0;

        /**
         * \brief Envoie un fichier sur le réseau
         *
         * Copie un fichier local vers le réseau. Si le fichier n'existe pas, il sera créé.
         *
         * \param tempName nom du fichier local
         * \param remoteName nom du fichier distant, dans le répertoire courant
         * \param fromTemp true si le fichier local est /tmp/{tempName}, false si tempName est un chemin complet
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         * \sa download
         */
        virtual bool upload(QString& tempName, QString& remoteName, LIOJob *job, bool fromTemp = true) = 0;

        /**
         * \brief Renomme un fichier/dossier
         *
         * Renomme un fichier ou un dossier distant, dans le répertoire courant.
         *
         * \param old nom du fichier avant renommage (doit exister)
         * \param mnew nom du fichier après renommage (ne peut pas exister)
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         */
        virtual bool rename(QString& old, QString& mnew, LIOJob *job) = 0;

        /**
         * \brief Crée un dossier
         *
         * Crée un nouveau dossier distant. Si le dossier existe déjà, la fonction retournera false.
         *
         * Pour les Providers le permettant, les permissions sont placées à celles par défaut, ainsi que
         * le groupe et le propriétaire.
         *
         * \param name nom du dissier à créer
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         * \sa upload
         */
        virtual bool mkdir(QString& name, LIOJob *job) = 0;

        /**
         * \brief Supprime un dossier ou un fichier
         *
         * Supprime un fichier (recursive=false), ou un dossier (recursive=true, s'il est false, le dossier
         * doit être vide).
         *
         * Si le fichier ou dossier n'existe pas, la fonction renverra tout de même true.
         *
         * \param name nom du fichier/dossier à supprimer dans le répertoire courant
         * \param recursive true pour supprimer récursivement, false sinon
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         */
        virtual bool rm(QString& name, LIOJob *job, bool recursive = false) = 0;

        /**
         * \brief Vérifie qu'un fichier existe
         * \param name nom du fichier/dossier qui devrait exister
         * \param job LIOJob utilisé pour la connexion
         * \return true s'il existe, false sinon
         */
        virtual bool exists(QString& name, LIOJob *job) = 0;

        /**
         * \brief Change les permissions d'un fichier
         *
         * Change les permissions du fichier name dans le répertoire courant pour qu'elle reflettent
         * les permissions permissions.
         *
         * \param name nom du fichier à modifier
         * \param permissions permissions nouvelles du fichier
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         */
        virtual bool chmod(QString& name, QFile::Permission permissions, LIOJob *job) = 0;

        /**
         * \brief Change le propriétaire d'un fichier
         * \param file nom du fichier dans le répertoire courant à changer
         * \param owner utilisateur propriétaire
         * \param group groupe propriétaire
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         */
        virtual bool chown(QString& file, QString& owner, QString& group, LIOJob *job) = 0;

        /**
         * \brief Lis le dossier en cours
         *
         * Place dans return le contenu du dossier courant. Cette fonction est utile pour les
         * explorateurs de fichiers.
         *
         * \param return adresse d'une liste de QUrlInfo qui représentent chacun un fichier/dossier
         * \param showHidden true pour lister les fichiers/dossiers cachés, false sinon
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         */
        virtual bool list(QList<QUrlInfo *> *mreturn, LIOJob *job, bool showHidden = false) = 0;

        /**
         * \brief Copie un fichier d'un emplacement à un autre
         *
         * Copie un fichier, local ou distant, vers un emplacement, local ou distant.
         *
         * Les adresses sont données complètement, donc avec leur chemin absolu.
         *
         * \param from nom du fichier à copier
         * \param to nom du fichier de destination
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         * \sa move
         */
        virtual bool copy(QString& from, QString& to, LIOJob *job) = 0;

        /**
         * \brief Déplace un fichier
         *
         * Déplace le fichier from vers l'emplacement to. Si un nom de fichier est spécifié dans to,
         * le fichier sera aussi renommé (move("/home/truc/machin.txt", "/home/chose/fichier.txt") par exemple).
         *
         * \param from nom du fichier d'origine
         * \param to emplacement où placer le fichier
         * \param job LIOJob utilisé pour la connexion
         * \return true si tout s'est bien passé, sinon false
         * \sa copy
         */
        virtual bool move(QString& from, QString& to, LIOJob *job) = 0;
};

Q_DECLARE_INTERFACE(ILIOProvider, "org.logram-project.lio.providerinterface/0.1")

#endif
