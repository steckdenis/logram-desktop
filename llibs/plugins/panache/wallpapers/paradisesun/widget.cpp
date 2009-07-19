/*
 * widget.cpp
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

#include "widget.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QTime>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>

#include <QtDebug>

SWidget::SWidget(QWidget *parent) : QWidget(parent)
{
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();

        setAttribute(Qt::WA_X11NetWmWindowTypeDesktop, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        resize(dw, dh);
        move(0, 0);

        settings = new QSettings("Logram", "ParadiseSunPlugin", this);

        theme = settings->value("Theme/ImagePath", "/usr/share/logram/themes/default/wallpapers/").toString();

        //Lire l'image et la mettre en cache
        QString bgstr = theme + settings->value("Theme/BackgroundImage", "paradisesun_bg.png").toString();
        QString fgstr = theme + settings->value("Theme/ForegroundImage", "paradisesun_fg.png").toString();

        // Si les fichiers existent dans le cache, les prendre
        QString hashedbg = QCryptographicHash::hash(QString(bgstr + QString::number(dw) + QString::number(dh)).toUtf8(), QCryptographicHash::Sha1).toHex();
        QString hashedfg = QCryptographicHash::hash(QString(fgstr + QString::number(dw) + QString::number(dh)).toUtf8(), QCryptographicHash::Sha1).toHex();

        // Créer le dossier s'il n'existe pas
        QDir dir(QDir::homePath());
        dir.mkpath(".thumbs");

        // Vérifier que ces fichiers existent
        bgstr = QDir::homePath() + "/.thumbs/" + hashedbg + ".png";
        fgstr = QDir::homePath() + "/.thumbs/" + hashedfg + ".png";

        if (QFile::exists(bgstr))
        {
                mbackground = QPixmap(bgstr);
        }
        else
        {
                //Mettre en cache
                mbackground = QPixmap(theme + settings->value("Theme/BackgroundImage", "paradisesun_bg.png").toString()).scaled(dw, dh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

                mbackground.save(bgstr);
        }

        if (QFile::exists(fgstr))
        {
                mforeground = QPixmap(fgstr);
        }
        else
        {	
                mforeground = QPixmap(theme + settings->value("Theme/ForegroundImage", "paradisesun_fg.png").toString()).scaled(dw, dh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

                mforeground.save(fgstr);
        } 

        msun = QPixmap(theme + settings->value("Theme/SunImage", "paradisesun_sun.png").toString());

        minH  = settings->value("Theme/MinHeight", 0.7).toDouble();
        maxH  = settings->value("Theme/MaxHeight", 0.0).toDouble();
        sLeft = settings->value("Theme/SunLeftPos", 0.6).toDouble();

        //Créer les timers
        timerVerif = new QTimer(this);

        connect(timerVerif, SIGNAL(timeout()), this, SLOT(verifSunPos()));

        timerVerif->start(60000);       //1 min

        //Vérifier une première fois la position du soleil
        verifSunPos();
}

void SWidget::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        //S'assurer qu'un bout de soleil ne dépasse pas
        QRect mrect;
        bool  drawSun = false;

        if (event->rect().intersects(sRect))
        {
                mrect   = event->rect().united(sRect);  //S'assurer qu'on dessine bien tout le soleil
                drawSun = true;
        }
        else
        {
                mrect = event->rect();
        }

        //Dessiner l'arrière-plan
        painter.drawPixmap(mrect, mbackground, mrect);

        //Dessiner le soleil
        if (drawSun)
        {
                painter.drawPixmap(sRect, msun);
        }

        //Dessiner le premier plan
        painter.drawPixmap(mrect, mforeground, mrect);
}

void SWidget::verifSunPos()
{
        QTime mtime = QTime::currentTime();

        //Nombre de minutes depuis minuit
        int numMins = (mtime.hour() * 60) + mtime.minute();

        //La hauteur du solei est manH-(|midi-heure|)
        int midiMins = 12 * 60;
        int diffMins = midiMins - numMins;

        if (diffMins < 0)
        {
                diffMins = 0 - diffMins;
        }

        //Calculer la hauteur
        double battement = minH - maxH;

        cHeight = maxH + (battement * diffMins / midiMins);

        //Mettre à jour la position du soleil
        updateSunPos();
}

void SWidget::updateSunPos()
{
        //Construire le rectangle du soleil
        QRect mRect(width() * sLeft, height() * cHeight, msun.width(), msun.height());

        //Calculer un rectangle qui comprend les deux, pour le redessin
        QRect deux = mRect.united(sRect);

        //Redessiner le soleil
        sRect = mRect;
        repaint(deux);
}
