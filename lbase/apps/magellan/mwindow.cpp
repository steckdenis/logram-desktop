#include "mwindow.h"
#include <LConfig.h>

mwindow::mwindow()
{	
	// création d'un QSettings et récupération du thème
	settings = new QSettings("Logram", "Magellan");

	// restauration de la fenêtre
	resize(settings->value("MainWindow/width").toInt(), settings->value("MainWindow/height").toInt());
	if(settings->value("MainWindow/isMaximized").toBool()) {}
	else { move(settings->value("MainWindow/positionX").toInt(), settings->value("MainWindow/positionY").toInt()); }
	setWindowTitle(tr("Navigateur web Magellan"));
	setWindowIcon(QIcon("/usr/share/magellan/icons/main.png"));

	// creation des actions de la fenêtre principale
	newTab = new QAction(tr("&Nouvel onglet"), this);
	closeTab = new QAction(tr("&Fermer l'onglet"), this);
	newWin = new QAction(tr("Ouvrir une fe&netre"), this);
	open = new QAction(tr("&Ouvrir un fichier"), this);
	openPage = new QAction(tr("Ouvrir un &emplacement"), this);
	save = new QAction(tr("&Telecharger la page"), this);
	print = new QAction(tr("&Imprimer"), this);
	quit = new QAction(tr("&Quitter"), this);
	search = new QAction(tr("&Rechercher"), this);
	back = new QAction(tr("&Page precedente"), this);
	forward = new QAction(tr("Page &suivante"), this);
	home = new QAction(tr("Page d'&acceuil"), this);
	go = new QAction(tr("A&ller"), this);
	reload = new QAction(tr("A&ctualiser"), this);
	stop = new QAction(tr("A&rreter"), this);
	changeHome = new QAction(tr("&Changer la page d'accueil"), this);
	addToTopSites = new QAction(tr("ALogram : Environnement de bureau unique et distribution jouter au top sites"), this);
	openSettingsWindow = new QAction(tr("&Preferences"), this);
	showMainToolBar = new QAction(tr("Afficher la barre &principale"), this);
	lockMainToolBar = new QAction(tr("&Verrouiller la barre principale"), this);
	showBookmarksToolBar = new QAction(tr("Afficher la barre des &marques-pages"), this);
	lockBookmarksToolBar = new QAction(tr("Verouiller la &barre des marques-pages"), this);
	googleTool = new QAction(tr("&Rechercher sur le web"), this);
	about = new QAction(tr("A propos de Magellan..."), this);
	qabout = new QAction(tr("A propos de Qt..."), this);
	labout = new QAction(tr("A propos de Logram..."), this);
	
	// connexion des slots de la fenêtre principale
	connect(newTab, SIGNAL(triggered()), this, SLOT(addTab()));
	connect(closeTab, SIGNAL(triggered()), this, SLOT(tabClose()));
	connect(newWin, SIGNAL(triggered()), this, SLOT(openWin()));
	connect(open, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(openPage, SIGNAL(triggered()), this, SLOT(pageOpen()));
	connect(save, SIGNAL(triggered()), this, SLOT(downloadPage()));
	connect(print, SIGNAL(triggered()), this, SLOT(printPage()));
	connect(quit, SIGNAL(triggered()), this, SLOT(exit())); 
	connect(search, SIGNAL(triggered()), this, SLOT(searchInText()));
	connect(back, SIGNAL(triggered()), this, SLOT(backPage()));
	connect(forward, SIGNAL(triggered()), this, SLOT(forwardPage()));
	connect(home, SIGNAL(triggered()), this, SLOT(goHome()));
	connect(reload, SIGNAL(triggered()), this, SLOT(reloadPage()));
	connect(stop, SIGNAL(triggered()), this, SLOT(stopLoading()));
	connect(go, SIGNAL(triggered()), this, SLOT(load()));
	connect(changeHome, SIGNAL(triggered()), this, SLOT(homeChange()));
	connect(openSettingsWindow, SIGNAL(triggered()), this, SLOT(openSettings()));
	connect(showMainToolBar, SIGNAL(toggled(bool)), this, SLOT(hideMainToolBar(bool)));
	connect(lockMainToolBar, SIGNAL(toggled(bool)), this, SLOT(mainToolBarLock(bool)));
	connect(showBookmarksToolBar, SIGNAL(toggled(bool)), this, SLOT(hideBookmarksToolBar(bool)));
	connect(lockBookmarksToolBar, SIGNAL(toggled(bool)), this, SLOT(bookmarksToolBarLock(bool)));
	connect(googleTool, SIGNAL(triggered()), this, SLOT(startGoogleSearch()));
	connect(addToTopSites, SIGNAL(triggered()), this, SLOT(addTopSite()));
	connect(about, SIGNAL(triggered()), this, SLOT(aboutIt()));
	connect(qabout, SIGNAL(triggered()), qApp, SLOT(aboutQt()));    
	connect(labout, SIGNAL(triggered()), this, SLOT(aboutLogram()));
	
	// création de la barre de menus
	menuBar = new QMenuBar();
		file  = menuBar->addMenu(tr("&Fichier"));
			file->addAction(newTab);
			file->addAction(closeTab);
			file->addAction(newWin);
			file->addSeparator();
			file->addAction(open);
			file->addAction(openPage);
			file->addAction(save);
			file->addSeparator();
			file->addAction(print);
			file->addSeparator();
			file->addAction(quit);
		edit = menuBar->addMenu(tr("&Edition"));
			edit->addAction(search);
		navig = menuBar->addMenu(tr("&Navigation"));
			navig->addAction(back);
			navig->addAction(forward);
			navig->addAction(reload);
			navig->addAction(stop);
			navig->addSeparator();
			navig->addAction(home);
			navig->addAction(go);
		config = menuBar->addMenu(tr("&Configuration"));
			config->addAction(changeHome);
			config->addAction(openSettingsWindow);
			toolBarsProperties = config->addMenu(tr("&Barres d'outils"));
				toolBarsProperties->addAction(showMainToolBar);
				toolBarsProperties->addAction(lockMainToolBar);
				toolBarsProperties->addAction(showBookmarksToolBar);
				toolBarsProperties->addAction(lockBookmarksToolBar);
					showMainToolBar->setCheckable(true);
					lockMainToolBar->setCheckable(true);
					showBookmarksToolBar->setCheckable(true);
					lockBookmarksToolBar->setCheckable(true);
		tools = menuBar->addMenu(tr("&Outils"));
			tools->addAction(googleTool);
		bookmarks = menuBar->addMenu(tr("&Marques-pages"));
		help = menuBar->addMenu(tr("&Aide"));
			help->addAction(about);
			help->addAction(qabout);
			help->addAction(labout);
 	//setMenuBar(menuBar); 
	
	updateBookmarks(bookmarks);

	// construction du menu ruban
	ribbon = new LRibbon(this);
		
	QString iconPath = "/usr/share/logram/magellan/icons/";
	
	navigLayout = new QHBoxLayout;
		navigBox = new QGroupBox(tr("Naviguer sur internet"));
			adressBar = new QLineEdit;
			goButton = new QPushButton(QIcon(iconPath + "go.svg"), tr("Aller"));
			backButton = new QPushButton(QIcon(iconPath + "back.png"), tr("Precedente"));
			forwardButton = new QPushButton(QIcon(iconPath + "forward.png"), tr("Suivante"));
			reloadButton = new QPushButton(QIcon(iconPath + "reload.png"), tr("Actualiser"));
			stopButton = new QPushButton(QIcon(iconPath + "stop.svg"), tr("Arreter"));
			homeButton = new QPushButton(QIcon(iconPath + "home.png"), tr("Page d'accueil"));
			newTabButton = new QPushButton(QIcon(iconPath + "tab-new-background"), tr("Nouvel onglet"));
			closeTabButton = new QPushButton(QIcon(iconPath + "tab-close-other"), tr("Fermer l'onglet"));
			newWindowButton = new QPushButton(QIcon(iconPath + "new-window.png"), tr("Nouvelle fenetre"));
			QGridLayout *navigBoxLayout = new QGridLayout;
				navigBoxLayout->addWidget(adressBar, 0, 0, 1, 7);
				navigBoxLayout->addWidget(goButton, 0, 7, 1, 1);
				navigBoxLayout->addWidget(backButton, 1, 0, 1, 1);
				navigBoxLayout->addWidget(forwardButton, 1, 1, 1, 1);
				navigBoxLayout->addWidget(reloadButton, 1, 2, 1, 1);
				navigBoxLayout->addWidget(stopButton, 1, 3, 1, 1);
				navigBoxLayout->addWidget(homeButton, 1, 4, 1, 1);
				navigBoxLayout->addWidget(newTabButton, 1, 5, 1, 1);
				navigBoxLayout->addWidget(closeTabButton, 1, 6, 1, 1);
				navigBoxLayout->addWidget(newWindowButton, 1, 7, 1, 1);
		navigBox->setLayout(navigBoxLayout);
		navigLayout->addWidget(navigBox);
		searchBox = new QGroupBox(tr("Rechercher"));
			googleBar = new QLineEdit;
			googleButton = new QPushButton(QIcon(iconPath + "google.png"), tr("Recherche avec Google"));
			searchButton = new QPushButton(QIcon(iconPath + "find.svg"),tr("Recherche sur la page"));
			QGridLayout *searchBoxLayout = new QGridLayout;
				searchBoxLayout->addWidget(googleBar, 0, 0, 1, 2);
				searchBoxLayout->addWidget(googleButton, 1, 0, 1, 1);
				searchBoxLayout->addWidget(searchButton, 1, 1, 1, 1);
		searchBox->setLayout(searchBoxLayout);
		navigLayout->addWidget(searchBox);
		historyBox = new QGroupBox(tr("Historique"));
			historyButton = new QPushButton(QIcon(iconPath + "history.png"), tr(""));
			QGridLayout *historyBoxLayout = new QGridLayout;
				historyBoxLayout->addWidget(historyButton, 0, 0, 1, 1);
			historyBoxLayout->addWidget(historyButton);
		historyBox->setLayout(historyBoxLayout);
		navigLayout->addWidget(historyBox);
	bookmarksLayout = new QHBoxLayout;
		addBox = new QGroupBox(tr("Ajouter"));
			addButton = new QPushButton(QIcon(iconPath + "add.png"), tr("Page actuelle comme nouveau marque-page"));
				addButton->setIconSize(QSize(48, 48));
			QGridLayout *addBoxLayout = new QGridLayout();
				addBoxLayout->addWidget(addButton, 0, 0, 1, 1);
		addBox->setLayout(addBoxLayout);
		bookmarksLayout->addWidget(addBox);
		bookmarksBox = new QGroupBox(tr("Marques-pages"));
			bookmarksButton = new QPushButton(QIcon(iconPath + "bookmark.png"), tr("Mes marque-pages"));
				bookmarksButton->setIconSize(QSize(48, 48));
				bookmarksButton->setMenu(bookmarks);
			QGridLayout *bookmarksBoxLayout = new QGridLayout;
				bookmarksBoxLayout->addWidget(bookmarksButton, 0, 0, 1, 1);
		bookmarksBox->setLayout(bookmarksBoxLayout);
		bookmarksLayout->addWidget(bookmarksBox);
	fileLayout = new QHBoxLayout;
		tabsBox = new QGroupBox(tr("Onglets"));
			QVBoxLayout *tabsBoxLayout = new QVBoxLayout;
				tabsBoxLayout->addWidget(newTabButton);
				tabsBoxLayout->addWidget(closeTabButton);
				tabsBoxLayout->addWidget(newWindowButton);
		tabsBox->setLayout(tabsBoxLayout);
		fileLayout->addWidget(tabsBox);
		filesBox = new QGroupBox(tr("Fichiers"));
			openButton = new QPushButton(QIcon(iconPath + "open-document.png"), tr("Ouvrir un fichier"));
			openUrlButton = new QPushButton(QIcon(iconPath + ""), tr("Ouvir une URL"));
			saveButton = new QPushButton(QIcon(iconPath + ""), tr("Télécharger la page"));
 			QVBoxLayout *filesBoxLayout = new QVBoxLayout;
				filesBoxLayout->addWidget(openButton);
				filesBoxLayout->addWidget(openUrlButton);
				filesBoxLayout->addWidget(saveButton);
		filesBox->setLayout(filesBoxLayout);
		fileLayout->addWidget(filesBox);
		specialBox = new QGroupBox(tr("Autres"));
			printButton = new QPushButton(QIcon(iconPath + "print.png"), tr("Imprimer"));
			quitButton = new QPushButton(tr("Quitter"));
			QVBoxLayout *specialBoxLayout = new QVBoxLayout;
				specialBoxLayout->addWidget(printButton);
				specialBoxLayout->addWidget(quitButton);
		specialBox->setLayout(specialBoxLayout);
		fileLayout->addWidget(specialBox);
	ribbon->AddTab(tr("Navigation"), navigLayout);
	ribbon->AddTab(tr("Marques-pages"), bookmarksLayout);
	ribbon->AddTab(tr("Fichiers"), fileLayout);
	updateBookmarks(bookmarks);
	
	// connexion des boutons du ruban à leurs slots
	connect(adressBar, SIGNAL(returnPressed()), this, SLOT(load()));
	connect(goButton, SIGNAL(clicked()), this, SLOT(load()));
	connect(backButton, SIGNAL(clicked()), this, SLOT(backPage()));
	connect(forwardButton, SIGNAL(clicked()), this, SLOT(forwardPage()));
	connect(reloadButton, SIGNAL(clicked()), this, SLOT(reloadPage()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(stopLoading()));
	connect(homeButton, SIGNAL(clicked()), this, SLOT(goHome()));
	connect(newTabButton, SIGNAL(clicked()), this, SLOT(addTab()));
	connect(closeTabButton, SIGNAL(clicked()), this, SLOT(tabClose()));
	connect(newWindowButton, SIGNAL(clicked()), this, SLOT(openWin()));
	connect(googleBar, SIGNAL(returnPressed()), this, SLOT(googleSearch()));
	connect(googleButton, SIGNAL(clicked()), this, SLOT(googleSearch()));
	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchInText()));
	connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addBookmark(bool)));
	
	
	// définition du chemin des icones et des icones des actions
	newTab->setIcon(QIcon(""));
	closeTab->setIcon(QIcon(""));
	newWin->setIcon(QIcon(iconPath + "new-window.png"));
	open->setIcon(QIcon(iconPath + "open-document.png"));
	print->setIcon(QIcon(iconPath + "print.png"));
	back->setIcon(QIcon(iconPath + "back.png"));
	forward->setIcon(QIcon(iconPath + "forward.png"));
	home->setIcon(QIcon(iconPath + "home.png"));
	reload->setIcon(QIcon(iconPath + "reload.png"));
	stop->setIcon(QIcon(iconPath + "stop.svg"));
	go->setIcon(QIcon(iconPath + "go.svg"));
	//searchAction->setIcon(QIcon(iconPath + "search.svg"));
	search->setIcon(QIcon(iconPath + "find.svg"));
	about->setIcon(QIcon("/usr/share/magellan/themes/default/icons/main.png"));
	qabout->setIcon(QIcon("/usr/share/magellan/themes/default/icons/qt.png"));
	labout->setIcon(QIcon("/usr/share/magellan/themes/default/icons/logram.png"));
	
	// définition des raccourcis claviers
	newTab->setShortcut(QKeySequence("Ctrl+T"));
	closeTab->setShortcut(QKeySequence("Ctrl+W"));
	newWin->setShortcut(QKeySequence("Ctrl+N"));
	open->setShortcut(QKeySequence("Ctrl+O"));
	print->setShortcut(QKeySequence("Ctrl+P"));
	quit->setShortcut(QKeySequence("Ctrl+Q"));
	openPage->setShortcut(QKeySequence("Ctrl+Shift+O"));
	save->setShortcut(QKeySequence("Ctrl+S"));
	reload->setShortcut(QKeySequence::Refresh);
	back->setShortcut(QKeySequence::Back);
	forward->setShortcut(QKeySequence::Forward);
	search->setShortcut(QKeySequence::Find);
	
	// construction de la barre d'état et de la barre de progression 
        statusBar = new QStatusBar();
	progressBar = new QProgressBar();
	progressBar->resize(300, 10);
	statusBar->addWidget(progressBar);
	setStatusBar(statusBar);

	// construction du système d'onglets
	tabs = new QTabWidget;
	tabs->setTabsClosable(true);
	tabs->setMovable(true);
	tabs->setTabShape(QTabWidget::Triangular);
	newTabButton = new QPushButton(tr("Nouvel onglet"));
	tabs->setCornerWidget(newTabButton);
	
	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeThisTab(int)));
	connect(newTabButton, SIGNAL(clicked()), this, SLOT(addTab()));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));

	// construction du layout
	QWidget *centralArea = new QWidget;
	layout = new QVBoxLayout;
	layout->addWidget(ribbon);
	layout->addWidget(tabs);
	layout->setContentsMargins(0, 0, 0, 0);
	centralArea->setLayout(layout);
	setCentralWidget(centralArea);
	
	// récupération des paramètres de démarrage
	int startup = settings->value("startup").toInt();
	if (startup == 0) { tabs->addTab(makeTab(settings->value("home").toString()), tr("Page d'accueil")); }
	else if (startup == 1) { tabs->addTab(makeTab(""), tr("Page blanche")); }
	else if (startup == 2) { readSavedTabs(); if (tabs->count() < 1) { tabs->addTab(makeTab(settings->value("home").toString()), tr("Page d'accueil")); } }
	else { tabs->addTab(makeTab(settings->value("home").toString()), tr("Page d'accueil")); }	
	
	// création des paramètres web 
	webSettings = currentPage()->page()->settings();
	webSettings->setFontFamily(QWebSettings::StandardFont, "Courrier");

}

