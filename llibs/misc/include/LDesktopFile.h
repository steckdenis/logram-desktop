/*
 * LDesktopFile.h
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

#ifndef __LDESKTOPFILE_H__
#define __LDESKTOPFILE_H__

#include <QFile>
#include <QString>
#include <QObject>

/**
 * \class LDesktopFile
 * \brief Permet d'accéder aux fichier .desktop FreeDesktop.org
 *
 * Cette classe contient des fonctions utiles pour accéder aux fichiers .desktop de Freedesktop.org.
 *
 * En particuluer, elle corrige les différents bugs de QSettings, utilisé à tort pour lire ces fichiers.
 * QSettings ne gère en effet pas les listes de valeurs ("Categories=Qt;Test" revoir Qt, mais pas Test avec QSettings)
 */
class LDesktopFile : public QFile
{
public:

        /**
         * \brief Constructeur
         * \param fileName Nom du fichier .desktop
         * \param parent QObject parent
         */
        LDesktopFile(const QString& fileName, QObject *parent = 0);

        /**
         * \brief Retourne le nom de l'application
         */
        QString name();

        /**
         * \brief Retourne la description de l'application
         */
        QString comment();

        /**
         * \brief Retourne le nom de l'icône de l'application.
         *
         * L'icône elle-même n'est pas retournée par optimisation. Il est mieux de n'avoir qu'un LIconLoader pour l'application,
         * plutôt que un par LDesktopFile
         */
        QString icon();

        /**
         * \brief Retourne l'adresse de l'exécutable
         */
        QString exec();

        /**
         * \brief Retourne une autre valeur
         * \param name Nom de la valeur ("Categories" par exemple)
         * \param localized True pour localiser si possible ("Name" devient "Name[fr]")
         */
        QString value(const QString& name, bool localized = false);

private:
        QString locale;
};

#endif /* __LDESKTOPFILE_H__ */
