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

#include <QCoreApplication>

#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>

#include <App.h>
#include <LConfig.h> 
#include <DesktopManager.h>

#include <ToolBar.h>

ToolBar::ToolBar(QWidget *mwidget) : QWidget(mwidget)
{
	  widget = mwidget; // définition du widget parent

	  // définition du thème
          QString theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
	  QString iconPath = LConfig::logramValue("ImagePath" , "/usr/share/logram/themes/default/", "Theme").toString() + "/icons";
	  brush = QPixmap(theme + "/toolbar-bg.png");
	  
	  // définition des widgets de la barre d'outils
	  QPushButton *back = new QPushButton();
	  QPushButton *forward = new QPushButton();
	  QPushButton *parent = new QPushButton();
	  QPushButton *go = new QPushButton();
	  QPushButton *reload = new QPushButton();
	  QPushButton *home = new QPushButton();
		back->setIcon(QIcon(iconPath + "/back.png"));
		forward->setIcon(QIcon(iconPath + "/forward.png"));
		parent->setIcon(QIcon(iconPath + "/parent.png"));
		go->setIcon(QIcon(iconPath + "/go.png")); 
		reload->setIcon(QIcon(iconPath + "/reload.png"));	
	  adressBar = new QLineEdit();  
	  
	  // connecting some signals/slots
	  connect(home, SIGNAL(clicked()), this, SIGNAL(goHome()));
	  connect(adressBar, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
	  connect(reload, SIGNAL(clicked()), this, SIGNAL(refresh()));
	  connect(go, SIGNAL(clicked()), this, SLOT(returnPressed()));
	  connect(back, SIGNAL(clicked()), this, SIGNAL(goBack()));
	  connect(forward, SIGNAL(clicked()), this, SIGNAL(goNext()));
	  connect(parent, SIGNAL(clicked()), this, SIGNAL(goUp()));

	  // construction du layout
	  QHBoxLayout *layout = new QHBoxLayout();
	  layout->addWidget(back);
	  layout->addWidget(forward);
	  layout->addWidget(parent);
	  layout->addWidget(reload);
	  layout->addWidget(home);
	  layout->addWidget(adressBar);
	  layout->addWidget(go);
	  setLayout(layout);
	  
	  // définit la position et la taille du widget et affice le wiget
	  updatePosition(); 
	  show();
}

void ToolBar::updatePosition() // place le widget à la bonne place
{
	  resize(750, 29);
	  move(25, 0);
}

void ToolBar::paintEvent(QPaintEvent *event) // s'occuppe du fond du widget
{
	  QPainter painter(this);



	  painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 0));
	  painter.fillRect(0, 0, 750, 29, brush);
	  QWidget::paintEvent(event);
}

void ToolBar::changeAdress(QString path)
{
	  adressBar->setText(path);
}

void ToolBar::returnPressed()
{
	QString adress = adressBar->text();
	emit go(adress);
}