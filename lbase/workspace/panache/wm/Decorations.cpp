/*
 * Decorations.cpp
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

#include <QPainter>
#include <QFontMetrics>
#include <QFont>

void Client::updateTransp()
{
        transp = QBitmap(width(), height());

        //Dessiner le masque des 4 angles
        transp.fill(Qt::color1);
        QPainter painter(&transp);
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(QBrush(Qt::color0));

        painter.drawPixmap(0, height() - bottomleft.height(), bottomleft.mask());
        painter.drawPixmap(width() - bottomright.width(), height() - bottomright.height(), bottomright.mask());
        painter.drawPixmap(0, 0, topleft.mask());
        painter.drawPixmap(width() - topright.width(), 0, topright.mask());

        setMask(transp);
}

void Client::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        QPixmap  corner;

        //Images des bords
        painter.drawPixmap(0, cornersize, border_width, height() - (2 * cornersize), mleft);
        painter.drawPixmap(width() - border_width, cornersize, border_width, height() - (2 * cornersize), mright);
        painter.drawPixmap(cornersize, height() - border_width, width() - (2 * cornersize), border_width, mbottom);

        //Image des angles
        painter.drawPixmap(0, height() - bottomleft.height(), bottomleft);
        painter.drawPixmap(width() - bottomright.width(), height() - bottomright.height(), bottomright);
        painter.drawPixmap(0, 0, topleft);
        painter.drawPixmap(0, 0, topleftborder); // bord noir de la barre de titre
        painter.drawPixmap(width() - topright.width(), 0, topright);
        painter.drawPixmap(width() - 1, 0, toprightborder); // deuxième bord noir

        //Barre de titre
        QFont        mfont(fontname, fontsize);
        QFontMetrics fm(mfont);
        int          fwidth = fm.width(title);
        fwidth += 10 + 10;              //Espace entre l'icône et le titre, qui doit être pris en compte

        int tbwidth = titlebar.width();

        if (fwidth > width() - (48 + (5 * border_width) + 10 + 16 + tbwidth))
        {
                fwidth = width() - (48 + (5 * border_width) + 10 + 16 + tbwidth);
        }

        painter.drawPixmap(cornersize, 0, fwidth, titlebar_height, mtitle);
        painter.drawPixmap(cornersize + fwidth, 0, titlebar);
        painter.drawPixmap(cornersize + fwidth + tbwidth, 0, width() - (2 * cornersize) - fwidth - tbwidth, titlebar_height, bar);

        //Dessiner le titre
        painter.setPen(QColor(textcolor));
        painter.setFont(mfont);
        painter.drawText(cornersize, 0, fwidth, titlebar_height, Qt::AlignCenter | Qt::AlignVCenter, title);

        //QWidget::paintEvent(event);

        (void)event;
}

void Client::addControls()
{
        //Emplacement de l'icône
        lblIcon = new QLabel(this);
        lblIcon->setGeometry(
                border_width,
                (titlebar_height / 2) - 8,
                16,
                16);

        lblIcon->show();

        //Boutons
        btnclose   = new LImageButton(QPixmap(theme + "windowclose.png"), QPixmap(theme + "windowclose_active.png"), this);
        btnmaximum = new LImageButton(QPixmap(theme + "windowmaxi.png"), QPixmap(theme + "windowmaxi_active.png"), this);
        btnminimum = new LImageButton(QPixmap(theme + "windowmini.png"), QPixmap(theme + "windowmini_active.png"), this);

        //Position
        calcPos();

        //Le bouton de maximisation doit être togglable
        btnmaximum->setCheckable(true);

        //Relier les contrôles
        connect(btnclose, SIGNAL(clicked(bool)), this, SLOT(quitClicked(bool)));
        connect(btnmaximum, SIGNAL(clicked(bool)), this, SLOT(maxClicked(bool)));
        connect(btnminimum, SIGNAL(clicked(bool)), this, SLOT(minClicked(bool)));

        //Afficher les boutons
        btnmaximum->show();
        btnminimum->show();
        btnclose->show();
}

void Client::calcPos()
{
        btnclose->setGeometry(
                width() - border_width - 16,
                (titlebar_height / 2) - 8,
                16,
                16);

        btnmaximum->setGeometry(
                width() - (2 * border_width) - 32,
                (titlebar_height / 2) - 8,
                16,
                16);

        btnminimum->setGeometry(
                width() - (3 * border_width) - 48,
                (titlebar_height / 2) - 8,
                16,
                16);
}
