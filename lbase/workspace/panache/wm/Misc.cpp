/*
 * Misc.cpp
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

#include <QIcon>

#include <Client.h>

extern "C" {
#include <X11/Xatom.h>
}

void Client::getTitle()
{
        char *result;
        unsigned long items;
        Status status;

        //D'abord tenter le _NET_WM_NAME
        status = cwin->getProperty(app->atoms->get("_NET_WM_NAME"), (void **)&result, 128, &items);

        if (items == 0 || status != Success)
        {
                //Il n'y a pas de _NET_WM_NAME
                XFetchName(app->dpy, client, &result);
        }

        title = QString(result);
        XFree(result);

        updateIcon();

        //Redessiner le client
        repaint(0, 0, width(), height());
}

void Client::updateProperty(Atom atom)
{
        if ((atom == app->atoms->get("_NET_WM_NAME")) || (atom == XA_WM_NAME) || (atom == XA_WM_ICON_NAME))
        {
                getTitle();
                updateTransp();
        }
        else if (atom == XA_WM_HINTS || atom == app->atoms->get("WM_HINTS"))
        {
                updateIcon();
        }
        else if (atom == XA_WM_NORMAL_HINTS)
        {
                //Changement de la taille
                XSizeHints hints;
                long       mreturn;

                if (!XGetWMNormalHints(app->dpy, client, &hints, &mreturn))
                {
                        return;
                }

                if ((mreturn & PMinSize) != 0)
                {
                        min_width  = hints.min_width;
                        min_height = hints.min_height;
                }
                if ((mreturn & PMaxSize) != 0)
                {
                        max_width  = hints.max_width;
                        max_height = hints.max_height;
                }
        }
}

void Client::updateIcon()
{
        XWMHints *xwmhints;

        if ((xwmhints = XGetWMHints(app->dpy, client)) != NULL)
        {
                if ((xwmhints->flags & IconPixmapHint) != 0)
                {
                        //Mettre à jour l'icône
                        //lblIcon->setPixmap(getServerIcon(xwmhints->icon_pixmap, (xwmhints->flags & IconMaskHint)?(xwmhints->icon_mask):(None)));
                        QIcon icon = cwin->windowIcon();
                        lblIcon->setPixmap(icon.pixmap(lblIcon->width(), lblIcon->height()));
                }
                if ((xwmhints->flags & InputHint) != 0)
                {
                        //La fenêtre veut le focus, le lui donner
                        XSetInputFocus(app->dpy, client, RevertToParent, CurrentTime);
                        XRaiseWindow(app->dpy, client);
                        app->restackWindows();
                }
                if ((xwmhints->flags & StateHint) != 0)
                {
                        if (xwmhints->initial_state == NormalState)
                        {
                                //La fenêtre veut s'afficher
                                XMapWindow(app->dpy, client);
                                show();
                        }
                        else if (xwmhints->initial_state == IconicState)
                        {
                                //La fenêtre veut ne plus s'afficher
                                XUnmapWindow(app->dpy, client);
                                hide();
                        }
                }
        }
}
