/*
 * LDirActions.cpp
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

#include <QPalette>
#include <QPainter>

#include <LDirActions.h>

#include "../plugins/actions/include/IActionPlugin.h"

#include <LConfig.h>

#include <QtDebug>

LDirActions::LDirActions(LDirView *dv, QBoxLayout::Direction _direction, QWidget *parent) : QScrollArea(parent)
{
        ldv = dv;
        
        // Charger la configuration nécessaire
        QString theme;
        theme          = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
        bgColor        = QBrush(QColor(LConfig::logramValue("DirActionsWidget/BackgroundColor", "#aaaaff", "Theme").toString()));
        linkColor      = QColor(LConfig::logramValue("DirActionsWidget/LinksColor", "#0000ff", "Theme").toString());
        pixEntry       = QPixmap(theme + LConfig::logramValue("DirActionsWidget/EntryPixmap", "ldiractions_entry.png", "Theme").toString());
        corner         = QPixmap(theme + LConfig::logramValue("DirActionsWidget/CornerPixmap", "ldiractions_corner.png", "Theme").toString());
        titlebarHeight = LConfig::logramValue("DirActionsWidget/TitlebarHeight", 20, "Theme").toInt();
        
        // Créer le widget à placer au centre
        wg = new LDWidget(this);
        setWidget(wg);
        setWidgetResizable(true);
        
        // Créer le layout qui contiendra les entrées
        mlayout = new QBoxLayout(_direction, wg);
        wg->setLayout(mlayout);
        wg->setAttribute(Qt::WA_NoSystemBackground, true);
        
        mlayout->addStretch();  //Espace après les éléments
        
        // Connecter les signaux nécessaires
        connect(dv, SIGNAL(updateActions()), this, SLOT(update()));     //itemClicked() est émis quand on clique sur un élément ou quand on change de dossier
        
        // Actualiser une première fois
        update();
}

void LDirActions::update()
{
        QList<IActionPlugin *> *actions = ldv->actions();
        
        //Explorer les actions
        for (int i=0; i<actions->count(); i++)
        {
                IActionPlugin *plugin = actions->at(i);
                
                //Ajouter l'action à la liste si elle n'y est pas
                if ((!items.contains(plugin)) && ((plugin->info()->flags & IActionPlugin::ActionShowInSideBar) != 0))
                {
                        
                        LDirActionItem *item = new LDirActionItem(plugin, this, wg);
                        
                        items.insert(plugin, item);
                        
                        mlayout->insertWidget(mlayout->count()-1, item);
                }
        }
        
        //Supprimer les actions dont on n'a plus besoin
        foreach (IActionPlugin *plug, items.keys())
        {
                if ((!actions->contains(plug)) || ((plug->info()->flags & IActionPlugin::ActionShowInSideBar) == 0))
                {
                        //Supprimer l'action dont on n'a plus besoin
                        LDirActionItem *item = items.take(plug);
                        
                        delete item;
                }
                else
                {
                        //Mettre à jour le plugin
                        LDirActionItem *item = items.value(plug);
                        
                        item->update();
                }
        }
}

void LDWidget::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        
        //Remplir le fond
        painter.setBackground(acts->bgColor);
        painter.eraseRect(0, 0, width(), height());
        
        //Dessiner l'image dans l'angle
        painter.drawPixmap(0, height()-(acts->corner.height()), acts->corner);
        
        QWidget::paintEvent(event);
}

QBoxLayout::Direction LDirActions::direction() const
{
        return mlayout->direction();
}

void LDirActions::setDirection(QBoxLayout::Direction _direction)
{
        mlayout->setDirection(_direction);
}

/*
        Classe LDirActionItem
*/

LDirActionItem::LDirActionItem(IActionPlugin *_infos, LDirActions *_acts, QWidget *parent) : QWidget(parent)
{
        infos = _infos;
        acts  = _acts;
        
        // Créer le layout
        mlayout = new QVBoxLayout(this);
        mlayout->setContentsMargins(5, acts->titlebarHeight, 5, 5);
        mlayout->setSpacing(0);
        setLayout(mlayout);
        
        setAttribute(Qt::WA_NoSystemBackground, true);
}

