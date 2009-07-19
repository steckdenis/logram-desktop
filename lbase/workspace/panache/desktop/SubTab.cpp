/*
 * SubTab.cpp
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

#include <SubTab.h>
#include <TabPluginContainer.h>
#include <LConfig.h>
#include <App.h>
#include <DesktopManager.h>

#include <QBitmap>
#include <QToolTip>
#include <QCursor>

SubTab::SubTab(QPixmap pix, QString _tabName, QWidget *parent, App *mapp) : QLabel(0)
{
        app     = mapp;
        tabName = _tabName;
        mparent = parent;

        lblIcon = new QLabel(this);
        lblIcon->setPixmap(pix);
        lblIcon->resize(16, 16);

        theme    = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
        tabInfos = new QSettings(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat, this);

        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        //Create actions
        //	(by Cleriot Simon)
        QAction *actionFix, *actionHide, *actionShow, *actionClose, *actionDesktop;

        actionFix = new QAction(this);
        actionFix->setText(tr("Fixer"));
        actionFix->setCheckable(true);
        connect(actionFix, SIGNAL(triggered()), this, SLOT(emitBlock()));

        actionHide = new QAction(this);
        actionHide->setText(tr("Cacher la tirette"));
        connect(actionHide, SIGNAL(triggered()), this, SLOT(hidePull()));

        actionShow = new QAction(this);
        actionShow->setText(tr("Afficher la tirette"));
        connect(actionShow, SIGNAL(triggered()), this, SLOT(showPull()));

        if (app->desktopManager()->exists)
        {
                actionDesktop = new QAction(this);
                actionDesktop->setText(tr("Envoyer sur le bureau"));
                connect(actionDesktop, SIGNAL(triggered()), this, SIGNAL(toDesktop()));
        }

        actionClose = new QAction(this);
        actionClose->setText(tr("Fermer le plugin"));
        connect(actionClose, SIGNAL(triggered()), this, SIGNAL(closed()));

        listeActions.append(actionFix);
        listeActions.append(actionHide);
        listeActions.append(actionShow);
        if (app->desktopManager()->exists) listeActions.append(actionDesktop);
        listeActions.append(actionClose);

        addActions(listeActions);
}

void SubTab::orientationChanged(TabPluginContainer::tabPosition pos)
{
        QPixmap mpix;

        switch (pos)
        {
                case TabPluginContainer::Left:
                        resize(30, 20);
                        lblIcon->move(10, 2);
                        mpix = QPixmap(theme + "/tabLeft.png");
                        break;

                case TabPluginContainer::Right:
                        resize(30, 20);
                        lblIcon->move(2, 2);
                        mpix = QPixmap(theme + "/tabRight.png");
                        break;

                case TabPluginContainer::Top:
                        resize(20, 30);
                        lblIcon->move(2, 10);
                        mpix = QPixmap(theme + "/tabTop.png");
                        break;

                case TabPluginContainer::Bottom:
                        resize(20, 30);
                        lblIcon->move(2, 2);
                        mpix = QPixmap(theme + "/tabBottom.png");
                        break;
        }

        setPixmap(mpix);
        setMask(mpix.mask());
}

void SubTab::enterEvent(QEvent *event)
{
        QToolTip::showText(QCursor::pos(), toolTip());

        emit entered();

        (void)event;
}

void SubTab::leaveEvent(QEvent *event)
{
        QToolTip::showText(QPoint(0, 0), QString());

        emit leaved();

        (void)event;
}

void SubTab::mousePressEvent(QMouseEvent *event)
{
        emit mousePressed(true);

        (void)event;
}

void SubTab::mouseReleaseEvent(QMouseEvent *event)
{
        emit mousePressed(false);

        (void)event;
}

void SubTab::mouseMoveEvent(QMouseEvent *event)
{
        emit mouseMoved(event->globalX(), event->globalY());
}

void SubTab::hidePull()
{
        hide();
        
        emit pullHided();

        mparent->repaint(0, 0, mparent->width(), mparent->height());

        tabInfos->setValue(tabName + "/showPull", false);
        tabInfos->sync();
}

void SubTab::showPull()
{
        show();

        mparent->repaint(0, 0, mparent->width(), mparent->height());

        tabInfos->setValue(tabName + "/showPull", true);
        tabInfos->sync();
}

void SubTab::emitBlock()
{
        if (listeActions.first()->isChecked())
        {
                emit block(true);
        }
        else
        {
                emit block(false);
        }
}
