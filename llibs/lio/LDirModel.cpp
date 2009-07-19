/*
 * LDirModel.cpp
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

#include <QList>
#include <QtDebug>
#include <QApplication>
#include <QStyle>
#include <QString>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QProgressDialog>
#include <QCryptographicHash>
#include <QLocale>

#include <LDirModel.h>
#include <LDesktopFile.h>
#include <LMimeData.h>

class Element
{
public:
        Element(LDirModel *model, QString path, QString fileName, Element *parent = 0);
        ~Element();

        Element *child(int index);
        int childCount() const;
        int childIndex(Element *element) const;
        Element *parent();
        bool rename(QString& value);

        QString& path();
        QString& fileName();
        Qt::ItemFlags flags();
        bool isDir();

        void reset();

        QPixmap icon();
        void setIcon(QPixmap icon);

        void setCachedSize(QSize size);
        QSize cachedSize();

        QUrlInfo *info();
        void setInfo(QUrlInfo *info);

        Element *childForPath(const QString& path);

        bool explored() const;
        void explore();
        bool update();

private:
        Element          *_parent;
        QList<Element *> children;
        bool             isExplored;
        QUrlInfo         *_info;
        QString          _path, _fileName;
        QPixmap          _icon;
        QSize            _csize;
        LDirModel        *_model;
};

/*
        Classe Element
 */

static int numElements = 0;

Element::Element(LDirModel *model, QString path, QString fileName, Element *parent)
{
        _model = model;
        _path  = path;
        _path.replace("//", "/");
        _parent    = parent;
        _fileName  = fileName;
        isExplored = false;
        _info      = 0;
        _icon      = QPixmap();

        numElements++;
}

Element::~Element()
{
        qDeleteAll(children);
        delete _info;
}

Element *Element::child(int index)
{
        return children.value(index);
}

int Element::childCount() const
{
        return children.count();
}

int Element::childIndex(Element *element) const
{
        return children.indexOf(element);
}

Element *Element::parent()
{
        return _parent;
}

bool Element::rename(QString& value)
{
        _model->job()->cd(_path);

        if (!_model->job()->rename(_fileName, value))
        {
                return false;
        }

        _fileName = value;

        return true;
}

QString& Element::path()
{
        return _path;
}

QString& Element::fileName()
{
        return _fileName;
}

QUrlInfo *Element::info()
{
        return _info;
}

void Element::setInfo(QUrlInfo *info)
{
        _info     = info;
        _fileName = info->name();
}

bool Element::explored() const
{
        return isExplored;
}

void Element::explore()
{
        /* Obtenir la liste des sous-dossiers et fichiers */

        update();

        isExplored = true;
}

void Element::reset()
{
        update();

        //Reseter chaque enfant
        foreach(Element * child, children)
        {
                if (child->explored())
                {
                        child->reset();
                }
        }
}

bool Element::update()
{
        if (!isDir())
        {
                return false;
        }

        //Se placer dans le bon dossier
        QString filePath = _path + _fileName;
        _model->job()->cd(filePath);

        //Lister le dossier
        QList<QUrlInfo *> infos;

        if (!_model->job()->list(&infos, _model->showHidden()))
        {
                return false;
        }

        //Supprimer tous les noeds qui ne sont plus dans la liste
        for (int i = 0; i < children.count(); i++)
        {
                bool found = false;

                foreach(QUrlInfo * minfo, infos)
                {
                        if (minfo->name() == children.at(i)->fileName())
                        {
                                found = true;
                        }
                }

                if (!found)
                {
                        children.removeAt(i);
                        i--;
                }
        }

        //Créer les noeds qui ne sont pas déjà dans la liste
        for (int j = 0; j < infos.count(); j++)
        {
                QUrlInfo *info = infos.at(j);

                Element *child = 0;
                Element *mchild;

                //Explorer les éléments enfant à la recherche de celui qui a le meme nom que l'info
                for (int i = 0; i < children.count(); i++)
                {
                        mchild = children.at(i);

                        if (mchild->fileName() == info->name())
                        {
                                //On a trouvé le bon
                                child = mchild;

                                break;
                        }
                }

                if (child != 0)
                {
                        //Un enfant existait déjà, le mettre à jour
                        child->setInfo(info);
                }
                else
                {
                        //On n'a pas encore cet enfant, l'ajouter
                        if (_model->showFiles() || info->isDir())
                        {
                                child = new Element(_model, _path + _fileName + "/", info->name(), this);

                                child->setInfo(info);

                                children.append(child);
                        }
                }
        }

        return true;
}

