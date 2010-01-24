#include "mwindow.h"
#include <LConfig.h>
#include "toolbar.h"

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

    // construction de la barre d'outils
    bar = new ToolBar();
    addToolBar(tbar);

    // construction du système d'onglets
    tabs = new Tabs(this);

    // récupération des paramètres de démarrage
    int startup = settings->value("startup").toInt();
    switch (startup) {
        case 0 :
            tabs->addTab(makeTab(QUrl()), QString()); this->home(); // 0 : ouvrir la page d'accueil
            break;
        case 1 :
            tabs->addTab(makeTab(QUrl()), QString()); // 1 : ouvrir une page vierge
            break;
        case 2 :
            readSavedTabs();
            if (tabs->count() < 1)
                tabs->addTab(makeTab(QUrl()), QString()); this->home(); // 2 : ouvrir les derniers onglets
            break;
        default :
            tabs->addTab(makeTab(QUrl()), QString()); this->home(); // si erreur dans le fichier de conf, on ouvre la page d'accueil
	    break; }

    // création des paramètres web
    if (!QDir(qApp->applicationDirPath() + "/cache/").exists()) {
        QDir(qApp->applicationDirPath()).mkdir("cache"); }

    QWebSettings::setIconDatabasePath(qApp->applicationDirPath() + "/cache/");

}

QWebView *mwindow::currentPage()
{
    return tabs->page();
}

void mwindow::changeTitle(const QString & title)
{
    bar->setText(title);
}

void mwindow::changeUrl(const QUrl & url)
{
    bar->setUrl(url);
}

void mwindow::tabChanged(int)
{
    changeTitle(currentPage()->title());
    changeUrl(currentPage()->title());
}

void mwindow::newWin()
{
}

/* files management slots */

void mwindow::open()
{
}

void mwindow::openUrl()
{
}

void mwindow::load()
{
}

void mwindow::about()
{
}

void mwindow::aboutLogram()
{
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

