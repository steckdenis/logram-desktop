/*
 * IActionPlugin.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher <steckdenis@yahoo.fr>
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

#ifndef __IACTIONPLUGIN_H__
#define __IACTIONPLUGIN_H__

#include <QtPlugin>
#include <QAction>
#include <QtNetwork/QUrlInfo>
#include <QPixmap>
#include <QList>

/**
 * \struct ActionSideBar
 * \brief Structure d'un élément d'une Action affiché dans le LDirSidebar
 */
struct ActionSideBar
{
        QPixmap pixmap;      /*!< Pixmap (icône) de l'élément */
        QString title;       /*!< Titre de l'élément */
        QString description; /*!< Description de l'élément */
        int     id;          /*!< Identifiant de l'élément. Il est au choix de l'Action, et est passé en paramètre à IActionPlugin::actionActivated() */
        bool    visible;     /*!< Infique que l'élément doit être affiché */
};

/**
 * \struct ActionPluginInfo
 * \brief Informations sur un plugin d'Action
 */
struct ActionPluginInfo
{
        QPixmap pixmap;      /*!< Pixmap (icône) de l'Action */
        QString title;       /*!< Titre de l'Action */
        QString description; /*!< Description de l'Action */
        QString author;      /*!< Auteur de l'Action */
        int     flags;       /*!< Flags (ensemble ORé de ActionFlags) */
};

/**
 * \class IActionPlugin
 * \brief Interface entre les Actions et les applications hôtes (LDirView, mais aussi ActionsManager, etc)
 */
class IActionPlugin
{
public:
        ~IActionPlugin() {}

        /**
         * \brief Initialise l'action. Est appelée quand l'action est chargée
         */
        virtual void init() = 0;

        /**
         * \brief Retourne les informations sur l'Action
         * \return Structure ActionPluginInfo qui contient les informations sur l'Action
         */
        virtual ActionPluginInfo *info() = 0;

        /**
         * \brief Retourne une QAction à afficher dans le menu
         *
         * Ne sert qu'aux actions supportant le ActionFlag <strong>ActionShowInMenu</strong>.
         *
         * L'action retourne une QAction qui sera ajoutée dans le menu conextuel. Cette action
         * est toujours créée, mais ne doit pas obligatoirement être affichée.
         *
         * La gestion des propriétés d'une QAction se fait dans la fonction filesClicked()
         *
         * \return QAction créée. Elle peut avoir des enfants
         * \sa filesClicked()
         */
        virtual QAction *menuAction() = 0;

        /**
         * \brief Appelée quand l'utilisateur clique sur un fichier
         *
         * Ne sert qu'aux actions supportant le ActionFlag <strong>ActionShowInMenu</strong> ou <strong>ActionShowInSideBar</strong>.
         *
         * Cette fonction est appelée par le LDirView quand l'utilisateur clique sur un
         * fichier/dossier.
         *
         * Le boulot de cette fonction est d'ajuster l'action retournée par menuAction(),
         * ses éventuels enfants, ainsi que les actions SideBar pour par exemple ne pas afficher et désactiver les actions
         * qui ne marchent que sur un dossier quand c'est un fichier qui a été cliqué.
         *
         * \param url Url du fichier cliqué (absolue : «ftp://truc.machin/chose» par exemple)
         * \param info QUrlInfo du fichier. Permet d'obtenir les informations nécessaires (nom, etc)
         * \sa menuAction()
         */
        virtual void filesClicked(QList<QString>& urls, QList<QUrlInfo *>& infos) = 0;

        /**
         * \brief Retourne la liste des éléments de la SideBar
         *
         * Ne sert qu'aux actions supportant le ActionFlag <strong>ActionShowInSideBar</strong>.
         *
         * Retourne une liste des éléments pouvant être affichés dans la barre sur le bord
         * d'un LDirView par exemple (mais qui peut se trouver n'importe où).
         *
         * Les éléments sont gérés par l'Action, elle doit donc en garder une trace (ne fusse
         * que pour gérer sidebarClicked() ).
         *
         * \return Liste des éléments gérés
         * \sa sidebarClicked()
         */
        virtual QList<ActionSideBar *> *sidebarActions() = 0;

        /**
         * \brief Est appelée quand un élément est cliqué
         *
         * Permet à l'Action de gérer le clic sur l'élément dont l'id est le paramètre id. C'est
         * en particulier à ça que sert le champ "id" dans la structure ActionSideBar
         *
         * \param id ID de l'action cliquée
         */
        virtual void sidebarClicked(int id) = 0;

        /**
         * \enum ActionFlags
         * \brief Flags qui décrivent les possibilité d'une action
         */
        enum ActionFlags
        {
                ActionShowInSideBar = 1,         /*!< L'action sait fournir des éléments à afficher dans le LDirSidebar */
                ActionShowInMenu = 2             /*!< L'action sait fournir des éléments à afficher dans le menu contextuel d'un LDirView */
        };
};

Q_DECLARE_INTERFACE(IActionPlugin, "org.logram-project.actionplugininterface/0.1")

#endif
