/*
 * Manage.cpp
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
#include <Client.h>
#include <QMessageBox>

Client::Client(App *mapp) : QWidget()
{
        app = mapp;
	QSettings set("Logram", "Theme");
        theme = set.value("Theme").toString();
	path = "/usr/share/logram/themes/" + theme + "/";
        bottomleft  = QPixmap(path + "bottomleftcorner.png");
        bottomright = QPixmap(path + "bottomrightcorner.png");
	topleft     = QPixmap(path + "window-topleft-corner.png");
	topright    = QPixmap(path + "window-topright-corner.png"); 
        mleft       = QPixmap(path + "leftside.png");
        mright      = QPixmap(path + "rightside.png");
        mbottom     = QPixmap(path + "bottomside.png");
        bar         = QPixmap(path + "window-titlebar.png");

	set.beginGroup("Windows");
	border_width = set.value("BorderWidth").toInt();
        cornersize = set.value("Titlebar-height").toInt();
	titlebar_height = cornersize;
        fontname   = set.value("Titlebar-font-family").toString();
        fontsize   = 12;
        textcolor  = "#000000";
	set.endGroup();
}

bool Client::init(Window win)
{
        XWindowAttributes wa;
        XSizeHints        wSize;
        long rs;

        //Gère la fenêtre
        setWindowFlags(Qt::X11BypassWindowManagerHint);
        setAttribute(Qt::WA_NoSystemBackground, true);

        setMouseTracking(true);
        qDebug() << "Management d'un client";

        client         = win;
        cwin           = new XWindow(client, app->dpy);
        maximized      = false;
        d_mousePressed = false;
        minit          = 7936898;
        min_width      = 0;
        min_height     = 0;
        max_width      = 0;
        max_height     = 0;

        //Vérifier le type de client pour savoir si on doit le gérer ou non, et comment
        XGetTransientForHint(app->dpy, client, &d_parent);
        if (d_parent != 0)
        {
                //Boîte de dialogue
                dialog = true;
        }

        Atom *atoms;
        unsigned long nitems;

        atoms = 0;

        cwin->getProperty(app->atoms->get("_NET_WM_WINDOW_TYPE"), (void **)&atoms, 0x7FFFFFFF, &nitems);

        bool tomanage = true;   //On manage par défaut

        //Vérifie que les atômes sont implémentés correctement
        if (atoms != 0)
        {
                for (int i = 0; i < nitems; i++)
                {
                        Atom atm;
                        atm = atoms[i];

                        if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_DESKTOP"))
                        {
                                //Fenêtre du bureau
                                app->lowWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_DOCK"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_TOOLBAR"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_MENU"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_SPLASH"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_DROPDOWN"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_POPUP_MENU"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_TOOLTIP"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_NOTIFICATION"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_COMBO"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_DND"))
                        {
                                app->topWindow(client);

                                tomanage = false;
                        }
                        else if (atm == app->atoms->get("_NET_WM_WINDOW_TYPE_UTILITY"))
                        {
                                app->topWindow(winId());
                        }
                }

                if ((tomanage == false) && (nitems != 0))
                {
                        //Afficher le client quand-même
                        XMapWindow(app->dpy, client);
                        XMapSubwindows(app->dpy, client);

                        return false;   //Pas gérer de fenêtre supportant _NET_WM_TYPE (nitems != 0) et n'étant pas d'un type connu
                }
        }

        //Sauvegarde de la fenêtre
        XAddToSaveSet(app->dpy, client);

        //Obtenir les infos nécessaires sur la fenêtre
        XGetWindowAttributes(app->dpy, client, &wa);

        //Le client ne peut pas avoir de bords
        XWindowChanges wchanges;
        wchanges.border_width = 0;
        XConfigureWindow(app->dpy, client, CWBorderWidth, &wchanges);

        //Initialiser les décorations
        if (wa.width < 60)
        {
                wa.width = 320;
        }
        if (wa.height < 60)
        {
                wa.height = 240;
        }

        this->setGeometry(
                wa.x <= 30 ? 30 : wa.x,
                wa.y <= 30 ? 30 : wa.y,
                wa.width + (border_width * 2),
                wa.height + titlebar_height + border_width);

        //Informer la fenêtre de la taille des décorations
        int wgeom[4];
        wgeom[0] = border_width;
        wgeom[1] = border_width;
        wgeom[2] = titlebar_height;
        wgeom[3] = border_width;

#ifndef XA_CARDINAL
 #define XA_CARDINAL    ((Atom)6)
#endif

        cwin->changeProperty(app->atoms->get("_NET_FRAME_EXTENTS"), XA_CARDINAL, 32, &wgeom, 4);

        //Reparenter la fenêtre dans le widget
        XReparentWindow(app->dpy, client, winId(), border_width, titlebar_height);

        //Afficher le client
        XMapWindow(app->dpy, client);
        XMapSubwindows(app->dpy, client);

        //Obtenir les bons événements
        XSelectInput(app->dpy, winId(),
                     KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask |
                     KeymapStateMask |
                     ButtonMotionMask |
                     PointerMotionMask |
                     EnterWindowMask | LeaveWindowMask |
                     FocusChangeMask |
                     ExposureMask |
                     StructureNotifyMask |
                     SubstructureRedirectMask |
                     SubstructureNotifyMask);

        //On a besoin des événements de la souris
        XGrabButton(app->dpy, Button1Mask | Button2Mask, AnyModifier, client, false, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);

        // Obtenir quelques événements sur la fenêtre
        XSetWindowAttributes attr;
        attr.event_mask = ColormapChangeMask | PropertyChangeMask | EnterWindowMask;
        XChangeWindowAttributes(app->dpy, client, CWEventMask, &attr);

        //Trouver la taille minimale de la fenêtre
        XGetWMNormalHints(app->dpy, client, &wSize, &rs);
        if ((rs & PMinSize) != 0)
        {
                min_width  = wSize.min_width;
                min_height = wSize.min_height;
        }
        if ((rs & PMaxSize) != 0)
        {
                max_height = wSize.max_height;
                max_width  = wSize.max_width;
        }

        //Obtenir les infos nécessaires
        addControls();
        getTitle();

        show();

        //Donner le focus à la fenêtre
        XSetInputFocus(app->dpy, client, RevertToParent, CurrentTime);

        //La passer au premier plan
        XRaiseWindow(app->dpy, winId());

        return true;    //La fenêtre est bien gérée
}

void Client::quitClicked(bool checked)
{
        //Simplement supprimer la fenêtre enfant, le serveur X se chargera d'envoyer le message DestroyNotify à Panache
        XEvent ev;

        //Redonner le focus à la fenêtre parente
        if (d_parent != 0)
        {
                //Boîte de dialogue
                XSetInputFocus(app->dpy, d_parent, RevertToParent, CurrentTime);
        }

        ev.type                 = ClientMessage;
        ev.xclient.window       = client;
        ev.xclient.message_type = app->atoms->get("WM_PROTOCOLS");
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = app->atoms->get("WM_DELETE_WINDOW");
        ev.xclient.data.l[1]    = CurrentTime;
        XSendEvent(app->dpy, client, False, NoEventMask, &ev);

        (void)checked;
}

void Client::minClicked(bool checked)
{
        XUnmapWindow(app->dpy, client);

        this->hide();

        //Changer la propriété de la fenêtre cliente
        int entries[2];
        cwin->getProperty(app->atoms->get("WM_STATE"), (void **)&entries, 2);

        entries[0] = IconicState;       //Fenêtre iconifiée

        cwin->changeProperty(app->atoms->get("WM_STATE"), app->atoms->get("WM_STATE"), 32, (void *)entries, 2);

        (void)checked;
}
