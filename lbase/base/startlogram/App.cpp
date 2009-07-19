/*
 * App.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#include <App.h>
#include <Screen.h>

App::App(int argc, char **argv) : LApplication(argc, argv)
{
        //Créer la fenêtre en plein écran

        scr = new SScreen(desktop()->width(), desktop()->height());

        connect(scr, SIGNAL(quit()), this, SLOT(quit()));
}

bool App::x11EventFilter(XEvent *event)
{
        return scr->appEvent(event);
}
