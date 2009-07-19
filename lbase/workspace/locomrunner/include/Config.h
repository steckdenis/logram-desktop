/*
 * Config.h
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
 
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <QObject>
#include <QVariant>
#include <QDomElement>

class QSettings;

/**
 * \class Config
 * \brief Représente un fichier de configuration
 */
class Config : public QObject
{
        Q_OBJECT
        public:
                /**
                 * \brief Constructeur
                 * \param element Élément XML qui contient les infos de configuration (path, etc)
                 * \param type Type de configuration (QSettings, base de registre, serveur, etc)
                 * \param parent QObject parent
                 */
                Config(QDomElement &element, const QString &type, QObject *parent=0);
                
                Q_INVOKABLE QVariant value(const QString &key, QVariant &defaultValue);
                
        public slots:
                void setValue(const QString &key, const QVariant &value);
                
        private:
                QSettings *set;
                QString _type;
};

#endif /* __CONFIG_H__ */

