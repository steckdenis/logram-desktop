/*
 * Config.cpp
 * This file is part of Locom Runner
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Locom Runner is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Locom Runner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Locom Runner; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */
 
#include <Config.h>

#include <QSettings>
#include <QDir>
#include <QString>

Config::Config(QDomElement &element, const QString &type, QObject *parent) : QObject(parent)
{
        _type = type;
        
        if (type == "qsettings")
        {
                //Explorer les enfants pour trouver le chemin d'accès au fichier
                QDomElement el = element.firstChildElement();
	        QString path;
	        
	        while (!el.isNull())
	        {
	                if (el.tagName() == "path")
	                {
	                        path = el.text();
	                        path.replace("~", QDir::homePath());
	                }
	                
	                el = el.nextSiblingElement();
	        }
	        
                set = new QSettings(path, QSettings::IniFormat, this);
        }
        
        (void) element; //Sera utilisé par d'autres types de Config
}

void Config::setValue(const QString &key, const QVariant &value)
{
        if (_type == "qsettings")
        {
                set->setValue(key, value);
        }
}

QVariant Config::value(const QString &key, QVariant &defaultValue)
{
        if (_type == "qsettings")
        {
                return set->value(key, defaultValue);
        }
        
        return QVariant();
}
