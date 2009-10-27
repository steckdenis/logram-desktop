/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef COMPILATEUR_H
#define COMPILATEUR_H

#include <QtGui>

class Compilateur : public QProcess
{
    Q_OBJECT

    public:
        Compilateur( QObject * parent = 0 );
        ~Compilateur();

        void compile(QString dossier); //on compile
        void clean(QString dossier); //on nettoie

    public slots:
        void actionFinished(); //une action est finie

    private:
        bool bQmake;//est-on en train de "qmake" :-°
        bool bMake; //est-on en train de "make" ^^
        bool bClean; //est-on en train de nettoyer ?
        QString DossierProjet; //dossier du projet

   signals:
        void compilationFinished(); //la compilation est terminée
        void cleanFinished(); //le nettoyage est terminé
};

#endif // COMPILATEUR_H
