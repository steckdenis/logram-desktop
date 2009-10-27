/*Copyright (c) 2008, Cleriot Simon
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "mainwindow.h"

Text::Text(QWidget *parent) : QsciScintilla(parent)
{

}

void Text::keyPressEvent(QKeyEvent *e)
{
    if ( !e->isAutoRepeat() && e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_Space )
    {
        autoCompleteFromAPIs();
        return;
    }
    QsciScintilla::keyPressEvent(e);
}

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    setupUi(this); //on charge le.ui

    textEdit=new Text();
    textEdit->setEnabled(false); //on désactive le TextEdit

    QList<int> sizes; //taille des composants de splitter
    sizes.append(150);
    sizes.append(width()-150);
    splitter->addWidget(textEdit); //on ajoute le textEdit au splitter
    splitter->setSizes(sizes);
    sizes.clear();

    wizard=new Wizard(this); //on créé le dialogue de création de projet
    wizard->setModal(true);

    addtextDialog=new Addtext(this); //boite de dialogue d'ajout de texte (licence..)

    compilateur=new Compilateur; //on créé le compilateur

    /**On configure le textEdit**/
    textEdit->setMarginLineNumbers(1,true); //affichage de la marge
    //textEdit->setAutoCompletionFillupsEnabled(true);
    textEdit->setMarginWidth(1,"-------"); //taille de la marge
    textEdit->setAutoIndent(true); //on active l'auto-indentation
    //textEdit->setAutoCompletionThreshold(0);
    //textEdit->setAutoCompletionSource(QsciScintilla::AcsAll);


    /*on met en place le colorateur et l'auto-completion*/
    lexerCPP = new QsciLexerCPP;
    api=new QsciAPIs(lexerCPP);
    api->load("config/qt-4.4.0.api");
    api->prepare();
    lexerCPP->setAPIs(api);
    textEdit->setLexer(lexerCPP);



    /*On charge les plugins*/
    loadPlugins();


    /***On connecte les différents menus**/
    connect(actionQuitter,SIGNAL(triggered()),this,SLOT(close()));
    connect(actionSave,SIGNAL(triggered()),this,SLOT(save()));
    connect(actionOuvrir,SIGNAL(triggered()),this,SLOT(load()));
    connect(actionCompil,SIGNAL(triggered()),this,SLOT(compile()));
    connect(actionClean,SIGNAL(triggered()),this,SLOT(clean()));
    connect(actionCreate,SIGNAL(triggered()),this,SLOT(launchWizard()));
    connect(actionFind,SIGNAL(triggered()),this,SLOT(dialFind()));
    connect(actionUndo,SIGNAL(triggered()),this,SLOT(undo()));
    connect(actionRedo,SIGNAL(triggered()),this,SLOT(redo()));
    connect(actionQt,SIGNAL(triggered()),this,SLOT(aboutQt()));
    connect(actionApropos,SIGNAL(triggered()),this,SLOT(about()));
    connect(actionLicence,SIGNAL(triggered()),this,SLOT(licence()));
    connect(actionAjout,SIGNAL(triggered()),this,SLOT(dialAjout()));


    connect(addtextDialog,SIGNAL(textToAdd(QString)),this,SLOT(addText(QString))); //on doit ajouter la licence

    connect(textEdit,SIGNAL(textChanged()),this,SLOT(setModified())); //le contenu du textEdit a changé

    connect(treeView,SIGNAL(doubleClicked ( const QModelIndex &)),this,SLOT(showFile())); //on veut afficher un fichier

    /*Pour le compilateur*/
    connect(compilateur,SIGNAL(readyReadStandardOutput ()),this,SLOT(updateOutputText()));
    connect(compilateur,SIGNAL(readyReadStandardError()),this,SLOT(updateOutputError()));
    connect(compilateur,SIGNAL(compilationFinished()),this,SLOT(endCompilation()));
    connect(compilateur,SIGNAL(cleanFinished()),this,SLOT(endClean()));

    /*on configure le treeView*/
    treeView->setHeaderHidden(true);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    toSave=false;
}

MainWindow::~MainWindow()
{

}

void MainWindow::loadPlugins() //Chargement des plugins
{
    QDir pluginDir(QApplication::applicationDirPath());

    #if defined(Q_OS_WIN)
    if (pluginDir.dirName().toLower() == "debug"
            || pluginDir.dirName().toLower() == "release")
        pluginDir.cdUp();
    #elif defined(Q_OS_MAC)
    if (pluginDir.dirName() == "MacOS") {
        pluginDir.cdUp();
        pluginDir.cdUp();
        pluginDir.cdUp();
    }
    #endif

    if(!pluginDir.cd("plugins"))
        return;

    menuPlugin->clear();

    foreach(QString filename,pluginDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginDir.absoluteFilePath(filename));
        if(PluginInterface *interface=qobject_cast<PluginInterface *>(loader.instance())){
            QString nom=interface->pluginName();
            QAction *action=new QAction(nom,this);
            menuPlugin->addAction(action);
            interface->initialize(textEdit,this);
            connect(action,SIGNAL(triggered()),interface,SLOT(Action()));
        }
    }
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    return QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) //Fermeture de la fenêtre
{
    if(toSave) //si on doit sauvegarder
    {
        int sauvegarder = QMessageBox::question(this,tr("Attention"),tr("Le projet n'a pas été sauvegardé, voulez-vous le sauvegarder ?"),QMessageBox::Yes | QMessageBox::No); //On demande si on veut sauvegarder

        if(sauvegarder==QMessageBox::Yes)
            save();
    }

    return QMainWindow::closeEvent(event); //on ferme la fenêtre proprement
}

