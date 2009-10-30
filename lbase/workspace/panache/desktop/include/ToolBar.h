/*
 * ToolBar.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Leo Testard <leo.testard@gmail.com>
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

#ifndef __TOOL_BAR__
#define __TOOL_BAR__

#include <QCoreApplication>
#include <QLineEdit>
#include <App.h>
#include <LConfig.h> 
#include <QHBoxLayout>
#include <QWidget>

/**
 * \class ToolBar
 * \brief Classe de la barre d'outils de navigation
 */

class ToolBar : public QWidget
{

	Q_OBJECT

	public:
	ToolBar(QWidget *mwidget);
	void changeAdress(QString path);

	private:

	protected:
	void updatePosition();
	virtual void paintEvent(QPaintEvent *event);
  
	signals:
	void go(QString adress);
 	void goHome();
	void refresh();
	void goBack();
	void goNext();
	void goUp();
	
	public slots:
	void returnPressed();

	private:
	QWidget *widget;
	QBrush brush;
	
	QLineEdit *adressBar;

};

#endif // __TOOL_BAR__