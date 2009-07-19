/*
 * View.cpp
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
 
#include <View.h>

#include <LDesktopFile.h>

#include <QLabel>
#include <QFont>
#include <QListWidgetItem>
#include <QFrame>
#include <QProcess>

View::View(QWidget *parent) : QScrollArea(parent)
{
        //Pour être joli, le fond doit être blanc (en tous cas la couleur de base)
        setBackgroundRole(QPalette::Base);
	setForegroundRole(QPalette::Text);

        //Créer le widget qui affichera les contenus
        widget = new QWidget(this);
        setWidget(widget);
        setWidgetResizable(true);
        
        //Créer le layout du widget
        mlayout = new QVBoxLayout(widget);
        widget->setLayout(mlayout);
}

ItemView *View::addItem(LAppMenu::Menu *menu)
{
        //Créer le widget qui va contenir l'entrée
        QWidget *entryWidget = new QWidget();
        
        //Créer l'icône du widget
        QLabel *lblIcon = new QLabel(entryWidget);
        lblIcon->resize(22, 22);
        lblIcon->setPixmap(menu->icon.pixmap(22, 22));
        
        //Créer le texte de titre
        QLabel *lblTitle = new QLabel(entryWidget);
        lblTitle->setText(menu->title);
        lblTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        lblTitle->setAlignment(Qt::AlignHCenter);
        QFont fnt = lblTitle->font();
        fnt.setBold(true);
        lblTitle->setFont(fnt);
        
        //Créer le conteneur des icônes
        RowIconView *iconView = new RowIconView(this);
        
        //Créer la barre de séparation
        QFrame *bar = new QFrame(entryWidget);
        bar->setFrameShape(QFrame::HLine);
        
        //Remplir le RowIconView
        foreach(LAppMenu::Entry *entry, menu->entries)
        {
                QListWidgetItem *item = new QListWidgetItem(entry->icon, entry->title, iconView);
                iconView->addItem(item);
                
                LDesktopFile fl(entry->desktopFile);
                
                cmds.insert(item, fl.exec());
        }
        
        //Connecter un signal
        connect(iconView, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(itemActivated(QListWidgetItem *)));
        
        //Organiser le tout dans des Layouts
        QHBoxLayout *titleLayout = new QHBoxLayout();
        titleLayout->setSpacing(10);
        titleLayout->addWidget(lblIcon);
        titleLayout->addWidget(lblTitle);
        
        QVBoxLayout *widgetLayout = new QVBoxLayout(entryWidget);
        widgetLayout->addLayout(titleLayout);
        widgetLayout->addWidget(bar);
        widgetLayout->addWidget(iconView);
        widgetLayout->setSpacing(0);
        
        entryWidget->setLayout(widgetLayout);
        
        //Ajouter le widget dans le layout principal
        mlayout->addWidget(entryWidget);
        
        //Créer le ViewEntry
        ItemView *ventry = new ItemView;
        ventry->visible = true;
        ventry->widget = entryWidget;
        
        items.append(ventry);
        
        return ventry;
}

void View::itemActivated(QListWidgetItem *item)
{
        QString cmd = cmds.value(item);
        
        QProcess::startDetached(cmd);
}
