/*
 * StartButton.h
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

#ifndef STARTBUTTON_H
#define STARTBUTTON_H

#include <QPushButton>
#include <LConfig.h>
#include <QPainter>
#include <App.h>
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QBitmap>

class StartButton : public QPushButton
{

    Q_OBJECT

    public:
        StartButton(QWidget *parent);

    protected:
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void paintEvent(QPaintEvent *event);

    signals:
        void enterred();
        void leaved();

    private:
        QString theme;
        QPainter painter;
        bool hover;
};

#endif // STARTBUTTON_H
