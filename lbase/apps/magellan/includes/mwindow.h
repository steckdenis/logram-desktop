#include <QtWebKit>
#include <QtNetwork>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QStatusBar>
#include <QProgressBar>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QInputDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QFontComboBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QSpinBox>
#include <QFontDialog>
#include "QTopSite.h"
#include <QGroupBox>
#include <QBoxLayout>
#include <QPixmap>
#include <QTransform>

class mwindow : public QMainWindow
{
        Q_OBJECT

public:
        mwindow(); // constructeur
        QWidget *makeTab(QString url);
        QWebView *currentPage();
        QIcon favicon(QString url);

public slots:

        // tabs and window management slots
        void changeUrl(const QUrl & url);
        void changeTitle(const QString & title);
        void switchTab(int index);

        void newWin();

        void addTab();
        void closeTab();

        // file slots
        void open();
        void openUrl();

        // web internal slots
        void load();
        void loadingStarted();
        void loadingProgress(int loading);
        void loadingFinished(bool ok);

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

        // bookmarks management slots
        void updateBookmarks(QMenu *menu);
	void addBookmark(bool);
	void bookmarkClicked(bool);

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
	
        // barre d'état
	QStatusBar *statusBar; 
	QProgressBar *progressBar;

        //bare d'outils
        QAction *reload;
        QLineEdit *adressBar;        
        QMenu *bookmarks;

	// onglets
	QTabWidget *tabs;
        QPushButton *newTabCornerButton;
	
	// fenetre d'options 
	QMainWindow *settingsWindow;
	QLineEdit *homeLineEdit;
	
	// structures et widgets relatifs à la gestion des marques pages
	struct Signet
        {
                QString title;
                QString url;
        };
        QHash<QAction *, Signet *> signets;

	// gestion des préférences (fichier de configuration et paramètres web)
	QSettings *settings;
	QWebSettings *webSettings;

protected:
	void closeEvent(QCloseEvent *event);
};
