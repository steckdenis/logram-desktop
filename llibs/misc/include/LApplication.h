/*
 * LApplication.h
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

#ifndef __LAPPLICATION_H__
#define __LAPPLICATION_H__

#include <QApplication>

class LCmdLineArgs;

/**
 * \class LApplication
 * \brief Représente une application Logram
 *
 * S'utilise exactement comme une QApplication, mais permet d'implémenter des fonctionnalités propres à Logram
 *
 */
class LApplication : public QApplication
{
public:

        /**
         * \brief Constructeur
         * \param argc : nombre d'arguments (paramètre argc de main())
         * \param argv : valeur des arguments (paramètre argv de main())
         */
        LApplication(int& agrc, char *argv[]);

        /**
         * \brief Retourne la liste des arguments de la ligne de commande, traîtés
         */
        LCmdLineArgs *cmdLine() const;

        /**
         * \brief Informe qu'une application est chargée
         *
         * Permet d'envoyer un message à la fenêtre root X11 qui indique que l'application dont l'ID
         * est identifier a fini de se charger.
         *
         * Cette fonction est principalement utilisée par Panache et par d'autres services d'arrière-plan qui
         * ont besoin d'être chargés avant que le bureau ne soit utilisable
         *
         * <strong>Attention !</strong> la taille de identifier ne peut pas dépasser les 19 caractères (20 en comptant le \0 final)
         *
         * \param identifier Identifiant de l'application ("panache-desktop" par exemple)
         */
        static void appLoaded(const QString& identifier);

private:
        LCmdLineArgs *cmds;
};

#endif /* __LAPPLICATION_H__ */
