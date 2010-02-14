/*
 * Win.cpp
 * This file is part of BLoQ (Logram)
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * BLoQ (Logram) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * BLoQ (Logram) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BLoQ (Logram); if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <QCloseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QToolBar>
#include <QDir>
#include <QDateTime>
#include <QInputDialog>

#include <Win.h>

#include <LIconLoader.h>
#include <LDirTree.h>
#include <LDirView.h>
#include <LDirActions.h>

Win::Win(QString path) : QMainWindow()
{
        //Charger la configuration
        cfg = new LConfig("BLoQ", this);
        restoreState();

        //Charger les icônes
        iloader = new LIconLoader(QString(), this);

        iconDir = iloader->icon("folder");

        setWindowIcon(QIcon(iloader->icon("file-manager")));

        //Créer les menus
        createMenus();

        //Créer la barre d'outils
        createTools();

        //Créer la LDirBar
        ldbar = new LDirBar();
        addToolBar(ldbar);

        //Créer l'espace de travail
        tabs = new QTabWidget(this);

#if QT_VERSION >= 0x040500
        tabs->setTabsClosable(true);
        tabs->setMovable(true);
        connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
#endif

        connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
        setCentralWidget(tabs);

        //Créer l'arbre
        trees = new QDockWidget(tr("Arborescence"), this);
        addDockWidget(Qt::LeftDockWidgetArea, trees);

        treesstack = new QStackedWidget(this);
        trees->setWidget(treesstack);
        
        //Créer les actions
        actions = new QDockWidget(tr("Actions"), this);
        addDockWidget(Qt::LeftDockWidgetArea, actions);
        
        actionsstack = new QStackedWidget(this);
        actions->setWidget(actionsstack);

        //Créer la zone des informations
        infos = new QDockWidget(tr("Informations"), this);
        addDockWidget(Qt::BottomDockWidgetArea, infos);

        lblInfos = new QLabel(this);
        infos->setWidget(lblInfos);

        //Afficher le dossier courant
        if (path.isEmpty())
                addTab(QDir::currentPath());
        else addTab(path);
}

void Win::addTab(QString path)
{
        LDirView *dv    = new LDirView(path, true, mshowHidden->isChecked(), this);
        LDirTree *dt    = new LDirTree(dv, this);
        LDirActions *da = new LDirActions(dv, QBoxLayout::TopToBottom, this);

        int sz = cfg->value("IconSize", 48).toInt();

        dv->setItemSize(QSize(sz, sz));

        QStringList parts = path.split('/');
        QString     part  = parts.at(parts.count() - 1);

        int tb = tabs->addTab(dv, iconDir, part);

        treesstack->addWidget(dt);
        actionsstack->addWidget(da);

        //Connecter les signaux
        connect(dv, SIGNAL(dirChanged(const QString &)), this, SLOT(dirChanged(const QString &)));
        connect(dv, SIGNAL(itemClicked(const QModelIndex &)), this, SLOT(itemClicked(const QModelIndex &)));

        //Appeler un changement de tab
        tabs->setCurrentIndex(tb);
        
                
        //Afficher les informations sur le dossier courant
        displayInfos(dv->model()->itemUrlInfo(dv->view()->rootIndex()));
}

void Win::restoreState()
{
        int x, y, w, h;

        cfg->beginGroup("Window");

        x = cfg->value("Left", -1).toInt();
        y = cfg->value("Top", -1).toInt();
        w = cfg->value("Width", 640).toInt();
        h = cfg->value("Height", 480).toInt();

        if ((x != -1) && (y != -1))
        {
                move(x, y);
        }

        cfg->endGroup();

        resize(w, h);
}

void Win::saveState()
{
        cfg->beginGroup("Window");

        cfg->setValue("Left", x());
        cfg->setValue("Top", y());
        cfg->setValue("Width", width());
        cfg->setValue("Height", height());

        cfg->endGroup();
}

void Win::createTools()
{
        QToolBar *bar = new QToolBar(this);

        mback = bar->addAction(iloader->icon("go-previous"), tr("Précédente"), this, SLOT(back()));
        mnext = bar->addAction(iloader->icon("go-next"), tr("Suivante"), this, SLOT(next()));
        mup   = bar->addAction(iloader->icon("go-up"), tr("Remonter"), this, SLOT(cdup()));
        bar->addSeparator();
        bar->addAction(iloader->icon("go-home"), tr("Home"), this, SLOT(cdhome()));

        mback->setEnabled(false);
        mnext->setEnabled(false);

        addToolBar(bar);
}

void Win::createMenus()
{
        QMenu *fichier   = menuBar()->addMenu(tr("&Fichier"));
        QMenu *edition   = menuBar()->addMenu(tr("&Édition"));
        QMenu *affichage = menuBar()->addMenu(tr("&Affichage"));

        mnuBookmarks = menuBar()->addMenu(tr("&Signets"));
        QMenu *aide = menuBar()->addMenu(tr("&?"));

        // Menu fichier
        fichier->addAction(iloader->icon("document-open"), tr("&Ouvrir un emplacement"), this, SLOT(open(bool)), QKeySequence(tr("Ctrl+O", "Ouvrir un emplacement")));
        fichier->addSeparator();
        fichier->addAction(iloader->icon("folder-new"), tr("&Nouvel onglet"), this, SLOT(newTab(bool)), QKeySequence(tr("Ctrl+T", "Nouvel onglet")));
        fichier->addAction(iloader->icon("window-new"), tr("Nouvelle &fenêtre"), this, SLOT(newWin(bool)), QKeySequence(tr("Alt+Ctrl+F", "Nouvelle fenêtre")));
        fichier->addSeparator();
        fichier->addAction(iloader->icon("gtk-quit"), tr("&Quitter"), this, SLOT(quit(bool)), QKeySequence(tr("Ctrl+Q", "Quitter")));

        // Menu édition
        edition->addAction(iloader->icon("edit-copy"), tr("&Copier"), this, SLOT(copy(bool)), QKeySequence(tr("Ctrl+C", "Copier")));
        edition->addAction(iloader->icon("edit-paste"), tr("C&oller"), this, SLOT(paste(bool)), QKeySequence(tr("Ctrl+V", "Coller")));

        // Menu affichage
        affichage->addAction(tr("Mode &icônes"), this, SLOT(toggleIcon(bool)));
        affichage->addAction(tr("Mode &liste"), this, SLOT(toggleList(bool)));
        affichage->addAction(tr("Mode &détails"), this, SLOT(toggleDetails(bool)));
        affichage->addAction(tr("Mode a&rbre"), this, SLOT(toggleTree(bool)));
        affichage->addSeparator();
        mshowHidden = affichage->addAction(tr("&Afficher les fichiers cachés"), this, SLOT(showHidden(bool)), QKeySequence(tr("Ctrl+H", "Afficher les fichiers cachés")));
        mshowHidden->setCheckable(true);

        // Menu signets
        updateBookmarks(mnuBookmarks);

        // Menu aide
        aide->addAction(iloader->icon("help-about"), tr("À propos de BLoQ"), this, SLOT(aboutBLoQ(bool)));
        aide->addAction(iloader->icon("help-about"), tr("À propos de Qt"), qApp, SLOT(aboutQt()));
}

void Win::updateBookmarks(QMenu *menu)
{
        Signet *sign;

        LConfig set("Bookmarks");

        //Effacer le menu et la liste des signets
        foreach(QAction * act, signets.keys())
        {
                delete signets.take(act);               //Suppression du signet
        }

        menu->clear();

        //Lire les entrées des signets
        foreach(QString grp, set.childGroups())
        {
                set.beginGroup(grp);

                sign        = new Signet;
                sign->title = set.value("Title").toString();
                sign->url   = set.value("Url").toString();

                QAction *act = menu->addAction(iconDir, sign->title, this, SLOT(bookmarkClicked(bool)));

                signets.insert(act, sign);

                set.endGroup();
        }

        //Ajouter les entrées manquantes
        menu->addSeparator();
        menu->addAction(tr("Dossier actuel comme nouveau signet"), this, SLOT(addBookmark(bool)), QKeySequence(tr("Ctrl+B", "Nouveau signet")));
}

LDirView *Win::currentDV()
{
        return static_cast<LDirView *>(tabs->currentWidget());
}

void Win::updateUndoButtons()
{
        //Mettre à jour l'état des boutons de retour en fonction de la possibilité d'annuler/refaire
        LDirView *dv = currentDV();

        mback->setEnabled(dv->undoStack()->canUndo());
        mnext->setEnabled(dv->undoStack()->canRedo());
}

/*
        Gestion des événements
 */