Element *Element::childForPath(const QString& path)
{
        QString childPath;

        //Vérifie que l'élément est peuplé, seulement si le chemin de l'élément est contenu dans le path
        QString mpath = this->path() + fileName();

        if (path.contains(mpath))
        {
                if (!explored())
                {
                        explore();
                }

                foreach(Element * element, children)
                {
                        childPath = element->path() + element->fileName();

                        if (childPath == path)
                        {
                                return element;
                        }
                        else
                        {
                                Element *match;

                                match = element->childForPath(path);

                                if (match != 0)
                                {
                                        return match;
                                }
                        }
                }
        }

        //Le sous-élément ne contient pas le chemin, ou le chemin de cet élément n'est pas dans path
        //  ==> ce n'est pas ici qu'il faut chercher

        return 0;
}

bool Element::isDir()
{
        if (_info == 0)
        {
                return true;            //RootItem
        }
        if (_info->isDir())
        {
                return true;
        }
        else
        {
                return false;
        }
}

Qt::ItemFlags Element::flags()
{
        if (isDir())
        {
                return Qt::ItemIsDropEnabled;
        }
        else
        {
                return Qt::NoItemFlags;
        }
}

QPixmap Element::icon()
{
        return _icon;
}

void Element::setIcon(QPixmap icon)
{
        _icon  = icon;
        _csize = icon.size();
}

void Element::setCachedSize(QSize size)
{
        _csize = size;
}

QSize Element::cachedSize()
{
        return _csize;
}

/*
        Classe LDirModel
 */

LDirModel::LDirModel(LIOJob *job, QString& dir, bool showFile, bool showHidden, QObject *parent) : QAbstractItemModel(parent)
{
        _job        = job;
        _showFile   = showFile;
        _showHidden = showHidden;
        _iconW      = 16;
        _iconH      = 16;
        rootElement = 0;
        _rootDir    = dir;
        iloader     = new LIconLoader(QString(), this);
        mimedata    = new LMimeData(this);

        setSupportedDragActions(Qt::MoveAction);

        //refresh();

        rootElement = new Element(this, _rootDir, QString(), 0);
}

bool LDirModel::refresh()
{
        rootElement->reset();

        return true;
}

void LDirModel::refreshItem(const QModelIndex& index)
{
        Element *element;

        if (!index.isValid())
        {
                element = rootElement;
        }
        else
        {
                element = getElement(index);
        }

        int oldcount = element->childCount();

        element->update();

        int newcount = element->childCount();

        if (oldcount < newcount)
        {
                //On a ajouté des fichiers
                beginInsertRows(index, oldcount, newcount - 1);
                endInsertRows();
        }
        else if (oldcount > newcount)
        {
                //On a retiré des fichiers
                beginRemoveRows(index, newcount, oldcount - 1);
                endRemoveRows();
        }
}

LDirModel::~LDirModel()
{
        delete rootElement;
}

Element *LDirModel::getElement(const QModelIndex& index) const
{
        if (index.isValid())
        {
                Element *element = 0;
                element = static_cast<Element *>(index.internalPointer());
                if (element != 0)
                {
                        return element;
                }
        }

        return rootElement;
}

