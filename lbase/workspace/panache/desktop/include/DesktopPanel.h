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

#ifndef DESKTOPPANEL_H
#define DESKTOPPANEL_H

#include <QWidget>
#include <StartButton.h>
#include <LConfig.h>
#include <QBoxLayout>
#include <QLabel>
#include <QPainter>

class DesktopPanel : public QWidget
{

    Q_OBJECT

    public:
        DesktopPanel(QWidget *parent);

    protected slots:
        //For clock
            void timerTimeout();
            void paintEvent(QPaintEvent *event);

    private:
        QWidget *widget;
        QBoxLayout *panel_organize;

        //Start Button
            StartButton *menu;

        //Clock
            QString         hour;
            QTimer          *timer;
            QLabel *time;
};

#endif // DESKTOPPANEL_H