void Win::closeTab(int index)
{
        //Il ne peut pas ne plus y avoir d'onglets
        if (tabs->count() == 1)
        {
                return;
        }

        LDirTree *tree = static_cast<LDirTree *>(treesstack->widget(index));
        LDirView *dv   = static_cast<LDirView *>(tabs->widget(index));

        treesstack->removeWidget(tree);
        tabs->removeTab(index);

        delete dv;
        delete tree;
}

void Win::changeTab(int index)
{
        if (index == -1)
        {
                return;
        }

        treesstack->setCurrentIndex(index);
        actionsstack->setCurrentIndex(index);

        //Adapter le LDirBar
        LDirView *dv = static_cast<LDirView *>(tabs->widget(index));
        ldbar->setView(dv);

        //Changer le titre de la fenêtre
        setWindowTitle(tabs->tabText(index) + " - BLoQ");

        updateUndoButtons();
}

void Win::back()
{
        //Retour
        currentDV()->undoStack()->undo();

        updateUndoButtons();
}

void Win::next()
{
        currentDV()->undoStack()->redo();

        updateUndoButtons();
}

void Win::cdup()
{
        LDirView *dv = currentDV();

        QStringList parts = dv->dir().split('/');

        parts.removeAt(parts.count() - 1);                //Retirer le dernier élément
        dv->setDir(parts.join("/"));                      //On a retiré le dossier, en envoie
}

