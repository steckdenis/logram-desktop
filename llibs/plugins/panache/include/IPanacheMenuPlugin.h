/*
 * IPanacheMenuPlugin.h
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

#ifndef __IPANACHEMENUPLUGIN_H__
#define __IPANACHEMENUPLUGIN_H__

#include <QtPlugin>

#include <QString>
#include <QPixmap>
#include <QList>
#include <QMenu>

class MenuManager;
class IPanacheMenuPlugin;

/**
 * \struct MenuEntry
 * \brief Informations sur une entrée de menu
 */
struct MenuEntry
{
        QString            title;       /*! Titre de l'entrée du menu rond */
        QPixmap            icon;        /*! Icône de cette entrée */
        bool               togglable;   /*! Indique si l'entée est poussable */
        bool               toggled;     /*! Indique si l'entrée est poussée */
        bool               separator;   /*! Indique si c'est un séparateur */
        IPanacheMenuPlugin *plugin;     /*! Plugin parent */
};

extern "C" {
#include <X11/Xlib.h>
}

/**
 * \class IPanacheMenuPlugin
 * \brief Plugin s'affichant dans le menu rond
 *
 * Représente un plugin qui s'affiche dans le menu rond
 */
class IPanacheMenuPlugin
{
public:
        ~IPanacheMenuPlugin() {}

        /**
         * \brief Intialise le plugin
         *
         * Est appelée quand le plugin est chargé en mémoire.
         *
         * \param parent MenuManager parent
         */
        virtual void init(MenuManager *parent) = 0;

        /**
         * \brief Obtiens les entrées du plugin
         *
         * Le plugin se sert de cette fonction pour retournées les entrées qui
         * doivent être insérées dans le menu à son nom.
         *
         * \return Liste des éléments
         */
        virtual QList<MenuEntry *> *menuEntries() = 0;

        /**
         * \brief Gère un événement de l'application
         *
         * Permet au plugin d'intercepter les événements X11 envoyés directement par l'application
         *
         * C'est utilisé par Taskbar
         *
         * \param event Structure XEvent de l'événement
         */
        virtual void appEvent(XEvent *event) = 0;


        /**
         * \brief Un élément a été cliqué
         *
         * Appelé par MenuManager quand l'utilisateur clique sur un élément de menu qui
         * appartient au plugin. L'index est l'index de l'élement cliqué dans la liste
         * retournée par menuEntries().
         *
         * \param index Index de l'élément cliqué
         * \param toggled Indique si l'entrée a été poussée ou pas (pour les entrées le gérant)
         */
        virtual void itemClicked(int index, bool toggled) = 0;

        /**
         * \brief Un élément a été cliqué avec le bouton gauche
         * \sa itemClicked()
         * \param index Index de l'élément cliqué
         * \param x coordonnée globale X du point où se trouve le pointeur
         * \param y coordonnée globale Y du point où se trouve le pointeur
         */
        virtual void itemContextMenu(int index, int x, int y) = 0;
};

Q_DECLARE_INTERFACE(IPanacheMenuPlugin, "org.logram-project.panache.menuPluginInterface/1.90")

#endif