QVariant LDirModel::data(const QModelIndex& index, int role) const
{
        if (!index.isValid())
        {
                return QVariant();
        }

        Element  *element = getElement(index);
        QUrlInfo *info    = element->info();

        if (info == 0)
        {
                return QVariant();
        }

        quint64 sz;
        QString rs;

        switch (role)
        {
                case Qt::DisplayRole:
                        switch (index.column())
                        {
                                case Name:
                                        if (element->fileName().endsWith(".desktop") && (_job->host() == "file://"))
                                        {
                                                //Fichier .desktop, prendre le nom dans ce fichier
                                                LDesktopFile df(_rootDir + element->path() + element->fileName());

                                                return QVariant(df.name());
                                        }
                                        else
                                        {
                                                return QVariant(element->fileName());
                                        }

                                case Size:
                                        sz = info->size();

                                        if (sz < 1024)
                                        {
                                                rs = QString::number(sz) + " o";
                                        }
                                        else if (sz < 1024 * 1024)
                                        {
                                                rs = QString::number(sz >> 10) + " Kio";
                                        }
                                        else if (sz < 1024 * 1024 * 1024)
                                        {
                                                rs = QString::number(sz >> 20) + " Mio";
                                        }
                                        else if (sz < (quint64)1024 * 1024 * 1024 * 1024)
                                        {
                                                rs = QString::number(sz >> 30) + " Gio";
                                        }
                                        else
                                        {
                                                rs = QString::number(sz >> 40) + " Tio";
                                        }

                                        return QVariant(rs);

                                case LastModified:
                                        return QVariant(info->lastModified().toString(Qt::DefaultLocaleLongDate));

                                case LastRead:
                                        return QVariant(info->lastRead().toString(Qt::DefaultLocaleLongDate));

                                case Owner:
                                        return QVariant(info->owner());

                                case Group:
                                        return QVariant(info->group());

                                default:
                                        return QVariant();
                        }
                        break;

                case Qt::DecorationRole:
                        if (index.column() == 0)
                        {
                                return QVariant(filePixmap(index));
                        }
                        return QVariant();

                case Qt::ToolTipRole:
                        //Pour un fichier .desktop, afficher sa description en tooltip
                        if (info->name().endsWith(".desktop") && (_job->host() == "file://"))
                        {
                                LDesktopFile df(_rootDir + element->path() + element->fileName());

                                return QVariant(df.comment());
                        }
                        return QVariant();

                case Qt::EditRole:
                        if (index.column() == 0)
                        {
                                return QVariant(info->name());
                        }

                default:
                        return QVariant();
        }

        return QVariant();
}

QVariant LDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if (role == Qt::DisplayRole)
        {
                switch (section)
                {
                        case Name:
                                return QVariant(tr("Nom"));

                        case Size:
                                return QVariant(tr("Taille"));

                        case LastModified:
                                return QVariant(tr("Dernière modification"));

                        case LastRead:
                                return QVariant(tr("Dernier accès"));

                        case Owner:
                                return QVariant(tr("Propriétaire"));

                        case Group:
                                return QVariant(tr("Groupe"));
                }
        }

        return QVariant();

        (void)orientation;
}

QModelIndex LDirModel::index(int row, int column, const QModelIndex& parent) const
{
        ;
        Element *mparent;

        if (!parent.isValid())
        {
                mparent = rootElement;
        }
        else
        {
                mparent = getElement(parent);
        }

        Element *child = mparent->child(row);

        if (child != 0)
        {
                return createIndex(row, column, child);
        }
        else
        {
                return QModelIndex();
        }
}

QModelIndex LDirModel::parent(const QModelIndex& index) const
{
        if (!index.isValid())
        {
                return QModelIndex();
        }

        Element *child   = getElement(index);
        Element *parent  = child->parent();
        Element *pparent = parent->parent(); //Utilisé pour trouvé l'index du parent

        if ((parent == rootElement) || (child == rootElement))
        {
                return QModelIndex();
        }

        return createIndex(pparent->childIndex(parent), 0, parent);
}

