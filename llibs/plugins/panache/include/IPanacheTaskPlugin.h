/*
 * IPanacheTaskPlugin.h
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

#ifndef __IPANACHETASKPLUGIN_H__
#define __IPANACHETASKPLUGIN_H__

#include <QtPlugin>
#include <QWidget>

/**
 * \class IPanacheTaskPlugin
 * \brief Interface entre la Barre de Panache-desktop et les plugins
 *
 * Permet à des plugins de fournir leur propre widget à insérer dans la barre des tâches de Panache-desktop
 *
 * Les plugins fournissent leur propre widget, ce qui leur permet de faire ce qu'ils veulent. Par exemple, le menu rond
 * ne permet que d'afficher des icônes, alors que la barre est préférable pour tout ce qui change souvent, et surtout
 * qui doit rester affiché quoiqu'il arrive (heure, zone de notification, etc).
 *
 * Si vous hésitez entre un plugin Task et un plugin Menu, préférez le plugin Menu, qui peut être plus contrôlé par l'utilisateur.
 * De plus, l'espace dans la Barre est limité (taille de l'écran), alors que le Menu est virtuellement infini !
 */
class IPanacheTaskPlugin
{
public:
        ~IPanacheTaskPlugin() {}

        /**
         * \brief Inititalise le plugin
         *
         * Demande au plugin de s'initialiser et de retourner le QWidget qu'il veut afficher dans la Barre.
         *
         * Ce QWidget doit pouvoir être redimensionnable en hauteur (le plugin ne connaît pas la hauteur de la Barre). Il sera
         * inséré dans un QLayout par panache-desktop. Si le widget doit se redimensionner dynamiquement (zone de notification, etc),
         * la méthode QWidget::sizeHint() devrait être implémentée.
         */
        virtual QWidget *init() = 0;

        virtual void appEvent(XEvent *event) = 0;
};

Q_DECLARE_INTERFACE(IPanacheTaskPlugin, "org.logram-project.panache.taskPluginInterface/1.90")

#endif /* __IPANACHETASKPLUGIN_H__ */
