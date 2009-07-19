/*
 * LImageButton.h
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

/**
 * \file LImageButton.h
 * \brief Déclaration de la classe LImageButton
 */

#ifndef __LIMAGEBUTTON_H__
#define __LIMAGEBUTTON_H__

#include <QPushButton>
#include <QPixmap>
#include <QPainter>

/**
 * \class LImageButton
 * \brief Affiche une image comme bouton
 *
 * Affiche un bouton qui n'est en fait qu'une image. Le bouton a deux images : une normale pour quand le
 * bouton n'est ni survolé, ni poussé, et une active pour quand la souris est sur le bouton
 *
 * Cette classe hérite de QPushButton. Vous pouvez donc utiliser LImageButton comme un QPushButton normal,
 * c'est à dire en utilisant ses slots et propriétés.
 *
 * La seule contrainte est que le bouton doit toujours avoir la même taille que les deux QPixmap qu'il affiche.
 */
class LImageButton : public QPushButton
{
public:

        /**
         * \brief Construit un nouveau LImageButton
         *
         * Créer un LImageButton en fonction des deux pixmaps données, qui seront affichées sur le bouton.
         *
         * \param mnorm QPixmap affichée quand le bouton est normal
         * \param mactive QPixmap affichée quand le bouton est survolé ou appuyé
         * \param parent QWidget parent du bouton
         */
        LImageButton(QPixmap mnorm, QPixmap mactive, QWidget *parent = 0);

protected:
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);
        virtual void paintEvent(QPaintEvent *event);

private:
        bool    mhover;
        QPixmap norm, active;
};

#endif
