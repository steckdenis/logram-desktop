/*
 * LIOJob.h
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

#ifndef __LIOJOB_H__
#define __LIOJOB_H__

#include <QObject>
#include <QPluginLoader>
#include <QFile>
#include <QtNetwork/QUrlInfo>
#include <QList>
#include <QHash>
#include <QString>

class ILIOProvider;
class LIOJob;
class QFileSystemWatcher;

/**
 * \class LIOJob
 * \brief Permet d'accéder aux fichiers au travers d'une couche d'abstraction.
 *
 * Les LIOs permettent aux applications d'accéder aux fichiers où qu'ils soient, dans des archives, en
 * local, sur le web (FTP, HTTP, etc). Ce mécanisme est inspiré des kio-slaves de KDE, mais implémenté
 * autrement.
 *
 */
class LIOJob : public QObject
{
        Q_OBJECT

public:

        /**
         * \brief Construit un job LIO
         * \param parent QObject parent
         */
        LIOJob(QObject *parent = 0);

        /**
         * \brief Se connecte à un hôte
         *
         * Établit une connexion avec un hôte, ou avec le système de fichiers local.
         *
         * \param host nom de l'hôte (QString() pour local, "ftp://my.com" par exemple)
         * \return true si tout s'est bien passé, sinon false
         */
        bool setHost(QString mhost = QString());

        /**
         * \brief Retourne le nom d'hôte
         * \return nom de l'hôte avec lequel on est connecté, QString() si pas connecté, "file://" si en local
         */
        QString& host();

        /**
         * \brief Se place dans un dossier
         *
         * Se place dans un dossier. Le dossier peut être de la forme suivante :
         *    - Chemin absolu local (/truc/chose)
         *    - Chemin relatif (truc).
         *
         * Pour les chemins relatifs, vous ne pouvez fournir qu'un seul nom de dossier. Il faut donc
         * appeler plusieurs fois de suite cd() pour se placer dans les différents dossiers
         *
         * \param directory nom du dossier
         * \return true si tout s'est bien passé, sinon false (le provider aura affiché le message d'erreur)
         * \sa cdUp()
         */
        bool cd(QString& directory);

        /**
         * \brief Se place dans le dossier parent
         *
         * Remonte d'un cran dans l'arborescence des dossiers. Si on se trouve déjà à la racine, rien n'est fait
         * et la fonction retourne false
         *
         * \return true si tout s'est bien passé, false sinon
         * \sa cd()
         */
        bool cdUp();

        /**
         * \brief Retourne le dossier courant
         *
         * Retourne le nom du dossier dans lequel se trouve l'accès LIO. Si withServer est égal à true, la chaîne
         * contenant le nom du serveur (si distant) est retournée (http://... compris)
         *
         * \return répertoire courant
         */
        QString pwd();

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
         * \return true si tout s'est bien passé, sinon false
         * \sa upload
         */
        bool download(QString& remoteName, QString& tempName, bool toTemp = true);

        /**
         * \brief Envoie un fichier sur le réseau
         *
         * Copie un fichier local vers le réseau. Si le fichier n'existe pas, il sera créé.
         *
         * \param tempName nom du fichier local
         * \param remoteName nom du fichier distant, dans le répertoire courant
         * \param fromTemp true si le fichier local est /tmp/{tempName}, false si tempName est un chemin complet
         * \return true si tout s'est bien passé, sinon false
         * \sa download
         */
        bool upload(QString& tempName, QString& remoteName, bool fromTemp = true);

        /**
         * \brief Retourne un chemin temporaire du fichier distant, en local
         *
         * Cette fonction permet à un programme d'ouvrir par les méthodes conventionnelles (QFile::open, QFileInfo, etc)
         * un fichier stocké en distant.
         *
         * Un QFileSystemWatcher est placé sur le fichier pour pouvoir le renvoyer sur le réseau quand il est modifié.
         * la modification d'un fichier distant devient donc transparante pour l'utilisateur
         *
         * <strong>Attention :</strong> ça ne marche qu'avec les fichiers, pas avec les dossiers
         *
         * \param url Url du fichier (ftp://ftp.gnu.org/gnu/gcc/gcc-4.2-3.tar.bz2 par exemple)
         * \return Chemin local du fichier téléchargé (/tmp/ee4f286c2798bbca339472.tmp par exemple)
         */
        QString localPath(const QString& url);

