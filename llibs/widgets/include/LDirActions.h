/*
 * LDirActions.h
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
 
#ifndef __LDIRACTIONS_H__
#define __LDIRACTIONS_H__

#include <QWidget>
#include <QList>
#include <QPixmap>
#include <QScrollArea>
#include <QBoxLayout>
#include <QHash>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QIcon>

#include <LDirView.h>

class IActionPlugin;
class ActionSideBar;
class ActionPluginInfo;
class LDirActionItem;

/**
 * \class LDirActions
 * \brief Affiche les actions associées à un fichier ou dossier
 *
 * Affiche une liste dynamique d'actions qui peuvent être appliquées sur un fichier. Les actions sont sous la forme de IActionPlugin.
 *
 */
class LDirActions : public QScrollArea
{
        Q_OBJECT
        
        public:
                /**
                 * \brief Construit un LDirActions
                 * \param dv LDirView qui fournit la liste des actions (toutes les classe LDir*** sont dépendants d'un LDirView)
                 * \param direction Direction du Layout
                 * \param parent QWidget parent
                 */
                LDirActions(LDirView *dv, QBoxLayout::Direction _direction=QBoxLayout::TopToBottom, QWidget *parent=0);
                
                /**
                 * \brief Retourne la direction du layout
                 */
                QBoxLayout::Direction direction() const;
                
                /**
                 * \brief Change la direction du layout
                 */
                void setDirection(QBoxLayout::Direction _direction);
                
                QPixmap pixEntry;       //Pixmap d'une entrée (divisée en 9 zone contigues, 3x3, représentant les angles et les bords)
                QPixmap corner;         //Pixmap à afficher dans l'angle inférieur-gauche
                QBrush  bgColor;        //Couleur du fond
                QColor  linkColor;      //Couleur des liens
                int     titlebarHeight; //Taille de la barre de titre
        
        public slots:
                /**
                 * \brief Actualise le LDirActions
                 *
                 * Récupère la liste des plugins d'actions du LDirView, et affiche leur contenu
                 *
                 */
                void update();
        
        private:
                LDirView *ldv;
                
                QWidget *wg;            //Widget à placer dans le QScrollArea
                QBoxLayout *mlayout;    //Layout dans lequel les entrées sont placées
                
                QHash<IActionPlugin *, LDirActionItem *> items;
};

class LDirActionItem : public QWidget
{
        Q_OBJECT
        
        public:
                LDirActionItem(IActionPlugin *_infos, LDirActions *_acts, QWidget *parent=0);
                
                void update();
                
        private slots:
                void buttonPressed();
                
        protected:
                virtual void paintEvent(QPaintEvent *event);
                
        private:
                IActionPlugin *infos;
                LDirActions *acts;
                
                QVBoxLayout *mlayout;
                QList <ActionSideBar *> actEntries;
                QList <QPushButton *>   btnEntries;
};

class MPushButton : public QPushButton
{
        public:
                MPushButton(LDirActions *_acts, QWidget *parent) : QPushButton(parent)
                {
                        acts = _acts;
                        hover = false;
                }
                
        protected:
                virtual void paintEvent(QPaintEvent *)
                {
                        QPainter painter(this);
                        
                        //Effacer le bouton
                        painter.setBackground(Qt::transparent);
                        painter.eraseRect(0, 0, width(), height());
                        
                        //Dessiner le texte
                        painter.setPen(QPen(acts->linkColor));
                        QFont fnt = font();
                        fnt.setUnderline(hover);
                        painter.setFont(fnt);
                        painter.drawText(5+5+16, 0, (width()-26), height(), Qt::AlignVCenter | Qt::AlignHCenter, text());
                        
                        //Dessiner l'icône
                        QPixmap pix = icon().pixmap(16, 16);
                        painter.drawPixmap(5, (height()-pix.height())/2, pix);
                }
                
                virtual QSize sizeHint() const
                {
                        return QSize(width(), 20);
                }
                
                virtual void enterEvent(QEvent *) { hover = true; repaint(0, 0, width(), height()); }
                virtual void leaveEvent(QEvent *) { hover = false; repaint(0, 0, width(), height()); }
                
        private:
                LDirActions *acts;
                bool hover;
};

class LDWidget : public QWidget
{
        public:
                LDWidget(LDirActions *_acts) : QWidget(_acts)
                {
                        acts = _acts;
                }
                
        protected:
                virtual void paintEvent(QPaintEvent *event);
                
        private:
                LDirActions *acts;
};

#endif /* __LDIRACTIONS_H__ */

