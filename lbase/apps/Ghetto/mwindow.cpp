/*
 * mwindow.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Léo Testard <leo.testard@gmail.com>
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

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

#include <mwindow.h>
#include <QDir>
#include <QSlider>


mwindow::mwindow()
{
	settings = new QSettings("Logram", "Ghetto_blaster");

	setWindowTitle("Ghetto Blaster");
	resize(1000, 650);

	createActions();
	createMenus();
	createToolBar();

	media_player = Phonon::createPlayer(Phonon::MusicCategory);
	timeSlider->setMediaObject(media_player);

	lv = new QTreeView;
        model = new QDirModel;
        lv->setModel(model);
        lv->setRootIndex(model->index(QDir::homePath() + tr("/Musique")));
	connect(lv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(lvDoubleClicked(const QModelIndex &)));

	treeDock = new QDockWidget;
	addDockWidget(Qt::LeftDockWidgetArea, treeDock);
	treeDock->setWidget(lv);

	trayIcon = new QSystemTrayIcon(QIcon(iconPath + "media-playback-pause"), this);
	trayIcon->show();
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconTriggered(QSystemTrayIcon::ActivationReason)));
}

void mwindow::quitMessageBox()
{
	int ans = QMessageBox::question(this, tr("Avertissement"), tr("Quitter Ghetto blaster ?"), QMessageBox::Yes | 													   QMessageBox::No);
      
	if (ans == QMessageBox::Yes) { qApp->quit(); }
	else {}
}

void mwindow::closeEvent(QCloseEvent *event)
{
	int ans = QMessageBox::question(this, tr("Avertissement"), tr("Quitter Ghetto blaster ?"), QMessageBox::Yes | 													   QMessageBox::No);
	if (ans == QMessageBox::Yes) { event->accept(); }
	else { event->ignore(); }
}

void mwindow::newWindow()
{
	mwindow *newWindow = new mwindow();
	newWindow->show();
}

void mwindow::play()
{
	disconnect(playpause);
	playpause->setIcon(QIcon(iconPath + "media-playback-pause.png"));
	playpause->setToolTip(tr("Met la lecture en pause"));
	playpause->setText(tr("Pause"));
	connect(playpause, SIGNAL(triggered()), this, SLOT(pause()));
	media_player->play();
}

void mwindow::pause()
{
	disconnect(playpause);
	playpause->setIcon(QIcon(iconPath + "media-playback-start.png"));
	playpause->setToolTip(tr("Demarre la lecture"));
	playpause->setText(tr("Play"));
	connect(playpause, SIGNAL(triggered()), this, SLOT(play()));
	media_player->pause();
}

void mwindow::stopReading()
{
	disconnect(playpause);
	playpause->setIcon(QIcon(iconPath + "media-playback-start.png"));
	playpause->setToolTip(tr("Demarre la lecture"));
	playpause->setText(tr("Play"));
	connect(playpause, SIGNAL(triggered()), this, SLOT(play()));
	media_player->stop();
}

void mwindow::importCD()
{}

void mwindow::lvDoubleClicked(const QModelIndex& index)
{
        QString filePath = model->filePath(index);

        media_player->setCurrentSource(filePath);

	disconnect(playpause);
	playpause->setIcon(QIcon(iconPath + "media-playback-pause.png"));
	playpause->setToolTip(tr("Met la lecture en pause"));
	playpause->setText(tr("Pause"));
	connect(playpause, SIGNAL(triggered()), this, SLOT(pause()));
        media_player->play();
}

void mwindow::iconTriggered(QSystemTrayIcon::ActivationReason)
{
	if (this->isHidden()) { this->show(); 
				this->move(posx, posy); }
	else { this->hide(); 
		posx = this->x();
		posy = this->y(); }
}
