/*
 * XWindow.cpp
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

#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QRgb>

#include <XWindow.h>

XWindow::XWindow(Window mwin, Display *mdpy)
{
        win = mwin;
        dpy = mdpy;
}

int XWindow::getProperty(Atom property,
                         void **prop_return,
                         int  length,
                         unsigned long *items_returned,
                         Atom req_type,
                         int  offset,
                         Atom *rs_type,
                         int  *rs_format,
                         unsigned long *bytes_after)
{
        Atom mrs_type;
        int  mrs_format;
        unsigned long int mbytes_after, mitems_rs;

        return XGetWindowProperty(
                       dpy,
                       win,
                       property,
                       offset,
                       length,
                       false,
                       req_type,
                       (rs_type == 0 ? &mrs_type : rs_type),
                       (rs_format == 0 ? &mrs_format : rs_format),
                       (items_returned == 0 ? &mitems_rs : (unsigned long int *)items_returned),
                       (bytes_after == 0 ? &mbytes_after : (unsigned long int *)bytes_after),
                       (unsigned char **)prop_return);
}

void XWindow::changeProperty(Atom property,
                             Atom type,
                             int  format,
                             void *data,
                             int  elements,
                             int  mode)
{
        XChangeProperty(dpy, win, property, type, format, mode, (unsigned char *)data, elements);
}

QIcon XWindow::windowIcon()
{
        //Lire la propriété _NET_WM_ICON
        QRgb *data = 0;
        unsigned long items;

        if (getProperty(XInternAtom(dpy, "_NET_WM_ICON", false), (void **)&data, 2147483647, &items) == Success)
        {
                if (items >= 3)
                {
                        //Minimum 3 : width, height et un pixel (image de 1x1)

                        //Lire les données
                        unsigned long i = 0;
                        int    width, height;
                        QIcon  rs;
                        QImage img;

                        while (i < items)
                        {
                                width = data[i];
                                i++;
                                i++;
                                height = data[i];
                                i++;
                                i++;
                                
                                if (width == 0 || height == 0) goto notok;

                                // Créer l'image
                                img = QImage(width, height, QImage::Format_ARGB32);

                                // Placer les données
                                for (int y = 0; y < height; y++)
                                {
                                        for (int x = 0; x < width; x++)
                                        {
                                                img.setPixel(x, y, data[i]);
                                                i++;
                                                i++;
                                        }
                                }

                                //Ajouter l'image à l'icône
                                rs.addPixmap(QPixmap::fromImage(img));
                        }

                        //Retourner l'icône
                        XFree(data);

                        return rs;
                }
        }
        
notok:

        //L'icône n'est pas dans _NET_WM_ICON, utiliser la méthode traditionnelle
        XWMHints *xwmhints;

        if ((xwmhints = XGetWMHints(dpy, win)) != NULL)
        {
                if ((xwmhints->flags & IconPixmapHint) != 0)
                {
                        Pixmap icon, mask;

                        icon = xwmhints->icon_pixmap;
                        mask = (xwmhints->flags & IconMaskHint) ? (xwmhints->icon_mask) : (None);

#if QT_VERSION >= 0x040500
                        //À partir de Qt 4.5, la fonction est disponible directement
                        QPixmap pix = QPixmap::fromX11Pixmap(icon);

                        if (mask != None)
                        {
                                QBitmap bmap = QPixmap::fromX11Pixmap(mask);
                                pix.setMask(bmap);
                        }

                        if (pix.isNull())
                        {
                                return QIcon();
                        }
                        else
                        {
                                return QIcon(pix);
                        }
#else
                        int    ix, iy;
                        uint   iw, ih, bwidth, depth;
                        Window w;
                        GC     gc;

                        if (!XGetGeometry(dpy, icon, &w, &ix, &iy, &iw, &ih, &bwidth, &depth))
                        {
                                return QIcon();
                        }

                        QPixmap pix(iw, ih);
                        pix.detach();
                        gc = XCreateGC(dpy, icon, 0, 0);
                        XCopyArea(dpy, icon, pix.handle(), gc, 0, 0, iw, ih, 0, 0);
                        XFreeGC(dpy, gc);

                        if (mask != None)
                        {
                                gc = XCreateGC(dpy, mask, 0, 0);
                                QBitmap bmap(iw, ih);
                                bmap.detach();
                                XCopyArea(dpy, mask, bmap.handle(), gc, 0, 0, iw, ih, 0, 0);
                                pix.setMask(bmap);
                                XFreeGC(dpy, gc);
                        }
                        if (pix.isNull())
                        {
                                return QIcon();
                        }
                        else
                        {
                                return QIcon(pix);
                        }
#endif
                }
        }

        // On n'a rien trouvé
        return QIcon();
}
