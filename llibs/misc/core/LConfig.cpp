/*
 * LConfig.cpp
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

#include <QTextCodec>

#include <LConfig.h>

LConfig::LConfig(QString appName, QObject *parent) : QSettings("Logram", appName, parent)
{
#if QT_VERSION >= 0x040500
        //Qt 4.5 supporte l'utf8 pour les LConfig
        setIniCodec(QTextCodec::codecForName(logramValue("TextCodec", "utf8").toByteArray()));
#endif
}

QVariant LConfig::value(const QString& key, const QVariant& defaultValue)
{
        //Si la valeur n'était pas dans le fichier de configuration, l'ajouter
        if (!contains(key))
        {
                setValue(key, defaultValue);
                sync();

                return defaultValue;    //Plus rapide qu'un reparsage
        }

        return QSettings::value(key, defaultValue);
}

QVariant LConfig::logramValue(const QString& key, const QVariant defaultValue, const QString module)
{
        QSettings settings("Logram", (module.isNull() ? "Core" : module), 0);

#if QT_VERSION >= 0x040500
        //Qt 4.5 supporte l'utf8 pour les LConfig
        settings.setIniCodec(QTextCodec::codecForName(QSettings("Logram", "Core", 0).value("TextCodec", "utf8").toByteArray()));
#endif

        if (!settings.contains(key))
        {
                settings.setValue(key, defaultValue);
                settings.sync();

                return defaultValue;
        }

        return settings.value(key, defaultValue);
}
