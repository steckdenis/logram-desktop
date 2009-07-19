/*
 * Client.h
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

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QBitmap>
#include <QLabel>
#include <QPixmap>

#include <App.h>

#include <LImageButton.h>

class Client : public QWidget
{
        Q_OBJECT
public:
        Client(App *mapp);

        bool init(Window cli);

        void updateProperty(Atom atom);
        void updateSize(int x, int y, int width, int height);
        void updateIcon();

protected:
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
        virtual void paintEvent(QPaintEvent *event);

public slots:
        void quitClicked(bool checked);
        void maxClicked(bool checked);
        void minClicked(bool checked);

private:
        int  min_width, min_height, max_width, max_height;
        bool dialog;                    //True si doit être décoré comme une boîte de dialogue
        bool maximized;
        int  d_mousePressed;
        int  d_x, d_y, m_x, m_y, r_x, r_y;
        int  ax, ay, aw, ah;                            //Anciennes positions et tailles de la fenêtre, avant maximisation
        int  minit;

        QBitmap transp;

        QLabel       *lblIcon;
        LImageButton *btnmaximum, *btnminimum, *btnclose;

        int     border_width, titlebar_height;
        QString theme, fontname, textcolor;
        QPixmap topleft, topright, bottomleft, bottomright, mleft, mright, mbottom, mtitle, titlebar, bar;
        int     fontsize, cornersize;

        void getTitle();
        void updateTransp();
        QPixmap getServerIcon(Pixmap icon, Pixmap mask);
        void addControls();
        void calcPos();

public:
        Window  client;
        Window  d_parent;
        App     *app;
        XWindow *cwin;
        QString title;
        bool    decorated;
};

#endif
