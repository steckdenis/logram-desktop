/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "wizard.h"

Wizard::Wizard(QWidget *parent, Qt::WindowFlags flags) : QDialog(parent, flags)
{
    setupUi(this);

    connect(buttonCreate,SIGNAL(clicked()),this,SLOT(createProject()));
    connect(checkCreate,SIGNAL(clicked(bool)),this,SLOT(activeCreate(bool)));
}

Wizard::~Wizard()
{
    //
}

/*Création d'un projet*/
void Wizard::createProject()
{
    QString nom=nameEdit->text().trimmed();
    QString chemin=cheminEdit->text().trimmed();

    if(nom=="")
    {
        QMessageBox::critical(this,tr("Erreur"),tr("Vous n'avez pas remplis le nom du projet. Création annulée"));
        return;
    }

    if(chemin=="")
    {
        QMessageBox::critical(this,tr("Erreur"),tr("Vous n'avez pas remplis le chemin du projet. Création annulée"));
        return;
    }

    if(!chemin.endsWith("/"))
        chemin.append("/");

    QString nom1="src/"+fenEdit->text();

    QDir dir(chemin);

    if(!QDir(chemin+"src/").exists())
        dir.mkdir("src/");

    QFile file(chemin+nom+".pro");

    if(file.exists())
    {
        QMessageBox::critical(this,tr("Erreur"),tr("Le fichier %1 existe déjà, il ne sera pas recréé.").arg(file.fileName()));
    }
    else if(!file.open(QIODevice::WriteOnly))
    {
        //
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << tr("## Projet Qt avec QStudio ##\n\n") << "TARGET="+nom+"\n" << "TEMPLATE=app\n";

    if(!checkCreate->isChecked()) //si on ne veut pas créér de fenêtre
    {
        out << "SOURCES += \n" << "HEADERS += \n" << endl;

        file.close();
    }
    else //Si on veut créér une fenêtre
    {
        if(!QDir(chemin+"ui/").exists())
            dir.mkdir("ui/");

        file.close();
        out << "SOURCES += "+nom1+".cpp\n" << "HEADERS += "+nom1+".h\n" << endl;
        file.close();
        file.setFileName(chemin+nom1+".cpp");

        if(file.exists())
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Le fichier %1 existe déjà, il ne sera pas recréé.").arg(file.fileName()));
        }
        else if(!file.open(QIODevice::WriteOnly))
        {
            //
        }

        out.setDevice(&file);
        out << "#include \""+nom1.section("/",-1)+".h\"\n" << endl;

        file.close();

        QString nomFen=fenEdit->text().trimmed();

        if(nomFen == "")
            nomFen="MainWindow";

        file.setFileName(chemin+"ui/"+nomFen+".ui");

        if(file.exists())
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Le fichier %1 existe déjà, il ne sera pas recréé.").arg(file.fileName()));
        }
        else if(!file.open(QIODevice::WriteOnly))
        {
            //
        }

        /*On remplis le 1er fichier .ui avec le code d'une mainwindow par défaut*/
        out.setDevice(&file);
        out << "<ui version=\"4.0\" >\n"
" <class>MainWindow</class>\n"
" <widget class=\"QMainWindow\" name=\"MainWindow\" >\n"
"  <property name=\"geometry\" >\n"
"   <rect>\n"
"    <x>0</x>\n"
"    <y>0</y>\n"
"    <width>800</width>\n"
"    <height>600</height>\n"
"   </rect>\n"
"  </property>\n"
"  <property name=\"windowTitle\" >\n"
"   <string>MainWindow</string>\n"
"  </property>\n"
"  <widget class=\"QWidget\" name=\"centralwidget\" />\n"
"  <widget class=\"QMenuBar\" name=\"menubar\" >\n"
"   <property name=\"geometry\" >\n"
"    <rect>\n"
"     <x>0</x>\n"
"     <y>0</y>\n"
"     <width>800</width>\n"
"     <height>26</height>\n"
"    </rect>\n"
"   </property>\n"
"  </widget>\n"
"  <widget class=\"QStatusBar\" name=\"statusbar\" />\n"
" </widget>\n"
" <resources/>\n"
" <connections/>\n"
"</ui>" << endl;

        file.close();
        file.setFileName(chemin+nom1+".h");

        if(file.exists())
        {
            QMessageBox::critical(this,tr("Erreur"),tr("Le fichier %1 existe déjà, il ne sera pas recréé.").arg(file.fileName()));
        }
            else if(!file.open(QIODevice::WriteOnly))
        {
            //
        }

        out.setDevice(&file);
        out << "#ifndef "+nom1.section("/",-1).toUpper()+"_H\n" << "#define "+nom1.section("/",-1).toUpper()+"_H\n\n\n" << "#endif //"+nom1.section("/",-1).toUpper()+"_H" << endl;

    }

    hide();
}

void Wizard::activeCreate(bool checked)
{
    groupBox->setEnabled(checked);
}
