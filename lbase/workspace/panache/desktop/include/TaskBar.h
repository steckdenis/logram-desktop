/*
 * TaskBar.h
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

#ifndef __TASKBAR_H__
#define __TASKBAR_H__

#include <QWidget>
#include <QBrush>
#include <QBitmap>
#include <QHash>

class App;
class IPanacheTaskPlugin;
class QHBoxLayout;

class TaskBar : public QWidget
{
        Q_OBJECT

public:
        TaskBar(App *mapp);

        void appEvent(XEvent *event);

        enum Position
        {
                TopLeft,
                TopRight,
                BottomLeft,
                BottomRight
        };

protected:
        virtual void resizeEvent(QResizeEvent *event);
        virtual void paintEvent(QPaintEvent *event);
        virtual bool event(QEvent *event);

private:
        App *app;

        Position position;
        QBrush   brush;                         //QBrush used to draw background
        QPixmap  mask;                          //Round picture(for mask)
        QPixmap  tlm, trm, blm, brm;            //TopLeftMask, TopRightMask, etc
        int      dw, dh;                        //DesktopWidth et DesktopHeight

        QHBoxLayout *layout;

        QHash<QWidget *, IPanacheTaskPlugin *> plugins;

        void updatePlugins();
        void updatePos();
        void updateMask();
};

#endif /* __TASKBAR_H__ */
