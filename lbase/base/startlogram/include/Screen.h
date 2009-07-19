/*
 * Screen.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <QWidget>
#include <QPixmap>
#include <QColor>
#include <QProcess>

#include <LConfig.h>

class SScreen : public QWidget
{
        Q_OBJECT

public:
        SScreen(int width, int height);

        bool appEvent(XEvent *event);

protected:
        virtual void paintEvent(QPaintEvent *event);

signals:
        void quit();

private:
        QPixmap background, box, pgsbg, pgsfg;
        QColor  textColor;
        int     fontSize, pgsx, pgsy, tx, ty;

        int numprocs, procstarted;

        QString  started;
        QProcess sessionProc;

        LConfig *cfg;
};

#endif /* __SCREEN_H__ */
