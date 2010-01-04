/*
 * DesktopPanel.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Flavien Lefebvre <flavien-lefebvre@hotmail.fr>
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

#include <DesktopPanel.h>

#include <DesktopManager.h>
#include <iostream>

DesktopPanel::DesktopPanel(QWidget *parent) : QWidget(parent)
{
    widget = parent; 	
	QSettings set("Logram", "Panel");	
	set.setValue("Background/Color", Qt::black);
	set.setValue("Background/FinalColor", Qt::white);
	switch (set.value("Position").toInt()) {
		case 1:
			resize(widget->width(), set.value("Size").toInt());
			move(0, 0);
			break;
		case 2:
			resize(set.value("Size").toInt(), widget->height());
			move(widget->width() - width(), 0);
			break;			
		case 4:
			resize(set.value("Size").toInt(), widget->height());
			move(0, 0);
			break;
		default:	// by default, the panel is at the bottom
			resize(widget->width(), set.value("Size").toInt());
			move(0, widget->height() - height());
			break;
		}    
	loadSettings();

    //menu
        menu = new StartButton(this);

    //clock
        //Create and personalize the Label
        set.beginGroup("Clock");
	time = new QLabel;
        time->setFont(QFont(set.value("Font").toString(), set.value("Fontsize").toInt()));
        time->setPalette(QPalette(QColor(set.value("Clock_color").toChar())));
        time->setPalette(QPalette(QColor(1, 1, 1)));
        time->setAlignment(Qt::AlignCenter);
	set.endGroup();
        time->setFixedSize(set.value("Size").toInt(), set.value("Size").toInt());

        //Call a timer to refresh the time every 1 second
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
        timer->start(1000);

    //Organization
        QBoxLayout *panel_organize = new QBoxLayout(QBoxLayout::TopToBottom); // give a default organization here
	int pos = set.value("Position").toInt();
        if(pos == 1 || pos == 3) 
            panel_organize->setDirection(QBoxLayout::LeftToRight); //horizontal
        else 
            panel_organize->setDirection(QBoxLayout::TopToBottom); //vertical
        
        panel_organize->addWidget(menu);
        panel_organize->addStretch();
        panel_organize->addWidget(time);
        panel_organize->setContentsMargins(0, 0, 0, 0);

        setLayout(panel_organize);

    show();
}

void DesktopPanel::timerTimeout()
{
    hour = QTime::currentTime().toString(Qt::SystemLocaleShortDate);

    time->setText(hour);
}

void DesktopPanel::loadSettings()
{
	QSettings set("Logram", "Panel");
	set.beginGroup("Background");
	QSettings theme("Logram", "Theme");
	pix = QPixmap(width(), height());
	QString type = set.value("Type").toString();
	if(type == "Picture") pix = QPixmap("/usr/share/Logram/themes/" + theme.value("Theme").toString() + "/panel.png");
	else {
		QVariant var = set.value("Color");	
		QColor Color = var.value<QColor>();
		if(type == "Gradient") {
			var = set.value("FinalColor");
			QColor FinalColor = var.value<QColor>();	
			QLinearGradient grad;
			if(set.value("GradientOrientation").toString() == "Vertical") grad = QLinearGradient(QPointF(0, 0), QPointF(width(), 0));
			else grad = QLinearGradient(QPointF(0, 0), QPointF(0, height()));
			grad.setColorAt(0, Color);
			grad.setColorAt(1, FinalColor);
			QPainter paint(&pix);	
			paint.fillRect(0, 0, width(), height(), grad);
		}
		else 
			pix.fill(Color);
	}
	set.endGroup();
}
			
void DesktopPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
	QBrush brush(pix);	
	painter.setBrush(brush);
	painter.setRenderHint(QPainter::Antialiasing);	
	painter.drawRect(0, 0, width(), height());
    

    QWidget::paintEvent(event);
}
