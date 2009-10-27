/*
 * mwindow.h
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

#include <phonon>

#include <QMainWindow>
#include <QtGui>
#include <QDockWidget>
#include <QSlider>
#include <QDirModel>
#include <QTreeView>
#include <QSystemTrayIcon>

class mwindow : public QMainWindow
{

	Q_OBJECT

public:
	mwindow();
	// building the main window
	void createActions();
	void createMenus();
	void createToolBar();

public slots:
	void newWindow();
	void quitMessageBox();

	void play();
	void pause();
	void stopReading();

        void lvDoubleClicked(const QModelIndex& index);

	void importCD();

	void iconTriggered(QSystemTrayIcon::ActivationReason);

signals:

protected:
	void closeEvent(QCloseEvent *event);

private:

		QSettings *settings;

		QAction *nwindow;
		QAction *quit;
		QAction *back;
		QAction *playpause;
		QAction *stop;
		QAction *forward;
		QAction *addCD;
		QAction *about;
		QAction *qabout;
		QAction *labout;
		QString iconPath;

		QMenuBar *menuBar;
		QMenu *file;
		QMenu *edit;
		QMenu *view;
		QMenu *media;
		QMenu *bibli;
		QMenu *help;

		QToolBar *mediaBar;
		Phonon::SeekSlider *timeSlider;

		QDockWidget *treeDock;
		QTreeView *lv;
		QDirModel *model;
		
		Phonon::MediaObject *media_player;

		QSystemTrayIcon *trayIcon;
			int posx;
			int posy;
};