        /**
         * \brief Renomme un fichier/dossier
         *
         * Renomme un fichier ou un dossier distant, dans le répertoire courant.
         *
         * \param old nom du fichier avant renommage (doit exister)
         * \param mnew nom du fichier après renommage (ne peut pas exister)
         * \return true si tout s'est bien passé, sinon false
         */
        bool rename(QString& old, QString& mnew);

        /**
         * \brief Crée un dossier
         *
         * Crée un nouveau dossier distant. Si le dossier existe déjà, la fonction retournera false.
         *
         * Pour les Providers le permettant, les permissions sont placées à celles par défaut, ainsi que
         * le groupe et le propriétaire.
         *
         * \param name nom du dissier à créer
         * \return true si tout s'est bien passé, sinon false
         * \sa upload
         */
        bool mkdir(QString& name);

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
         * \return true si tout s'est bien passé, sinon false
         */
        bool rm(QString& name, bool recursive = false);

        /**
         * \brief Vérifie qu'un fichier existe
         * \param name nom du fichier/dossier qui devrait exister
         * \return true s'il existe, false sinon
         */
        bool exists(QString& name);

        /**
         * \brief Change les permissions d'un fichier
         *
         * Change les permissions du fichier name dans le répertoire courant pour qu'elle reflettent
         * les permissions permissions.
         *
         * \param name nom du fichier à modifier
         * \param permissions permissions nouvelles du fichier
         * \return true si tout s'est bien passé, sinon false
         */
        bool chmod(QString& name, QFile::Permission permissions);

        /**
         * \brief Change le propriétaire d'un fichier
         * \param file nom du fichier dans le répertoire courant à changer
         * \param owner utilisateur propriétaire
         * \param group groupe propriétaire
         * \return true si tout s'est bien passé, sinon false
         */
        bool chown(QString& file, QString& owner, QString& group);

        /**
         * \brief Lis le dossier en cours
         *
         * Place dans return le contenu du dossier courant. Cette fonction est utile pour les
         * explorateurs de fichiers.
         *
         * \param return adresse d'une liste de QUrlInfo qui représentent chacun un fichier/dossier
         * \param showHidden true pour lister les fichiers/dossiers cachés, false sinon
         * \return true si tout s'est bien passé, sinon false
         */
        bool list(QList<QUrlInfo *> *mreturn, bool showHidden = false);

        /**
         * \brief Copie un fichier d'un emplacement à un autre
         *
         * Copie un fichier, local ou distant, vers un emplacement, local ou distant.
         *
         * Les adresses sont données complètement, donc avec leur chemin absolu.
         *
         * \param from nom du fichier à copier
         * \param to nom du fichier de destination
         * \return true si tout s'est bien passé, sinon false
         * \sa move
         */
        bool copy(QString& from, QString& to);

        /**
         * \brief Déplace un fichier
         *
         * Déplace le fichier from vers l'emplacement to. Si un nom de fichier est spécifié dans to,
         * le fichier sera aussi renommé (move("/home/truc/machin.txt", "/home/chose/fichier.txt") par exemple).
         *
         * \param from nom du fichier d'origine
         * \param to emplacement où placer le fichier
         * \return true si tout s'est bien passé, sinon false
         * \sa copy
         */
        bool move(QString& from, QString& to);

private slots:
        void fileChanged(const QString& path);

private:
        ILIOProvider *provider;
        QString      cProvider;
        QString      _host;

        QHash<QString, QString> watchedFiles;                   //Fichiers à renvoyer quand on les modifie
        QFileSystemWatcher      *watch;

        bool loadProvider(QString mprovider);
};

#endif
