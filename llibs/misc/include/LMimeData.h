/*
 * LMimeData.h
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
 
#ifndef __LMIMEDATA_H__
#define __LMIMEDATA_H__

#include <QObject>
#include <QString>
#include <QFile>
#include <QStringList>
#include <QHash>

#include <LDesktopFile.h>


/**
 * \class LMimeData
 * \brief Obtiens des informations sur le type MIME d'un fichier
 *
 * Interroge la base de donnée MIME pour obtenir les informations demandées sur un fichier
 *
 */
class LMimeData : public QObject
{
        public: 
                /**
                 * \brief Constructeur
                 */
                LMimeData(QObject *parent=0);
                
                /**
                 * \brief Constructeur
                 * \param fileName Nom du fichier dont on veut obtenir le MIME
                 */
                LMimeData(const QString &fileName, QObject *parent=0);
                
                /**
                 * \brief Change le nom du fichier courant
                 */
                void setFileName(const QString &fileName);
                
                /**
                 * \brief Obtiens le nom du fichier courant
                 */
                QString fileName() const;
                
                /**
                 * \brief Retourne le type MIME du fichier courant (text/html par exemple)
                 */
                QString mime();
                
                /**
                 * \brief Retourne le commentaire du type MIME (Document HTML par exemple)
                 */
                QString comment();
                
                /**
                 * \brief Retourne le nom de l'icône du type MIME
                 */
                QString iconName();
                
                /**
                 * \brief Retourne la liste des fichiers .desktop avec lesquels on peut ouvrir le MIME
                 */
                QList<LDesktopFile *> openWith();
                
        private:
                QString _fileName;
                QString mimeCache;
                
                QStringList desktopFiles;
                
                QHash<QString, QString> knownExts;
                
                void findDesktopFiles(QString path);
};

#endif /* __LMIMEDATA_H__ */