QWidget *mwindow::makeTab(QString url)
{
	QWidget *tab = new QWidget;
  
	QWebView *webView = new QWebView;

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(webView);
	tab->setLayout(layout);

        if (url.left(7) != "http://") { url = "http://" + url; }

	webView->load(QUrl(url));

	connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(changeTitle(QString)));
	connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(changeUrl(QUrl)));
	connect(webView, SIGNAL(iconChanged()), this, SLOT(changeIcon()));
	connect(webView, SIGNAL(loadStarted()), this, SLOT(startLoading()));
	connect(webView, SIGNAL(loadProgress(int)), this, SLOT(runLoading(int)));
	connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(endLoading(bool)));

	return tab;
}

QWebView *mwindow::currentPage()
{
	return tabs->currentWidget()->findChild<QWebView *>();
}

void mwindow::load()
{
	if (tabs->count() > 0) { QString url = adressBar->text();
		
		if (url.left(7) == "http://") {}
		else if (url.left(6) == "ftp://") {}
		else if (url.left(8) == "https://") {}
		else { url = "http://" + url;
			adressBar->setText(url); }

		currentPage()->load(QUrl(url)); }

	else { QString url = adressBar->text();
		if (url.left(7) != "http://") { url = "http://" + url;
			adressBar->setText(url); }
		tabs->addTab(makeTab(url), ""); } 
}

