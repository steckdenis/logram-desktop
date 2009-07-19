/*
 * View.h
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
 
#ifndef __VIEW_H__
#define __VIEW_H__

#include <QListWidget>
#include <QScrollArea>
#include <QWidget>
#include <QList>
#include <QVBoxLayout>
#include <QHash>
#include <QEvent>

#include <LAppMenu.h>

#include <QDebug>

/**
 * \struct ItemView
 * \brief Représentation d'une catégorie dans le View
 */
struct ItemView
{
        bool visible;    /*!< True pour afficher la catégorie */
        QWidget *widget; /*!< Widget utilisé par View pour affiche le rang */
};

/**
 * \class RowIconView
 * \brief Surcharge QListWidget pour s'afficher correctement
 *
 * NOTE : Cette classe vient de KDE
 *
 */
class RowIconView : public QListWidget
{
        public:
                RowIconView( QWidget* parent ) : QListWidget( parent )
                {
                        setResizeMode(Adjust);
                        setViewMode(IconMode);
                        setMovement(Static);
                        setFrameShape(NoFrame);
                        setWordWrap(true);
                        setWrapping(true);
                        
                        setGridSize(QSize(100, 100));
                        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                        
                        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                }
                
        protected:
                bool viewportEvent(QEvent *event)
                {
                        bool rs = QListWidget::viewportEvent(event);
                        
                        if (event->type() == QEvent::Resize)
                        {
                                int itemsize = gridSize().width()+spacing();
                                int itemrows = width()/itemsize;
                                int numrows = count()/itemrows;
                                numrows++;
                                
                                setMinimumHeight((numrows*itemsize)+8);
                                
                                event->accept();
                                
                                return true;
                        }
                        
                        return rs;
                }
};

/**
 * \class View
 * \brief Contrôle qui affiche les catégories de configuration et leur contenu
 */
class View : public QScrollArea
{
        Q_OBJECT
        public:
                View(QWidget *parent=0);                /*!< Constructeur */
                
                ItemView *addItem(LAppMenu::Menu *menu); /*!< Ajoute une catégorie */
                
                QVBoxLayout *mlayout;
                
        private slots:
                void itemActivated(QListWidgetItem *item);
                
        private:
                QList<ItemView *> items;
                QWidget *widget;
                
                QHash<QListWidgetItem *, QString> cmds; //Commandes à éxécuter quand on appuie sur un widget
};


#endif /* __VIEW_H__ */

