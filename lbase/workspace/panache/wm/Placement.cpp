/*
 * Placement.cpp
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

#include <QDesktopWidget>

#include <Client.h>

void Client::mouseMoveEvent(QMouseEvent *event)
{
        if (maximized)
        {
                return;
        }

        if (d_mousePressed)
        {
                int diffx = (event->globalX() - d_x);
                int diffy = (event->globalY() - d_y);

                int mx = (diffx * m_x);
                int my = (diffy * m_y);
                int rx = diffx * r_x;
                int ry = diffy * r_y;

                if ((rx + width()) < (min_width + (2 * border_width)))
                {
                        rx = min_width - width() + (2 * border_width);
                        mx = 0;
                }
                if ((ry + height()) < (min_height + titlebar_height + border_width))
                {
                        ry = min_height - height() + titlebar_height + border_width;
                        my = 0;
                }
                if ((max_height != 0) && (max_width != 0))
                {
                        //Gérer également la taille maximum
                        if ((rx + width()) > (max_width + (2 * border_width)))
                        {
                                rx = min_width - width() + (2 * border_width);
                                mx = 0;
                        }
                        if ((ry + height()) > (max_height + titlebar_height + border_width))
                        {
                                ry = min_height - height() + titlebar_height + border_width;
                                my = 0;
                        }
                }

                this->setGeometry(mx + x(), my + y(), rx + width(), ry + height());

                d_x = event->globalX();
                d_y = event->globalY();

                //this->move(x()-1, y()-1);
        }

        //Afficher le bon curseur
        int x          = event->x();
        int y          = event->y();
        int border     = border_width;
        int twoborders = border * 2;

        if ((y <= twoborders) && (x <= twoborders))
        {
                //Poignée en haut à gauche
                setCursor(Qt::SizeFDiagCursor);
        }
        else if (y <= twoborders && x >= (width() - twoborders))
        {
                //Poignée en haut à droite
                setCursor(Qt::SizeBDiagCursor);
        }
        else if (y >= (height() - twoborders) && x <= twoborders)
        {
                //Poignée en bas à gauche
                setCursor(Qt::SizeBDiagCursor);
        }
        else if (y >= (height() - twoborders) && x >= (width() - twoborders))
        {
                //Poignée en bas à droite
                setCursor(Qt::SizeFDiagCursor);
        }
        else if (y <= border)
        {
                //Poignée du dessus
                setCursor(Qt::SizeVerCursor);
        }
        else if (y >= (height() - border))
        {
                //Poignée du dessous
                setCursor(Qt::SizeVerCursor);
        }
        else if (x <= border)
        {
                //Poignée à gauche
                setCursor(Qt::SizeHorCursor);
        }
        else if (x >= (width() - border))
        {
                //Poignée à droite
                setCursor(Qt::SizeHorCursor);
        }
        else
        {
                //Barre de titre
                setCursor(Qt::ArrowCursor);
        }
}

void Client::mousePressEvent(QMouseEvent *event)
{
        //Savoir où on a cliqué sur la fenêtre
        d_x = event->globalX();
        d_y = event->globalY();
        int x          = event->x();
        int y          = event->y();
        int border     = border_width;
        int twoborders = border * 2;

        if ((y <= twoborders) && (x <= twoborders))
        {
                //Poignée en haut à gauche
                m_x = 1;
                m_y = 1;
                r_x = -1;
                r_y = -1;
        }
        else if (y <= twoborders && x >= (width() - twoborders))
        {
                //Poignée en haut à droite
                m_x = 0;
                m_y = 1;
                r_x = 1;
                r_y = -1;
        }
        else if (y >= (height() - twoborders) && x <= twoborders)
        {
                //Poignée en bas à gauche
                m_x = 1;
                m_y = 0;
                r_x = -1;
                r_y = 1;
        }
        else if (y >= (height() - twoborders) && x >= (width() - twoborders))
        {
                //Poignée en bas à droite
                m_x = 0;
                m_y = 0;
                r_x = 1;
                r_y = 1;
        }
        else if (y <= border)
        {
                //Poignée du dessus
                m_x = 0;
                m_y = 1;
                r_x = 0;
                r_y = -1;
        }
        else if (y >= (height() - border))
        {
                //Poignée du dessous
                m_x = 0;
                m_y = 0;
                r_x = 0;
                r_y = 1;
        }
        else if (x <= border)
        {
                //Poignée à gauche
                m_x = 1;
                m_y = 0;
                r_x = -1;
                r_y = 0;
        }
        else if (x >= (width() - border))
        {
                //Poignée à droite
                m_x = 0;
                m_y = 0;
                r_x = 1;
                r_y = 0;
        }
        else
        {
                //Barre de titre
                m_x = 1;
                m_y = 1;
                r_x = 0;
                r_y = 0;
        }

        d_mousePressed = true;

        //Donner le focus à la fenêtre
        app->raiseClient(this);
}

void Client::mouseReleaseEvent(QMouseEvent *event)
{
        d_mousePressed = false;

        (void)event;
}

void Client::resizeEvent(QResizeEvent *event)
{
        if (minit != 7936898)
        {
                return;
        }

        //Redimensionner la fenêtre X
        XResizeWindow(app->dpy, client, width() - (border_width * 2), height() - border_width - titlebar_height);

        updateTransp();
        calcPos();

        (void)event;
}

void Client::updateSize(int x, int y, int width, int height)
{
        resize(width + (2 * border_width), height + border_width + titlebar_height);
        move(x - border_width, y - titlebar_height);

        //Remettre le client à la bonne position
        XMoveWindow(app->dpy, client, border_width, titlebar_height);
}

void Client::maxClicked(bool checked)
{
        if (checked)
        {
                //Sauvegarder l'ancienne position et taille
                ax = x();
                ay = y();
                aw = width();
                ah = height();

                //Redimensionner à la taille de la fenêtre Root
                setGeometry(0, 0, QApplication::desktop()->width(), QApplication::desktop()->height());
        }
        else
                setGeometry(ax, ay, aw, ah);
}

void Client::mouseDoubleClickEvent(QMouseEvent *event)
{
    //Vérfier qu'on clique bien sur la barre de titre
    if (event->y() <= titlebar_height)
    {
        //Changer l'était du bouton de maximisation
        btnmaximum->setChecked(!btnmaximum->isChecked());

        // Si le bouton était décoché (mode normal), il est maintenant coché (mode maximum).
        // L'inverse est bien sûr possible. Nous allons donc maintenant appeler la fonction
        // maxClicked(bool) pour réutiliser un code qui marche, facilement, et faire comme si
        // l'utilisateur avait cliqué sur le bouton
        maxClicked(btnmaximum->isChecked());
    }
}
