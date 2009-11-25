#include "mwindow.h"

void mwindow::copy() {
    currentPage()->triggerPageAction(QWebPage::Copy); }

void mwindow::cut() {
    currentPage()->triggerPageAction(QWebPage::Cut); }

void mwindow::paste() {
    currentPage()->triggerPageAction(QWebPage::Paste); }

void mwindow::contextMenu(QPoint position)
{
    QMenu *contextMenu = new QMenu();

    QAction *back = contextMenu->addAction(QIcon(iconPath + "back.png"), tr("Pr�c�dente"), this, SLOT(back()));
    QAction *forward = contextMenu->addAction(QIcon(iconPath + "forward.png"), tr("Suivante"), this, SLOT(forward()));
    contextMenu->addAction(reload);
    contextMenu->addSeparator();
    QAction *cut = contextMenu->addAction(QIcon(iconPath + "cut.png"), tr("Couper"), this, SLOT(cut()));
    QAction *copy = contextMenu->addAction(QIcon(iconPath + "copy.png"), tr("Copier"), this, SLOT(copy()));
    QAction *paste = contextMenu->addAction(QIcon(iconPath + "paste.png"), tr("Coller"), this, SLOT(paste()));
    QAction *find = contextMenu->addAction(QIcon(iconPath + "find.png"), tr("Rechercher"), this, SLOT(find()));
    QAction *print = contextMenu->addAction(QIcon(iconPath + "print.png"), tr("Imprimer"), this, SLOT(print()));

    contextMenu->popup(position);
}


