/*
 * LIconLoader.h
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

#ifndef __LICONLOADER_H__
#define __LICONLOADER_H__

#include <QIcon>
#include <QStringList>
#include <QHash>
#include <QObject>

#include <LConfig.h>

/**
 * \class LIconLoader
 * \brief Charge des icônes
 *
 * Charge des icônes aux normes Freedesktop.org en fonction de leur nom.
 *
 * Par exemple, si vous chargez l'icône "edit-undo", le fichier /usr/share/icons/votre_theme/48x48/actions/edit-undo.png sera chargé.
 *
 */
class LIconLoader : public QObject
{
public:

        /**
         * \brief Initialise le LIconLoader (mise en cache, etc)
         * \param theme Thème (QString() pour le thème par défaut)
         */
        LIconLoader(QString theme = QString(), QObject *parent = 0);

        /**
         * \brief Retourne l'icône demandée
         * \param name Nom de l'icône (gnome-control-panel par exemple)
         */
        QIcon icon(const QString& name);

private:
        QStringList         dirs;
        QHash<int, QString> themedirs;

        void exploreThemes(QString& themeName);
};

#endif /* __LICONLOADER_H__ */
