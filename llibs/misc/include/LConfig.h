/*
 * LConfig.h
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

#ifndef __LCONFIG_H__
#define __LCONFIG_H__

#include <QSettings>

/**
 * \class LConfig
 * \brief Gère les fichiers de configuration
 *
 * Cette classe s'utilise comme la classe QSettings de Qt, et permet aux applications
 * d'accéder à leurs configurations.
 *
 * Sa principale différence concerne les utilisateurs avantureux : alors que QSettings
 * ne touche pas au fichier de configuration quand la valeur par défaut est utilisée,
 * l'utilisateur ne sachant rien de la configuration dont le programme a beson,
 * LConfig enregistre directement les valeurs par défaut, ce qui au passage, évite
 * de re-parser le fichier en entier à l'avenir, la clef étant directement trouvée.
 */
class LConfig : public QSettings
{
public:

        /**
         * \brief Construit un LConfig
         * \param appName nom de l'application à configurer (utilisé pour nommer le fichier)
         * \param parent QObject parent
         */
        LConfig(QString appName, QObject *parent = 0);

        /**
         * \brief Lit une valeur
         * \param key nom de la valeur, comme pour QSettings ("truc" ou "parent/enfant")
         * \param defaultValue valeur par défaut, retournée et enregistrée dans le fichier si la clef n'existe pas
         * \return valeur de la clef, ou valeur par défaut
         * \sa logramValue()
         */
        QVariant value(const QString& key, const QVariant& defaultValue = QVariant());

        /**
         * \brief Lit une valeur de Logram
         *
         * Se comporte comme value(), mais lit une valeur dans la base de configuration de Logram.
         *
         * C'est très utile quand une application a besoin d'une valeur utilisée par d'autres
         * applications, comme le thème, on une configuration système
         *
         * \param key nom de la valeur
         * \param defaultValue valeur par défaut
         * \param module module de Logram, pour le nommage du fichier (Theme, Core, etc)
         * \return valeur de la clef
         * \sa value()
         */
        static QVariant logramValue(const QString& key, const QVariant defaultValue = QVariant(), const QString module = QString());
};

#endif /* __LCONFIG_H__ */
