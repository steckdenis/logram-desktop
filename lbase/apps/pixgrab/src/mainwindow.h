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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*!
 * \file mainwindow.h
 * \brief Fenêtre principale, permettant de lancer la capture d'écran et de choisir les paramètres.
 * \author Cleriot S.
 * \version 2.5
 */

#include <QtGui>
#include "ui_mainwindow.h"
#include "fullscreen.h"


#ifdef Q_OS_WIN
 #include <stdlib.h>
#endif

#ifdef Q_OS_WIN
 #include <qt_windows.h>
#endif

#ifdef Q_WS_X11
extern "C" {
 #include <X11/Xlib.h>
}
#endif


/*! \class MainWindow
 * \brief classe representant la fenêtre principale de selection des paramètres
 *
 *  Cette classe gère la sélection des paramètres (timing, plein écran ou pas, etc... ) et de lancement de la capture.
 */
class MainWindow : public QMainWindow
{
        Q_OBJECT

public:

        /*!
         *  \brief Constructeur
         *
         *  Constructeur de la classe MainWindow
         *
         *  \param parent : widget parent
         *  \param flags : flag de cette fenêtre principale
         */
        MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);

        /*!
         *  \brief Destructeur
         *
         *  Destructeur de la classe MainWindow
         */
        ~MainWindow();

public slots:

        /*!
         *  \brief Capturer l'écran
         *
         *  Lance la capture de l'écran, et si besoin est, lance chooseSize (pour selectionner le cadre)
         */
        void capture();

        /*!
         *  \brief Choix du cadre
         *
         *  Permet de choisir le cadre à enregistrer.
         *
         *  \param image : QPixmap à mettre en fond de fenêtre pour la 2nd capture d'écran
         */
        void chooseSize(QPixmap *image);

        /*!
         *  \brief A propos de Qt
         *
         *  Lance une boite de dialogue affichant des infos sur Qt
         */
        void aboutQt();

        /*!
         *  \brief A propos de Qt
         *
         *  Lance une boite de dialogue affichant des infos sur l'auteur et le programme
         */
        void about();

        /*!
         *  \brief A propos de Qt
         *
         *  Lance une boite de dialogue affichant la licence du programme
         */
        void licence();

        /*!
         *  \brief Lors d'un clique sur l'icone de la barre des tâches
         *
         *  Lors du clique sur l'icone de la barre des tâches, montre ou cache la fenêtre
         */
        void trayActivated();

private:
        Ui::MainWindowClass ui;    /*!< Interface utilisateur de la fenêtre principale */
        QSystemTrayIcon     *tray; /*!< Icone dans la barre des tâches */
        fullDialog          full;  /*!< Fenêtre plein écran */
};

#endif // MAINWINDOW_H
