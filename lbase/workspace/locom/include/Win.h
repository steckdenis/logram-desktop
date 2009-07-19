/*
 * Win.h
 * This file is part of Logram
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Logram is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Logram; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */
 
#ifndef __WIN_H__
#define __WIN_H__

#include <QWidget>
#include <QHash>

#include <LAppMenu.h>

class LIconLoader;
class View;
class ItemView;
class QCheckBox;

class Win : public QWidget
{
        Q_OBJECT
        
        public:
                Win();
                
                QCheckBox *addChkCat(LAppMenu::Menu *menu);
                
        private slots:
                void chkClicked(bool toggled);
                
        private:
                LIconLoader *iloader;
                LAppMenu *appmenu;
                View *view;
                
                QHash<QCheckBox *, ItemView *> chkItems;        //Chaque checkbox contrôle l'affiche d'un item
};

#endif /* __WIN_H__ */