void LDirActionItem::update()
{
        //Explorer les sous-éléments de l'action et ajouter ceux qui manquent au layout
        QList<ActionSideBar *> *actions = infos->sidebarActions();
        
        int count = 0;      //Nombre d'actions (pour la troncation)
        
        for (int i=0; i<actions->count(); i++)
        {
                ActionSideBar *act = actions->at(i);
                
                if (act->visible)
                {
                        QPushButton *btn;
                        
                        //Si on ne l'a pas déjà, l'ajouter
                        if (count >= btnEntries.count())
                        {
                                btn = new MPushButton(acts, this);
                                
                                mlayout->addWidget(btn);
                                
                                connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonPressed()));
                                
                                actEntries.append(act);
                                btnEntries.append(btn);
                        }
                        else
                        {
                                btn = btnEntries.at(count);
                        }
                        
                        if (!act->pixmap.isNull())
                        {
                                btn->setIcon(QIcon(act->pixmap));
                        }
                        
                        btn->setText(act->title);
                        btn->setToolTip(act->description);
                        
                        count++;
                }
        }
        
        //On peut supprimer tous les boutons qui ne servent à rien
        if (count < btnEntries.count())
        {
                for (int i=count; i<btnEntries.count(); i++)
                {
                        //On retire tout ce qui suit action->count() (qui est forcément plus petit, voir for() plus haut)
                        QPushButton *btn = btnEntries.takeAt(i);
                        
                        delete btn;
                        
                        actEntries.removeAt(i);
                        
                        i--;    //Un bouton en moins, donc count() a diminué
                }
        }
}

void LDirActionItem::buttonPressed()
{
        QPushButton *btn = static_cast<QPushButton *>(sender());
        
        //Trouver l'index du bouton cliqué
        ActionSideBar *act =  actEntries.at(btnEntries.indexOf(btn));
        
        infos->sidebarClicked(act->id);
}

void LDirActionItem::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        
        //Dessiner les angles
        int ps = acts->pixEntry.width()/3;
        
        painter.drawPixmap(0, 0, ps, ps, acts->pixEntry, 0, 0, ps, ps);
        painter.drawPixmap(width()-ps, 0, ps, ps, acts->pixEntry, ps*2, 0, ps, ps);
        
        if (actEntries.count() != 0)
        {
                //Ces angles et bords ne doivent pas être dessinés quand l'Action est vide
                painter.drawPixmap(0, height()-ps, ps, ps, acts->pixEntry, 0, ps*2, ps, ps);
                painter.drawPixmap(width()-ps, height()-ps, ps, ps, acts->pixEntry, ps*2, ps*2, ps, ps);
                
                //Dessiner les bords
                painter.drawPixmap(0, ps, ps, height()-(2*ps), acts->pixEntry, 0, ps, ps, ps);                  //Gauche
                painter.drawPixmap(width()-ps, ps, ps, height()-(2*ps), acts->pixEntry, 2*ps, ps, ps, ps);      //Droite
                painter.drawPixmap(ps, height()-ps, width()-(2*ps), ps, acts->pixEntry, ps, 2*ps, ps, ps);      //Bas
                
                //Dessiner le centre
                painter.drawPixmap(ps, ps, width()-(2*ps), height()-(2*ps), acts->pixEntry, ps, ps, ps, ps);      //Centre
        }
        
        painter.drawPixmap(ps, 0, width()-(2*ps), ps, acts->pixEntry, ps, 0, ps, ps);                           //Haut (hors du if car c'est la barre de titre
        
        //Dessiner l'icône de l'entrée
        ActionPluginInfo *inf = infos->info();
        
        int sz = inf->pixmap.width();
        painter.drawPixmap(2, (ps-sz)/2, sz, sz, inf->pixmap);
        
        //Dessiner le titre
        painter.drawText(sz+4, 2, width()-(sz+6), sz, Qt::AlignVCenter | Qt::AlignHCenter, inf->title);
        
        QWidget::paintEvent(event);
}
