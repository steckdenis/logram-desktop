/*
 * widget.h
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

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <QWidget>
#include <QSettings>
#include <QPixmap>
#include <QRect>
#include <QPaintEvent>
#include <QTimer>

class SWidget : public QWidget
{
        Q_OBJECT
public:
        SWidget(QWidget *parent = 0);

        void updateSunPos();

protected:
        virtual void paintEvent(QPaintEvent *event);

public slots:
        void verifSunPos();

private:
        QSettings *settings;
        QPixmap   mbackground, mforeground, msun;
        QString   theme;

        QRect  sRect;                   //Rectangle de la position du soleil
        double minH, maxH, sLeft, cHeight;

        QTimer *timerVerif;
};

#endif
