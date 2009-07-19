/*
 * dummy.h
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
 * \file ghetto.h
 * \brief Lecteur de musique minimaliste intégré à Panache
 * \author Denis.S
 * \version 0.6.0
 */

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QTreeView>
#include <QPushButton>
#include <QDirModel>
#include <QDir>
#include <phonon>

#include <IPanacheDesktopPlugin.h>

/**
 * \class GhettoPlugin
 * \brief Lecteur de musique minimaliste intégré à Panache
 */
class GhettoPlugin : public QObject, public IPanacheDesktopPlugin
{
        Q_OBJECT
        Q_INTERFACES(IPanacheDesktopPlugin)

public :
                void load();
        void linkParent(QWidget *parent);
        void unlinkParent();

public slots:
        void lvDoubleClicked(const QModelIndex& index);
        void mediaFinished();

private:
        QVBoxLayout        *layoutGeneral;
        QHBoxLayout        *layoutButton;
        Phonon::SeekSlider *slider;
        QTreeView          *lv;
        QPushButton        *play, *pause;
        QLabel             *mtitle;

        QWidget *mparent;

        QDirModel           *model;
        Phonon::MediaObject *media;
};
