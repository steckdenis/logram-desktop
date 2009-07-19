/*
 * Win.cpp
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

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

#include <Win.h>
#include <View.h>

#include <LIconLoader.h>
#include <LAppMenu.h>

Win::Win() : QWidget(0)
{
        iloader = new LIconLoader(QString(), this);
        appmenu = new LAppMenu("/etc/xdg/menus/logram-system-settings.menu", this);
        
        setWindowTitle(tr("Logram Configuration Manager"));
        setWindowIcon(iloader->icon("logram-settings"));                //TODO: Quand Logram aura son propre thème, cette icône existera
        
        /* Créer la GUI */
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        
        //Créer la liste des catégories
        QVBoxLayout *ccontentsLayout = new QVBoxLayout();       //Le layout pour le contenu du GroupBox
        QVBoxLayout *catLayout = new QVBoxLayout();             //Le layout qui place le GroupBox au bon endroit
        QGroupBox *gbox = new QGroupBox(tr("Catégories"), this);
        
        gbox->setLayout(ccontentsLayout);
        
        catLayout->addWidget(gbox);
        catLayout->addStretch();
        
        // Créer la vue
        view = new View(this);
        
        // Ajouter au layout
        mainLayout->addLayout(catLayout);
        mainLayout->addWidget(view);
        
        
        //Explorer les sous-menus du menu principal (catégories)
        LAppMenu::Menu *root = appmenu->root();
        
        foreach(LAppMenu::Menu *menu, root->menus)
        {
                //Ajouter une checkbox de catégorie
                QCheckBox *chk = addChkCat(menu);
                
                //Ajouter un élément dans la vue
                ItemView *vi = view->addItem(menu);
                
                //Remplir le tableau
                chkItems.insert(chk, vi);
                
                //Ajouter le checkbox au widget
                ccontentsLayout->addWidget(chk);
        }
        
        //Ajouter un spacer au layout de la vue
        view->mlayout->addStretch();
}

QCheckBox *Win::addChkCat(LAppMenu::Menu *menu)
{
        QCheckBox *chk = new QCheckBox(this);
        
        chk->setText(menu->title);
        chk->setChecked(true);
        
        connect(chk, SIGNAL(clicked(bool)), this, SLOT(chkClicked(bool)));
        
        return chk;
}

void Win::chkClicked(bool toggled)
{
        //Obtenir l'enregistrement d'entrée dans la vue correspondant au checkbox
        ItemView *item = chkItems.value(static_cast<QCheckBox *>(sender()));
        
        item->widget->setVisible(toggled);
}