void mwindow::switchTab(int index)
{
	changeUrl(currentPage()->url());
	changeTitle(currentPage()->title());

	webSettings = currentPage()->page()->settings();
}

void mwindow::changeUrl(const QUrl & url)
{
	adressBar->setText(url.toString());
}

void mwindow::changeTitle(const QString & title)
{
	QString newTitle = title;
	if (title.size() > 30) { newTitle = title.left(30) + "..."; }

	setWindowTitle(title + " - " + "Navigateur web Magellan");
	tabs->setTabText(tabs->currentIndex(), newTitle);
}

void mwindow::changeIcon()
{    

}

void mwindow::startLoading()
{
	progressBar->setVisible(true);
	stop->setEnabled(true);
}

void mwindow::runLoading(int loading)
{
	progressBar->setValue(loading);
}

void mwindow::endLoading(bool ok)
{
	progressBar->setVisible(false);
	statusBar->showMessage(tr("Pret"), 2000);
	stop->setEnabled(false);
	
	QUrl currentUrl = currentPage()->url();
	QIcon tabIcon = webSettings->iconForUrl(currentUrl);
	tabs->setTabIcon(tabs->currentIndex(), tabIcon);
	
	changeUrl(currentPage()->url());
	changeTitle(currentPage()->title());
	
	addToHistory(currentPage()->title(), currentPage()->url().toString());
}

