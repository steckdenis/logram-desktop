#ifndef TABS_H
#define TABS_H

#include <QTabWidget>
#include "webview.h"
#include "mwindow.h"

class Tabs : public QTabWidget
{
public:
    Tabs(mwindow *mparent);
    WebView *page();
public slots:
    void makeTab(QUrl);
private:
    mwindow *parent;
};

#endif // TABS_H
