/*
 * datetime.h
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

#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <QWidget>
#include <QObject>
#include <QString>
#include <QSize>
#include <QList>
#include <QColor>
#include <QPixmap>
#include <QBrush>

#include <IPanacheTaskPlugin.h>

class NotifyWidget;
class XAtoms;
class QX11EmbedContainer;

/*
        Classe DatetimePlugin
 */
class NotifyPlugin : public QObject, public IPanacheTaskPlugin
{
        Q_OBJECT
        Q_INTERFACES(IPanacheTaskPlugin)

public :
                QWidget *init();

        void appEvent(XEvent *event);

private:
        NotifyWidget *widget;
};

/*
        Classe DatetimeWidget
 */
class NotifyWidget : public QWidget
{
        Q_OBJECT

public:
        NotifyWidget(NotifyPlugin *mplugin);

        void appEvent(XEvent *event);
        virtual QSize sizeHint() const;

private slots:
        void initSelection();

        void clientClosed();

protected:
        virtual void paintEvent(QPaintEvent *);
        virtual void resizeEvent(QResizeEvent *);

private:
        NotifyPlugin *plugin;
        XAtoms       *atoms;

        int     atmST, iconSize, mwidth, iconMargin;
        QBrush  brhColor;
        QPixmap bgPix;

        QList<QX11EmbedContainer *> clients;

        void updatePos();
};

#endif /* __DATETIME_H__ */
