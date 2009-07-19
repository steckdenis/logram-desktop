/*
 * DesktopPluginContainer.h
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

#ifndef __DESKTOPPLUGINCONTAINER_H__
#define __DESKTOPPLUGINCONTAINER_H__

#include <QWidget>
#include <QPainter>
#include <QSettings>
#include <QPixmap>
#include <QMouseEvent>
#include <QColor>

#include <IPanacheDesktopPlugin.h>

class App;

class DesktopPluginContainer : public QWidget
{
        Q_OBJECT

public:
        DesktopPluginContainer(QString title, QString icon, QString id, IPanacheDesktopPlugin *plugin, App *mapp);

        IPanacheDesktopPlugin *getPlugin();

        QString tabName, micon, mtitle;

protected:
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

        virtual void resizeEvent(QResizeEvent *event);
        virtual void paintEvent(QPaintEvent *event);

signals:
        void toTab();
        void closed();

private:
        QSettings *tabInfos;
        App       *app;
        QWidget   *pluginWidget;

        int     titlebar_height, border_size, cornersize, fontsize;
        QPixmap mpix;
        QString theme;
        QColor  bgColor, textColor;

        enum ResizeCorner { TopLeft, TopRight, BottomLeft, BottomRight, None };
        ResizeCorner rcorner;
        bool         onDrag, onMove;
        int          a_x, a_y;

        IPanacheDesktopPlugin *nplugin;
};

#endif