void Win::cdhome()
{
        LDirView *dv = currentDV();

        dv->setDir(QDir::homePath());
}

void Win::dirChanged(const QString& dir)
{
        //Adapter le titre du tab
        LDirView *dv   = static_cast<LDirView *>(sender());
        int      index = tabs->indexOf(dv);

        QStringList parts = dir.split('/');

        tabs->setTabText(index, parts.at(parts.count() - 1));

        //Changer le titre de la fenêtre (si tab actif)
        if (index == tabs->currentIndex())
        {
                setWindowTitle(tabs->tabText(index) + " - BLoQ");
        }

        updateUndoButtons();
}

void Win::displayInfos(QUrlInfo *info)
{
        QString str;

        //Titre
        str = "<h2>" + info->name() + "</h2>";

        //Taille
        quint64 sz, msz;
        QString suffix;
        msz = info->size();

        if (msz <= 1 << 10)
        {
                sz     = msz;
                suffix = " o";
        }
        else if (msz <= 1 << 20)
        {
                sz     = msz >> 10;
                suffix = " Kio";
        }
        else if (msz <= 1 << 30)
        {
                sz     = msz >> 20;
                suffix = " Mio";
        }
        else if (msz <= (quint64)1 << 40)
        {
                sz     = msz >> 30;
                suffix = " Gio";
        }
        else
        {
                sz     = msz >> 40;
                suffix = " Tio";
        }
        str += "Taille : <strong>" + QString::number(sz) + suffix + "</strong><br />";

        //Type
        str += tr("Type : <strong>");
        if (info->isDir())
        {
                str += tr("Dossier");
        }
        else
        {
                str += tr("Fichier");
        }
        if (info->isSymLink())
        {
                str += tr(", Lien symbolique");
        }
        if (info->isExecutable())
        {
                str += tr(", Éxécutable");
        }
        str += "</strong><br />";

        //Propriétaire et groupe
        str += tr("Propriétaire : <strong>") + info->owner() + tr("</strong>, groupe : <strong>") + info->group() + "</strong><br />";

        //Permissions
        QString triade;
        int     perms = info->permissions();

        if (perms & QFile::ReadOwner)
        {
                triade += "r";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::WriteOwner)
        {
                triade += "w";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::ExeOwner)
        {
                triade += "x";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::ReadGroup)
        {
                triade += "r";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::WriteGroup)
        {
                triade += "w";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::ExeGroup)
        {
                triade += "x";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::ReadOther)
        {
                triade += "r";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::WriteOther)
        {
                triade += "w";
        }
        else
        {
                triade += "-";
        }
        if (perms & QFile::ExeOther)
        {
                triade += "x";
        }
        else
        {
                triade += "-";
        }

        str += tr("Permissions : <strong>") + triade + "</strong><br />";

        //Date de dernière modification
        str += tr("Dernière modification : <strong>") + info->lastModified().toString(Qt::DefaultLocaleLongDate) + "</strong><br />";


        lblInfos->setText(str);
}

