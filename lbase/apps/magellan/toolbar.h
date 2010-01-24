#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <LAdressBar.h>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QHBoxLayout>
#include <QWidget>

/* Signals from ToolBar public class
   - back : emitted when the "back" button is pressed
   - next : emitted when the "next" button is pressed
   - reload : emitted when the "reload" button is pressed
   - go : emitted either when the "go" button is pressed either when the "Enter" key is pressed in the adress bar
 * Public slots
   - setText : sets the text of the LAdressBar. Takes a QString as parameter
   - setUrl : sets the URL of the LAdressBar. Takes a QUrl as parameter
 */

class ToolBar : public QToolBar
{ 
public:
    ToolBar();
    QString text();
    QUrl url();
public slots:
    void setText(QString);
    void setUrl(QUrl);
    void setValue(int);
signals:
    void back();
    void next();
    void reload();
    void go();
private:
    QHBoxLayout *stdBtns;
        QPushButton *backBtn, *nextBtn, *reloadBtn;
    QPushButton *bookmarksBtn;
        QMenu *bookmarks;
    LAdressBar *adressBar;
    void styleSheets();
};

#endif // TOOLBAR_H