void mwindow::addTab()
{
	QString homeUrl = settings->value("home").toString();
	int tabIndex = tabs->addTab(makeTab(homeUrl), tr("Page d'accueil - Navigateur web Magellan"));
	tabs->setCurrentIndex(tabIndex);
	webSettings = currentPage()->page()->settings();
}	

void mwindow::tabClose()
{
        tabs->removeTab(tabs->currentIndex());
}

void mwindow::openFile() 
{
	QFileDialog *dialog = new QFileDialog;
	QString fileName;
	fileName = dialog->getOpenFileName(this, "Ouvrir un fichier...", "/home", tr("Pages web (*.html, *.htm, *.php)"));
	currentPage()->load(fileName);
}

void mwindow::openWin()
{
	mwindow *magellan = new mwindow();
	magellan->show();
}

void mwindow::pageOpen() 
{
	adressBar->setFocus(Qt::OtherFocusReason);
}

void mwindow::printPage() 
{
	QPrinter *printer = new QPrinter;
	currentPage()->print(printer);
}

void mwindow::exit() 
{
	qApp->quit();
}

void mwindow::backPage()
{
	currentPage()->back();
}

void mwindow::forwardPage()
{
	currentPage()->forward();
}

void mwindow::reloadPage()
{
	currentPage()->reload();
}