void Win::itemClicked(const QModelIndex& index)
{
        //Actualiser les informations
        LDirView *dv = static_cast<LDirView *>(sender());

        QUrlInfo *info = dv->model()->itemUrlInfo(index);

        displayInfos(info);
}

void Win::newWin(bool)
{
        //Ouvrir simplement une autre fenêtre
        LDirView *dv   = currentDV();
        Win      *neww = new Win(dv->dir());

        neww->show();
}

void Win::open(bool)
{
        bool    ok;
        QString path = QInputDialog::getText(this, tr("Ouvrir un emplacement"), tr("Tapez l'adresse de l'emplacement à ouvrir"), QLineEdit::Normal, QDir::homePath(), &ok);

        if (ok)
        {
                addTab(path);
        }
}

void Win::newTab(bool)
{
        //Prendre l'url courant
        LDirView *dv = currentDV();

        addTab(dv->dir());
}

void Win::quit(bool)
{
        //Fermer la fenêtre
        close();
}

void Win::copy(bool)
{
        //Copier un fichier
        LDirView *dv = currentDV();

        dv->copy();
}

void Win::paste(bool)
{
        //Coller un fichier
        LDirView *dv = currentDV();

        dv->paste();
}

void Win::showHidden(bool toggled)
{
        LDirView *dv = currentDV();

        dv->setShowHidden(toggled);
}

void Win::aboutBLoQ(bool)
{
        QString str;

        str  = tr("<h1>BLoQ : Explorateur de fichiers de Logram</h1>");
        str += tr("<p>BLoQ est l'explorateur de fichiers de Logram, conçu pour être pratique et léger</p>");
        str += tr("<p>BLoQ est distribué sous les termes de la licence GPL</p>");
        str += tr("<p><strong>Auteur :</strong> steckdenis &lt;steckdenis@yahoo.fr&gt;</p>");
        str += tr("<p>Reporter les bugs <a href=\"http://redmine.logram-project.org/projects/steckdenis\">sur le Bug Tracker</a></p>");

        QMessageBox::information(this, tr("À propos de BLoQ"), str);
}

void Win::bookmarkClicked(bool)
{
        //Afficher la page actuelle des bookmarks
        QAction *act = static_cast<QAction *>(sender());

        Signet *sign = signets.value(act);

        LDirView *dv = currentDV();

        dv->setDir(sign->url);
}

void Win::addBookmark(bool)
{
        //Ajouter dans les bookmarks le répertoire actuel
        LDirView *dv = currentDV();

        QString     dir    = dv->dir();
        QStringList parts  = dir.split('/');
        QString     folder = parts.at(parts.count() - 1);

        LConfig set("Bookmarks");

        set.beginGroup(folder.replace("/", "").replace(".", "") + QString::number(parts.count()) + QString::number(dir.length()));
        set.setValue("Title", folder);
        set.setValue("Url", dir);
        set.endGroup();

        set.sync();

        updateBookmarks(mnuBookmarks);
}

void Win::toggleIcon(bool)
{
        currentDV()->setViewMode(LDirView::IconMode);
}

void Win::toggleList(bool)
{
        currentDV()->setViewMode(LDirView::ListMode);
}

void Win::toggleDetails(bool)
{
        currentDV()->setViewMode(LDirView::DetailsMode);
}

void Win::toggleTree(bool)
{
        currentDV()->setViewMode(LDirView::TreeMode);
}
