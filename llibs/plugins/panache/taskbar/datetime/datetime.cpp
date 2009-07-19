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

#include <QDate>
#include <QTime>
#include <QPainter>
#include <QPen>
#include <QSizePolicy>

#include <QtDebug>

#include <datetime.h>

#include <LConfig.h>

/*
        Le plugin
 */

QWidget *DatetimePlugin::init()
{
        //Créer le widget
        widget = new DatetimeWidget(this);

        return widget;
}

/*
        Le widget
 */

DatetimeWidget::DatetimeWidget(DatetimePlugin *mplugin)
{
        plugin = mplugin;

        // Donner la bonne taille
        w = LConfig::logramValue("DateTimePlugin/Width", 100, "Theme").toInt();
        h = LConfig::logramValue("DateTimePlugin/Height", 25, "Theme").toInt();

        resize(w, h);

        // Charger la configuration
        dateSize = LConfig::logramValue("DateTimePlugin/DateSize", 8, "Theme").toInt();
        timeSize = LConfig::logramValue("DateTimePlugin/TimeSize", 14, "Theme").toInt();
        color    = QColor(LConfig::logramValue("DateTimePlugin/TextColor", "#000000", "Theme").toString());

        fh.setFamily("DejaVu Sans");
        fh.setPixelSize(timeSize);
        fh.setWeight(QFont::Bold);

        fd.setFamily("DejaVu Sans");
        fd.setPixelSize(dateSize);

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
        timer->start(1000);

        // Flags
        setAttribute(Qt::WA_NoSystemBackground, true);

        // Redimensionnement
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
}

QSize DatetimeWidget::sizeHint() const
{
        return QSize(w, h);
}

void DatetimeWidget::timerTimeout()
{
        date = QDate::currentDate().toString();
        hour = QTime::currentTime().toString(Qt::SystemLocaleShortDate);

        repaint(0, 0, width(), height());
}

void DatetimeWidget::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        // Effacer le contrôle
        painter.setBackground(QBrush(QColor(0, 0, 0, 0)));
        painter.eraseRect(0, 0, width(), height());

        // Dessiner l'heure
        painter.setPen(QPen(color));

        painter.setFont(fh);
        painter.drawText(0, 0, (width() / 2) - 2, height(), Qt::AlignHCenter | Qt::AlignVCenter, hour);

        // Dessiner la date
        painter.setFont(fd);
        painter.drawText((width() / 2) + 2, 0, (width() / 2) - 2, height(), Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, date);
}

Q_EXPORT_PLUGIN2(datetime, DatetimePlugin)