void mwindow::goHome()
{
	QString homeUrl = settings->value("home").toString();
	currentPage()->load(QUrl(homeUrl));

}

void mwindow::loadUrl(QString url)
{
	currentPage()->load(url);
}

void mwindow::aboutIt()
{
	QMessageBox::information(this, tr("A propos de Magellan..."), tr("Magellan 1.0 Alpha : Magellan est le navigateur concu pour l'environnement de bureau Panache-DE du projet Logram. Magellan est un logiciel libre sous licence GNU GPL. Rendez vous sur <a href='http://www.logram-project.org/'>site du projet</a> pour plus d'informations."));
}

void mwindow::aboutLogram()
{
	QMessageBox::information(this, tr("A propos de Logram..."), tr("Bla bla bla"));
}

void mwindow::homeChange()
{
	QString newHome = homeOpenBox();
	settings->setValue("home", newHome);
}

QString mwindow::homeOpenBox()
{
	bool ok = false;
	QString newHome = QInputDialog::getText(this, tr("Nouvelle page d'accueil"), tr("Entrez l'adresse URL de votre nouvelle page d'accueil"), QLineEdit::Normal, QString(), &ok);
 
	if (ok) { }
	else { }
  
	return newHome;
}

void mwindow::stopLoading()
{
	currentPage()->stop();	
	statusBar->showMessage(tr("Arrete"), 2000);
}

