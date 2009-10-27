/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "addtext.h"

Addtext::Addtext(QWidget *parent, Qt::WindowFlags flags) : QDialog(parent, flags)
{
    setupUi(this);

    connect(buttonAdd,SIGNAL(clicked()),this,SLOT(emitTextToAdd()));
    connect(comboBox,SIGNAL(activated(QString)),this,SLOT(changeText(QString)));

    /**on récupère la liste des templates de config/licences.xml et on ajout leur nom au combobox**/
    QFile file("config/licences.xml");
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly)){
        return;}

    QString error;
    int line;
    if (!doc.setContent(&file,false,&error,&line)) {
        file.close();
        QMessageBox::information(this,tr("Erreur"),tr("Erreur dans le fichier xml : %1 à la ligne %2").arg(error).arg(QString().setNum(line)));
        return;
    }
    file.close();

    QDomElement licence;
    QDomElement racine = doc.documentElement();
    QDomNode noeud = racine.firstChild();
    while(!noeud.isNull())
    {
        licence = noeud.toElement();
        if (licence.tagName() == "licence")
        {
            comboBox->addItem(licence.attribute("nom"));
        }
        noeud = noeud.nextSibling();
    }

}

Addtext::~Addtext()
{

}

void Addtext::emitTextToAdd()
{
    emit textToAdd(textAdd->toPlainText());

    hide();
}

/*on ajout le template au textEdit*/
void Addtext::changeText(QString nom)
{
    QFile file("config/licences.xml");
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly)){
        return;}

    QString error;
    int line;
    if (!doc.setContent(&file,false,&error,&line)) {
        file.close();
        QMessageBox::information(this,tr("Erreur"),tr("Erreur dans le fichier xml : %1 à la ligne %2").arg(error).arg(QString().setNum(line)));
        return;
    }
    file.close();

    QDomElement licence;
    QDomElement racine = doc.documentElement();
    QDomNode noeud = racine.firstChild();
    while(!noeud.isNull())
    {
        licence = noeud.toElement();
        if (licence.tagName() == "licence" && licence.attribute("nom")==nom)
        {
            textAdd->setPlainText(licence.text());
        }
        noeud = noeud.nextSibling();
    }
}
