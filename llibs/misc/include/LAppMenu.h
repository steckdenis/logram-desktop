/*
 * LAppMenu.h
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

#ifndef __LAPPMENU_H__
#define __LAPPMENU_H__

#include <QObject>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QString>
#include <QHash>

#include <LIconLoader.h>

/**
 * \brief Représente le menu Applications
 *
 * Cette classe est utilisée pour afficher sous forme d'arbre le contenu du menu
 * Applications. Elle peut aussi servir à afficher n'importe quel menu aux normes
 * Freedesktop.org
 */
class LAppMenu : public QObject
{
public:

        /**
         * \brief Construit un LAppMenu
         * \param fileName Nom du fichier (relatif, absolu, ou NULL pour le fichier applications.menu)
         * \param parent QObject parent
         */
        LAppMenu(QString fileName = QString(), QObject *parent = 0);

        struct Menu;

        /**
         * \brief Entrée dans le menu
         *
         * Représente une entrée dans un menu (programme)
         */
        struct Entry
        {
                QString desktopFile;            /*!< Nom du fichier .desktop de l'entrée */
                QString categoryString;         /*!< Chaîne de catégories (Qt;Examples par exemple) */
                QString title;                  /*!< Titre de l'entrée */
                QIcon   icon;                   /*!< Icône de l'entrée */
                QString iconName;               /*!< Nom de l'icône (usage interne) */
                Menu    *parent;                /*!< Menu parent */
        };

        /**
         * \brief Représente un menu
         */
        struct Menu
        {
                QString        title;           /*!< Titre du menu (Applications, etc) */
                QIcon          icon;            /*!< Icône du menu */
                Menu           *parent;         /*!< Menu parent (0 si root) */
                bool           onlyUnallocated; /*!< Le menu ne contient que des entrées qui ne sont pas dans les autres menus */
                bool           deleted;         /*!< Le menu ne doit pas être affiché */
                QList<Entry *> entries;         /*!< Entrées que contient le menu */
                QList<Menu *>  menus;           /*!< Sous-menus du menu */
        };

        /**
         * \brief Retourne le menu principal
         */
        Menu *root() const;

private:
        enum MatchRule
        {
                Or,
                And
        };

        void parseMenu(QDomElement& element, Menu *parent = 0);
        void readEntries(QString dir = QString());
        void includeExclude(QDomElement& element, Menu *menu, bool include);
        bool isMatch(QDomElement& element, QString& fileName, QString& categoryString, MatchRule rule);

        QHash<QString, Menu *>  knowMenus;
        QHash<QString, Entry *> knowEntries;
        Menu *rootMenu;

        LIconLoader *iloader;

        QDomDocument document;
};

#endif /* __LAPPMENU_H__ */