void mwindow::closeThisTab(int index)
{
	tabs->removeTab(index);
}

void mwindow::openSettings()
{
	settingsWindow = new QMainWindow();
	settingsWindow->setWindowFlags(Qt::Tool);
	settingsWindow->setFixedSize(650, 475);
	settingsWindow->setWindowTitle(tr("Preferences de Magellan"));
	QGridLayout *layout = new QGridLayout();
	
	QLabel *start = new QLabel(tr("Demarrage"));
	QLabel *atStart = new QLabel(tr("Quand j'ouvre le navigateur : "));
	QComboBox *startComboBox = new QComboBox();
		startComboBox->addItem(tr("Afficher ma page d'accueil"));
		startComboBox->addItem(tr("Afficher une page blanche"));
		startComboBox->addItem(tr("Afficher les dernieres pages visitees"));
	int startup = settings->value("startup").toInt();
		startComboBox->setCurrentIndex(startup);
		connect(startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(defineStart(int)));
	startup = startComboBox->currentIndex();
	QLabel *homeLabel = new QLabel(tr("Page d'accueil"));
	homeLineEdit = new QLineEdit();
		homeLineEdit->setText(settings->value("home").toString());
		connect(homeLineEdit, SIGNAL(textEdited(QString)), this, SLOT(defineHome(QString)));
	QPushButton *homeCurrentPage = new QPushButton(tr("Page actuelle"));
		connect(homeCurrentPage, SIGNAL(clicked()), this, SLOT(defineHomeCurrentPage()));
	QPushButton *homeBlankPage = new QPushButton(tr("Page blanche"));
		connect(homeBlankPage, SIGNAL(clicked()), this, SLOT(defineHomeBlankPage()));
	QPushButton *homeDefaultPage = new QPushButton(tr("Par defaut"));
		connect(homeDefaultPage, SIGNAL(clicked()), this, SLOT(defineHomeDefaultPage()));
		layout->addWidget(start, 1, 1);
		layout->addWidget(atStart, 2, 1);
		layout->addWidget(startComboBox, 2, 2, 1, 3);
		layout->addWidget(homeLabel, 3, 1);
		layout->addWidget(homeLineEdit, 3, 2, 1, 3);
		layout->addWidget(homeCurrentPage, 4, 2);
		layout->addWidget(homeBlankPage, 4, 3);
		layout->addWidget(homeDefaultPage, 4, 4);
		
	QLabel *font = new QLabel(tr("Polices"));
	QLabel *fontLabel = new QLabel(tr("Police par defaut : "));
	QFontComboBox *fontComboBox = new QFontComboBox();
	QSpinBox *fontSize = new QSpinBox();
	QPushButton *openDialog = new QPushButton(tr("Plus..."));
		connect(openDialog, SIGNAL(clicked()), this, SLOT(fontDialog()));
		layout->addWidget(font, 6, 1);
		layout->addWidget(fontLabel, 7, 1);
		layout->addWidget(fontComboBox, 7, 2);
		layout->addWidget(fontSize, 7, 3);
		layout->addWidget(openDialog, 7, 4);
		
	


	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	settingsWindow->setCentralWidget(widget);
	settingsWindow->show();	
}

