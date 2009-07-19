/*
 * LDesktopFile.cpp
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

#include <LDesktopFile.h>

#include <QLocale>
#include <QStringList>
#include <QtDebug>

LDesktopFile::LDesktopFile(const QString& fileName, QObject *parent) : QFile(fileName, parent)
{
        if (!open(QIODevice::ReadOnly | QIODevice::Text))
        {
                qWarning("Unable to open file %s", qPrintable(fileName));
        }

        locale = QLocale::system().name().section('_', 0, 0);
}

QString LDesktopFile::name()
{
        return value("Name", true);
}

QString LDesktopFile::comment()
{
        return value("Comment", true);
}

QString LDesktopFile::icon()
{
        return value("Icon", false);
}

QString LDesktopFile::exec()
{
        return value("Exec", false);
}

QString LDesktopFile::value(const QString& name, bool localized)
{
        //Se placer au début
        seek(0);

        QString rs;

        while (!atEnd())
        {
                //Lire chaque ligne pour trouver la clef
                QString str = readLine();

                if (str == "[Desktop Entry]")
                {
                        continue;
                }

                QStringList lst = str.split('=');
                
                if (localized)
                {
                        QString locname = name + "[" + locale + "]";
                        
                        if (lst[0] == locname)
                        {
                                rs = lst[1].remove('"').trimmed();
                                break;
                        }
                }

                if (lst[0] == name)
                {
                        rs = lst[1].remove('"').trimmed();

                        if (!localized)
                        {
                                break;
                        }
                }
        }

        return rs;
}
