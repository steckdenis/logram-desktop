/*
 * dummy.cpp
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

#include "ghetto.h"

#include <QtDebug>

void GhettoPlugin::load()
{
        //Créer les objets partagés entre les languettes et les widgets de bureau
        media = Phonon::createPlayer(Phonon::MusicCategory);
}

void GhettoPlugin::linkParent(QWidget *parent)
{
        mparent = parent;

        //Initialisation des widgets
        lv     = new QTreeView();
        mtitle = new QLabel(tr("En cours :"));
        mtitle->setForegroundRole(QPalette::HighlightedText);

        //Initialisation des boutons
        play  = new QPushButton(tr("Jouer"));
        pause = new QPushButton(tr("Pause"));

        //Initialisation du modèle
        model = new QDirModel(parent);
        lv->setModel(model);
        lv->setRootIndex(model->index(QDir::homePath() + tr("/Musique")));

        //Initialisation du média
        slider = new Phonon::SeekSlider;
        slider->setMediaObject(media);

        //Initialisation des layouts
        layoutButton  = new QHBoxLayout();
        layoutGeneral = new QVBoxLayout();

        layoutButton->addWidget(play);
        layoutButton->addWidget(pause);

        layoutGeneral->addWidget(mtitle);
        layoutGeneral->addWidget(slider);
        layoutGeneral->addWidget(lv);
        layoutGeneral->addLayout(layoutButton);

        //Initialisation du widget proprement dit
        mparent->resize(400, 300);
        mparent->setLayout(layoutGeneral);

        //Signaux
        connect(lv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(lvDoubleClicked(const QModelIndex &)));
        connect(play, SIGNAL(clicked(bool)), media, SLOT(play()));
        connect(pause, SIGNAL(clicked(bool)), media, SLOT(pause()));
        connect(media, SIGNAL(finished()), this, SLOT(mediaFinished()));
}

void GhettoPlugin::unlinkParent()
{
        //Rien à faire, les widgets sont supprimés automatiquement, et le lecteur Phonon doit rester
        return;
}

void GhettoPlugin::lvDoubleClicked(const QModelIndex& index)
{
        QString filePath = model->filePath(index);

        media->setCurrentSource(filePath);
        media->play();

        mtitle->setText(tr("En cours : ") + model->fileName(index));
}

void GhettoPlugin::mediaFinished()
{
        media->seek(0);
        media->play();
}

//Pas oublier d'exporter le plugin.
Q_EXPORT_PLUGIN2(dummy, GhettoPlugin)
