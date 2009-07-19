/*
 * TaskBar.cpp
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

#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QDir>
#include <QStringList>
#include <QLayout>
#include <QPainter>
#include <QPluginLoader>
#include <QEvent>
#include <QLayout>

#include <QtDebug>

#include <LConfig.h>

#include <TaskBar.h>
#include <IPanacheTaskPlugin.h>

TaskBar::TaskBar(App *mapp) : QWidget(0)
{
        app = mapp;
        QString theme;
        layout = 0;

        //Don't be managed by PanacheWM
        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        
        //Don't show the taskbar if the user doesn't want that
        if (QCoreApplication::arguments().contains("notaskbar"))
        {
                return;
        }

        //Load configuration
        theme = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();

        position = (Position) LConfig::logramValue("TaskBar/Position", BottomRight, "Theme").toInt();
        brush    = QBrush(QPixmap(theme + LConfig::logramValue("TaskBar/BrushImage", "taskbar_bg.png", "Theme").toString()));
        tlm      = QPixmap(theme + LConfig::logramValue("TaskBar/TopLeftMask", "taskbar_tlm.png", "Theme").toString());
        trm      = QPixmap(theme + LConfig::logramValue("TaskBar/TopRightMask", "taskbar_trm.png", "Theme").toString());
        blm      = QPixmap(theme + LConfig::logramValue("TaskBar/BottomLeftMask", "taskbar_blm.png", "Theme").toString());
        brm      = QPixmap(theme + LConfig::logramValue("TaskBar/BottomRightMask", "taskbar_brm.png", "Theme").toString());

        dw = QApplication::desktop()->width();
        dh = QApplication::desktop()->height();

        //Update bar
        updateMask();

        //Resize widget
        resize(width(), LConfig::logramValue("TaskBar/Height", 30, "Theme").toInt());

        //Create layout
        layout = new QHBoxLayout(this);
        layout->setSizeConstraint(QLayout::SetMinimumSize);
        layout->setSpacing(LConfig::logramValue("TaskBar/ItemSpacing", 5, "Theme").toInt());
        setLayout(layout);

        //Load plugins (call updatePos())
        updatePlugins();

        show();
}

void TaskBar::updatePlugins()
{
        QSettings          plist(QDir::homePath() + "/.panache/taskplugins.list", QSettings::IniFormat);
        QStringList        splugins = plist.childGroups();
        QString            flname;
        IPanacheTaskPlugin *nplugin;
        QWidget            *widget;

        foreach(QString plugin, splugins)
        {
                plist.beginGroup(plugin);

                flname = plist.value("FileName", "NIL").toString();

                if (flname != "NIL")
                {
                        QPluginLoader loader(flname);
                        QObject       *mplugin = loader.instance();
                        nplugin = 0;
                        if (mplugin != 0)
                        {
                                nplugin = qobject_cast<IPanacheTaskPlugin *>(mplugin);
                                widget  = nplugin->init();
                                if (widget != 0)
                                {
                                        plugins.insert(widget, nplugin);

                                        //Insert plugin in layout
                                        layout->addWidget(widget, 0, 0);
                                        widget->show();
                                }
                        }
                }

                plist.endGroup();
        }

        updatePos();
}

void TaskBar::updatePos()
{
        if (position == TopLeft)
        {
                move(0, -1);
                layout->setContentsMargins(5, 0, mask.width(), 0); //Margin
        }
        else if (position == TopRight)
        {
                move(dw - width(), -1);
                layout->setContentsMargins(mask.width(), 0, 5, 0); //Margin
        }
        else if (position == BottomLeft)
        {
                move(0, dh - height() + 1);
                layout->setContentsMargins(5, 0, mask.width(), 0); //Margin
        }
        else                                                       /* BottomRight */
        {
                move(dw - width(), dh - height() + 1);
                layout->setContentsMargins(mask.width(), 0, 5, 0); //Margin
        }
}

void TaskBar::updateMask()
{
        //Update mask
        if (position == TopLeft)
        {
                mask = tlm;
        }
        else if (position == TopRight)
        {
                mask = trm;
        }
        else if (position == BottomLeft)
        {
                mask = blm;
        }
        else /* BottomRight */
        {
                mask = brm;
        }
}

/* Events */

void TaskBar::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        painter.fillRect(0, 0, width(), height(), brush);

        //Draw edge
        if ((position == TopLeft) || (position == BottomLeft))
        {
                //Rounds right
                painter.drawPixmap(width() - mask.width(), 0, mask);
        }
        else
        {
                //Rounds left
                painter.drawPixmap(0, 0, mask);
        }

        QWidget::paintEvent(event);
}

void TaskBar::resizeEvent(QResizeEvent *event)
{
        //Update mask
        QBitmap mmask(width(), height());

        //Depending on the position, drawing the mask to the right place
        mmask.fill(Qt::color1);

        QPainter painter(&mmask);
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(QBrush(Qt::color0));

        if ((position == TopLeft) || (position == BottomLeft))
        {
                //Rounds right
                painter.drawPixmap(width() - mask.width(), 0, mask.mask());
        }
        else
        {
                //Rounds left
                painter.drawPixmap(0, 0, mask.mask());
        }

        setMask(mmask);

        updatePos();

        (void)event;
}

bool TaskBar::event(QEvent *event)
{
        //Resize widget because of children
        if (event->type() == QEvent::LayoutRequest)
        {
                if (layout != 0)
                {
                        int width = 0;

                        foreach(QWidget * widget, plugins.keys())
                        {
                                qDebug() << widget->sizeHint().width();

                                width += widget->sizeHint().width() + 5;
                        }

                        resize(mask.width() + width + 5, height());

                        event->accept();
                        return true;
                }
        }

        return QWidget::event(event);
}

void TaskBar::appEvent(XEvent *event)
{
        foreach(IPanacheTaskPlugin * plugin, plugins.values())
        {
                plugin->appEvent(event);
        }
}
