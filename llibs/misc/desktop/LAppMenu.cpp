/*
 * LAppMenu.cpp
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

#include <QSettings>
#include <QDir>
#include <QFile>
#include <QtDebug>

#include <LAppMenu.h>
#include <LDesktopFile.h>

LAppMenu::LAppMenu(QString fileName, QObject *parent) : QObject(parent)
{
        //Ouvrir le document
        bool rs;

        if (fileName.isNull())
        {
                rs = document.setContent(new QFile("/etc/xdg/menus/applications.menu"));
        }
        else if (fileName.startsWith('/'))
        {
                rs = document.setContent(new QFile(fileName));
        }
        else
        {
                rs = document.setContent(new QFile(QString("/etc/xdg/menus/") + fileName));
        }

        if (rs)
        {
                QDomElement menu = document.documentElement();

                //Charger le thème d'icônes
                iloader = new LIconLoader(QString(), this);

                //Parser le menu
                parseMenu(menu);
        }
        else
        {
                //Le chargement a raté, créer un menu vide
                rootMenu = new Menu;

                rootMenu->parent          = 0;
                rootMenu->onlyUnallocated = false;
                rootMenu->deleted         = false;
        }
}

LAppMenu::Menu *LAppMenu::root() const
{
        return rootMenu;
}

void LAppMenu::parseMenu(QDomElement& element, Menu *parent)
{
        //Créer un menu
        Menu *menu = 0;

        //Explorer les éléments
        QDomElement el = element.firstChildElement();

        while (!el.isNull())
        {
                if (el.tagName() == "Name")
                {
                        //Nom du menu

                        //Créer le menu s'il n'est pas déjà connu
                        if (!knowMenus.contains(el.text()))
                        {
                                menu = new Menu;
                                menu->onlyUnallocated = false;
                                menu->deleted         = false;

                                knowMenus.insert(el.text(), menu);
                        }
                        else
                        {
                                menu = knowMenus.value(el.text());
                        }

                        menu->title = el.text();
                }
                else if (el.tagName() == "Directory")
                {
                        //Dossier (icône)
                        LDesktopFile set(QString("/usr/share/desktop-directories/") + el.text());

                        menu->icon  = iloader->icon(set.icon());
                        menu->title = set.name();
                }
                else if (el.tagName() == "DefaultAppDirs")
                {
                        //Dossiers des .desktop
                        readEntries();
                }
                else if (el.tagName() == "AppDir")
                {
                        //Dossier qui contient des .desktop
                        readEntries(el.text());
                }
                else if (el.tagName() == "OnlyUnallocated")
                {
                        //Le menu ne contient que des entrées nulle part d'autre
                        menu->onlyUnallocated = true;
                }
                else if (el.tagName() == "NotOnlyUnallocated")
                {
                        //Le menu contient tout type d'entrées
                        menu->onlyUnallocated = false;
                }
                else if (el.tagName() == "Deleted")
                {
                        //Le menu est supprimé, ne pas l'afficher
                        menu->deleted = true;
                }
                else if (el.tagName() == "NotDeleted")
                {
                        //Le menu n'est pas supprimé
                        menu->deleted = false;
                }
                else if (el.tagName() == "Include")
                {
                        //Inclure des éléments
                        if (!menu->onlyUnallocated)
                        {
                                includeExclude(el, menu, true);                         //True = inclure
                        }
                }
                else if (el.tagName() == "Exclude")
                {
                        //Exclure des éléments
                        if (!menu->onlyUnallocated)
                        {
                                includeExclude(el, menu, false);                     //False = exclure
                        }
                }
                else if (el.tagName() == "Menu")
                {
                        //Inclure un sous-menu
                        parseMenu(el, menu);
                }

                el = el.nextSiblingElement();
        }

        if (parent != 0)
        {
                menu->parent = parent;
                parent->menus.append(menu);
        }
        else
        {
                rootMenu     = menu;
                menu->parent = 0;
        }
}

void LAppMenu::readEntries(QString dir)
{
        //Lire les fichiers .desktop de /usr/share/applications, et les mettre dans la liste
        QDir mdir;

        if (dir.isNull())
        {
                mdir = QDir("/usr/share/applications");
        }
        else
        {
                mdir = QDir(dir);
        }

        //Explorer les fichiers
        foreach(QString file, mdir.entryList(QDir::Files))
        {
                //QSettings set(mdir.absoluteFilePath(file), QSettings::IniFormat);
                LDesktopFile df(mdir.absoluteFilePath(file));

                //N'ajouter l'entrée que si elle n'est pas déjà connue
                if (!knowEntries.contains(df.name()))
                {
                        Entry *entry = new Entry;

                        entry->desktopFile    = mdir.absoluteFilePath(file);
                        entry->categoryString = df.value("Categories");
                        entry->title          = df.name();
                        entry->iconName       = df.icon();
                        entry->parent         = 0;

                        knowEntries.insert(df.name(), entry);
                }
        }

        //Explorer les sous-dossiers
        foreach(QString dr, mdir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
                readEntries(mdir.absoluteFilePath(dr));
        }
}

void LAppMenu::includeExclude(QDomElement& element, Menu *menu, bool include)
{
        //Pour chaque entrée, vérifier si elle entre dans le menu
        if (include)
        {
                foreach(QString title, knowEntries.keys())
                {
                        Entry *entry = knowEntries.value(title);

                        if (isMatch(element, entry->desktopFile, entry->categoryString, Or))
                        {
                                entry->icon = iloader->icon(entry->iconName);
                                entry->parent = menu;
                                menu->entries.append(entry);
                        }
                }
        }
        else
        {
                foreach(Entry * entry, menu->entries)
                {
                        if (isMatch(element, entry->desktopFile, entry->categoryString, Or))
                        {
                                entry->icon = iloader->icon(entry->iconName);
                                entry->parent = 0;
                                menu->entries.removeOne(entry);
                        }
                }
        }
}

bool LAppMenu::isMatch(QDomElement& element, QString& fileName, QString& categoryString, MatchRule rule)
{
        QDomElement el = element.firstChildElement();

        bool rs = false;

        //qDebug() << "isMatch" << element.tagName() << "." << el.tagName() << ", rule =" << rule << ", fileName =" << fileName << ", cat =" << categoryString;

        //Explorer les sous-éléments
        while (!el.isNull())
        {
                if (el.tagName() == "FileName")
                {
                        QStringList parts = fileName.split('/');

                        return el.text() == parts[parts.count() - 1];
                }
                else if (el.tagName() == "Category")
                {
                        QStringList categories = categoryString.split(';', QString::SkipEmptyParts);

                        return categories.contains(el.text());
                }
                else if (el.tagName() == "Not")
                {
                        return !isMatch(el, fileName, categoryString, Or);
                }
                else if (el.tagName() == "And")
                {
                        bool r = isMatch(el, fileName, categoryString, And);

                        switch (rule)
                        {
                                case Or:
                                        if (r)
                                        {
                                                rs = true;
                                        }
                                        break;

                                case And:
                                        if (!r)
                                        {
                                                rs = false;
                                        }
                        }
                }
                else if (el.tagName() == "Or")
                {
                        bool r = isMatch(el, fileName, categoryString, Or);

                        switch (rule)
                        {
                                case Or:
                                        if (r)
                                        {
                                                rs = true;
                                        }
                                        break;

                                case And:
                                        if (!r)
                                        {
                                                rs = false;
                                        }
                        }
                }

                el = el.nextSiblingElement();
        }

        return rs;
}
