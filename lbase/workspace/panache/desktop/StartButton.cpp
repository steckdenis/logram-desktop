/*
 * StartButton.cpp
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

#include <StartButton.h>

StartButton::StartButton(QWidget *parent) : QPushButton(parent)
{
    theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();

    setCheckable(true);

    hover = false;

    setIconSize(QSize(50, 50));

    setMinimumSize(50, 50);
}

void StartButton::enterEvent(QEvent *event)
{
    hover = true;
    repaint(0, 0, width(), height());

    emit enterred();

    (void)event;
}


void StartButton::leaveEvent(QEvent *event)
{
    hover = false;
    repaint(0, 0, width(), height());

    emit leaved();

    (void)event;
}

void StartButton::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        if (hover)
        {
                painter.drawPixmap(0, 0, QPixmap(theme + "startMenu_hover.png"));
        }
        else
        {
                painter.drawPixmap(0, 0, QPixmap(theme + "startMenu_normal.png"));
        }

        (void)event;
}

