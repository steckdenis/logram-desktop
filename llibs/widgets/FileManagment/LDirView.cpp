/*
 * LDirView.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher <steckdenis@yahoo.fr>
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

#include <QtDebug>
#include <QPluginLoader>
#include <QStringList>
#include <QAction>
#include <QContextMenuEvent>
#include <QModelIndexList>
#include <QItemSelectionModel>
#include <QCursor>
#include <QDesktopServices>
#include <QClipboard>
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QMouseEvent>

#include "../plugins/actions/include/IActionPlugin.h"

#include <LConfig.h>
#include <LDirView.h>

LDirView::LDirView(QString baseUrl, bool showFile, bool showHidden, QWidget *parent) : QWidget(parent)
{
        _parent     = parent;
        _view       = 0;
        _model      = 0;
        _showFiles  = showFile;
        _showHidden = showHidden;
        tree        = 0;
        list        = 0;
        undos       = new QUndoStack(this);
        isTree      = true; //On va changer à false juste après. Ca force la création du widget

        //Créer le menu contextuel
        contextMenu = new QMenu(this);
        buildMenu();

        //Créer le job LIO
        _job = new LIOJob(this);

        //Définir le dossier racine
        setDir(baseUrl);

        //Créer les controles, en changeant de mode
        setViewMode(IconMode);

        //Redéfinir le dossier racine
        setDir(baseUrl);

        //Lancer le timer (seulement fichier locaux)
        timerId = 0;
        if (_job->host() == "file://")
        {
                timerId = startTimer(4000);
        }
}

void LDirView::buildMenu()
{
        LConfig     cfgfile("Actions", this);
        QString     actions     = cfgfile.value("Actions").toString();
        QStringList actionslist = actions.split(" ");

        //Créer les éléments standards
        contextMenu->addAction(tr("Copier"), this, SLOT(copy()));
        contextMenu->addAction(tr("Coller"), this, SLOT(paste()));
        contextMenu->addAction(tr("Supprimer"), this, SLOT(remove()));

        contextMenu->addSeparator();

        //Charger les actions
        foreach(QString action, actionslist)
        {
                QPluginLoader loader(cfgfile.value(action + "/FileName").toString());
                QObject       *mplugin = loader.instance();
                IActionPlugin *plugin  = 0;

                if (mplugin)
                {
                        plugin = qobject_cast<IActionPlugin *>(mplugin);

                        plugin->init();

                        plugins.append(plugin);

                        //Vérifier que le plugin gère bien le menu contextuel
                        if ((plugin->info()->flags | IActionPlugin::ActionShowInMenu) != 0)
                        {
                                contextMenu->addAction(plugin->menuAction());
                        }
                }
        }

        //Ajouter l'option Propriété
        contextMenu->addSeparator();
        contextMenu->addAction(tr("Propriétés..."), this, SLOT(propertiesClicked()));
}

void LDirView::setDir(const QString& url, bool isUndoAction)
{
        //Pusher l'action de retour
        if ((!isUndoAction) && (!_url.isEmpty()) && (_url != url))
        {
                LDUndoAction *act = new LDUndoAction(this, url, _url);
                undos->push(act);
        }

        _url = url;

        //Définir l'index root
        if (url.isNull() || url.startsWith('/'))
        {
                if (!_job->host().startsWith("file://")) //file:/// par exemple pour le dossier /
                {
                        _job->setHost();                 //Root local
                        resetModel();
                }

                if (_view != 0)
                {
                        _view->setRootIndex(_model->indexForPath(url));
                }
                
                //Le timer doit démarrer
                if (timerId == 0)
                {
                        timerId = startTimer(4000);
                }
        }
        else
        {
                //Url
                QUrl murl(url);

                QString mhost;

                if (url.startsWith("file://"))
                {
                        mhost = "file://";
                }
                else
                {
                        mhost = murl.scheme() + "://" + murl.authority();
                }

                if (_job->host() != mhost)
                {
                        _job->setHost(mhost);
                        resetModel();
                }

                if (_view != 0)
                {
                        _view->setRootIndex(_model->indexForPath(murl.path()));
                }
                
                //On ne réactualise plus les fichiers
                if (timerId != 0)
                {
                        killTimer(timerId);
                        
                        timerId = 0;
                }
        }

        //Mettre à jour la liste des actions disponibles
        if (_view != 0)
        {
                viewClicked(_view->rootIndex());
        }

        emit dirChanged(url);
}

void LDirView::resetModel()
{
        //Réinitialiser le modèle, car on a changé quelque chose qui nécessite une réexploration
        if (_model != 0)
        {
                _model->refresh();
        }
        else
        {
                //Créer le modèle
                QString mdir("/");
                _model = new LDirModel(_job, mdir, _showFiles, _showHidden, this);
        }
}

QString LDirView::dir() const
{
        return _url;
}

void LDirView::setShowFiles(bool showFiles)
{
        _showFiles = showFiles;

        _model->setShowFiles(showFiles);

        _model->refreshItem(_view->rootIndex());
}

bool LDirView::showFiles() const
{
        return _showFiles;
}

void LDirView::setShowHidden(bool showHidden)
{
        _showHidden = showHidden;

        _model->setShowHidden(showHidden);

        _model->refreshItem(_view->rootIndex());
}

bool LDirView::showHidden() const
{
        return _showHidden;
}

void LDirView::setViewMode(ViewMode mode)
{
        bool        aistree    = isTree;
        bool        createView = false;
        QModelIndex aroot;

        if (_view != 0)
        {
                aroot = _view->rootIndex();
        }

        _viewMode = mode;

        if (_viewMode == IconMode)
        {
                isTree = false;
        }
        else if (_viewMode == ListMode)
        {
                isTree = false;
        }
        else if (_viewMode == DetailsMode)
        {
                isTree = true;
        }
        else if (_viewMode == TreeMode)
        {
                isTree = true;
        }

        if (isTree && !aistree)
        {
                //Passage de liste à arbre
                if (list != 0)
                {
                        delete list;
                }

                tree = new QTreeView(this);

                //La vue est l'arbre
                _view = tree;

                createView = true;

                tree->setWordWrap(true);
        }
        else if (!isTree && aistree)
        {
                //Passage d'arbre en liste
                if (tree != 0)
                {
                        delete tree;
                }

                list       = new QListView(this);
                _view      = list;
                createView = true;

                //A l'origine par Cleriot Simonon <malgon33@gmail.com>
                list->setSpacing(5);
                list->setMovement(QListView::Free);
                list->setResizeMode(QListView::Adjust);
                list->setWordWrap(true);
        }

        if (createView)
        {
                _view->setSelectionMode(QAbstractItemView::ContiguousSelection);

                _view->setDragEnabled(true);
                _view->viewport()->setAcceptDrops(true);
                _view->setDropIndicatorShown(true);
                _view->setDragDropMode(QAbstractItemView::DragDrop);

                _view->setContextMenuPolicy(Qt::CustomContextMenu);
                _view->setTextElideMode(Qt::ElideRight);

                _view->setModel(_model);
                _view->setEditTriggers(QAbstractItemView::SelectedClicked);

                _view->resize(size());
                
                //On a besoin des événements intimes de View
                _view->viewport()->installEventFilter(this);

                if (aroot.isValid())
                {
                        _view->setRootIndex(aroot);
                }

                //Connecter les signaux
                connect(_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewActivated(const QModelIndex &)));
                connect(_view, SIGNAL(clicked(const QModelIndex &)), this, SLOT(viewClicked(const QModelIndex &)));
                connect(_view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(viewContextMenu(const QPoint &)));
        }

        switch (_viewMode)
        {
                case ListMode:
                        list->setViewMode(QListView::ListMode);
                        break;

                case IconMode:
                        list->setViewMode(QListView::IconMode);
                        break;

                case DetailsMode:
                        tree->setItemsExpandable(false);
                        tree->setRootIsDecorated(false);
                        break;

                case TreeMode:
                        tree->setItemsExpandable(true);
                        tree->setRootIsDecorated(true);
                        break;
        }

        //Remettre à jour la taille des éléments
        setItemSize(itemSize());

        _view->show();
}

void LDirView::setItemSize(const QSize& size)
{
        _itemSize = size;

        //Mettre à jour le controle
        if (!isTree)
        {
                list->setIconSize(size);
                list->setGridSize(QSize(size.width() + 40, size.height() + 40));

                _model->setIconSize(size.width(), size.height());
        }
        else
        {
                _model->setIconSize(16, 16);
        }
}

QSize LDirView::itemSize() const
{
        return _itemSize;
}

LDirView::ViewMode LDirView::viewMode() const
{
        return _viewMode;
}

QUrl LDirView::filePath(const QModelIndex& index)
{
        return QUrl(_model->itemPath(index));
}

QAbstractItemView *LDirView::view()
{
        return _view;
}

LDirModel *LDirView::model()
{
        return _model;
}

LIOJob *LDirView::job()
{
        return _job;
}

QList<IActionPlugin *> *LDirView::actions()
{
        return &plugins;
}

/*
        Gestion des événements
 */

