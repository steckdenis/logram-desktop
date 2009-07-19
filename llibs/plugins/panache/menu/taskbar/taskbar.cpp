/*
 * taskbar.cpp
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

#include <QColor>
#include <QPainter>
#include <QIcon>

#include "taskbar.h"

#include <XWindow.h>

static const char *atom_names[] =
{
        "KWM_WIN_ICON",
        "_NET_CURRENT_DESKTOP",
        "_NET_CLIENT_LIST",
        "WM_STATE",
        "_NET_CLOSE_WINDOW",
        "_NET_ACTIVE_WINDOW",
        "_NET_WM_DESKTOP",
        "_NET_WM_WINDOW_TYPE",
        "_NET_WM_WINDOW_TYPE_DOCK",

        "_NET_WM_STATE",
        "_NET_WM_STATE_SHADED",
        "_NET_WM_STATE_SKIP_TASKBAR",
        "_NET_WM_STATE_DEMANDS_ATTENTION",
        "_NET_WM_NAME"
};

#define ATOM_COUNT    (sizeof(atom_names) / sizeof(atom_names[0]))

static Atom atoms[ATOM_COUNT];

#define atom_KWM_WIN_ICON                       atoms[0]
#define atom__NET_CURRENT_DESKTOP               atoms[1]
#define atom__NET_CLIENT_LIST                   atoms[2]
#define atom_WM_STATE                           atoms[3]
#define atom__NET_CLOSE_WINDOW                  atoms[4]
#define atom__NET_ACTIVE_WINDOW                 atoms[5]
#define atom__NET_WM_DESKTOP                    atoms[6]
#define atom__NET_WM_WINDOW_TYPE                atoms[7]
#define atom__NET_WM_WINDOW_TYPE_DOCK           atoms[8]

#define atom__NET_WM_STATE                      atoms[9]
#define atom__NET_WM_STATE_SHADED               atoms[10]
#define atom__NET_WM_STATE_SKIP_TASKBAR         atoms[11]
#define atom__NET_WM_STATE_DEMANDS_ATTENTION    atoms[12]
#define atom__NET_WM_NAME                       atoms[13]

void TaskPlugin::init(MenuManager *parent)
{
        mparent = parent;

        //Autres
        dpy  = QX11Info::display();
        root = QX11Info::appRootWindow();
        XInternAtoms(dpy, (char **)atom_names, ATOM_COUNT, False, atoms);
        
        connect(this, SIGNAL(updateMenu()), (QObject *) parent, SLOT(updateMenu()));

        //Mettre à jour la liste des clients
        updateClientList();
}

/* Originally from wmctrl by Tomas Styblo */
int TaskPlugin::client_msg(Window win, Atom msg,
                           unsigned long data0, unsigned long data1,
                           unsigned long data2, unsigned long data3,
                           unsigned long data4)
{
        XEvent event;
        long   mask = SubstructureRedirectMask | SubstructureNotifyMask;

        event.xclient.type         = ClientMessage;
        event.xclient.serial       = 0;
        event.xclient.send_event   = True;
        event.xclient.message_type = msg;
        event.xclient.window       = win;
        event.xclient.format       = 32;
        event.xclient.data.l[0]    = data0;
        event.xclient.data.l[1]    = data1;
        event.xclient.data.l[2]    = data2;
        event.xclient.data.l[3]    = data3;
        event.xclient.data.l[4]    = data4;

        if (XSendEvent(dpy, root, False, mask, &event))
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

void *TaskPlugin::get_prop_data(Window win, Atom prop, Atom type, int *items)
{
        Atom          type_ret;
        int           format_ret;
        unsigned long items_ret;
        unsigned long after_ret;
        unsigned char *prop_data;

        prop_data = 0;

        XGetWindowProperty(dpy, win, prop, 0, 0x7fffffff, False,
                           type, &type_ret, &format_ret, &items_ret,
                           &after_ret, &prop_data);
        if (items)
        {
                *items = items_ret;
        }

        return prop_data;
}

#define STATE_ICONIFIED            1
#define STATE_SHADED               2
#define STATE_HIDDEN               4
#define STATE_DEMANDS_ATTENTION    8

unsigned long TaskPlugin::get_state(Window win)
{
        Atom          *data;
        int           items, a;
        unsigned long state = 0;

        data = (Atom *)get_prop_data(win, atom__NET_WM_STATE, XA_ATOM, &items);
        if (data)
        {
                for (a = 0; a < items; a++)
                {
                        if (data[a] == atom__NET_WM_STATE_SHADED)
                        {
                                state |= STATE_SHADED;
                        }
                        if (data[a] == atom__NET_WM_STATE_SKIP_TASKBAR)
                        {
                                state |= STATE_HIDDEN;
                        }
                        if (data[a] == atom__NET_WM_STATE_DEMANDS_ATTENTION)
                        {
                                state |= STATE_DEMANDS_ATTENTION;
                        }
                }
                XFree(data);
        }

        if (is_iconified(win))
        {
                state |= STATE_ICONIFIED;
        }

        return state;
}

bool TaskPlugin::is_iconified(Window win)
{
        unsigned long *data;
        int           ret = false;

        data = (unsigned long *)get_prop_data(win, atom_WM_STATE, atom_WM_STATE, 0);
        if (data)
        {
                if (data[0] == IconicState)
                {
                        ret = true;
                }
                XFree(data);
        }

        return ret;
}

void TaskPlugin::updateClientList()
{
        Window            *wins;
        Window            focus_win;
        int               num, rev;
        Client            *cli;
        XWindowAttributes wa;

        XGetInputFocus(dpy, &focus_win, &rev);

        qDebug() << "updateClientList()";

        wins = (Window *)get_prop_data(root, atom__NET_CLIENT_LIST, AnyPropertyType, &num);

        if (wins == 0)
        {
                return;
        }

        //Supprimer les fenêtres qui ne sont plus dans la liste
        for (int i = 0; i < windows.count(); i++)
        {
                Window ww;
                ww  = windows.at(i);
                cli = clients.at(i);
                unsigned long state = get_state(ww);

                //Vérifier que la fenêtre est dans la liste
                for (int j = 0; j < num; j++)
                {
                        if ((ww == wins[j]) && ((state & STATE_HIDDEN) == 0))
                        {
                                goto dont_del;
                        }
                }

                //Supprimer la fenêtre, elle n'est plus dans la liste
                clients.removeAt(i);
                windows.removeAt(i);

                MenuEntry *entry;
                entry = entries.at(i);
                entries.removeAt(i);

                delete cli;
                delete entry;

                continue;

dont_del:;

                if (ww == focus_win)
                {
                        cli->isTop = true;
                }
                else
                {
                        cli->isTop = false;
                }
        }

        //Ajouter les nouvelles fenêtres
        for (int i = 0; i < num; i++)
        {
                Window ww;
                ww = wins[i];
                unsigned long state = get_state(ww);

                if (!windows.contains(ww) || ((state | STATE_HIDDEN) == 0))
                {
                        //Ajouter la fenêtre
                        if (!XGetWindowAttributes(dpy, ww, &wa))
                        {
                                return;
                        }
                        if (wa.override_redirect)
                        {
                                return;
                        }

                        char *name;

                        getName(ww, &name);

                        //Créer l'entrée
                        MenuEntry *entry;
                        entry            = new MenuEntry;
                        entry->title     = QString(name);
                        entry->icon      = getIcon(ww);
                        entry->togglable = false;
                        entry->toggled   = false;
                        entry->separator = false;
                        entry->plugin    = this;

                        entries.append(entry);

                        //Ajouter la fenêtre dans la liste des gérées
                        windows.append(ww);

                        //Créer le client
                        Client *cli = new Client;

                        cli->win         = ww;
                        cli->isTop       = true;
                        cli->isActive    = true;
                        cli->isAttention = false;

                        clients.append(cli);

                        XFree(name);

                        XSelectInput(dpy, ww,
                                     KeyPressMask | KeyReleaseMask |
                                     ButtonPressMask | ButtonReleaseMask |
                                     KeymapStateMask |
                                     ButtonMotionMask |
                                     PointerMotionMask |
                                     EnterWindowMask | LeaveWindowMask |
                                     FocusChangeMask |
                                     ExposureMask |
                                     StructureNotifyMask |
                                     PropertyChangeMask);
                }
        }
        
        emit updateMenu();
}

void TaskPlugin::itemClicked(int index, bool toggled)
{
        Client *cli = clients.at(index);

        if (cli->isTop)
        {
                //Iconifier la fenêtre
                cli->isActive = false;
                cli->isTop    = false;
                XIconifyWindow(dpy, cli->win, 0);
        }
        else
        {
                //La restaurer
                client_msg(cli->win, atom__NET_ACTIVE_WINDOW, 0, 0, 0, 0, 0);
                XSetInputFocus(dpy, cli->win, RevertToNone, CurrentTime);

                cli->isActive = true;
                cli->isTop    = true;

                //Les autres fenêtres ne sont plus top
                foreach(Client * client, clients)
                {
                        if (client != cli)
                        {
                                client->isTop = false;
                        }
                }
        }

        (void)toggled;
}

QList<MenuEntry *> *TaskPlugin::menuEntries()
{
        return &entries;
}

void TaskPlugin::itemContextMenu(int index, int x, int y)
{
        (void)index;
        (void)x;
        (void)y;
}

void TaskPlugin::appEvent(XEvent *event)
{
        Client    *cli;
        MenuEntry *entry;
        Window    w;
        QPixmap   pix;
        Atom      at;

        XSelectInput(dpy, root, FocusChangeMask | PropertyChangeMask);

        switch (event->type)
        {
                case PropertyNotify:
                        w  = event->xproperty.window;
                        at = event->xproperty.atom;

                        if (w == root)
                        {
                                if (at == atom__NET_CLIENT_LIST)
                                {
                                        updateClientList();
                                }
                        }
                        else
                        {
                                if (windows.contains(w))
                                {
                                        cli   = clients.at(windows.indexOf(w));
                                        entry = entries.at(windows.indexOf(w));

                                        if ((at == XA_WM_NAME) || (at == atom__NET_WM_NAME))
                                        {
                                                //Le titre d'une fenêtre a changé
                                                char *titre;

                                                titre = (char *)get_prop_data(w, XA_WM_NAME, XA_STRING, 0);

                                                entry->title = QString(titre);

                                                XFree(titre);
                                        }
                                        else if (at == atom__NET_WM_STATE)
                                        {
                                                unsigned long state = get_state(w);

                                                cli->isAttention = (state & STATE_DEMANDS_ATTENTION) != 0;
                                                cli->isActive    = (state & STATE_ICONIFIED) == 0;
                                        }
                                        else if (at == XA_WM_HINTS)
                                        {
                                                entry->icon = getIcon(w);
                                        }
                                }
                        }

                        break;

                case FocusIn:
                        Window focus_win;
                        focus_win = event->xfocus.window;

                        for (int i = 0; i < windows.count(); i++)
                        {
                                Window ww;
                                ww  = windows.at(i);
                                cli = clients.at(i);

                                if (ww == focus_win)
                                {
                                        cli->isTop = true;
                                }
                                else
                                {
                                        cli->isTop = false;
                                }
                        }

                        break;
        }
}

void TaskPlugin::getName(Window w, char **name)
{
        Atom          type;
        int           format;
        unsigned long nitems = 0;
        unsigned long extra  = 0;

        XGetWindowProperty(QX11Info::display(), w, atom__NET_WM_NAME, 0, 500, false, AnyPropertyType, &type, &format, &nitems, &extra, (unsigned char **)name);

        if (*name != NULL)
        {
                return;
        }

        XFetchName(QX11Info::display(), w, name);
}

QPixmap TaskPlugin::getIcon(Window w)
{
        XWindow win(w, QX11Info::display());
        QIcon   rs = win.windowIcon();

        if (rs.isNull())
        {
        	return QPixmap("/usr/share/logram/themes/default/icons/xapp.png");
        }
        //TODO: Ces lignes plantes (segfault) sur le if (rs.isNull()), il faut trouver pourquoi

        return rs.pixmap(32, 32);
}

//Pas oublier d'exporter le plugin.
Q_EXPORT_PLUGIN2(taskbar, TaskPlugin)
