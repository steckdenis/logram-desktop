/*
 * IPanacheDesktopPlugin.h
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

#ifndef __IPANACHEDESKTOPPLUGIN_H__
#define __IPANACHEDESKTOPPLUGIN_H__

#include <QWidget>
#include <QtPlugin>

/**
 * \class IPanacheDesktopPlugin
 * \brief Interface de gestion des plugins de Panache
 *
 * Cette classe réprésente le squellette d'un plugin de Panache, c'est à dire de quelque chose qui s'affiche dans une languette ou sur le bureau
 */
class IPanacheDesktopPlugin
{
public:
        ~IPanacheDesktopPlugin() {}

        /**
         * \brief Charge le plugin
         *
         * Appelée quand le plugin est chargé au démarrage de panache-desktop
         *
         */
        virtual void load() = 0;

        /**
         * \brief Initialise le plugin
         *
         * Appelée quand le plugin est affiché pour la première fois dans un widget, donc quand on passe d'une
         * languette à un widget de bureau, ou l'inverse, ou quand on charge le plugin au démarrage
         *
         * \param parent : pointeur sur le QWidget parent
         * \sa destroy()
         */
        virtual void linkParent(QWidget *parent) = 0;

        /**
         * \brief Supprime le plugin
         *
         * Est appelé quand le plugin quitte son widget parent.
         *
         * Par exemple, si l'utilisateur souhaite transformer une languette en widget de bureau, la fonction destroy() sera
         * d'abord appelée, puis le widget de la languette sera supprimé, puis le widget de bureau sera créé, puis la fonction Initialize()
         * sera appelée.
         *
         * \sa init()
         */
        virtual void unlinkParent() = 0;
};

Q_DECLARE_INTERFACE(IPanacheDesktopPlugin, "org.logram-project.panache.desktopPluginInterface/1.90")

#endif /* __IPANACHEPLUGIN_H__ */
