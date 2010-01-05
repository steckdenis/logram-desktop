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
#include <iostream>
#include <ToolBar.h>
#include <QMessageBox>

ToolBar::ToolBar(QWidget *mwidget) : QWidget(mwidget)
{
	  widget = mwidget; // définition du widget parent
	  resize(600, 35);

	  QSettings set("Logram", "DesktoQ");
	  QString iconPath = "/usr/share/logram/icons";

	  set.setValue("Background/Color", Qt::blue);
	  set.setValue("Background/FinalColor", Qt::red);

	  loadSettings();
		  


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
	  
	  // définit la position et la taille du widget et affiche le wiget
	  show();
}

void ToolBar::updatePosition(int pos, int size) // place le widget à la bonne place
{
	  QSettings settings("Logram", "DesktoQ");
	 /* settings.setValue("Position", pos);
	  settings.setValue("Size", size); */
	  QSettings set("Logram", "Panel");

	  int n_parent_height = widget->height() / 2;
	  int n_parent_width = widget->width() / 2;  
	  int n_height = height() / 2;
	  int n_width = width() / 2;

	  if(set.value("Position").toInt() == pos) {
		switch (pos) {
			case 1:
				move(n_parent_width - n_width, set.value("Height").toInt()); break; 
			case 2:
				move(widget->width() - set.value("Width").toInt() - width(), n_parent_height - n_height); break;
			case 3:
				move(n_parent_width - n_width, widget->height() - set.value("Height").toInt() - height()); break;
			case 4:
				move(set.value("Width").toInt(), n_parent_height - n_height); break;
		} }
	 else { 
		switch (set.value("Position").toInt()) {
			case 1:
				n_parent_height += set.value("Height").toInt() / 2; break;
			case 3:	
				n_parent_height -= set.value("Height").toInt() / 2;  break;
			case 2:
				n_parent_width -= set.value("Width").toInt() / 2; break;
		 	case 4:
				n_parent_width += set.value("Width").toInt() / 2; break; }
		switch (pos) {
			case 1:
				move(n_parent_width - n_width, 0); break;
			case 2:
				move(widget->width() - width(), n_parent_height - n_height); break;
			case 3:
				move(n_parent_width - n_width, widget->height() - height()); break;
			case 4:
				move(0, n_parent_height - n_height); break; } 
		}

	 int n_size = width() * size / 100;
	// resize(n_size, height());
}

void ToolBar::loadSettings()
{	
	QSettings set("Logram", "DesktoQ");
	QSettings theme("Logram", "Theme");

	pos = set.value("Position").toInt();
	int size = set.value("Size").toInt();
	updatePosition(pos, size);

	// theme management
	set.beginGroup("Background"); 
	Type = set.value("Type").toString();
	pix = QPixmap(750, 35);
	if(Type == "Picture") pix = QPixmap("/usr/share/logram/themes/" + set.value("Theme").toString() + "/toolbar.png");
	else {	
		QVariant var = set.value("Color");
		Color = var.value<QColor>();
		if(Type == "Gradient") {
			var = set.value("FinalColor");
			FinalColor = var.value<QColor>();
			if (set.value("GradientOrientation").toString() == "Vertical") grad = QLinearGradient(QPointF(0, 0), QPointF(750, 0));
			else grad = QLinearGradient(QPointF(0, 0), QPointF(0, 35));
			grad.setColorAt(0, Color);
			grad.setColorAt(1, FinalColor);
			QPainter paint(&pix);
			paint.fillRect(0, 0, 750, 35, grad);			
		}
		else { pix.fill(Color); }
	}

	set.endGroup();
}
			
void ToolBar::paintEvent(QPaintEvent *event) // s'occuppe du fond du widget
{
	  QPainter painter(this);
	QPixmap left("/home/lfs/Bureau/default/pictures/desktoq-left.png");
	QPixmap center("/home/lfs/Bureau/default/pictures/desktoq.png");
	QPixmap right("/home/lfs/Bureau/default/pictures/desktoq-right.png");

	painter.drawPixmap(0, 0, 50, 35, left);
	painter.drawPixmap(50, 0, width() - 100, 35, center);
	painter.drawPixmap(width() - 50, 0, 50, 35, right);

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
