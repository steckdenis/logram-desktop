#include "toolbar.h"

ToolBar::ToolBar()
{
    QString ic = "/usr/share/logram/magellan/icons/";
    stdBtns = new QHBoxLayout();
        backBtn = new QPushButton(QIcon(ic + "back.png"), tr(""));
            backBtn->setToolTip(tr("Go to last page in the history"));
            backBtn->setShortcut(QKeySequence(QKeySequence::Back));
            connect(backBtn, SIGNAL(clicked()), this, SIGNAL(back()));
        reloadBtn = new QPushButton(QIcon(ic + "reload.png"), tr(""));
            reloadBtn->setToolTip(tr("Displays and up-to-date version of the current page"));
            reloadBtn->setShortcut(QKeySequence(QKeySequence::Refresh));
            connect(reloadBtn, SIGNAL(clicked()), this, SIGNAL(reload()));
        nextBtn = new QPushButton(QIcon(ic + "forward.png"), tr(""));
            nextBtn->setToolTip(tr("Go to next page in the history"));
            nextBtn->setShortcut(QKeySequence(QKeySequence::Forward));
            connect(nextBtn, SIGNAL(clicked()), this, SIGNAL(next()));
    stdBtns->addWidget(backBtn);
    stdBtns->addWidget(reloadBtn);
    stdBtns->addWidget(nextBtn);
    stdBtns->setSpacing(0);
    stdBtns->setContentsMargins(0, 0, 0, 0);
    QWidget *stdW = new QWidget;
    stdW->setLayout(stdBtns);

addWidget(stdW);

    bookmarksBtn = new QPushButton(QIcon(ic + "bookmarks.png"), tr(""));
        bookmarks = new QMenu(tr("Bookmarks"));
        bookmarksBtn->setMenu(bookmarks);

addWidget(bookmarksBtn);

    adressBar = new LAdressBar();
    connect(adressBar, SIGNAL(triggered()), this, SIGNAL(go()));

addWidget(adressBar);
setStyleSheet("QToolBar { spacing: 3px; }");
styleSheets();

connect(this, SLOT(setText(QString)), adressBar, SLOT(setText(QString)));
connect(this, SLOT(setUrl(QUrl)), adressBar, SLOT(setUrl(QUrl)));
connect(this, SLOT(setValue(int)), adressBar, SLOT(setValue(int)));
}

void ToolBar::styleSheets()
{
    backBtn->setStyleSheet("QPushButton { \n"
                           "border-top-left-radius: 4px; \n"
                           "border-bottom-left-radius: 4px; \n"
                           "background-color: rgb(240, 240, 240); \n"
                           "border: 1px solid gray; \n"
                           "height: 26px; \n "
                           "margin: 3px; \n"
                           "margin-right: 0px; \n } \n"
                           "QPushButton::hover { \n"
                           "background-color: rgb(250, 250, 250);\n } \n"
                           "QPushButton::pressed {\n"
                           "background-color: rgbrgb(255, 255, 255);\n }");
    reloadBtn->setStyleSheet("QPushButton { \n"
                             "background-color: rgb(240, 240, 240); \n"
                             "border: 1px solid gray; \n"
                             "border-left-width: 0px; \n"
                             "border-right-width: 0px; \n"
                             "height: 26px; \n "
                             "margin: 3px; \n"
                             "margin-right: 0px; \n \n"
                             "margin-left: 0px; \n } \n"
                             "QPushButton::hover { \n"
                             "background-color: rgb(250, 250, 250);\n } \n"
                             "QPushButton::pressed {\n"
                             "background-color: rgbrgb(255, 255, 255);\n }");
    nextBtn->setStyleSheet("QPushButton { \n"
                           "border-top-right-radius: 4px; \n"
                           "border-bottom-right-radius: 4px; \n"
                           "background-color: rgb(240, 240, 240); \n"
                           "border: 1px solid gray; \n"
                           "height: 26px; \n "
                           "margin: 3px; \n"
                           "margin-left: 0px; \n } \n"
                           "QPushButton::hover { \n"
                           "background-color: rgb(250, 250, 250);\n } \n"
                           "QPushButton::pressed {\n"
                           "background-color: rgbrgb(255, 255, 255);\n }");
    bookmarksBtn->setStyleSheet("QPushButton { \n"
                                "border-radius: 4px; \n"
                                "background-color: rgb(240, 240, 240); \n"
                                "border: 1px solid gray; \n"
                                "height: 26px; \n "
                                "margin: 3px; \n"
                                "margin-left: 0px; \n } \n"
                                "QPushButton::hover { \n"
                                "background-color: rgb(250, 250, 250);\n } \n"
                                "QPushButton::pressed {\n"
                                "background-color: rgbrgb(255, 255, 255);\n }");


}

QUrl ToolBar::url()
{
    return adressBar->url();
}

QString ToolBar::text()
{
    return adressBar->text();
}
