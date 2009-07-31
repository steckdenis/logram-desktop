/*
 * WikiSearch.cpp
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

#include "WikiSearch.h"

void WikiSearchPlugin::load()
{
}

void WikiSearchPlugin::linkParent(QWidget *parent)
{
        mparent = parent;

        //Initialisation des widgets
        espace = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        wikipedia = new QPushButton();
        wikipedia->setIcon(QIcon(":/logo.png"));
        wikipedia->setIconSize(QSize(274, 88));
        wikipedia->setFlat(true);
        barreDeRecherche = new QLineEdit();
        lire  = new QPushButton(tr("Lire"));
        rechercher  = new QPushButton(tr("Rechercher"));

        //Initialisation des layouts
        layout  = new QGridLayout();

        layout->addWidget(wikipedia, 0, 0, 1, 3);
        layout->addWidget(barreDeRecherche, 1, 0, 1, 3);
        layout->addItem(espace, 2, 0, 1, 1);
        layout->addWidget(rechercher, 2, 1, 1, 1);
        layout->addWidget(lire, 2, 2, 1, 1);

        //Initialisation de la languette
        mparent->resize(390, 230);
        mparent->setLayout(layout);
        
        //Connexion des signaux Ã  leurs slots 
        connect(wikipedia, SIGNAL(clicked()), this, SLOT(ouvrirWikipedia()));
        connect(rechercher, SIGNAL(clicked()), this, SLOT(chercher()));
        connect(lire, SIGNAL(clicked()), this, SLOT(slotLire()));
}

void WikiSearchPlugin::unlinkParent()
{
    //Rien Ã  faire
    return;
}

void WikiSearchPlugin::chercher()
{
    QString motARechercher = barreDeRecherche->text();
    motARechercher.replace(QString(" "), QString("+")); //remplace les espaces par des +
    QString adresse = "http://fr.wikipedia.org/w/index.php?title=Sp%C3%A9cial%3ARecherche&search=" + motARechercher + "&fulltext=Search&searchengineselect=mediawiki&ns0=1";
    QDesktopServices::openUrl(QUrl(adresse));
}
void WikiSearchPlugin::slotLire()
{
    QString motALire = barreDeRecherche->text();
    motALire.replace(QString(" "), QString("_")); //remplace les espaces par des _
    QString adresse = "http://fr.wikipedia.org/wiki/" + motALire;
    QDesktopServices::openUrl(QUrl(adresse));
}

void WikiSearchPlugin::ouvrirWikipedia()
{
    QDesktopServices::openUrl(QUrl("http://fr.wikipedia.org/wiki/Accueil"));
}

//export du plugin.
Q_EXPORT_PLUGIN2(dummy, WikiSearchPlugin)
