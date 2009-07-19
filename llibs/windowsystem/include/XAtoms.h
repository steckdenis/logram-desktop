/*
 * XAtoms.h
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

#ifndef __XATOMS_H__
#define __XATOMS_H__

#include <QString>
#include <QMap>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

class XAtoms
{
public:
        XAtoms(Display *mdpy);

        Atom get(QString name);

private:
        QMap<QString, Atom> knownAtoms;
        Display             *dpy;
};

#endif
