/*
 * LRibbon.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Flavien Lefebvre <flavien-lefebvre@hotmail.fr>
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
 *
 * The Logram team decline any responsibility of the further use of this class which would go against the Microsoft copyright about the ribbon
 *
 */

#ifndef LRIBBON_H
#define LRIBBON_H

#include <QWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QList>

class LRibbon : public QTabWidget
{
    Q_OBJECT

    public:
        LRibbon(QWidget *parent);


    public slots:
        void AddTab(QString name, QHBoxLayout *layout);


    private:

};

#endif // LRIBBON_H
