/*
 * IPanacheDesktopDecoratorPlugin.h
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

/**
 * \file IPanachePlugin.h
 * \brief Contient la définition de l'interface de communication entre Panache et ses plugins
 * \author Denis.S
 * \version 1.90
 */

#ifndef __IPANACHEDESKTOPDECORATORPLUGIN_H__
#define __IPANACHEDESKTOPDECORATORPLUGIN_H__

#include <QWidget>
#include <QtPlugin>

/**
 * \class IPanacheDesktopDecoratorPlugin
 * \brief Interface pour les plugins décorant le bureau
 *
 * Un Plugin décorateur de bureau créer le widget de papier peint, et se charge de dessiner dessus.
 *
 * Ces plugins peuvent tout faire avec le widget (ils le créent), ce qui permet d'afficher des images,
 * des vidéos, des annimations, des contrôles, etc.
 */
class IPanacheDesktopDecoratorPlugin
{
public:
        ~IPanacheDesktopDecoratorPlugin() {}

        /**
         * \brief Initialise le plugin
         *
         * Fonction appelée pour initialiser le plugin
         *
         * \param parent : pointeur sur le QWidget parent
         * \return QWidget créé pour l'occasion
         */
        virtual QWidget *Initialize() = 0;
};

Q_DECLARE_INTERFACE(IPanacheDesktopDecoratorPlugin, "org.logram-project.panache.desktopDecoratorPluginInterface/1.90")

#endif /* __IPANACHEDESKTOPDECORATORPLUGIN_H__ */
