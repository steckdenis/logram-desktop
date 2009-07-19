/* Copyright (c) 2009, Cleriot Simon <malgon33@gmail.com>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/


#ifndef FULLSCREEN_H
#define FULLSCREEN_H

/*!
 * \file fullscreen.h
 * \brief Fenêtre plein ecran pour selectionner le cadre
 * \author Cleriot S.
 * \version 2.5
 */

#include <QtGui>

/*! \class fullDialog
 * \brief classe representant la fenêtre plein ecran
 *
 *  Cette classe gère l'affichage du QPixmap, et la gestion des evenements souris et de dessin.
 */
class fullDialog : public QLabel
{
        Q_OBJECT

public:

        /*!
         *  \brief Constructeur
         *
         *  Constructeur de la classe fullDialog
         *
         *  \param parent : widget parent
         *  \param flags : flag de ce widget
         */

        fullDialog(QWidget *parent = 0, Qt::WFlags flags = 0);

        /*!
         *  \brief Destructeur
         *
         *  Destructeur de la classe fullDialog
         */
        ~fullDialog();

//public slots:

protected:

        /*!
         *  \brief Evenement lors d'un appui sur un bouton de la souris
         *
         *  Gère l'evenement souris d'appui sur un bouton.
         *
         *  \param event : QMouseEvent contenant la position du curseur
         */
        void mousePressEvent(QMouseEvent *event);

        /*!
         *  \brief Evenement lors du dessin de la fenêtre
         *
         *  Gère le dessin de la fenêtre
         *
         *  \param event : QPaintEvent
         */
        void mouseMoveEvent(QMouseEvent *event);

        /*!
         *  \brief Evenement lorsque on relache un bouton de la souris
         *
         *  Gère le relachement d'un bouton de la souris.
         *
         *  \param event : QMouseEvent contenant la position du curseur
         */
        void mouseReleaseEvent(QMouseEvent *event);

        /*!
         *  \brief Evenement de dessin de la fenetre
         *
         *  Redessin le QPixmap et le cadre en fonction de x,y,x2 et y2.
         *
         *  \param event : QPaintEvent
         */
        void paintEvent(QPaintEvent *event);

        void focusOutEvent(QFocusEvent *event);

signals:
        void imageSaved();

private:
        bool    pressed; /*!< Boolean pour savoir si le bouton de la souris est pressé ou pas */
        bool    capture; /*!< Boolean pour savoir si on est en train de capturer */
        int     x;       /*!< Position initial X du curseur */
        int     y;       /*!< Position initial Y du curseur */
        int     x2;      /*!< Position actuelle X du curseur */
        int     y2;      /*!< Position actuelle Y du curseur */
        QPixmap image;   /*!< Image de fond */
};


#endif // FULLSCREEN_H
