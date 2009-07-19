/*
 * Fen.cpp
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

#include <QMenu>
#include <QKeySequence>
#include <QMenuBar>
#include <QScrollArea>
#include <QPalette>
#include <QToolBar>
#include <QStringList>
#include <QDockWidget>
#include <QListView>
#include <QSize>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>

#include <Fen.h>

Fen::Fen(QString fileName) : QMainWindow(0)
{
        iloader = new LIconLoader(QString(), this);

        /* Créer l'interface */
        resize(640, 480);

        setWindowIcon(QIcon(iloader->icon("image")));

        //Créer le menu
        QMenu *fichier = new QMenu(tr("&Fichier"), this);
        QMenu *aide    = new QMenu(tr("&?"), this);

        fichier->addAction(iloader->icon("document-open"), tr("&Ouvrir..."), this, SLOT(open(bool)), QKeySequence(tr("Ctrl+O")));
        fichier->addSeparator();
        fichier->addAction(iloader->icon("document-save"), tr("&Enregistrer"), this, SLOT(save(bool)), QKeySequence(tr("Ctrl+S")));
        fichier->addAction(iloader->icon("document-save-as"), tr("Enregistrer &sous"), this, SLOT(saveas(bool)), QKeySequence(tr("Ctrl+Alt+S")));
        fichier->addSeparator();
        fichier->addAction(iloader->icon("application-exit"), tr("&Quitter"), this, SLOT(quit(bool)), QKeySequence(tr("Ctrl+Q")));

        aide->addAction(tr("A propos de &Qt"), qApp, SLOT(aboutQt()));

        menuBar()->addMenu(fichier);
        menuBar()->addMenu(aide);

        //Créer la zone centrale
        QScrollArea *area = new QScrollArea(this);

        area->setBackgroundRole(QPalette::Dark);
        area->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        image = new QLabel(this);

        area->setWidget(image);
        setCentralWidget(area);

        //Créer la barre d'outils d'édition
        QToolBar *tb = new QToolBar(tr("&Édition"), this);

        tb->addAction(iloader->icon("object-rotate-left"), tr("Tourner à &gauche"), this, SLOT(rotateLeft(bool)));
        tb->addAction(iloader->icon("object-rotate-right"), tr("Tourner à &droite"), this, SLOT(rotateRight(bool)));
        tb->addAction(iloader->icon("zoom-in"), tr("&Redimensionner"), this, SLOT(scale(bool)));

        addToolBar(Qt::RightToolBarArea, tb);

        //Créer la zone d'exploration des fichiers
        QStringList parts = fileName.split('/');
        parts.removeAt(parts.count() - 1);

        dv = new LDirView(parts.join("/"));

        dv->setItemSize(QSize(64, 64));
        dv->setViewMode(LDirView::IconMode);

        connect(dv, SIGNAL(openFile(QString, bool *)), this, SLOT(dvOpen(QString, bool *)));

        QListView *ld = static_cast<QListView *>(dv->view());
        ld->setFlow(QListView::LeftToRight);
        ld->setWrapping(false);

        QDockWidget *dock = new QDockWidget(tr("&Exploration"), this);
        dock->setWidget(dv);
        addDockWidget(Qt::BottomDockWidgetArea, dock);

        dock->resize(dock->width(), 100);

        //Ouvrir le fichier image si précisé
        openFile(fileName);
}

void Fen::openFile(const QString& fileName)
{
        //Trouver le nom de l'image
        QStringList parts = fileName.split('/');
        QString     name  = parts.last();

        //Changer le titre
        setWindowTitle(name + " - Logram Image Viewer");

        //Charger l'image et l'afficher
        img = QPixmap(fileName);
        image->setPixmap(img);
        image->resize(img.width(), img.height());

        //Adapter le LDirView
        parts.removeAt(parts.count() - 1);
        dv->setDir(parts.join("/"));

        //Mettre à jour le nom de fichier
        flname = fileName;
}

/*
        Événements
 */

void Fen::dvOpen(QString path, bool *cancel)
{
        openFile(dv->job()->localPath(path));

        *cancel = true;
}

void Fen::open(bool)
{
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(), tr("Images (*.png *.gif *.jpg *.jpeg)"));

        openFile(fichier);
}

void Fen::save(bool)
{
        if (flname.isNull())
        {
                saveas(false);
        }
}

void Fen::saveas(bool)
{
        QString fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer un fichier"), QString(), tr("Images (*.png *.gif *.jpg *.jpeg)"));

        img.save(fichier);
        flname = fichier;
}

void Fen::quit(bool)
{
        close();
}

void Fen::rotateLeft(bool)
{
        QTransform trans;

        trans.rotate(-90.0);

        img = img.transformed(trans);
        image->setPixmap(img);
        image->resize(img.width(), img.height());
}

void Fen::rotateRight(bool)
{
        QTransform trans;

        trans.rotate(90.0);

        img = img.transformed(trans);
        image->setPixmap(img);
        image->resize(img.width(), img.height());
}

void Fen::scale(bool)
{
        QString join = tr("x", "Séparateur entre 800x600 par exemple");

        QString size = QString::number(img.width()) + join + QString::number(img.height());

        size = QInputDialog::getText(this, tr("Redimensionner"), tr("Saisissez la nouvelle taille de l'image"), QLineEdit::Normal, size);

        int width  = size.split(join).at(0).toInt();
        int height = size.split(join).at(1).toInt();

        img = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        image->setPixmap(img);
        image->resize(img.width(), img.height());
}
