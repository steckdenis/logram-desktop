/*
 * XAtoms.cpp
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

#include <XAtoms.h>

XAtoms::XAtoms(Display *mdpy)
{
        dpy = mdpy;
}

Atom XAtoms::get(QString name)
{
        //Met en cache la correspondance nom->Atom pour éviter du traffic réseau et des lenteurs
        if (knownAtoms.contains(name))
        {
                return knownAtoms[name];
        }
        else
        {
                Atom atom;
                atom = XInternAtom(dpy, name.toLatin1(), false);
                knownAtoms.insert(name, atom);
                return atom;
        }
}
