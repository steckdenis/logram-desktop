/*
 * Win.h
 * This file is part of BLoQ (Logram)
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *               2010 - Flavien Lefebvre <flavien-lefebvre@hotmail.fr>
 *
 * BLoQ (Logram) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * BLoQ (Logram) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BLoQ (Logram); if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef __WIN_H__
#define __WIN_H__

#include <QMainWindow>
#include <QHash>
#include <QDockWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QLabel>

#include <LConfig.h>
#include <LDirBar.h>

class LIconLoader;
class QMenu;
class QAction;
class QUrlInfo;

class Win : public QMainWindow
{
        Q_OBJECT

public:
        Win(QString path = QString());

        void addTab(QString path);
        void displayInfos(QUrlInfo *info);

private slots:
        void closeTab(int);
        void changeTab(int);

        void back();
        void next();
        void cdup();
        void cdhome();

        void open(bool);
        void newTab(bool);
        void newWin(bool);

        void quit(bool);

        void copy(bool);
        void paste(bool);

        void showHidden(bool toggled);

        void aboutBLoQ(bool);

        void bookmarkClicked(bool);
        void addBookmark(bool);

        void dirChanged(const QString& dir);
        void itemClicked(const QModelIndex& index);

        void toggleIcon(bool);
        void toggleList(bool);
        void toggleDetails(bool);
        void toggleTree(bool);

private:
        void createMenus();
        void createTools();

        void restoreState();
        void saveState();
        void updateBookmarks(QMenu *menu);
        void updateUndoButtons();

        inline LDirView *currentDV();

        struct Signet
        {
                QString title;
                QString url;
        };

        LConfig     *cfg;
        LIconLoader *iloader;
        LDirBar     *ldbar;
        QIcon       iconDir;

        QAction *mback, *mnext, *mup, *mshowHidden;

        QTabWidget     *tabs;
        QDockWidget    *trees, *infos, *actions;
        QStackedWidget *treesstack;
        QStackedWidget *actionsstack;
        QLabel         *lblInfos;
        QMenu          *mnuBookmarks;

        QHash<QAction *, Signet *> signets;
};

#endif /* __WIN_H__ */