void MainWindow::save() //sauvegarde du projet
{
    QFile file;

    QMapIterator<QString,QString> i(Fichiers);
    while(i.hasNext()) //on parcourt les fichiers
    {
        i.next();

        if(!i.key().endsWith(".ui")) //si c'est un fichier source ou .pro
        {
            file.setFileName(DossierProjet+i.key());

            if(!file.open(QIODevice::WriteOnly))
            {
                cerr << "Erreur dans l'ouverture du fichier en ecriture." << endl;
                return;
            }

            QTextStream out(&file);
            QString contenu=i.value();
            out << contenu << endl; //on sauvegarde

            file.close();
        }
    }

    status->showMessage(tr("Projet enregistré"),5000);
    toSave=false; //plus besoin de sauvegarder
}

void MainWindow::load() //chargement d'un projet
{
    fileName=QFileDialog::getOpenFileName(this,tr("Ouvrir un fichier .pro"),QDir::homePath(),tr("Projet Qt (*.pro)"));

    if(fileName!=NULL)
    {
        QFile file(fileName);

        if(!file.open(QIODevice::ReadOnly))
        {
            cerr << "Erreur dans l'ouverture du fichier en lecture." << endl;
            return;
        }

        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString contenu;
        contenu=in.readAll();

        Fichiers.clear();
        listeFichiers.clear();

        Fichiers.insert(fileName.section("/",-1),contenu);

        QRegExp regex("(\\S+\\.cpp|\\S+\\.h|\\S+\\.ui)+"); //on récupère les fichiers ui et cpp et h
        int pos=0,count=0;

        while((pos=regex.indexIn(contenu,pos)) != -1)
        {
            listeFichiers << regex.cap(1);
            pos+=regex.matchedLength();
            count++;
        }

        QStandardItemModel *Model=new QStandardItemModel;
        QStandardItem *Item1 = new QStandardItem(fileName.section("/",-1));

        listeFichiers.sort();

        file.close();

        /*****On récupère le dossier du fichier .pro en enlevant tout ce qui se trouve après le dernier /  ****/
        DossierProjet=fileName;
        int size=DossierProjet.size();
        int change=0;
        for(int i = size; i; i--)
        {
            if(DossierProjet[i] == '/')
            {
                break;
            }
            if(change==0)
            {
                DossierProjet[i] = ' ';
            }
        }
        DossierProjet=DossierProjet.trimmed(); //on enlève les espaces

        /****On lit tous les fichiers, et on les ajoutes au QTreeView****/
        for (int i = 0; i < count; ++i)
        {
            QStandardItem *item = new QStandardItem(QString(listeFichiers.at(i)));
            Item1->appendRow(item);

            file.setFileName(DossierProjet+listeFichiers.at(i));

            if(!file.open(QIODevice::ReadOnly))
            {
                cerr << "Erreur dans l'ouverture du fichier en lecture." << endl;
                return;
            }

            contenu=file.readAll();
            Fichiers.insert(listeFichiers.at(i),contenu);
            file.close();
        }
        Model->setItem(0,0,Item1);
        treeView->setModel(Model);

        treeView->expandAll();


        /**on active tous les menus utiles**/
        menuProjet->setEnabled(true);
        actionCompil->setEnabled(true);
        actionClean->setEnabled(true);
        actionSave->setEnabled(true);
        actionAjout->setEnabled(true);
        toSave=false;
    }
}

void MainWindow::showFile() //On affiche un fichier du projet
{
    QModelIndex index=treeView->currentIndex();

    fileName=index.data().toString();

    /****Si c'est un fichier ui, on lance qt4-designer avec le nom du fichier en argument ;) ****/
    if(fileName.endsWith(".ui"))
    {
        QDir::setCurrent(DossierProjet);
        QProcess::startDetached("designer-qt4",QStringList() << fileName);
    }
    else
    {
        QString contenu=Fichiers[fileName]; //le contenu du fichiers en mémoires (donc avec les modifs ^^ )

        bool toSave2=toSave;
        textEdit->setText(contenu);
        if(!toSave2)
            toSave=false;

        menuEdit->setEnabled(true);
        textEdit->setEnabled(true);
        actionFind->setEnabled(true);
    }
}

void MainWindow::launchWizard() //on affiche la fenêtre de création de projet
{
    wizard->show();
}

