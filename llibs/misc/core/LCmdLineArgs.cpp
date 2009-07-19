/*
 * LCmdLineArgs.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - leo2urlevan (applied patch by Denis Steckelmacher)
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

#include <LCmdLineArgs.h>

LCmdLineArgs::LCmdLineArgs(int argc, char **argv, QObject *parent) : QObject(parent)
{
        for (int i = 1; i < argc; ++i)
        {
                instructions << QString(argv[i]);
        }

        size = instructions.size();
        lastPositionMemorized = 0;
        lastCom = ' ';
        lastKey = QString();
}

bool LCmdLineArgs::containsKey(QString str) const
{
        return instructions.contains(QString("--") + str);
}

bool LCmdLineArgs::prepareKey(QString str)
{
        lastKey = str;

        for (int i = 0; i < size; ++i)
        {
                if (instructions.at(i).contains("--") && instructions.at(i).contains(str + "="))
                {
                        lastPositionMemorized = i;
                        return true;
                }
        }

        return containsKey(str);
}

QString LCmdLineArgs::value(QString str, bool toVerify)
{
        if (toVerify || (lastKey != str))
        {
                if (!containsKey(str))
                {
                        return QString();
                }
        }

        return getArgsKey();
}

QStringList LCmdLineArgs::values(char c, bool toVerify)
{
        if (toVerify || (QChar(c) != lastCom))
        {
                if (!containsCommutator(c))
                {
                        return QStringList();
                }
        }

        return getArgsCom();
}

bool LCmdLineArgs::prepareCommutator(char c)
{
        lastCom = QChar(c);

        return prepareCommutator(lastCom);
}

bool LCmdLineArgs::prepareCommutator(QChar c)
{
        lastCom = c;

        for (int i = 0; i < size; ++i)
        {
                if (instructions.at(i).startsWith('-') && instructions.at(i).contains(c) && !instructions.at(i).startsWith("--"))
                {
                        lastPositionMemorized = i;
                        return true;
                }
        }

        lastPositionMemorized = instructions.size();

        return containsCommutator(c);
}

bool LCmdLineArgs::containsCommutator(char c) const
{
        QString character("-" + QString(c));

        return instructions.contains(character);
}

bool LCmdLineArgs::containsCommutator(QChar c) const
{
        QString character("-" + QString(c));

        return instructions.contains(character);
}

QStringList LCmdLineArgs::orphans() const
{
        bool        assigned = false;
        QStringList result;

        for (int i = 0; i < size; ++i)
        {
                if (instructions.at(i).startsWith('-'))
                {
                        assigned = true;
                }

                if (!assigned)
                {
                        result << instructions.at(i);
                }

                if (instructions.at(i).startsWith("--"))
                {
                        assigned = false;
                }
        }
        return result;
}

QString LCmdLineArgs::getArgsKey()
{
        QString     word = instructions.at(lastPositionMemorized);
        QString&    temp = word.remove("--");
        QStringList arg  = temp.split("=");

        return arg.at(1);
}

QStringList LCmdLineArgs::getArgsCom()
{
        QStringList result;
        int         j = lastPositionMemorized + 1;

        while (j < size && !instructions.at(j).startsWith('-'))
        {
                result << instructions.at(j);
                ++j;
        }
        return result;
}
