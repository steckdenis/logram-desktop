/*
 * TabPluginContainer.h
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

#ifndef __TABPLUGINCONTAINER_H__
#define __TABPLUGINCONTAINER_H__

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QWidget>
#include <QSettings>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QColor>
#include <QPaintEvent>
#include <QPluginLoader>
#include <QDesktopWidget>
#include <QApplication>
#include <QColor>
#include <QPainter>

#include <QX11Info>

#include <IPanacheDesktopPlugin.h>

class SubTab;
class App;

class TabPluginContainer : public QWidget
{
        Q_OBJECT

	public:
        TabPluginContainer(QString title, QString icon, QString id, IPanacheDesktopPlugin *plugin, App *mapp);
        ~TabPluginContainer();


        IPanacheDesktopPlugin *getPlugin();

        void arrange();
        void calcPos();

        enum tabPosition
        {
                Left,
                Right,
                Top,
                Bottom
        };

        QString tabName, micon, mtitle;
        SubTab  *subtab;                //Tab itself

	public slots:
        void timeout();
        void iconClicked(bool toggled);
        void tabMousePressed(bool pressed);
        void tabMouseMoved(int globalx, int globaly);
        void buttonEntered();
        void pullHided();

	signals:
        void closed();
        void toDesktop();

	protected:
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

        virtual void resizeEvent(QResizeEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void paintEvent(QPaintEvent *event);

	private:
        QSettings *tabInfos;                    //Information about tab saved
        IPanacheDesktopPlugin *nplugin;         //Plugin
        App *app;

        tabPosition position;
        QTimer      *timer;
        bool        fixed, deroule, tabPressed;
        int         d_x, d_y, ax, ay;

        QPixmap HG, HD, BG, BD;
        QColor  bgColor, borderColor;

        enum ResizeCorner { TopLeft, TopRight, BottomLeft, BottomRight, None };
        ResizeCorner rcorner;
        bool         onDrag;
        int          a_x, a_y;

        QString theme;
};

#endif /* __TABPLUGINCONTAINER_H__ */
