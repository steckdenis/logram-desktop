/*
 * LApplication.cpp
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

#include <LApplication.h>
#include <LConfig.h>
#include <LCmdLineArgs.h>

#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QLibraryInfo>

#include <QX11Info>

extern "C" {
#include <X11/Xlib.h>
}

LApplication::LApplication(int& argc, char *argv[]) : QApplication(argc, argv)
{
        // Textes en utf8
        QByteArray codec = LConfig::logramValue("TextCodec", "utf8").toByteArray();

        QTextCodec::setCodecForCStrings(QTextCodec::codecForName(codec));
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codec));

        // Obtiens le nom de la locale
        QString locale = QLocale::system().name().section('_', 0, 0);

        // Installe les traductions internes à Qt (boîtes de messages, warnings, etc)
        QTranslator translator;
        translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
        installTranslator(&translator);

        // Installe les traductions de Logram
        QTranslator translogram;
        translogram.load(QString("logram_") + locale, "/usr/share/logram/translations");
        installTranslator(&translogram);

        // Initialise et lis les paramètres
        cmds = new LCmdLineArgs(argc, argv);
}

void LApplication::appLoaded(const QString& identifier)
{
        //Originairement de Kwin

        XEvent e;

        e.xclient.type         = ClientMessage;
        e.xclient.message_type = XInternAtom(QX11Info::display(), "_LOGRAM_SPLASH_PROGRESS", False);
        e.xclient.display      = QX11Info::display();
        e.xclient.window       = QX11Info::appRootWindow();
        e.xclient.format       = 8;
        strcpy(e.xclient.data.b, identifier.toUtf8().data());
        XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False, SubstructureNotifyMask, &e);
}

LCmdLineArgs *LApplication::cmdLine() const
{
        return cmds;
}
