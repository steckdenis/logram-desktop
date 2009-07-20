/*
 * Search.cpp
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

#include "Search.h"

void SearchPlugin::load()
{
}

void SearchPlugin::linkParent(QWidget *parent)
{
        mparent = parent;

        //Initialisation des widgets
        espace = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        espace2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        google = new QPushButton();
        google->setIcon(QIcon(":/logo.png"));
        google->setIconSize(QSize(276, 110));
        google->setFlat(true);
        barreDeRecherche = new QLineEdit();
        rechercher  = new QPushButton(tr("Rechercher"));

        //Initialisation des layouts
        layout  = new QGridLayout();

        layout->addWidget(google, 0, 0, 1, 3);
        layout->addWidget(barreDeRecherche, 1, 0, 1, 3);
        layout->addItem(espace, 2, 0, 1, 1);
        layout->addWidget(rechercher, 2, 1, 1, 1);
        layout->addItem(espace2, 2, 2, 1, 1);

        //Initialisation de la languette
        mparent->resize(390, 230);
        mparent->setLayout(layout);
        
        //Connexion des signaux à leurs slots 
        connect(google, SIGNAL(clicked()), this, SLOT(ouvrirGoogle()));
        connect(rechercher, SIGNAL(clicked()), this, SLOT(chercher()));
}

void SearchPlugin::unlinkParent()
{
    //Rien à faire
    return;
}

void SearchPlugin::chercher()
{
    QString motARechercher = barreDeRecherche->text();
    motARechercher.replace(QString(" "), QString("+")); //remplace les espaces par des +
    QString adresse = "http://www.google.fr/search?hl=fr&q=" + motARechercher + "&btnG=Recherche+Google&meta=&aq=f&oq=";
    QDesktopServices::openUrl(QUrl(adresse));
}

void SearchPlugin::ouvrirGoogle()
{
    QDesktopServices::openUrl(QUrl("http://www.google.fr"));
}

//export du plugin.
Q_EXPORT_PLUGIN2(dummy, SearchPlugin)
