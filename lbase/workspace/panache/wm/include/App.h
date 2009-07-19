/*
 * App.h
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

#ifndef __APP_H__
#define __APP_H__

#include <QList>
#include <QtDebug>

#include <LApplication.h>
#include <LConfig.h>
#include <XWindow.h>
#include <XAtoms.h>

class Client;   //Juste besoin de sa déclaration ici. Ca évité les includes-infinite-loops

class App : public LApplication
{
        Q_OBJECT
public:
        App(int& argc, char *argv[]);

        Client *findClient(Window win);

        void setActiveWindow(Window win);
        void raiseClient(Client *cli);

        void restackWindows();
        void topWindow(Window win);
        void lowWindow(Window win);

protected:
        virtual bool x11EventFilter(XEvent *event);

private:
        void scan();
        void manage(Window win);
        void updateClientList();

public:
        Display         *dpy;
        Window          rootWindow;
        
        QList<Client *> clients;
        
        XAtoms          *atoms;
        LConfig         *cfg;
        
        Client          *topClient;             //Client actuellement en haut de la pile (évite le clignottement quand on double-clique dessus par exemple)
        
        QList<Window>   topWindows;             //Fenêtres qui doivent rester au dessus
        QList<Window>   lowWindows;             //Fenêtres qui doivent rester en dessous
};

#endif
