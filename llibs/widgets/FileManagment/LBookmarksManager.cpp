#include <LBookmarksManager.h>

LBookmarksManager::LBookmarksManager(QString fileName, QMenu *parentMenu, bool forWebBrowsing = false)
{
    set = new QSettings("Logram", fileName);
    menu = parentMenu;

    webBrowsing = forWebBrowsing;
    if(webBrowsing) {
	if (!QDir(qApp->applicationDirPath() + "/cache/").exists()) 
	    QDir(qApp->applicationDirPath()).mkdir("cache"); 
	QWebSettings::setIconDatabasePath(qApp->applicationDirPath() + "/cache/"); }
}

QIcon LBookmarksManager::favicon(QUrl adress)
{
    if(adress.toString().isNull())
        return QIcon();
    else
        return QWebSettings::iconForUrl(adress);
}

void LBookmarksManager::updateBookmarks()
{
    Signet *sign;

    // Effacer le menu et la liste des signets
    foreach(QAction * act, signets.keys())
    {
        delete signets.take(act);               //Suppression du signet
    }

    menu->clear();

    QAction *add = menu->addAction(QIcon(), tr("Ajouter un marque-page"), this, SLOT(addBookmark(bool)));
    menu->addSeparator();

    // Lire les entrées des signets
    foreach(QString grp, set->childGroups())
    {
        set->beginGroup(grp);

        sign        = new Signet;
        sign->title = set->value("Title").toString();
        sign->url   = set->value("Url").toString();

        QIcon icon;
        if(webBrowsing) {
            icon = favicon(sign->url);
            if(icon.isNull()) icon = QIcon(); }
        else
            icon = QIcon();

        QAction *act = menu->addAction(icon, sign->title, this, SLOT(bookmarkIsClicked(bool)));

        signets.insert(act, sign);

        set->endGroup();
    }

}

void LBookmarksManager::bookmarkIsClicked(bool)
{
    QAction *act = static_cast<QAction *>(sender());
    Signet *sign = signets.value(act);

    emit bookmarkClicked(sign->url);
}

void LBookmarksManager::addBookmark(QString title, QUrl adress)
{
    set->beginGroup(title);
    set->setValue("Title", title);
    set->setValue("Url", adress);
    set->endGroup();

    set->sync();

    updateBookmarks();
}