void LDirView::viewActivated(const QModelIndex& index)
{
        //Gérer l'ouverture de fichier/dossier
        bool cancel = false;

        //Savoir si c'est un dossier ou un fichier
        QUrlInfo *info;

        info = _model->itemUrlInfo(index);

        if (info->isDir())
        {
                emit openDir(_job->host() + _model->itemPath(index), &cancel);
        }
        else
        {
                emit openFile(_job->host() + _model->itemPath(index), &cancel);
        }

        if (cancel)
        {
                return;
        }

        //Ouvrir le fichier ou le dossier. Dans le cas d'un dossier, simplement le changer en répertoire affiché,
        //seulement si l'affichage n'est pas en arbre (puisque l'arbre affiche tout)
        if ((_viewMode != TreeMode) && info->isDir())
        {
                QString dir = _model->itemPath(index);
                setDir(_job->host() + dir);
        }

        if (!info->isDir())
        {
                //C'est un fichier, l'ouvrir
                if (info->name().endsWith(".desktop") && (_job->host() == "file://"))
                {
                        //C'est un .desktop, l'exécuter
                        QSettings set(_model->itemPath(index), QSettings::IniFormat);

                        QString exec = set.value("Desktop Entry/Exec").toString();

                        if (!exec.isEmpty())
                        {
                                QProcess::startDetached(exec.replace("%U", QString()));
                        }
                }
                else
                {
                        QDesktopServices::openUrl(QUrl(_job->localPath(_job->host() + _model->itemPath(index))));
                }
        }
}