void MainWindow::dialFind() ///Boite de dialogue de rechercher/remplacer => ça marche !! ^^
{
    Seek *seekDial=new Seek(this); //on créé une boite
    connect(seekDial,SIGNAL(seek(QString)),this,SLOT(seek(QString)));
    connect(seekDial,SIGNAL(replace(QString)),this,SLOT(replace(QString)));
    seekDial->show();
}

/** le projet a été modifié**/
void MainWindow::setModified()
{
    Fichiers[fileName]=textEdit->text();
    toSave=true;
}


/**on fait une recherche dans le fichier en cours, et QScintilla la met en surbrillance :-° **/
void MainWindow::seek(QString recherche)
{
    if(!textEdit->findFirst(recherche,false,false,false,false,-1,-1,true))
        textEdit->findFirst(recherche,false,false,false,false,1,1,true);
}

/**on remplace : QScintilla fait encore une fois tout ^^ **/
void MainWindow::replace(QString string)
{
    textEdit->replace(string);
}

/**annuler une action**/
void MainWindow::undo()
{
    textEdit->undo();
}
/**refaire une action**/
void MainWindow::redo()
{
    textEdit->redo();
}








/************************/
/*****La compilation*****/
/************************/
void MainWindow::compile() //on lance la compilation
{
    save();
    errorEdit->setPlainText("");
    compilateur->compile(DossierProjet); //compilation dans le dossier du projet
}

void MainWindow::updateOutputText() //on met à jour la sortie de qmake et make
{
    QByteArray newData = compilateur->readAllStandardOutput();
    QString text = outputEdit->toPlainText() + QString::fromLocal8Bit(newData);
    outputEdit->setPlainText(text);

    sorties->setCurrentIndex(0);

    outputEdit->verticalScrollBar()->setValue( outputEdit->verticalScrollBar()->maximum() );
}

void MainWindow::updateOutputError() //on met à jour la sortie d'erreur
{
    QByteArray newData = compilateur->readAllStandardError();
    QString text = errorEdit->toPlainText() + QString::fromLocal8Bit(newData);
    errorEdit->setPlainText(text);

    sorties->setCurrentIndex(1);

    errorEdit->verticalScrollBar()->setValue( errorEdit->verticalScrollBar()->maximum() );
}

void MainWindow::endCompilation() //la compilation est terminée
{
    QString text = outputEdit->toPlainText() + tr("\nCompilation terminée \n\n");
    outputEdit->setPlainText(text);
    outputEdit->verticalScrollBar()->setValue( outputEdit->verticalScrollBar()->maximum() );
}

void MainWindow::clean() //on lance le nettoyage
{
    compilateur->clean(DossierProjet);
}

void MainWindow::endClean() //le nettoyage est terminé
{
    QString text = outputEdit->toPlainText() + tr("\nNetoyage terminée \n\n");
    outputEdit->setPlainText(text);

    outputEdit->verticalScrollBar()->setValue( outputEdit->verticalScrollBar()->maximum() );
}



void MainWindow::updateCode() //on met à jour l'affichage du code
{
    textEdit->setText(Fichiers[fileName]);
}


void MainWindow::dialAjout() //boite de dialogue ajout de licence
{
    addtextDialog->show();
}

void MainWindow::addText(QString txt) //on ajout du txt à tous les fichiers du projet
{
    QMapIterator<QString,QString> i(Fichiers);
    while(i.hasNext())
    {
        i.next();
        if(!i.key().endsWith(".pro") && !i.key().endsWith(".ui"))
            Fichiers[i.key()]="/*"+txt+"*/\n\n"+i.value();
    }
    toSave=true;

    updateCode();

}








/*******************************/
/****Les boites de dialogues****/
/*******************************/

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this,tr("Bibliothèque Qt"));
}

void MainWindow::about()
{
    QMessageBox::about(this,tr("A propos"),tr("<h1><i>QStudio</i></h1><br />Ce programme a été réalisé par Cleriot Simon &lt;<a href='mailto:malgon33@gmail.com'>malgon33@gmail.com</a>&gt;<br /><br />Vous trouverez la licence dans le fichier LICENCE.txt contenu dans le dossier du programme<br /><br /><u>Mises à jours</u> (pour plus de renseignements voir CHANGELOG.txt)<ul><li><u>Vendredi 12/12/2008 [19h]</u> : <b>Version 0.1</b></li><li><u>Vendredi 12/12/2008 [21h]</u> : <b>Version 0.1.10</b></li><li><u>Samedi 13/12/2008 [20h20]</u> : <b>Version 0.2</b></li><li><u>Dimanche 14/12/2008 [21h]</u> : <b>Version 0.3</b></li><li><u>Jeudi 18/12/2008 [18h]</u> : <b>Version 0.4</b></li></ul>"));
}

void MainWindow::licence()
{
    QMessageBox::about(this,tr("Licence de QStudio"),"Copyright (c) 2008, Cleriot Simon<br />"
"All rights reserved.<br />"
"<br />"
"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:<br />"
"<br />"
"    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.<br />"
"    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.<br />"
"<br />"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.<br />");
}
