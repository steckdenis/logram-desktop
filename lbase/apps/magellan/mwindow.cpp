#include "mwindow.h"
#include <LConfig.h>

mwindow::mwindow()
{
    // création d'un QSettings et récupération du thème
    settings = new QSettings("Logram", "Magellan");
    iconPath = "/usr/share/logram/magellan/icons/";

    // restauration de la fenêtre
    resize(settings->value("MainWindow/width").toInt(), settings->value("MainWindow/height").toInt());
    if(settings->value("MainWindow/isMaximized").toBool()) {}
    else { move(settings->value("MainWindow/positionX").toInt(), settings->value("MainWindow/positionY").toInt()); }
    setWindowTitle(tr("Navigateur web Magellan"));
    setWindowIcon(QIcon("/usr/share/magellan/icons/main.png"));

    adressBar = new QLineEdit();
    connect(adressBar, SIGNAL(returnPressed()), this, SLOT(load()));

    // construction de la barre d'outils
    QToolBar *mainToolBar = addToolBar(tr("Navigation"));
    QAction *back = mainToolBar->addAction(QIcon(iconPath + "back.png"), tr("Page précédente"), this, SLOT(back()));
    QAction *forward = mainToolBar->addAction(QIcon(iconPath + "forward.png"), tr("Page suivante"), this, SLOT(forward()));
    reload = mainToolBar->addAction(QIcon(iconPath + "reload.png"), tr("Actualiser"), this, SLOT(refresh()));
    QAction *home = mainToolBar->addAction(QIcon(iconPath + "home.png"), tr("Page d'accueil"), this, SLOT(home()));
    QAction *bookmarksAct = mainToolBar->addAction(QIcon(iconPath + "bookmarks.png"), tr("Marques-pages"));
        bookmarks = new QMenu(tr("Marques-pages"));
        updateBookmarks(bookmarks);
        bookmarksAct->setMenu(bookmarks);
    mainToolBar->addWidget(adressBar);
    QAction *go = mainToolBar->addAction(QIcon(iconPath + "go.png"), tr("Aller"), this, SLOT(load()));

    QMenu *edit = new QMenu(tr("Édition"));
        QAction *cut = edit->addAction(QIcon(iconPath + "cut.png"), tr("Couper"), this, SLOT(cut()));
        QAction *copy = edit->addAction(QIcon(iconPath + "copy.png"), tr("Copier"), this, SLOT(copy()));
        QAction *paste = edit->addAction(QIcon(iconPath + "paste.png"), tr("Coller"), this, SLOT(paste()));
        edit->addSeparator();
        QAction *print = edit->addAction(QIcon(iconPath + "print.png"), tr("Imprimer la page"), this, SLOT(print()));
        QAction *find = edit->addAction(QIcon(iconPath + "find.png"), tr("Rechercher"), this, SLOT(find()));
    QMenu *navig = new QMenu(tr("Menu"));
        QAction *newWin = navig->addAction(QIcon(iconPath + "win.png"), tr("Nouvelle fenêtre"), this, SLOT(newWin()));
        QAction *newTab = navig->addAction(QIcon(iconPath + "tab.png"), tr("Nouvel onglet"), this, SLOT(addTab()));
        navig->addSeparator();
        QAction *options = navig->addAction(QIcon(iconPath + "configure.png"), tr("Configurer"), this, SLOT(openSettings()));
        QAction *quit = navig->addAction(QIcon(iconPath + "quit.png"),tr("Quitter"), qApp, SLOT(quit()));

    QAction *editAct = mainToolBar->addAction(QIcon(iconPath + "edit.png"), tr("Édition"));
        editAct->setMenu(edit);
    QAction *navigAct = mainToolBar->addAction(QIcon(iconPath + "navig.png"), tr("Navigateur"));
        navigAct->setMenu(navig);

    // construction de la barre d'état et de la barre de progression
    statusBar = new QStatusBar();
    progressBar = new QProgressBar();
    progressBar->setMaximumHeight(20);
    statusBar->addWidget(progressBar);
    setStatusBar(statusBar);

    // construction du système d'onglets
    tabs = new QTabWidget;

#if QT_VERSION >= 0x040500
    tabs->setTabsClosable(true); // icône "fermer" sur les onglets
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab()));
    tabs->setMovable(true); // icône déplaçable
#endif // à condition que Qt 4.5 soit installé

    tabs->setTabPosition(QTabWidget::West);

    QPushButton *newTabButton = new QPushButton(QIcon(iconPath + "add.png"), QString());
    newTabButton->setToolTip(tr("Nouvel onglet"));
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(addTab()));
    tabs->setCornerWidget(newTabButton, Qt::TopLeftCorner);

    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));

    setCentralWidget(tabs);

    // récupération des paramètres de démarrage
    int startup = settings->value("startup").toInt();
    if (startup == 0) { tabs->addTab(makeTab(""), QString()); this->home(); } // 0 : ouvrir la page d'accueil
    else if (startup == 1) { tabs->addTab(makeTab(""), QString()); } // 1 : ouvrir une page vierge
    else if (startup == 2) { readSavedTabs(); if (tabs->count() < 1) { tabs->addTab(makeTab(""), QString()); this->home(); } } // 2 : ouvrir les derniers onglets
    else { tabs->addTab(makeTab(""), QString()); this->home(); } // si erreur dans le fichier de conf, on ouvre la page d'accueil

    // création des paramètres web
    if (!QDir(qApp->applicationDirPath() + "/cache/").exists()) {
        QDir(qApp->applicationDirPath()).mkdir("cache"); }

    QWebSettings::setIconDatabasePath(qApp->applicationDirPath() + "/cache/");

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
    webView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(webView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(changeTitle(QString)));
    connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(changeUrl(QUrl)));
    connect(webView, SIGNAL(loadStarted()), this, SLOT(loadingStarted()));
    connect(webView, SIGNAL(loadProgress(int)), this, SLOT(loadingProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadingFinished(bool)));
    connect(webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(download(QNetworkReply*)));

    return tab;
}