int LDirModel::rowCount(const QModelIndex& parent) const
{
        Element *element;

        if (parent.column() > 0)
        {
                return 0;
        }

        if (!parent.isValid())
        {
                element = rootElement;
        }
        else
        {
                element = getElement(parent);
        }

        if (!element->explored())
        {
                if (_job->host() == "file://" || element == rootElement)
                {
                        element->explore();
                }
                else
                {
                        //Sur le réseau, on n'explore pas tout de suite un dossier
                        return element->info()->isDir() ? 1 : 0;       //CanFetchMore fera le reste
                }
        }

        return element->childCount();
}

bool LDirModel::canFetchMore(const QModelIndex &parent) const
{
        if (!parent.isValid())
        {
                return false;
        }
        
        Element *element = getElement(parent);
        
        return !element->explored();
}

void LDirModel::fetchMore(const QModelIndex &parent)
{
        if (parent.isValid())
        {
                Element *element = getElement(parent);
                
                element->explore();
        }
}

int LDirModel::columnCount(const QModelIndex& parent) const
{
        return 6; //NOTE: Nombre d'entrées dans la structure Field

        (void)parent;
}

Qt::ItemFlags LDirModel::flags(const QModelIndex& index) const
{
        if (!index.isValid())
        {
                return 0;
        }

        Element *element = getElement(index);

        return (index.column() == 0 ? Qt::ItemIsEditable | Qt::ItemIsDragEnabled : Qt::NoItemFlags) | Qt::ItemIsEnabled | Qt::ItemIsSelectable | element->flags();
}

bool LDirModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
        if (!index.isValid())
        {
                return false;
        }

        if (index.column() != 0)
        {
                return false;
        }

        if (role != Qt::EditRole)
        {
                return false;
        }

        Element *element = getElement(index);

        QString mvalue = value.toString();

        return element->rename(mvalue);
}

Qt::DropActions LDirModel::supportedDropActions() const
{
        return Qt::CopyAction | Qt::MoveAction;
}

QStringList LDirModel::mimeTypes() const
{
        QStringList types;

        types << "text/uri-list";

        return types;
}

QMimeData *LDirModel::mimeData(const QModelIndexList& indexes) const
{
        QMimeData *mimeData = new QMimeData();

        QList<QUrl> urls;

        foreach(QModelIndex index, indexes)
        {
                if (index.isValid())
                {
                        Element *element = getElement(index);

                        urls.append(QUrl(_job->host() + "/" + element->path() + element->fileName()));
                }
        }

        mimeData->setUrls(urls);
        return mimeData;
}

bool LDirModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
        if (action == Qt::IgnoreAction)
        {
                return true;
        }

        QList<QUrl> urls;

        urls = data->urls();

        Element *element;

        if (!parent.isValid())
        {
                element = rootElement;
        }
        else
        {
                element = getElement(parent);
        }

        //Emettre le signal de redimensionnement
        beginInsertRows(parent, element->childCount(), element->childCount() + urls.count());

        LIOJob mjob;

        //Afficher une jolie boîte de progression
        QProgressDialog pgs(tr("Copie de fichiers..."), tr("Annuler"), 0, urls.count(), 0);
        pgs.setMinimumDuration(500);
        int i = 0;

        foreach(QUrl url, urls)
        {
                if (!pasteFile(url, parent, action, &mjob))
                {
                        return false;
                }

                i++;
                pgs.setValue(i);

                if (pgs.wasCanceled())
                {
                        endInsertRows();        //NOTE: Vérifier que c'est utile
                        return false;
                }
        }

        //Mettre à jour la liste des enfants de l'élément dans lequel on a transféré les fichiers
        if (!element->update())
        {
                return false;
        }

        //Emettre la suite du signal
        endInsertRows();

        return true;

        (void)row;
}

