/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_mainwindow.h"
#include "wizard.h"
#include "compilateur.h"
#include "seek.h"
#include "addtext.h"

#include "plugininterface.h"

//#include "colorer.h"

/*QScintilla*/
#include <Qsci/qsciscintilla.h>
#include<Qsci/qscilexercpp.h>
#include <Qsci/qsciapis.h>
#include <iostream>

using namespace std;

/**Redéfinition de QScintilla pour mieux gérer l'auto-complétion**/
class Text : public QsciScintilla
{
    Q_OBJECT

    public:
        Text(QWidget *parent=0);

    protected:
        void keyPressEvent( QKeyEvent* e );
};


/**Fenêtre principale**/
class MainWindow : public QMainWindow, Ui::MainWindowClass
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MainWindow();

public slots:
    /*Gestion des fichiers*/
    void save();
    void load();
    void showFile();

    /*La compilation */
    void compile();
    void clean();
    void updateOutputText(); //on afficher la sortie de make et qmake
    void updateOutputError(); //on affiche les erreurs
    void endCompilation(); //compilation terminée
    void endClean(); //nettoyage terminé

    /*Créér un projet*/
    void launchWizard(); //on lance la fenêtre de création de projet

    /*action de rechercher/remplacer*/
    void dialFind(); //affichage de la boite de dialogue
    void seek(QString recherche); //on recherche
    void replace(QString string); //on remplace

    /*L'ajout de licence*/
    void dialAjout(); //boite de dialogue d'ajout de licence
    void addText(QString txt); //on ajoute le texte

    /*Annuler/Refaire*/
    void undo(); //Annuler
    void redo(); //refaire


    void loadPlugins(); //on charge les plugins

    void setModified(); //le projet a été modifié

    void updateCode(); //met à jour l'affichage du code


    /*Les boites de dialogue ^^ */
    void aboutQt();
    void about();
    void licence();

protected:
    void resizeEvent ( QResizeEvent * event ); //Quand on redimensionne la fenêtre
    void closeEvent( QCloseEvent * event ) ; //Fermeture de la fenêtre (on demande si on veut enregistrer)

private:
    QString fileName; //le nom du fichier .pro
    QString DossierProjet; //le dossier du projet
    QStringList listeFichiers; //la liste des fichiers
    Wizard *wizard; //la fenetre de creation de projet
    Compilateur *compilateur; //le compilateur
    Addtext *addtextDialog; //la fenetre d'ajout de texte (GPL etc)
    bool toSave; //faut-il sauvegarder ?

    Text *textEdit; //le QScintilla modifié
    QsciLexerCPP *lexerCPP; //pour la coloration du code
    QsciAPIs *api; //pour l'auto-complétion

    QMap<QString,QString> Fichiers; //La liste des fichiers avec leur contenu non sauvegardé
};

#endif // MAINWINDOW_H
