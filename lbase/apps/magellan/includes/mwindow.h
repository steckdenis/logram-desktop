#include <QtWebKit>
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
#include <LIconLoader.h>
#include <LRibbon.h>
#include <QGroupBox>
#include <QBoxLayout>

class mwindow : public QMainWindow
{
	Q_OBJECT

public:
	mwindow(); // constructeur
	QWidget *makeTab(QString url);
	QWebView *currentPage();
	QString homeOpenBox();
	void loadUrl(QString url);
	void updateBookmarks(QMenu *menu);
	void addToHistory(QString title, QString url);

public slots:
	void startLoading();
	void runLoading(int loading);
	void endLoading(bool ok);
	void changeUrl(const QUrl & url);
	void changeTitle(const QString & title);
	void load();
	void switchTab(int index);
	void addTab();
	void tabClose();
	void openWin();
	void openFile();
	void pageOpen();
	void printPage();
	void exit();
	void backPage();
	void forwardPage();
	void reloadPage();
	void stopLoading();
	void goHome();
	void homeChange();
	void aboutIt();
	void closeThisTab(int index);
	void openSettings();
	void googleSearch();
	void startGoogleSearch();
	void searchInText();
	void changeIcon();
	void aboutLogram();
	void readSavedTabs();
	void addBookmark(bool);
	void bookmarkClicked(bool);

	// slots de la fenêtre d'options
	void defineStart(int index);
	void defineHome(QString home);
	void defineHomeCurrentPage();
	void defineHomeDefaultPage();
	void defineHomeBlankPage();
	void fontDialog();

private:
	// actions et sous menus
	QAction *newTab;
	QAction *closeTab;
	QAction *newWin;
	QAction *open;
	QAction *openPage;
	QAction *save;
	QAction *print;
	QAction *quit;
	QAction *search;
	QAction *back;
	QAction *forward;
	QAction *reload;
	QAction *stop;
	QAction *home;
	QAction *go;
	QAction *changeHome;
	QMenu *toolBarsProperties;
	QAction *showMainToolBar;
	QAction *showBookmarksToolBar;
	QAction *lockBookmarksToolBar;
	QAction *openSettingsWindow;
	QAction *addToTopSites;
	QAction *googleTool;
	QAction *about;
	QAction *qabout;
	QAction *labout;
	QAction *lockMainToolBar;

	// barre de menus et menus
	QMenuBar *menuBar;
	QMenu *file;
	QMenu *edit;
	QMenu *navig;
	QMenu *config;
	QMenu *tools;
	QMenu *bookmarks;
	QMenu *help;
	
	// ruban, group boxes et push buttons
	LRibbon *ribbon;
		QHBoxLayout *navigLayout;
			QGroupBox *navigBox;
				QLineEdit *adressBar;
				QPushButton *goButton;
				QPushButton *backButton;
				QPushButton *forwardButton;
				QPushButton *reloadButton;
				QPushButton *stopButton;
				QPushButton *homeButton;
				QPushButton *newTabButton;
				QPushButton *closeTabButton;
				QPushButton *newWindowButton;
			QGroupBox *searchBox;
				QLineEdit *googleBar;
				QPushButton *googleButton;
				QPushButton *searchButton;
			QGroupBox *historyBox;
				QPushButton *historyButton;
		QHBoxLayout *bookmarksLayout;
			QGroupBox *addBox;
				QPushButton *addButton;
			QGroupBox *bookmarksBox;
				QPushButton *bookmarksButton;
		QHBoxLayout *fileLayout;
			QGroupBox *tabsBox;
			QGroupBox *filesBox;
				QPushButton *openButton;
				QPushButton *openUrlButton;
				QPushButton *saveButton;
			QGroupBox *specialBox;
				QPushButton *printButton;
				QPushButton *quitButton;
		QHBoxLayout *editLayout;
			QGroupBox *textBox;
			QGroupBox *undoredoBox;
			QGroupBox *settingsBox;

	// barre d'état
	QStatusBar *statusBar; 
	QProgressBar *progressBar;

	// onglets
	QTabWidget *tabs;
	QPushButton *newTabCornerButton;
	
	// layout principal
	QVBoxLayout *layout;
	
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
	QIcon       iconDir;

	// gestion des préférences (fichier de configuration et paramètres web)
	QSettings *settings;
	QWebSettings *webSettings;

protected:
	void closeEvent(QCloseEvent *event);
};
