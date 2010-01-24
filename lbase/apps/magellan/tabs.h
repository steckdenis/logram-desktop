#ifndef TABS_H
#define TABS_H

#include <QTabWidget>
#include "webview.h"
#include "mwindow.h"

class Tabs : public QTabWidget
{
public:
    Tabs();
    WebView *page();
public slots:
    void makeTab(QUrl);
private:
   };

#endif // TABS_H
