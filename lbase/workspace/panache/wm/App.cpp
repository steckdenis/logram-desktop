/*
 * App.cpp
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

#include <QX11Info>

#include <App.h>
#include <Client.h>

App::App(int& argc, char *argv[]) : LApplication(argc, argv)
{
        dpy        = QX11Info::display();
        rootWindow = QX11Info::appRootWindow();
        atoms      = new XAtoms(dpy);
        cfg        = new LConfig(QString("Panache-wm"));

        /* Initialise l'interface avec X11 */

        //Crée la fenêtre de conformité aux normes
        Window  cfg_win = XCreateSimpleWindow(dpy, rootWindow, 0, 0, 10, 10, 0, 0, 0);
        XWindow conf_win(cfg_win, dpy);
        conf_win.changeProperty(atoms->get("_NET_SUPPORTING_WM_CHECK"), atoms->get("WINDOW"), 32, &cfg_win, 1);
        conf_win.changeProperty(atoms->get("_NET_WM_NAME"), atoms->get("UTF8_STRING"), 8, (char *)"Panache-wm", 11);
        XWindow rootwin(rootWindow, dpy);
        rootwin.changeProperty(atoms->get("_NET_SUPPORTING_WM_CHECK"), atoms->get("WINDOW"), 32, &cfg_win, 1);

        //Informer les clients qu'on supporte un certain nombre d'Atoms
        Atom supportedAtoms[10] =
        {
                atoms->get("_NET_CLIENT_LIST"),
                atoms->get("_NET_NUMBER_OF_DESKTOPS"),
                atoms->get("_NET_CURRENT_DESKTOP"),
                atoms->get("_NET_ACTIVE_WINDOW"),
                atoms->get("_NET_WORKAREA"),
                atoms->get("_NET_CLOSE_WINDOW"),
                atoms->get("_NET_MOVERESIZE_WINDOW"),
                atoms->get("_NET_RESTACK_WINDOW"),
                atoms->get("_NET_REQUEST_FRAME_EXTENTS"),
                atoms->get("_NET_WINDOW_TYPE")
        };

        rootwin.changeProperty(atoms->get("_NET_SUPPORTED"), atoms->get("ATOM"), 32, &supportedAtoms, 10);

        scan();         //Obtiens la liste des fenêtres déjà présentes dans root

        //L'application est totalement chargée
        appLoaded("panache-wm");
}

void App::scan()
{
        unsigned int      num, i;
        Window            *wins, d1, d2;
        XWindowAttributes wa;

        //On liste les fenêtres de root, donc les fenêtres principales affichées à l'écran
        wins = NULL;
        if (XQueryTree(dpy, rootWindow, &d1, &d2, &wins, &num))
        {
                for (i = 0; i < num; i++)
                {
                        if (!XGetWindowAttributes(dpy, wins[i], &wa))
                        {
                                continue;
                        }
                        if (wa.override_redirect)
                        {
                                continue;
                        }
                        if (wa.map_state == IsViewable)
                        {
                                manage(wins[i]);
                        }
                }
        }

        if (wins)
        {
                XFree(wins);
        }

        //Définis le curseur par défaut
        XDefineCursor(dpy, rootWindow, QCursor(Qt::ArrowCursor).handle());
}

void App::manage(Window win)
{
        Client *cli     = new Client(this);
        bool   tomanage = cli->init(win);

        restackWindows();       //On a peut-être ajouté des DOCKs, menus, etc

        if (!tomanage)
        {
                return;
        }

        clients.append(cli);
        updateClientList();
}

void App::setActiveWindow(Window win)
{
        XWindow rootwin(rootWindow, dpy);

        rootwin.changeProperty(atoms->get("_NET_ACTIVE_WINDOW"), atoms->get("WINDOW"), 32, &win, 1);
}

void App::updateClientList()
{
        //Met à jour la propriété _NET_CLIENT_LIST de la fenêtre root
        QList<Window> wins;
        int           i;

        for (i = 0; i < clients.count(); i++)
        {
                wins.append(clients.at(i)->client);
        }

        Window *windows = new Window[wins.count()];

        for (i = 0; i < wins.count(); i++)
        {
                windows[i] = wins.at(i);
        }

        //Met à jour la propriété de la fenêtre root
        XWindow rootwin(rootWindow, dpy);
        rootwin.changeProperty(atoms->get("_NET_CLIENT_LIST"), atoms->get("WINDOW"), 32, windows, wins.count());

        delete[] windows;
}

void App::restackWindows()
{
        //Raise les fenêtres du dessus, et Low celles qui voivent être en dessous
        for (int i = 0; i < topWindows.count(); i++)
        {
                XRaiseWindow(dpy, topWindows.at(i));
        }

        for (int i = 0; i < lowWindows.count(); i++)
        {
                XLowerWindow(dpy, lowWindows.at(i));
        }
}

void App::topWindow(Window win)
{
        if (!topWindows.contains(win))
        {
                topWindows.append(win);
        }
}

void App::lowWindow(Window win)
{
        if (!lowWindows.contains(win))
        {
                lowWindows.append(win);
        }
}

void App::raiseClient(Client *cli)
{
        if (cli == topClient)
        {
                return;
        }
        
        XSetInputFocus(dpy, cli->client, RevertToParent, CurrentTime);
        XRaiseWindow(dpy, cli->winId());
        
        setActiveWindow(cli->client);
        restackWindows();
        
        topClient = cli;
}

