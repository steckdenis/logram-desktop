/*
 * Fen.h
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

#ifndef __FEN_H__
#define __FEN_H__

#include <QMainWindow>
#include <QApplication>
#include <QLabel>
#include <QPixmap>

#include <LIconLoader.h>
#include <LDirView.h>

class Fen : public QMainWindow
{
        Q_OBJECT

public:
        Fen(QString fileName);

        void openFile(const QString& fileName);

private slots:
        void open(bool);
        void save(bool);
        void saveas(bool);
        void quit(bool);
        void rotateLeft(bool);
        void rotateRight(bool);
        void scale(bool);

        void dvOpen(QString path, bool *cancel);

private:
        LIconLoader *iloader;

        QLabel  *image;
        QPixmap img;

        QString flname;

        LDirView *dv;
};

#endif /* __FEN_H__ */
