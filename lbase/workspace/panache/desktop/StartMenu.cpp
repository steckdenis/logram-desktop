/*
 * StartMenu.cpp
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

#include <QDesktopWidget>
#include <QPainter>
#include <QBitmap>
#include <QCursor>
#include <QSettings>
#include <QProcess>

#include <App.h>
#include <StartMenu.h>

StartMenu::StartMenu(App *mapp, QWidget *parent) : QWidget(parent)
{
        app    = mapp;
        inmove = false;

        QString theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();

        pnormal = QPixmap(theme + "startMenu_normal.png");
        phover  = QPixmap(theme + "startMenu_hover.png");

        setMask(pnormal.mask());

        //Move menu on the screen side
        int smenuSize = LConfig::logramValue("RoundMenu/StartMenuSize", 40, "Theme").toInt();
        position = (MenuManager::MenuPosition)LConfig::logramValue("RoundMenu/Position", MenuManager::Left, "Theme").toInt();

        resize(smenuSize, smenuSize);

        updatePos(position);

        hover = false;

        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        //Create contextual menu
        contextMenu = new QMenu(this);
        contextMenu->addAction(tr("Envoyer à &gauche"), this, SLOT(goLeft(bool)));
        contextMenu->addAction(tr("Envoyer à &droite"), this, SLOT(goRight(bool)));
        contextMenu->addAction(tr("Envoyer en &haut"), this, SLOT(goTop(bool)));
        contextMenu->addAction(tr("Envoyer en &bas"), this, SLOT(goBottom(bool)));

        //Create Application menu
        LAppMenu mnu;
        menu = populateMenu(mnu.root(), true);
}

void StartMenu::updatePos(MenuManager::MenuPosition mposition)
{
        if (mposition == MenuManager::Left)
        {
                move(0 - (width() / 2), (QApplication::desktop()->height() / 2) - (height() / 2));
        }
        else if (mposition == MenuManager::Right)
        {
                move(QApplication::desktop()->width() - (width() / 2), (QApplication::desktop()->height() / 2) - (height() / 2));
        }
        else if (mposition == MenuManager::Top)
        {
                move((QApplication::desktop()->width() / 2) - (width() / 2), 0 - (height() / 2));
        }
        else if (mposition == MenuManager::Bottom)
        {
                move((QApplication::desktop()->width() / 2) - (width() / 2), QApplication::desktop()->height() - (height() / 2));
        }

        position = mposition;
}

QMenu *StartMenu::populateMenu(LAppMenu::Menu *menu, bool isRoot)
{
        QMenu *rs = new QMenu(this);

        rs->setTitle(menu->title);
        rs->setIcon(menu->icon);

        if (isRoot)
        {
                //It's root menu, add button to deconnect
                rs->addAction(tr("Fermer la session"), this, SLOT(logoutTriggered(bool)));
                rs->addSeparator();
        }

        //Browse entryes
        foreach(LAppMenu::Menu * mnu, menu->menus)
        {
                //Don't show empty menu
                if ((mnu->entries.count() != 0) || (mnu->menus.count() != 0))
                {
                        rs->addMenu(populateMenu(mnu));
                }
        }

        //Browse entryes
        foreach(LAppMenu::Entry * entry, menu->entries)
        {
                QAction *act = new QAction(entry->icon, entry->title, this);

                act->setVisible(true);

                //Add action to list
                actionsDesktops.insert(act, entry->desktopFile);

                //Connect signal
                connect(act, SIGNAL(triggered(bool)), this, SLOT(mnuTriggered(bool)));

                rs->addAction(act);
        }

        return rs;
}

void StartMenu::mnuTriggered(bool checked)
{
        QAction *sender = static_cast<QAction *>(this->sender());

        QString desktopfile = actionsDesktops.value(sender);

        //Launch application
        QSettings set(desktopfile, QSettings::IniFormat);

        QProcess::startDetached(set.value("Desktop Entry/Exec").toString().replace("%U", QString()));

        (void)checked;
}

void StartMenu::logoutTriggered(bool checked)
{
        //In order to deconnect, we leave Panache (session manager close session when Panache quit)
        QCoreApplication::exit(0);

        (void)checked;
}

void StartMenu::enterEvent(QEvent *event)
{
        hover = true;
        repaint(0, 0, width(), height());

        emit enterred();

        (void)event;
}

void StartMenu::leaveEvent(QEvent *event)
{
        hover = false;
        repaint(0, 0, width(), height());

        emit leaved();

        (void)event;
}

void StartMenu::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        if (hover)
        {
                painter.drawPixmap(0, 0, phover);
        }
        else
        {
                painter.drawPixmap(0, 0, pnormal);
        }

        (void)event;
}

void StartMenu::mousePressEvent(QMouseEvent *event)
{
        emit clicked();

        if (event->button() == Qt::LeftButton)
        {
                //Show menu
                menu->popup(QCursor::pos());
        }
        else if (event->button() == Qt::RightButton)
        {
                //Show contextual menu
                contextMenu->popup(QCursor::pos());
        }

        (void)event;
}

void StartMenu::goLeft(bool)
{
        updatePos(MenuManager::Left);

        emit posChanged(position);
}

void StartMenu::goRight(bool)
{
        updatePos(MenuManager::Right);

        emit posChanged(position);
}

void StartMenu::goTop(bool)
{
        updatePos(MenuManager::Top);

        emit posChanged(position);
}

void StartMenu::goBottom(bool)
{
        updatePos(MenuManager::Bottom);

        emit posChanged(position);
}
