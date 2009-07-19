/*
 * Screen.cpp
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

#include <QX11Info>
#include <QStringList>
#include <QProcess>
#include <QPainter>

#include <QtDebug>

#include <Screen.h>

extern "C" {
#include <X11/Xlib.h>
}

SScreen::SScreen(int width, int height) : QWidget(0)
{
        procstarted = 0;

        // Initialiser le widget
        move(0, 0);
        resize(width, height);

        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        // Charger la configuration
        QString theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();

        background = QPixmap(theme + LConfig::logramValue("SplashScreen/BackgroundImage", "splash_bg.png", "Theme").toString()).scaled(width, height);
        box        = QPixmap(theme + LConfig::logramValue("SplashScreen/BoxImage", "splash_box.png", "Theme").toString());
        pgsbg      = QPixmap(theme + LConfig::logramValue("SplashScreen/ProgressbarBackground", "splash_pgsbg.png", "Theme").toString());
        pgsfg      = QPixmap(theme + LConfig::logramValue("SplashScreen/ProgressbarForeground", "splash_pgsfg.png", "Theme").toString());

        textColor = QColor(LConfig::logramValue("SplashScreen/TextColor", "#FFFFFF", "Theme").toString());
        fontSize  = LConfig::logramValue("SplashScreen/FontSize", 12, "Theme").toInt();

        pgsx = LConfig::logramValue("SplashScreen/ProgressbarX", 59, "Theme").toInt();
        pgsy = LConfig::logramValue("SplashScreen/ProgressbarY", 356, "Theme").toInt();
        tx   = LConfig::logramValue("SplashScreen/TextX", 60, "Theme").toInt();
        ty   = LConfig::logramValue("SplashScreen/TextY", 340, "Theme").toInt();

        // Afficher
        show();

        // Démarrer les processus
        cfg = new LConfig("RunSession", this);
        QStringList procs = cfg->childGroups();
        numprocs = procs.count();

        foreach(QString proc, procs)
        {
                cfg->beginGroup(proc);

                qDebug() << proc;

                if (cfg->value("Session", false).toBool())
                {
                        //Connecter des signaux
                        connect(&sessionProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(quit()));

                        sessionProc.start(cfg->value("Exec").toString());
                }
                else
                {
                        QProcess::startDetached(cfg->value("Exec").toString());
                }
                
                //Si le processus ne veut pas être surveillé, ne pas l'attendre
                if (cfg->value("DontWatch", false).toBool())
                {
                        numprocs--;
                }

                cfg->endGroup();
        }
}

bool SScreen::appEvent(XEvent *event)
{
        XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), SubstructureNotifyMask | StructureNotifyMask);

        if (event->type == ClientMessage)
        {
                XClientMessageEvent *cv;

                cv = &event->xclient;

                if (cv->message_type == XInternAtom(QX11Info::display(), "_LOGRAM_SPLASH_PROGRESS", False))
                {
                        //Un processus a fini de démarrer
                        procstarted++;

                        if (procstarted == numprocs)
                        {
                                //Tout le monde a démarré
                                hide();

                                qDebug() << "On ferme !";
                        }

                        //Trouver le nom du processus démarré
                        QStringList procs = cfg->childGroups();

                        QString id(event->xclient.data.b);

                        foreach(QString proc, procs)
                        {
                                cfg->beginGroup(proc);

                                if (cfg->value("Identifier").toString() == id)
                                {
                                        started = cfg->value("DisplayName", id).toString();

                                        int x = (width() - box.width()) / 2;
                                        int y = (height() - box.height()) / 2;

                                        repaint(x, y, x + box.width(), y + box.height());

                                        break;
                                }

                                cfg->endGroup();
                        }

                        return true;
                }
        }

        return false;
}

void SScreen::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        //Dessiner l'arrière-plan
        painter.drawPixmap(0, 0, background);

        //Dessiner la boîte de progression aux bonnes positions
        int x = (width() - box.width()) / 2;
        int y = (height() - box.height()) / 2;

        painter.drawPixmap(x, y, box);

        //Dessiner l'arrière-plan de la barre de progression
        painter.drawPixmap(x + pgsx, y + pgsy, pgsbg);

        //Dessiner le premier-plan de la barre de progression à la bonne taille
        int pgssize = pgsfg.width() * procstarted / numprocs;
        painter.drawPixmap(x + pgsx, y + pgsy, pgsfg, 0, 0, pgssize, pgsfg.height());

        //Dessiner le texte
        QFont fnt("DejaVu Sans");
        fnt.setPixelSize(fontSize);

        painter.setFont(fnt);
        painter.setPen(QPen(textColor));

        painter.drawText(x + tx, y + ty, pgsfg.width(), fontSize, Qt::AlignVCenter | Qt::AlignLeft, (started.isNull() ? QString() : started + tr(" démarré")));

        (void)event;
}