bool LDirModel::pasteFile(const QUrl& url, const QModelIndex& index, Qt::DropAction action, LIOJob *mjob)
{
        if (!index.isValid())
        {
                return false;
        }

        Element *element = getElement(index);

        if (!element->info()->isDir())
        {
                return false;
        }

        //Si l'url et le LIOJob ont le meme host, pas besoin de reconnecter, on copie betement
        QString uHost = url.scheme() + "://" + url.authority();

        QString     upath        = url.path();
        QStringList pathElements = upath.split('/', QString::SkipEmptyParts);
        QString     dpath        = element->path() + element->fileName() + "/" + pathElements[pathElements.count() - 1];
        bool        deleteMJob   = false;

        qDebug() << "Host : " << uHost << ", upath : " << upath << ", dpath" << dpath;
        qDebug() << "_job->host() = " << _job->host();

        if (uHost == _job->host())
        {
                qDebug() << "_job->copy(" << upath << "," << dpath << ");";

                if (action == Qt::CopyAction)
                {
                        if (!_job->copy(upath, dpath))
                        {
                                return false;
                        }
                }
                else
                {
                        if (!_job->move(upath, dpath))
                        {
                                return false;
                        }
                }
        }
        else
        {
                if (mjob == 0)
                {
                        mjob = new LIOJob(this);
                        deleteMJob = true;              //mjob est temporaire à l'application, il faut le supprimer
                }
                
                QString mhost = url.scheme() + "://" + url.authority();
                
                //Il va falloir connecter un autre job
                if (mjob->host() != mhost)
                {
                        //Besoin de connecter le job
                        if (!mjob->setHost(mhost))
                        {
                                return false;
                        }
                }

                qDebug() << "mjob->host() = " << mjob->host();

                //TODO: Gérer Qt::MoveAction ici aussi (faire attention à la suppression récursive de dossiers)

                if (mjob->host() == "file://")
                {
                        //On upload de local vers distant
                        if (!_job->upload(upath, dpath, false))
                        {
                                return false;
                        }
                }
                else if (_job->host() == "file://")
                {
                        //On download de distant vers local
                        if (!mjob->download(upath, dpath, false))
                        {
                                return false;
                        }
                }
                else
                {
                        //On transfert de distant vers distant, il faut passer par un download
                        QString tmpFile = "LDirModel_copyFile.tmp";
                        if (!mjob->download(upath, tmpFile))
                        {
                                return false;
                        }
                        if (!_job->upload(tmpFile, dpath))
                        {
                                return false;
                        }
                }
        }

        return true;
}

QString LDirModel::itemPath(const QModelIndex& index) const
{
        if (!index.isValid())
        {
                return QString();
        }

        Element *element = getElement(index);
        return element->path() + element->fileName();
}

QUrlInfo *LDirModel::itemUrlInfo(const QModelIndex& index) const
{
        if (!index.isValid())
        {
                return 0;
        }

        Element *element = getElement(index);

        return element->info();
}

void LDirModel::setIconSize(int width, int height)
{
        _iconW = width;
        _iconH = height;
}

