/*
 * WikiSearch.h
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Florent Revest <florent.revest666@gmail.com>
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
 * \file WikiSearch.h
 * \brief languette permettant la recherche d'information sur le web avec Wikipedia
 * \author Florent.R
 * \version 1
 */

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

#include <IPanacheDesktopPlugin.h>

/**
 * \class WikiSearchPlugin
 * \brief classe créant les widget et les slots de la languette
 */
class WikiSearchPlugin : public QObject, public IPanacheDesktopPlugin
{
        Q_OBJECT
        Q_INTERFACES(IPanacheDesktopPlugin)

public :
                void load();
        void linkParent(QWidget *parent);
        void unlinkParent();

public slots:
        void slotLire();
        void chercher();
        void ouvrirWikipedia();

private:
        QSpacerItem        *espace;
        QGridLayout        *layout;
        QPushButton        *rechercher, *lire, *wikipedia;
        QLineEdit          *barreDeRecherche;
        QWidget            *mparent;
};