bool App::x11EventFilter(XEvent *event)
{
        Client            *cli;
        XWindowAttributes wa;

        //Récupérer les bons événements
        XSelectInput(dpy, rootWindow, SubstructureRedirectMask | SubstructureNotifyMask | StructureNotifyMask);

        switch (event->type)
        {
                case MapRequest:
                        //On a créé une nouvelle fenêtre
                        if (!XGetWindowAttributes(dpy, event->xmaprequest.window, &wa))
                        {
                                return false;
                        }
                        if (wa.override_redirect)
                        {
                                return false;
                        }

                        manage(event->xmaprequest.window);

                        return true;

                        break;

                case PropertyNotify:
                        //Une propriété a changé
                        cli = findClient(event->xproperty.window);

                        if (cli != 0)
                        {
                                cli->updateProperty(event->xproperty.atom);
                        }

                        break;

                case ButtonPress:
                        cli = findClient(event->xbutton.window);

                        if ((event->xbutton.button == Button1) || (event->xbutton.button == Button2))
                        {
                                if (cli != 0)
                                {
                                        raiseClient(cli);
                                }
                        }

                        // Ne pas oublier d'envoyer l'événement à la fenêtre
                        XAllowEvents(dpy, ReplayPointer, CurrentTime);
                        break;

                case EnterNotify:
                        //Le curseur entre sur la fenêtre
                        cli = findClient(event->xcrossing.window);

                        if (cli != 0)
                        {
                                //Le pointeur du client doit redevenir le pointeur normal
                                cli->setCursor(Qt::ArrowCursor);
                        }

                        break;

                case ConfigureRequest:
                        //Configure Request d'une fenêtre
                        XConfigureRequestEvent * cev;

                        cev = &event->xconfigurerequest;

                        cli = findClient(cev->window);
                        if (cli != 0)
                        {
                                if ((cev->value_mask & (CWX | CWY | CWWidth | CWHeight)) != 0)
                                {
                                        cli->updateSize(cev->x, cev->y, cev->width, cev->height);
                                }

                                //Mettre la fenêtre au premier/dernier plan si demandé
                                if ((cev->value_mask & CWStackMode) != 0)
                                {
                                        XWindowChanges wch;
                                        wch.stack_mode = cev->detail;
                                        wch.sibling    = cev->above;
                                        XConfigureWindow(dpy, cli->winId(), CWStackMode, &wch);

                                        restackWindows();
                                }

                                return true;
                        }
                        else
                        {
                                //Fenêtre non-gérée, ne pas la gêner
                                if ((cev->value_mask & (CWX | CWY | CWWidth | CWHeight)) != 0)
                                {
                                        XMoveResizeWindow(dpy, cev->window, cev->x, cev->y, cev->width, cev->height);
                                }

                                //Mettre la fenêtre au premier/dernier plan si demandé
                                if ((cev->value_mask & CWStackMode) != 0)
                                {
                                        XWindowChanges wch;
                                        wch.stack_mode = cev->detail;
                                        wch.sibling    = cev->above;
                                        XConfigureWindow(dpy, cev->window, CWStackMode, &wch);

                                        restackWindows();
                                }
                        }

                        break;

                case DestroyNotify:
                        //Un client est fini
                        cli = findClient(event->xdestroywindow.window);
                        if (cli != 0)
                        {
                                int clindex = clients.indexOf(cli);
                                clients.removeAt(clindex);
                                delete cli;

                                qDebug() << "*** Suppression d'un client";

                                updateClientList();

                                return true;
                        }

                        //Retirer de la liste la fenêtre
                        topWindows.removeOne(event->xdestroywindow.window);
                        lowWindows.removeOne(event->xdestroywindow.window);

                        break;

                case UnmapNotify:
                        //Un client ne s'affiche plus
                        cli = findClient(event->xunmap.window);
                        if (cli != 0)
                        {
                                cli->hide();
                        }

                        break;

                case MapNotify:
                        //Un client est affiché
                        cli = findClient(event->xmap.window);
                        if (cli != 0)
                        {
                                cli->show();
                        }

                        break;

                case ClientMessage:
                        XClientMessageEvent * cv;

                        cv = &event->xclient;

                        if (cv->message_type == atoms->get("_NET_ACTIVE_WINDOW"))
                        {
                                //Un pager veut changer la fenêtre active, la mapper
                                cli = findClient(cv->window);
                                if (cli != 0)
                                {
                                        cli->show();
                                        XMapWindow(dpy, cli->client);
                                        XSetInputFocus(dpy, cli->client, RevertToParent, CurrentTime);
                                        XRaiseWindow(dpy, cli->winId());
                                        setActiveWindow(cv->window);

                                        restackWindows();
                                }

                                return true;
                        }
                        else if (cv->message_type == atoms->get("WM_CHANGE_STATE") && cv->data.l[0] == IconicState)
                        {
                                //On iconifie une fenêtre
                                cli = findClient(cv->window);
                                if (cli != 0)
                                {
                                        cli->minClicked(false);         //La paramètre n'est pas utilisé
                                }
                        }
                        else if (cv->message_type == atoms->get("_NET_CLOSE_WINDOW"))
                        {
                                cli = findClient(cv->window);
                                if (cli != 0)
                                {
                                        cli->quitClicked(false);
                                }

                                return true;
                        }
        }

        return false;
}

Client *App::findClient(Window win)
{
        Client *cli;

        for (int i = 0; i < clients.count(); i++)
        {
                cli = clients.at(i);
                if (cli->client == win)
                {
                        return cli;
                }
        }

        return 0;
}