QPixmap LDirModel::filePixmap(const QModelIndex& index) const
{
        if (!index.isValid())
        {
                return QPixmap();
        }

        Element  *element = getElement(index);
        QUrlInfo *info    = element->info();

        if (info == 0)
        {
                return QPixmap();
        }

        QPixmap eicon = element->icon();
        QSize   csz   = element->cachedSize();

        if (!eicon.isNull() && ((csz.width() == _iconW) || (csz.height() == _iconH)))
        {
                return element->icon();
        }

        QStyle *style = QApplication::style();

        QString path = element->path() + "/" + element->fileName();
        path.replace(QString("//"), QString("/")); //Remplacer les // par des /

        if (info->isDir())
        {
                if (path == QDir::homePath())
                {
                        element->setIcon(style->standardIcon(QStyle::SP_DirHomeIcon).pixmap(_iconW, _iconH));
                        return element->icon();
                }
                else if (info->isSymLink())
                {
                        element->setIcon(style->standardIcon(QStyle::SP_DirLinkIcon).pixmap(_iconW, _iconH));
                        return element->icon();
                }
                else
                {
                        element->setIcon(style->standardIcon(QStyle::SP_DirIcon).pixmap(_iconW, _iconH));
                        return element->icon();
                }
        }

        if (info->isSymLink())
        {
                element->setIcon(style->standardIcon(QStyle::SP_FileLinkIcon).pixmap(_iconW, _iconH));
                return element->icon();
        }

        //Trouver l'extension du fichier
        QStringList parts = element->fileName().split('.');
        QString     ext   = parts.at(parts.count() - 1).toLower();

        //Charger les extensions connues
        QStringList imageExts = LConfig::logramValue("FileSystem/ImageExtensions", "png jpg jpeg tiff tga xpm gif").toString().split(' ');

        if (imageExts.contains(ext) && (_job->host() == "file://"))
        {
                /* Créer la pixmap, seulement pour les fichiers locaux, et quand on les affiche en grand */
                if (QFile::exists(path) && (_iconW >= 32) && (_iconH >= 32))
                {
                        //Construire le nom sha1 du fichier
                        QString hashed = QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Sha1).toHex();
                        QString mdir   = QString::number(_iconW) + "x" + QString::number(_iconH);

                        QDir dir(QDir::homePath());
                        dir.mkpath(QString(".thumbs/") + mdir);

                        QString filePath = QDir::homePath() + "/.thumbs/" + mdir + "/" + hashed + ".png";

                        if (QFile::exists(filePath))
                        {
                                element->setIcon(QPixmap(filePath));
                        }
                        else
                        {
                                QPixmap fpix = QPixmap(path).scaled(_iconW, _iconH, Qt::KeepAspectRatio);
                                fpix.toImage().save(filePath);
                                qDebug() << filePath;
                                element->setIcon(fpix);
                        }
                        return element->icon();
                }
        }

        //Voir si c'est un fichier .desktop
        if ((ext == "desktop") && (_job->host() == "file://"))
        {
                LDesktopFile df(_rootDir + element->path() + element->fileName());

                QString pixmapname = df.icon();

                if (pixmapname.startsWith('/'))
                {
                        element->setIcon(QPixmap(pixmapname).scaled(_iconW, _iconH, Qt::KeepAspectRatio));
                        return element->icon();
                }
                else
                {
                        element->setIcon(iloader->icon(pixmapname).pixmap(_iconW, _iconH));

                        QSize ccsz(_iconW, _iconH);
                        element->setCachedSize(ccsz);

                        return element->icon();
                }
        }

        //On n'a rien trouvé, essayer avec les types MIME
        mimedata->setFileName(element->fileName());
        QPixmap pix = iloader->icon(mimedata->iconName()).pixmap(_iconW, _iconH);
        
        if (!pix.isNull())
        {
                element->setIcon(pix);
                
                QSize ccsz(_iconW, _iconH);
                element->setCachedSize(ccsz);
                
                return element->icon();
        }

        element->setIcon(style->standardIcon(QStyle::SP_FileIcon).pixmap(_iconW, _iconH));      //Icône par défaut
        return element->icon();
}

QModelIndex LDirModel::indexForPath(const QString& path)
{       
        Element *match = rootElement->childForPath(path);

        if (match != 0)
        {
                if (match == rootElement)
                {
                        return createIndex(0, 0, match);
                }

                Element *parent = match->parent();
                
                //Explorer l'élément car il doit afficher un contenu
                if (!match->explored())
                {
                        match->explore();
                }

                return createIndex(parent->childIndex(match), 0, match);
        }

        return QModelIndex();
}

void LDirModel::setShowFiles(bool showFile)
{
        _showFile = showFile;

        //refresh();
}

void LDirModel::setShowHidden(bool showHidden)
{
        _showHidden = showHidden;

        //refresh();
}

bool LDirModel::showFiles()
{
        return _showFile;
}

bool LDirModel::showHidden()
{
        return _showHidden;
}

LIOJob *LDirModel::job()
{
        return _job;
}
