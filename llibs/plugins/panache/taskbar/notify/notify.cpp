/*
 * datetime.cpp
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
#include <QX11EmbedContainer>
#include <QTimer>
#include <QPainter>

#include <QtDebug>

#include <iostream>

#include <notify.h>

#include <LConfig.h>
#include <XAtoms.h>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xatom.h>
}

#define SYSTEM_TRAY_REQUEST_DOCK      0
#define SYSTEM_TRAY_BEGIN_MESSAGE     1
#define SYSTEM_TRAY_CANCEL_MESSAGE    2

using namespace std;

/*
        Le plugin
 */

QWidget *NotifyPlugin::init()
{
        //Créer le widget
        widget = new NotifyWidget(this);

        return widget;
}

void NotifyPlugin::appEvent(XEvent *event)
{
        widget->appEvent(event);
}

/*
        Le widget
 */

NotifyWidget::NotifyWidget(NotifyPlugin *mplugin)
{
        plugin = mplugin;
        atoms  = new XAtoms(QX11Info::display());

        // Charger l'atôme du System Tray
        atmST = atoms->get("_NET_SYSTEM_TRAY_OPCODE");

        // Flags
        setAttribute(Qt::WA_NoSystemBackground, true);

        // Redimensionnement
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

        // Theme
        QString theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
        iconSize   = LConfig::logramValue("NotifyArea/IconSize", 20, "Theme").toInt();
        iconMargin = LConfig::logramValue("NotifyArea/IconMargin", 2, "Theme").toInt();
        brhColor   = QBrush(QColor(LConfig::logramValue("NotifyArea/BackgroundColor", "#FFFFFF", "Theme").toString()));
        bgPix      = QPixmap(theme + LConfig::logramValue("NotifyArea/BackgroundImage", "notify_bg.png", "Theme").toString());
        mwidth     = iconMargin;

        // Initier la sélection. Utilisation d'un QTimer pour éviter que Qt nous passe dessus
        QTimer::singleShot(0, this, SLOT(initSelection()));
}

void NotifyWidget::clientClosed()
{
        QX11EmbedContainer *container = static_cast<QX11EmbedContainer *>(sender());

        //Supprimer le conteneur de la liste
        clients.removeOne(container);
        delete container;

        //Mettre à jour la taille
        mwidth -= iconSize + (2 * iconMargin);
        updatePos();
        updateGeometry();
}

void NotifyWidget::initSelection()
{
        // Obtenir la sélection pour le TrayManager
        int     screen       = QX11Info::appScreen();
        QString net_sys_tray = QString::fromLatin1("_NET_SYSTEM_TRAY_S%1").arg(screen); //Cette ligne vient de Qt 4.5

        XSetSelectionOwner(QX11Info::display(), atoms->get(net_sys_tray), winId(), CurrentTime);
        XFlush(QX11Info::display());
        if (XGetSelectionOwner(QX11Info::display(), atoms->get(net_sys_tray)) != winId())
        {
                qFatal("Cannot install System Tray. Verify that there are not an other System Tray installed");
                return;         //Erreur
        }

        qDebug() << "Selection :" << XGetSelectionOwner(QX11Info::display(), atoms->get(net_sys_tray));

        // Envoyer l'événement comme quoi on est le manager
        XEvent ev;
        ev.xclient.type         = ClientMessage;
        ev.xclient.message_type = XA_RESOURCE_MANAGER;
        ev.xclient.format       = 32;
        ev.xclient.window       = winId();
        ev.xclient.data.l[0]    = CurrentTime;
        ev.xclient.data.l[1]    = atoms->get(net_sys_tray);
        ev.xclient.data.l[2]    = winId();
        ev.xclient.data.l[3]    = 0;
        ev.xclient.data.l[4]    = 0;
        XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), false, StructureNotifyMask, &ev);
}

QSize NotifyWidget::sizeHint() const
{
        return QSize(mwidth, parentWidget()->height());
}

void NotifyWidget::updatePos()
{
        if (clients.count() == 0)
        {
                return;
        }

        int index = 0;

        int space = width() - (2 * iconMargin);
        int spe   = space / clients.count();

        foreach(QX11EmbedContainer * cli, clients)
        {
                int sp = space * index / clients.count();

                cli->move(sp * index + ((spe - iconSize) / 2), (height() - iconSize) / 2);

                index++;
        }
}

void NotifyWidget::resizeEvent(QResizeEvent *)
{
        updatePos();
}

void NotifyWidget::paintEvent(QPaintEvent *)
{
        //Dessiner l'arrière-plan en fonction de l'image d'arrière-plan
        if (width() >= 24)
        {
                QPainter painter(this);

                int y = (height() - bgPix.height()) / 2;

                painter.drawPixmap(0, y, bgPix, 0, 0, 8, bgPix.height());                 //Bord gauche
                painter.drawPixmap(width() - 8, y, bgPix, 16, 0, 8, bgPix.height());      //Bord droit

                painter.drawPixmap(8, y, width() - 16, bgPix.height(), bgPix, 8, 0, 8, bgPix.height());
        }
}

void NotifyWidget::appEvent(XEvent *event)
{
        if (event->type == SelectionClear)
        {
                //On a perdu la sélection
                qWarning("Panache-desktop has lost the selection of the System Tray."
                         "Another System Tray will display the System Tray's Icons, not Panache");
        }

        /*else if (event->type == SelectionRequest)
           {

           }*/
        else if (event->type == ClientMessage)
        {
                if (event->xclient.message_type == atmST)
                {
                        int opcode = event->xclient.data.l[1];          //l[0] est le timestamp, on n'en a pas besoin ici

                        switch (opcode)
                        {
                                case SYSTEM_TRAY_REQUEST_DOCK:
                                                                                        //Une application veut une place dans le doc
                                        Window win = event->xclient.data.l[2];          //Fenêtre cliente

                                        QX11EmbedContainer *cli = new QX11EmbedContainer(this);
                                        cli->resize(iconSize, iconSize);

                                        QPalette p = cli->palette();
                                        p.setBrush(QPalette::Window, brhColor);
                                        cli->setPalette(p);

                                        connect(cli, SIGNAL(clientClosed()), this, SLOT(clientClosed()));

                                        //Ajouter le conteuneur
                                        clients.append(cli);
                                        mwidth += iconSize + (2 * iconMargin);
                                        updateGeometry();

                                        //Replacer les conteneurs
                                        updatePos();

                                        //Insérer le client
                                        cli->embedClient(win);
                                        cli->show();

                                        break;
                                        //TODO: Gérer les infobulles
                        }
                }
        }
}

Q_EXPORT_PLUGIN2(lnotify, NotifyPlugin)
