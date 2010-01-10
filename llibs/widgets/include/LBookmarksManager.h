#ifndef LBOOKMARKSMANAGER_H
#define LBOOKMARKSMANAGER_H

/* This file provides a class which manages bookmarks. Useful methods :
 * - addBookmark -> Add a bookmark to the configuration file by taking as parameters the URL and the title
 * - bookmarkClicked -> Signal emitted when a bookmark is clicked and which sends the bookmark's URL
 * - updateBookmark -> Take as parameter a QMenu and fill this QMenu with the bookmark list.
 */

#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QDir>

#include <QSettings>

#include <QMenu>
#include <QAction>

#include <QWebSettings>

class LBookmarksManager : public QObject
{
    Q_OBJECT

public:
    LBookmarksManager(QString, QMenu *, bool);

    void updateBookmarks();
    QIcon favicon(QUrl);

public slots:
    void addBookmark(QString, QUrl);

private slots:
    void bookmarkIsClicked(bool);

signals:
    void bookmarkClicked(QUrl);

private:
    QSettings *set;
    QMenu *menu;
    struct Signet // This structure represents a bookmark.
        {
                QString title;
                QString url;
        };
     QHash<QAction *, Signet *> signets;
     bool webBrowsing;
};

#endif // BOOKMARKSMANAGER_H
