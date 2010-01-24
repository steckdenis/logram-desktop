#ifndef _MWINDOW_
#define _MWINDOW_

#include <QtWebKit>
#include <QtNetwork>
#include <QtGui>

#include <LAdressBar.h>
#include <LBookmarksManager.h>

#include "googlecompleter.h"
#include "tabs.h"
#include "toolbar.h"

class mwindow : public QMainWindow
{
        Q_OBJECT

public:
        mwindow(); // constructeur
        QIcon favicon(QString url);
        QUrl parseUrl(QUrl);

public slots:

        // tabs and window management slots
        void changeTitle(const QString & title);
        void changeUrl(const QUrl & url);
        void tabChanged(int);

        void newWin();

        // file slots
        void open();
        void openUrl();

        // web internal slots
        void load();
        void loadingStarted();
        void loadingProgress(int);
        void loadingFinished(bool);

        // navigation slots
        void back();
        void forward();
        void refresh();
        void stop();
        void home();

        // edition slots
        void print();
        void find();
        void cut();
        void copy();
        void paste();

        void contextMenu(QPoint);

        // options slots
        void readSavedTabs();

        // help and about slots
        void about();
        void aboutLogram();

	// slots de la fenêtre d'options
        void openSettings();
	void defineStart(int index);
	void defineHome(QString home);
        void defineHomeCurrentPage();
	void defineHomeDefaultPage();
	void defineHomeBlankPage();
	void fontDialog();
        void defineFont(QFont);

        // download management slots
        void download(QNetworkReply*);
        void replyFinished(QNetworkReply*);

private:
        QString iconPath;

        // barre
        ToolBar *bar;

	// onglets
        Tabs *tabs;
        QPushButton *newTabCornerButton;
	
	// fenetre d'options 
	QMainWindow *settingsWindow;
        QLineEdit *homeLineEdit;

	// gestion des préférences (fichier de configuration et paramètres web)
	QSettings *settings;
	QWebSettings *webSettings;

protected:
	void closeEvent(QCloseEvent *event);
};

#endif