QWebView *mwindow::currentPage()
{
    return tabs->currentWidget()->findChild<QWebView *>();
}

QIcon mwindow::favicon(QString url)
{
    if(url.isNull())
    return QWebSettings::iconForUrl(QUrl(currentPage()->url()));
    else
    return QWebSettings::iconForUrl(url);
}

/* tabs and window management slots */

void mwindow::changeUrl(const QUrl & url)
{
    adressBar->setText(url.toString());
}

void mwindow::changeTitle(const QString & title)
{
    setWindowTitle(title + " - " + "Navigateur web Magellan");
    tabs->setTabToolTip(tabs->currentIndex(), currentPage()->title());
}

void mwindow::switchTab(int index)
{
    changeUrl(currentPage()->url());
    changeTitle(currentPage()->title());

    webSettings = currentPage()->page()->settings();
}

void mwindow::newWin()
{
    mwindow *magellan = new mwindow();
    magellan->show();
}

void mwindow::addTab()
{
    QString homeUrl = settings->value("home").toString();
    int tabIndex = tabs->addTab(makeTab(homeUrl), QString());
    tabs->setCurrentIndex(tabIndex);
}

void mwindow::closeTab()
{
    tabs->removeTab(tabs->currentIndex());
}

/* files management slots */

void mwindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier...", "/home", tr("Pages web (*.html, *.htm, *.php)"));
    currentPage()->load(fileName);
}

void mwindow::openUrl()
{
    adressBar->setFocus(Qt::OtherFocusReason);
    adressBar->selectAll();
}

/* internal web slots */

void mwindow::load()
{
    if (tabs->count() > 0) { QString url = adressBar->text();

        if(url.startsWith("http://", Qt::CaseSensitive)) {}
        else if(url.startsWith("https://", Qt::CaseSensitive)) {}
        else if(url.startsWith("ftp://", Qt::CaseSensitive)) {}
        else if(url.startsWith("www", Qt::CaseSensitive)) { url = "http://" + url; }
        else { url = "http://www.google.fr/search?hl=fr&q=" + url + "&btnG=Recherche+Google&meta=&aq=f&oq=";
            }

        adressBar->setText(url);
        currentPage()->load(QUrl(url)); }

    else { QString url = adressBar->text();

        if(url.startsWith("http://", Qt::CaseSensitive)) {}
        else if(url.startsWith("https://", Qt::CaseSensitive)) {}
        else if(url.startsWith("ftp://", Qt::CaseSensitive)) {}
        else if(url.startsWith("www", Qt::CaseSensitive)) { url = "http://" + url; }
        else { url = "http://www.google.fr/search?hl=fr&q=" + url + "&btnG=Recherche+Google&meta=&aq=f&oq=";
            }

        tabs->addTab(makeTab(url), ""); }
}

void mwindow::loadingStarted()
{
    progressBar->setVisible(true);

    reload->setIcon(QIcon(iconPath + "stop.png"));
    reload->setText(tr("Arrêter"));
    reload->disconnect();
    connect(reload, SIGNAL(triggered()), this, SLOT(stop()));
}

void mwindow::loadingProgress(int loading)
{
    progressBar->setValue(loading);
}

void mwindow::loadingFinished(bool ok)
{
    progressBar->setVisible(false);
    statusBar->showMessage(tr("Terminé"));

    changeUrl(currentPage()->url());
    changeTitle(currentPage()->title());
    QIcon ic = favicon(QString());
    if(ic.isNull())
        ic = QIcon();

    QPixmap pix = ic.pixmap(22, 22);
    QTransform trans;
    trans.rotate(90);
    QIcon icon(pix.transformed(trans));

    tabs->setTabIcon(tabs->currentIndex(), icon);

    reload->setIcon(QIcon(iconPath + "reload.png"));
    reload->setText(tr("Actualiser"));
    reload->disconnect();
    connect(reload, SIGNAL(triggered()), this, SLOT(refresh()));
}

/* edition slots */

void mwindow::print()
{
    QPrinter *printer = new QPrinter;
    currentPage()->print(printer);
}

void mwindow::find()
{
    QString word = QInputDialog::getText(this, tr("Recherche"), tr("Entrez le mot à rechercher"));
    currentPage()->findText(word);
}

void mwindow::about()
{
    QMessageBox::information(this, tr("A propos de Magellan..."), tr("Magellan 1.0 Alpha : Magellan est le navigateur concu pour l'environnement de bureau Panache-DE du projet Logram. Magellan est un logiciel libre sous licence GNU GPL. Rendez vous sur <a href='http://www.logram-project.org/'>site du projet</a> pour plus d'informations."));
}

void mwindow::aboutLogram()
{
    QMessageBox::information(this, tr("A propos de Logram..."), tr("Bla bla bla"));
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

    QAction *add = menu->addAction(QIcon(iconPath + "add.png"), tr("Ajouter un marque-page"), this, SLOT(addBookmark(bool)));
    menu->addSeparator();

    //Lire les entrées des signets
    foreach(QString grp, set.childGroups())
    {
        set.beginGroup(grp);

        sign        = new Signet;
        sign->title = set.value("Title").toString();
        sign->url   = set.value("Url").toString();

        QIcon icon = favicon(sign->url);

        if(icon.isNull()) icon = QIcon();

        QAction *act = menu->addAction(icon, sign->title, this, SLOT(bookmarkClicked(bool)));

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

