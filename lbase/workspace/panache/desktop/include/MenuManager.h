/*
 * MenuManager.h
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

#ifndef __MENUMANAGER_H__
#define __MENUMANAGER_H__

#include <QWidget>
#include <QTimer>
#include <QHash>
#include <QList>
#include <QColor>
#include <QPoint>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMap>

#include <IPanacheMenuPlugin.h>

class StartMenu;
class App;

class MenuManager : public QWidget
{
        Q_OBJECT
public:
        MenuManager(App *mapp);

        void drawLevel(QPainter *painter, int num);
        void drawEntry(QPainter *painter, MenuEntry *entry);
        bool entryContains(MenuEntry *entry, int x, int y);

        enum MenuPosition
        {
                Left,
                Right,
                Top,
                Bottom
        };

        void appEvent(XEvent *event);

public slots:
        void startMenuEnterred();
        void startMenuLeaved();
        void startMenuClicked();
        void startMenuTimerTimeout();
        void wTimerTimeout();
        void posChanged(MenuManager::MenuPosition pos);

        void updatePlugins();                   //Update entry list from plugins
        void updateMenu();                      //Calls updatePlugins and redraw the menu

protected:
        virtual void paintEvent(QPaintEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void wheelEvent(QWheelEvent *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);

private:
        App       *app;
        StartMenu *smenu;
        QTimer    *smenuExpand;
        bool      smenuE;               //True if menu is unfolding, else false
        int       smenuETicks;
        QPoint    circleCenterPos;

        MenuPosition position;

        QHash<int, IPanacheMenuPlugin *> plugins;
        QHash<int, MenuEntry *>          entries;
        QHash<int, int>   levelCurrent;                         //Icon at the center of each level
        QHash<int, qreal> levelAngle;                           //rotating angle of each level
        int maxlevel;

        bool   wDown;                                           //Scrolling bottom
        int    wTicks;
        QTimer *wTimer;

        int       activeLayer;
        MenuEntry *activeEntry;

        int    stagesize, fontsize, buttonsize, entryspacing;
        QColor bcolor, fontcolor;
        QBrush brush, brushactive;

        QPixmap pcache, activeItemBg;
        QString theme;

        QTransform getTransform(MenuEntry *entry, bool *toShow = 0, double *ma = 0);
};

#endif
