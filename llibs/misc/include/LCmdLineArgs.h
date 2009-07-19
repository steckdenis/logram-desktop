/*
 * LCmdLineArgs.h
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

#ifndef __LCMDPARSER_H__
#define __LCMDPARSER_H__

#include <QString>
#include <QStringList>
#include <QChar>
#include <QDialog>
#include <QLabel>
#include <QObject>

/**
 * \class LCmdLineArgs
 * \brief Gère les paramètres de la ligne de commande
 *
 * Cette classe permet de gérer les paramètres de la ligne de commande, à savoir leur lecture (commutateurs, clef-valeur, etc). Cette classe est
 * conçue pour être bien plus facile à utiliser que le tableau associatif QApplication::arguments
 *
 */
class LCmdLineArgs : public QObject
{
public:

        /**
         * \brief Constructeur
         *
         * <strong>Attention :</strong> Il est déconseillé d'instancier soi-même cette classe. Utilisez LApplication::cmdLine() à la place
         *
         * \param argc Nombre d'arguments envoyés à l'application
         * \param argv Valeur des arguments
         * \param parent QObject parent
         */
        LCmdLineArgs(int argc, char **argv, QObject *parent = 0);

        /**
         * \brief Vérifie que le commutateur a été passé à l'application
         *
         * Permet de vérifier que le commutateur c a été passé à l'application.
         *
         * Si l'utilisateur a envoyé "-a -c", ou "-ac", les commutateurs "a" et "c" seront détectés
         *
         * \param c Lettre du commutateur
         * \sa containsKey()
         */
        bool containsCommutator(char c) const;

        /**
         * \brief Vérifie que le commutateur a été passé à l'application
         *
         * Permet de vérifier que le commutateur c a été passé à l'application.
         *
         * Si l'utilisateur a envoyé "-a -c", ou "-ac", les commutateurs "a" et "c" seront détectés
         *
         * \param c Lettre du commutateur
         * \sa containsKey
         */
        bool containsCommutator(QChar c) const;

        /**
         * \brief Vérifie qu'un argument a été passé à l'application
         *
         * Cette fonction vérifie qu'un argument a été passé, mais pas qu'il a une valeur (--help et --truc=chose sont permis)
         *
         * Si "-a --help" est passé comme ligne de commande, "a" est un commutateur et "help" est un argument
         *
         * \param str Nom du paramètre
         * \sa containsCommutator()
         */
        bool containsKey(QString str) const;

        /**
         * \brief Prépare un commutateur à la lecture
         *
         * Retourne true si le commutateur existe (comme containsCommutator()), et le prépare à la lecture. Avant de lire la valeur
         * d'un commutateur, vous devez appeler cette fonction.
         *
         * \param c Lettre du commutateur
         * \sa containsCommutator()
         * \sa prepareKey()
         */
        bool prepareCommutator(char c);

        /**
         * \brief Prépare un commutateur à la lecture
         *
         * Retourne true si le commutateur existe (comme containsCommutator()), et le prépare à la lecture. Avant de lire la valeur
         * d'un commutateur, vous devez appeler cette fonction.
         *
         * \param c Lettre du commutateur
         * \sa containsCommutator()
         * \sa prepareKey()
         */
        bool prepareCommutator(QChar c);

        /**
         * \brief Prépare la lecture d'un argument
         *
         * Vérifie si l'argment existe (comme containsKey()), et le prépare à la lecture. Avant de lire la valeur d'un argument,
         * vous devez avoir appelé cette fonction.
         *
         * \param str Nom de l'argument
         */
        bool prepareKey(QString str);

        /**
         * \brief Retourne les valeurs d'un commutateur
         *
         * Retourne la liste des valeurs suivant un commutateur, et n'étant ni un commutateur ni un paramètre.
         *
         * La fonction s'arrête au premier commutateur ou paramètre trouvé. Par exemple "-c machin valeur --truc=chose -d objet"
         * va retourner "machin" et "valeur" pour l'argument "c".
         *
         * <strong>Attention :</strong> prepareCommutator() doit avoir été appelée en lui passant en paramètre le commutateur que vous voulez lire.
         *
         * \param c Lettre du commutateur
         * \param toVerify True pour vérifier que le commutateur est présent (retourne alors QStringList() s'il n'est pas présent)
         */
        QStringList values(char c, bool toVerify = false);

        /**
         * \brief Retourne la valeur d'un paramètre
         *
         * Retourne la valeur d'un paramètre. Par exemple, pour "-c --verify=false", "verify" va renvoyer "false".
         *
         * <strong>Attention :</strong> prepareKey() doit avoir été appelée en lui passant en paramètre le commutateur que vous voulez lire.
         *
         * \param str Nom du paramètre à lire
         * \param toVerify True pour vérifier que le paramètre existe (retourne alors QString() s'il n'est pas présent)
         */
        QString value(QString str, bool toVerify = false);

        /**
         * \brief Retourne les paramères orphelins
         *
         * Un paramètre orphelin est un paramètre qui ne peut pas être récupéré par une autre fonction. Par exemple,
         * "-c truc param --machin=chose orphelin -d objet", "orphelin" est orphelin.
         *
         */
        QStringList orphans() const;

private:
        QStringList instructions;
        int         size;
        int         lastPositionMemorized;
        QChar       lastCom;
        QString     lastKey;

        QStringList getArgsCom();
        QString getArgsKey();
};

# endif
