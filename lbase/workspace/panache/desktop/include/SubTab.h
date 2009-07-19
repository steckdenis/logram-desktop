/*
 * SubTab.h
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

#ifndef __SUBTAB_H__
#define __SUBTAB_H__

#include <QLabel>
#include <QPixmap>
#include <QList>
#include <QAction>
#include <QSettings>
#include <QWidget>

class App;

#include <TabPluginContainer.h>

class SubTab : public QLabel
{
        Q_OBJECT

public:
        SubTab(QPixmap icon, QString _tabName, QWidget *parent, App *mapp);

        QList<QAction *> listeActions;

protected:
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

public slots:
        void orientationChanged(TabPluginContainer::tabPosition pos);
        void emitBlock();
        void hidePull();
        void showPull();

signals:
        void entered();
        void leaved();
        void mousePressed(bool pressed);
        void mouseMoved(int globalx, int globaly);
        void block(bool block);
        void closed();
        void toDesktop();
        void pullHided();

private:
        QLabel    *lblIcon;
        App       *app;
        QSettings *tabInfos;
        QString   theme;
        QString   tabName;

        QWidget *mparent;
};

#endif