void mwindow::googleSearch()
{
	QString search = googleBar->text();
	search.replace(QString(" "), QString("+"));
	QString languageIndicator = tr("fr");  
	QString searchAdress = "http://www.google." + languageIndicator + "/search?hl=fr&q=" + search + "&btnG=Recherche+Google&meta=&aq=f&oq=";
	currentPage()->load(QUrl(searchAdress));
}

void mwindow::startGoogleSearch()
{
	googleBar->setFocus(Qt::OtherFocusReason);
	googleBar->setText("");
}

void mwindow::searchInText()
{
	QString word = googleBar->text();
	bool notFound = currentPage()->findText(word, QWebPage::FindWrapsAroundDocument);

	if (notFound==0) { QMessageBox::warning(this, tr("Recherche"), tr("Aucune occurence de ce mot trouvee")); }
}

void mwindow::closeEvent(QCloseEvent *event)
{
	// while closing - save the size and the position of the main window
	int windowHeight = this->height();
	int windowWidth = this->width();
	int posx = this->x();
	int posy = this->y();
	bool isMaximized = this->isMaximized();
	settings->setValue("MainWindow/height", windowHeight);
	settings->setValue("MainWindow/width", windowWidth);
	settings->setValue("MainWindow/isMaximized", isMaximized);
	settings->setValue("MainWindow/positionX", posx);
	settings->setValue("MainWindow/positionY", posy);
	int startup = settings->value("startup").toInt();
		if (startup == 2) { int tabsNumber = tabs->count();
					       int tabIndex = 0;
					       QString tab = "0";
					       settings->remove("savedTabs");
		                               while (tabIndex < tabsNumber) {
						        tabs->setCurrentIndex(tabIndex);
							QString tabUrl = currentPage()->url().toString();
						        settings->setValue("savedTabs/tabsNumber", tabsNumber);
							settings->setValue("savedTabs/tabIndex", tabIndex);
							tab = "tab" + settings->value("savedTabs/tabIndex").toString(); 
							settings->setValue("savedTabs/tab" + tab, tabUrl);
							settings->remove("savedTabs/tabIndex"); 
							tabIndex++; }}
		else { settings->remove("savedTabs"); }
	event->accept();
}

void mwindow::readSavedTabs()
{
	int tabsNumber = settings->value("savedTabs/tabsNumber").toInt();
	int tabIndex = 0;
	QString tab = "0";
	while (tabIndex < tabsNumber) {
		settings->setValue("savedTabs/tabsNumber", tabsNumber);
		settings->setValue("savedTabs/tabIndex", tabIndex);
		tab = "tab" + settings->value("savedTabs/tabIndex").toString();
		QString url = settings->value("savedTabs/tab" + tab).toString();
		tabs->addTab(makeTab(url), tr(""));
		tabIndex++; }
}

void mwindow::updateBookmarks(QMenu *menu)
{
        Signet *sign;

        LConfig set("MagellanBookmarks");

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

                QAction *act = menu->addAction(QIcon(""), sign->title, this, SLOT(bookmarkClicked(bool)));

		signets.insert(act, sign);

                set.endGroup();
        }

}

void mwindow::bookmarkClicked(bool)
{
        //Afficher la page actuelle des bookmarks
        QAction *act = static_cast<QAction *>(sender());

        Signet *sign = signets.value(act);

        QWebView *web = currentPage();

        web->load(sign->url);
}

void mwindow::addBookmark(bool)
{
        //Ajouter dans les bookmarks le répertoire actuel
        QWebView *web = currentPage();

        QString url   = web->url().toString();
	QString title = web->title();

        LConfig set("MagellanBookmarks");

        set.beginGroup(title);
        set.setValue("Title", title);
        set.setValue("Url", url);
        set.endGroup();

        set.sync();

        updateBookmarks(bookmarks);
}
