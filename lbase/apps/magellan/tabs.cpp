#include "tabs.h"

Tabs::Tabs(mwindow *mparent)
{
    parent = mparent;

    //set object properties
#if QT_VERSION >= 0x040500
    setMovable(true);
#endif
    setTabPosition(QTabWidget::West);

/*
    QPushButton *newTabButton = new QPushButton(QIcon(iconPath + "add.png"), QString());
    newTabButton->setToolTip(tr("Nouvel onglet"));
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(addTab()));
    tabs->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));
*/
    setStyleSheet("QTabWidget::pane { \n"
                  "   border: 1px solid rgb(180, 180, 180);\n"
                  "   left: 0.1ex;\n"
                  " }\n"
                  "QTabWidget::tab-bar {\n"
                  "   alignment: center;\n"
                  " }\n"
                  "QTabBar::tab {\n"
                  "   background: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:0, stop:0 rgba(180, 180, 180, 255), stop:1 rgba(230, 230, 230, 255));\n"
                  "   border: 1px solid rgb(180, 180, 180);\n"
                  "   border-right-color: rgb(180, 180, 180);\n"
                  "   min-width: 8ex;\n"
                  "   padding: 2px;\n"
                  " }\n"
                  "QTabBar::tab:selected, QTabBar::tab:hover {\n"
                  "   background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 rgb(255, 255, 255, , 255), stop: 1.0 rgb(230, 230, 230));\n"
                  " }\n"
                  "\n"
                  "QTabBar::tab:hover {\n"
                  "}\n"
                  "QTabBar::tab:!selected {\n"
                  "   margin-left: 2px; \n"
                  " }\n"
                  " QTabBar::tab:selected {\n"
                  "   margin-top: -2px;\n"
                  "   margin-bottom: -2px;\n"
                  "}\n"
                  "QTabBar::tab:first:selected {\n"
                  "   margin-top: 0; \n"
                  " }\n"
                  "\n"
                  "QTabBar::tab:last:selected {\n "
                  "   margin-bottom: 0; \n"
                  " }\n"
                  "\n"
                  "QTabBar::tab:only-one {\n"
                  "   margin: 0; \n"
                  " }");
    setIconSize(QSize(32, 32));
    connect(this, SIGNAL(currentChanged(int)), parent, SLOT(tabChanged(int)));
}

WebView *Tabs::page()
{
    return this->currentWidget()->findChild<WebView *>();
}

void Tabs::makeTab(QUrl)
{
    WebView *wv = new WebView();
    addTab(wv, QString());
    connect(wv, SIGNAL(titleChanged(QString)), parent, SLOT(changeTitle(QString)));
    connect(wv, SIGNAL(urlChanged(QUrl)), parent, SLOT(changeUrl(QUrl)));
    connect(wv, SIGNAL(loadStarted()), parent, SLOT(loadingStarted()));
    connect(wv, SIGNAL(loadProgress(int)), parent, SLOT(loadProgress(int)));
    connect(wv, SIGNAL(loadFinished(bool)), parent, SLOT(loadingFinished(bool)));
}