void LDirView::viewClicked(const QModelIndex& index)
{
        //Egalement informer les plugins du changement d'index
        QList<QString>    urls;
        QList<QUrlInfo *> infos;

        if (!index.isValid())
        {
                return;
        }

        urls.append(_job->host() + _model->itemPath(index));
        infos.append(_model->itemUrlInfo(index));

        foreach(IActionPlugin * plugin, plugins)
        {
                plugin->filesClicked(urls, infos);
        }

        emit updateActions();
        emit itemClicked(index);
}

void LDirView::copy()
{
        QClipboard *brd = QApplication::clipboard();

        brd->setText(_job->host() + _model->itemPath(_view->currentIndex()));
}

void LDirView::paste()
{
        QClipboard  *brd = QApplication::clipboard();
        QModelIndexList selection = _view->selectionModel()->selectedIndexes();
        QModelIndex item;
        
        if (selection.count() == 0)
        {
                item = _view->rootIndex();        
        }
        else
        {
                item = selection.at(0);
        }

        QUrlInfo *info = _model->itemUrlInfo(item);

        if (!info->isDir())
        {
                item = _model->parent(item);
        }

        _model->pasteFile(brd->text(), item);
        _model->refreshItem(item);
}

void LDirView::remove()
{
        QModelIndex item   = _view->selectionModel()->selectedIndexes()[0];
        QModelIndex parent = _model->parent(item);

        //Supprimer le fichier/dossier de l'index
        QUrlInfo *info = _model->itemUrlInfo(item);
        QString  path  = _model->itemPath(item);

        qDebug() << path;

        QStringList elements = path.split('/', QString::SkipEmptyParts);
        QString     fname    = elements.at(elements.count() - 1);
        QString     fpath;

        //Reconstituer le chemin
        for (int i = 0; i < elements.count() - 1; i++)
        {
                fpath += "/" + elements.at(i);
        }

        _job->cd(fpath);
        _job->rm(fname, info->isDir());

        //Mettre à jour l'item du dessus
        _model->refreshItem(parent);
}

void LDirView::propertiesClicked()
{
}

void LDirView::viewContextMenu(const QPoint& pos)
{
        //Explorer les plugins et les informer de l'événement
        QList<QString>    urls;
        QList<QUrlInfo *> infos;

        QList<QModelIndex> indexes = _view->selectionModel()->selectedIndexes();

        if (indexes.count() != 0)
        {
                foreach(QModelIndex index, indexes)
                {
                        urls.append(_job->host() + _model->itemPath(index));
                        infos.append(_model->itemUrlInfo(index));
                }
        }
        else
        {
                urls.append(_job->host() + _model->itemPath(_view->rootIndex()));
                infos.append(_model->itemUrlInfo(_view->rootIndex()));
        }

        foreach(IActionPlugin * plugin, plugins)
        {
                plugin->filesClicked(urls, infos);
        }

        contextMenu->popup(QCursor::pos());
}

void LDirView::timerEvent(QTimerEvent *event)
{
        _model->refreshItem(_view->rootIndex());

        (void)event;
}

void LDirView::resizeEvent(QResizeEvent *event)
{
        _view->resize(size());
}

bool LDirView::eventFilter(QObject *watched, QEvent *event)
{
        if (watched == _view->viewport() && event->type() == QEvent::MouseButtonPress)
        {
                QMouseEvent *evt = static_cast<QMouseEvent *>(event);
                
                if (evt->button() == Qt::LeftButton)
                {
                        QModelIndex index = _view->indexAt(evt->pos());
                        
                        if (!index.isValid())
                        {
                                //On a cliqué dans une zone vide
                                viewClicked(_view->rootIndex());
                        }
                }
        }
        
        return false;
}

QUndoStack *LDirView::undoStack()
{
        return undos;
}
