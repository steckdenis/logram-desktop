#include "mwindow.h"

/* file providing overloaded high-level functions */

void mwindow::back() {
    currentPage()->back(); }

void mwindow::forward() {
    currentPage()->forward(); }

void mwindow::refresh() {
    currentPage()->reload(); }

void mwindow::stop() {
    currentPage()->stop();
    progressBar->setVisible(false);
    statusBar->showMessage(tr("Arrêté"), 2000); }

void mwindow::home() {
    currentPage()->load(settings->value("home").toString()); }
