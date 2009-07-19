/*
 * StartMenu.h
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

#ifndef __STARTMENU_H__
#define __STARTMENU_H__

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QMenu>
#include <QAction>
#include <QHash>

class App;
#include <MenuManager.h>

#include <LAppMenu.h>

class StartMenu : public QWidget
{
        Q_OBJECT

public:
        StartMenu(App *mapp, QWidget *parent = 0);

protected:
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void paintEvent(QPaintEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);

signals:
        void enterred();
        void leaved();
        void clicked();

        void posChanged(MenuManager::MenuPosition pos);

private slots:
        void mnuTriggered(bool checked);
        void logoutTriggered(bool checked);

        void goLeft(bool);
        void goRight(bool);
        void goTop(bool);
        void goBottom(bool);

private:
        App     *app;
        bool    hover;
        QPixmap pnormal, phover;
        MenuManager::MenuPosition position;

        QMenu *menu, *contextMenu;
        QHash<QAction *, QString> actionsDesktops;

        QMenu *populateMenu(LAppMenu::Menu *menu, bool isRoot = false);
        void updatePos(MenuManager::MenuPosition mposition);

        bool inmove;
};

#endif
