/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "compilateur.h"

Compilateur::Compilateur(QObject *parent)
        : QProcess(parent)
{
    connect(this,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(actionFinished()));

    bMake=false;
    bQmake=false;
    bClean=false;
}

Compilateur::~Compilateur()
{
    //
}


/**on démarre la compilation**/
void Compilateur::compile(QString dossier)
{
    DossierProjet=dossier;
    QDir::setCurrent(dossier);
    bQmake=true;
    start("qmake");
}

/**on démarre le nettoyage**/
void Compilateur::clean(QString dossier)
{
    DossierProjet=dossier;
    bClean=true;
    start("make clean");
}

/**un action est terminée**/
void Compilateur::actionFinished()
{
    if(bQmake==true) //si on était en train de "qmake"
    {
        QDir::setCurrent(DossierProjet);
        bQmake=false;
        bMake=true;
        start("make");
    }
    else if(bMake==true) //si on était en train de "make"
    {
        bMake=false;

        emit compilationFinished();
    }
    else if(bClean==true) //si on était en train de nettoyer
    {
        bClean=false;
        emit cleanFinished();
    }
}
