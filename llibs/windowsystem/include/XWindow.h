/*
 * XWindow.h
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

#ifndef __XWINDOW_H__
#define __XWINDOW_H__

#include <QIcon>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

/**
 * \clas XWindow
 * \brief Représente une fenêtre X
 *
 * Cette classe permet de "facilement" accéder aux propriétés des fenêtres X. Elle est surtout utilisée par Panache-wm, et par quelques
 * plugins de panache-desktop (taskbar, notification, etc).
 *
 */
class XWindow
{
public:

        /**
         * \brief Constructeur
         * \param mwin Fenêtre X11 à gérer
         * \param mdpy Display X11
         */
        XWindow(Window mwin, Display *mdpy);

        /**
         * \brief Récupère la valeur d'une propriété
         * \param property Atome de la propriété
         * \param prop_return Pointeur vers un pointeur qui contiendra les données lues
         * \param length Taille des données à lire
         * \param items_returner Nombre d'éléments retournés
         * \param req_type Type de la propriété
         * \param offset Décalage dans les données à partir duquel il faut lire
         * \param rs_type Type de donnée retourné
         * \param rs_format Format de donnée retourné (8, 16 ou 32)
         * \param bytes_after Nombre d'octets qu'il reste à lire après l'emplacement où la lecture s'était arrêtée (à cause de length)
         */
        int getProperty(Atom property, void **prop_return, int length, unsigned long *items_returned = 0, Atom req_type = AnyPropertyType, int offset = 0, Atom *rs_type = 0, int *rs_format = 0, unsigned long *bytes_after = 0);

        /**
         * \brief Change la valeur d'une propriété
         * \param property Atome de la propriété
         * \param type Type de la valeur écrite (atome aussi)
         * \param format Format des données (8, 16 ou 32, nombre de bits)
         * \param data Données proprement dites
         * \param elements Nombre d'éléments à écrire (pour un format 32, 10 éléments vont faire 40 octets écrits)
         * \param mode Mode d'affectation (remplacer ou ajouter)
         */
        void changeProperty(Atom property, Atom type, int format, void *data, int elements, int mode = PropModeReplace);

        /**
         * \brief Retourne l'icône de la fenêtre
         *
         * Cette fonction se trouve dans cette classe car elle est plus complexe que simplement obtenir le titre, et souvent utilisée
         *
         */
        QIcon windowIcon();

private:
        Window  win;
        Display *dpy;
};

#endif
