/*
 * DesktopPluginContainer.cpp
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

#include <QDir>
#include <QAction>

#include <DesktopPluginContainer.h>

#include <App.h>

DesktopPluginContainer::DesktopPluginContainer(QString title, QString icon, QString id, IPanacheDesktopPlugin *plugin, App *mapp) : QWidget()
{
        app     = mapp;
        micon   = icon;
        tabName = id;
        mtitle  = title;
        rcorner = None;
        onDrag  = false;
        onMove  = false;
        setMouseTracking(true);

        tabInfos = new QSettings(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat, this);

        //Load configuration
        theme           = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
        border_size     = LConfig::logramValue("DesktopPlugin/BorderSize", 5, "Theme").toInt();
        titlebar_height = LConfig::logramValue("DesktopPlugin/TitlebarHeight", 20, "Theme").toInt();
        cornersize      = LConfig::logramValue("DesktopPlugin/CornerSize", 20, "Theme").toInt();
        fontsize        = LConfig::logramValue("DesktopPlugin/FontSize", 12, "Theme").toInt();
        mpix            = QPixmap(theme + LConfig::logramValue("DesktopPlugin/Pixmap", "desktopplugin_pixmap.png", "Theme").toString());
        bgColor         = QColor(LConfig::logramValue("DesktopPlugin/BackgroundColor", "#FFFFFF", "Theme").toString());
        bgColor.setAlpha(LConfig::logramValue("DesktopPlugin/BackgroundAlpha", 64, "Theme").toInt());
        textColor = QColor(LConfig::logramValue("DesktopPlugin/TextColor", "#000000", "Theme").toString());

        tabInfos->setValue(tabName + "/IsTab", false);
        tabInfos->sync();

        setAttribute(Qt::WA_NoSystemBackground, true);

        pluginWidget = new QWidget(this);
        pluginWidget->resize(300, 50);
        pluginWidget->move(border_size, titlebar_height);               //Espace pour la barre de titre
        pluginWidget->setCursor(Qt::ArrowCursor);
        nplugin = plugin;
        nplugin->linkParent(pluginWidget);

        //If you have stored a size, restore
        //NOTE: This must be after nplugin-> Initialize, which can resize the plugin against the wishes of the user
        if (tabInfos->contains(tabName + "/width"))
        {
                pluginWidget->resize(tabInfos->value(tabName + "/width").toInt(), tabInfos->value(tabName + "/height").toInt());
        }

        move(tabInfos->value(tabName + "/left", 50).toInt(), tabInfos->value(tabName + "/top", 50).toInt());

        //Adapt to plugin's size
        resize(pluginWidget->width() + (2 * border_size), pluginWidget->height() + border_size + titlebar_height);

        //Create actions
        QAction *actionTab, *actionClose;

        actionTab = new QAction(this);
        actionTab->setText(tr("Transformer en languette"));
        connect(actionTab, SIGNAL(triggered()), this, SIGNAL(toTab()));

        actionClose = new QAction(this);
        actionClose->setText(tr("Fermer le plugin"));
        connect(actionClose, SIGNAL(triggered()), this, SIGNAL(closed()));

        addAction(actionTab);
        addAction(actionClose);

        setContextMenuPolicy(Qt::ActionsContextMenu);

        //Show plugin
        pluginWidget->show();
}

IPanacheDesktopPlugin *DesktopPluginContainer::getPlugin()
{
        return nplugin;
}

void DesktopPluginContainer::mousePressEvent(QMouseEvent *event)
{
        pluginWidget->setFocus(Qt::MouseFocusReason);

        onDrag = true;
        a_x    = event->globalX();
        a_y    = event->globalY();

        if ((event->x() < width() - cornersize) && (event->x() > cornersize) && (event->y() < titlebar_height))
        {
                onMove = true;
        }

        QWidget::mousePressEvent(event);
}

void DesktopPluginContainer::mouseReleaseEvent(QMouseEvent *event)
{
        onDrag = false;
        onMove = false;

        QWidget::mouseReleaseEvent(event);
}

void DesktopPluginContainer::mouseMoveEvent(QMouseEvent *event)
{
        QWidget::mouseMoveEvent(event);

        if (!onDrag)
        {
                //Know what angle is the mouse
                ResizeCorner mcorner = rcorner;
                int          mx      = event->x();
                int          my      = event->y();

                if ((mx < 30) && (my < 30))
                {
                        //Angle top-left
                        rcorner = TopLeft;
                }
                else if (mx < 30 && my > height() - 30)
                {
                        //Angle bottom-left
                        rcorner = BottomLeft;
                }
                else if (mx > width() - 30 && my < 30)
                {
                        //Angle top-right
                        rcorner = TopRight;
                }
                else if (mx > width() - 30 && my > height() - 30)
                {
                        //Angle bottom-right
                        rcorner = BottomRight;
                }
                else
                {
                        //Nowhere
                        rcorner = None;
                }

                //If angle has changed, change cursor
                if (mcorner != rcorner)
                {
                        if (rcorner == TopLeft)
                        {
                                setCursor(Qt::SizeFDiagCursor);
                        }
                        if (rcorner == TopRight)
                        {
                                setCursor(Qt::SizeBDiagCursor);
                        }
                        if (rcorner == BottomLeft)
                        {
                                setCursor(Qt::SizeBDiagCursor);
                        }
                        if (rcorner == BottomRight)
                        {
                                setCursor(Qt::SizeFDiagCursor);
                        }
                        if (rcorner == None)
                        {
                                setCursor(Qt::ArrowCursor);
                        }
                }
        }
        else
        {
                //On mouse down, move/resize plugin
                int diffx = event->globalX() - a_x;
                int diffy = event->globalY() - a_y;

                if (onMove)
                {
                        //Move
                        move(x() + diffx, y() + diffy);
                }
                else if (rcorner == TopLeft)
                {
                        setGeometry(x() + diffx, y() + diffy, width() - diffx, height() - diffy);
                }
                else if (rcorner == TopRight)
                {
                        setGeometry(x(), y() + diffy, width() + diffx, height() - diffy);
                }
                else if (rcorner == BottomLeft)
                {
                        setGeometry(x() + diffx, y(), width() - diffx, height() + diffy);
                }
                else if (rcorner == BottomRight)
                {
                        resize(width() + diffx, height() + diffy);
                }

                a_x = event->globalX();
                a_y = event->globalY();
        }
}

void DesktopPluginContainer::resizeEvent(QResizeEvent *event)
{
        //Save tab size
        tabInfos->setValue(tabName + "/width", width());
        tabInfos->setValue(tabName + "/height", height());
        tabInfos->sync();

        pluginWidget->resize(width() - (2 * border_size), height() - border_size - titlebar_height);

        QWidget::resizeEvent(event);
}

void DesktopPluginContainer::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        //delete widget
        painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 0));

        //Draw angles
        painter.drawPixmap(0, 0, cornersize, cornersize, mpix, 0, 0, cornersize, cornersize);                                                                           //HG
        painter.drawPixmap(0, height() - cornersize, cornersize, cornersize, mpix, 0, (2 * cornersize), cornersize, cornersize);                                        //BG
        painter.drawPixmap(width() - cornersize, 0, cornersize, cornersize, mpix, (2 * cornersize), 0, cornersize, cornersize);                                         //HD
        painter.drawPixmap(width() - cornersize, height() - cornersize, cornersize, cornersize, mpix, (2 * cornersize), (2 * cornersize), cornersize, cornersize);      //BD

        //Draw edges
        painter.drawPixmap(cornersize, 0, width() - (2 * cornersize), cornersize, mpix, cornersize, 0, cornersize, cornersize);                                         //Haut
        painter.drawPixmap(cornersize, height() - cornersize, width() - (2 * cornersize), cornersize, mpix, cornersize, (2 * cornersize), cornersize, cornersize);      //Bas
        painter.drawPixmap(0, cornersize, cornersize, height() - (2 * cornersize), mpix, 0, cornersize, cornersize, cornersize);                                        //Gauche
        painter.drawPixmap(width() - cornersize, cornersize, cornersize, height() - (2 * cornersize), mpix, 2 * cornersize, cornersize, cornersize, cornersize);        //Droite

        //Fill the center
        painter.fillRect(cornersize, cornersize, width() - (2 * cornersize), height() - (2 * cornersize), bgColor);

        //Draw title
        painter.setFont(QFont("DejaVu Sans", fontsize));
        painter.setPen(QPen(textColor));
        painter.drawText(border_size, 0, width() - (2 * border_size), cornersize, Qt::AlignHCenter | Qt::AlignVCenter, mtitle);

        QWidget::paintEvent(event);
}
