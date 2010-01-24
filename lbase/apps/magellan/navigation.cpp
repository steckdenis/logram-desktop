#include "mwindow.h"

/* file providing overloaded high-level functions */

void mwindow::back() {
    currentPage()->back(); }
void mwindow::forward() {
    currentPage()->forward(); }
void mwindow::refresh() {
    currentPage()->reload(); }
void mwindow::stop() {
    currentPage()->stop(); }
void mwindow::home() {
   /* currentPage()->load(settings->value("home").toString()); */ }
void mwindow::loadingStarted() {
    bar->setValue(0); }
void mwindow::loadingProgress(int value) {
    bar->setValue(value); }
void mwindow::loadingFinished(bool ok) {
/*
    QIcon ic = favicon(QString());
    if(ic.isNull())
        ic = QIcon();
    QPixmap pix = ic.pixmap(32, 32);
    QTransform trans;
    trans.rotate(90);
    trans.scale(32, 32);
    QIcon icon(pix.transformed(trans));
*/
    bar->setValue(0); }
QUrl mwindow::parseUrl(QUrl url)
{
    if(url.toString().startsWith("http://", Qt::CaseSensitive)) {}
    else if(url.toString().startsWith("https://", Qt::CaseSensitive)) {}
    else if(url.toString().startsWith("ftp://", Qt::CaseSensitive)) {}
    else if(url.toString().startsWith("www", Qt::CaseSensitive)) url = QUrl("http://" + url.toString());
    else url = QUrl("http://www.google.fr/search?hl=fr&q=" + url.toString() + "&btnG=Recherche+Google&meta=&aq=f&oq=");

    return url;
}
