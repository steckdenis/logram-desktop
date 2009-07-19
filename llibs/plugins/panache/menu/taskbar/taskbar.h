/*
 * taskbar.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher <steckdenis@yahoo.fr>
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

#ifndef __TASKBAR_H__
#define __TASKBAR_H__

#include <QList>
#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QPixmap>
#include <QBitmap>

#include <QtDebug>

class LIconLoader;

#include <IPanacheMenuPlugin.h>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
}

struct Client
{
        Window win;
        bool   isTop, isActive, isAttention;
};

/**
 * \class TaskPlugin
 * \brief Affiche la liste des fenêtres ouvertes
 */
class TaskPlugin : public QObject, public IPanacheMenuPlugin
{
        Q_OBJECT
        Q_INTERFACES(IPanacheMenuPlugin)

        public :
                void init(MenuManager *parent);

                QList<MenuEntry *> *menuEntries();
                void appEvent(XEvent *event);

                void itemClicked(int index, bool toggled);
                void itemContextMenu(int index, int x, int y);
                
        signals:
                void updateMenu();

        private:
                QList<MenuEntry *> entries;
                QList<Client *>    clients;
                QList<Window>      windows;
                MenuManager        *mparent;

                Display *dpy;
                Window  root;

                QPixmap getIcon(Window w);
                QPixmap getServerIcon(Pixmap icon, Pixmap mask);
                void getName(Window w, char **name);

                void *get_prop_data(Window win, Atom prop, Atom type, int *items);
                unsigned long get_state(Window win);
                bool is_iconified(Window win);
                int client_msg(Window win, Atom msg, unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3, unsigned long data4);

                void updateClientList();
                void updateClientIcons();
};

#endif
