/*
 * Runner.cpp
 * This file is part of Locom Runner
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *
 * Locom Runner is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Locom Runner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Locom Runner; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <QStringList>
#include <QFile>
#include <QDir>
#include <QUiLoader>
#include <QWidget>
#include <QMetaObject>
#include <QMetaMethod>
#include <QLocale>
#include <QTranslator>
#include <QApplication>

#include <QtDebug>

#include <Runner.h>
#include <Config.h>

Runner::Runner(const QString &fileName) : QObject(0)
{
        mainWindow = 0;
        
        //Extraire le chemin d'accès du nom de fichier
        QString flname;
        
        if (fileName.startsWith('/'))
        {
                QStringList parts = fileName.split('/');
                parts.removeAt(parts.count()-1);
                path = parts.join("/");
                flname = parts.at(parts.count()-1);
        }
        else
        {
                path = QDir::currentPath();
                flname = fileName;
        }
        
        //Ouvrir le fichier
        QFile fl(fileName);
        if (!fl.exists())
        {
                qFatal("ERROR: File %s cannot be found", qPrintable(fileName));
        }
        
        doc.setContent(&fl);
        
        // Changer de réperoire courant
        QDir::setCurrent(path);
        
        // Si le codeur en a précisé, charger des fichiers de traduction
        QString locale = QLocale::system().name().section('_', 0, 0);
 
        QStringList flparts = flname.split('.');
        flparts.removeAt(flparts.count()-1);
        QString fltitle = flparts.join(".");                    //Retirer l'extension
 
        QTranslator translator;
        translator.load(fltitle + "_" + locale + ".qm");
        
        qApp->installTranslator(&translator);                   //Installer les traductions
        
        // Initialiser le chargeur d'UI
        uiloader = new QUiLoader(this);
        uiloader->setWorkingDirectory(QDir::current());
	
	// Initialiser le moteur de script
	engine = new QScriptEngine(this);
	
	// Exposer le Runner au script
	engine->globalObject().setProperty("runner", engine->newQObject(this));
        
        //Commencer le parsage
        parse(doc.documentElement());
        
        //Appeler resetConfig pour mettre les valeurs par défaut dans les champs
        resetConfig();
        
        //Emettre le signal permettant la personnalisation de l'UI par script
        emit preLoad();
        
        // Afficher la fenêtre principale
        if (mainWindow == 0)
        {
                qFatal("You must define a main window (<window name=\"...\" main=\"true\">)"); 
        }
        else
        {
                mainWindow->show();
        }
}

void Runner::parse(const QDomElement &element)
{
        //Explorer les propriétés de l'application
	QDomElement el = element.firstChildElement();
	
	while (!el.isNull())
	{
	        if (el.tagName() == "window")
	        {
	                //Ajouter une fenêtre
	                addWindow(el);
	        }
	        else if (el.tagName() == "configfile")
	        {
	                //Ajouter un fichier de configuration
	                QString name = el.attribute("name");
	                QString type = el.attribute("type", "qsettings");
	                
	                Config *cfg = new Config(el, type, this);
	                
	                //Ajouter à la liste
	                configs.insert(name, cfg);
	                
	                //Ajouter au script
	                engine->globalObject().setProperty(name, engine->newQObject(cfg));
	        }
	        
	        el = el.nextSiblingElement();
	}
}

void Runner::addWindow(const QDomElement &element)
{
        QString isMain = element.attribute("main", "false");    //Fenêtre principale ?
        QString name = element.attribute("name");
        QWidget *wg;
        Window *win = new Window;
        
        QDomElement el = element.firstChildElement();
	
	while (!el.isNull())
	{
	        if (el.tagName() == "ui")
	        {
	                //Nom de fichier de l'UI de la fenêtre
	                QString uiFile = el.text();
	                
	                QFile fl(uiFile);
	                if (!fl.exists())
	                {
	                        qFatal("UI file %s don't exists", qPrintable(uiFile));
	                }
	                
	                wg = uiloader->load(&fl);
	                win->widget = wg;
	                
	                // Le widget est chargé, affecter ses widgets enfants au QScriptEngine
	                win->scriptObject = engine->newQObject(wg);
	                engine->globalObject().setProperty(name, win->scriptObject);
	                
	                //Ajouter à l'objet fenêtre les enfants
	                appendChildren(wg, win->scriptObject);
	        }
	        if (el.tagName() == "bindings")
	        {
	                //Bindings entre des slots et des signaux ou fonctions script
	                binds(el, win);
	        }
	        
	        el = el.nextSiblingElement();
	}
	
	// Ajouter la fenêtre à la liste
	windows.insert(name, win);
	
	// Si c'est la fenêtre principale, la déclarer comme telle
	if (isMain == "true")
	{
	        mainWindow = wg;
	}
}

void Runner::binds(QDomElement &element, Window *win)
{
        QDomElement el = element.firstChildElement();

        while (!el.isNull())
        {
                if (el.tagName() == "code")
                {
                        //Un signal d'un objet de la fenêtre va exécuter du code
                        QString objectName = el.attribute("object");
                        QString signal = el.attribute("signal");
                        QString name = el.attribute("name");
                        QString function;
                        
                        //Explorer les enfants pour trouver la section CDATA du code
                        QDomNode nd = el.firstChild();
                        while (!nd.isNull())
                        {
                                if (nd.isCDATASection())
                                {
                                        //On a trouvé
                                        function = nd.toCDATASection().data();
                                }
                                
                                nd = nd.nextSibling();
                        }
                        
                        //Créer le QScriptValue pour la fonction
                        QScriptValue func = engine->evaluate(function);
                        win->functions.insertMulti(name, func);
                        
                        //Récupérer l'adresse de l'objet et son signal pour la connexion
                        QObject *obj;
                        
                        if (objectName == "runner")
                        {
                                //C'est le runner qui doit être relié
                                obj = this;
                        }
                        else
                        {
                                obj = objectByName(win->widget, objectName);
                        }
                        
                        qScriptConnect(obj, qPrintable(QString("2") + signal), engine->newQObject(obj), func);
                }
                else if (el.tagName() == "function")
                {
                        //Le signal d'un objet va exécuter une fonction déjà définie (un slot)
                        QString objectName = el.attribute("object");
                        QString signal = el.attribute("signal");
                        QString name = el.attribute("name");
                        
                        //Adresse de l'objet pour la connexion
                        QObject *obj = objectByName(win->widget, objectName);
                        
                        qScriptConnect(obj, qPrintable(QString("2") + signal), engine->newQObject(obj), win->functions.value(name));
                }
                else if (el.tagName() == "predef")
                {
                        //Connexion d'un signal à une fonction prédéfinie (un slot de Runner)
                        QString objectName = el.attribute("object");
                        QString signal = el.attribute("signal");
                        QString func = el.attribute("function");
                        
                        //Adresse de l'objet pour la connexion
                        QObject *obj = objectByName(win->widget, objectName);
                        
                        //Fonction prédéfinie
                        if (func == "saveConfig")
                        {
                                //Enregistrer la configuration dans les fichier et quitter
                                connect(obj, qPrintable(QString("2") + signal), this, SLOT(saveConfig()));
                        }
                        else if (func == "applyConfig")
                        {
                                //Enregistrer la configuration dans les fichiers
                                connect(obj, qPrintable(QString("2") + signal), this, SLOT(applyConfig()));
                        }
                        else if (func == "resetConfig")
                        {
                                //Remet les valeurs par défaut
                                connect(obj, qPrintable(QString("2") + signal), this, SLOT(resetConfig()));
                        }
                }
                else if (el.tagName() == "config")
                {
                        //La valeur d'une propriété dépend d'une clef de configuration
                        BindConfig *bnd = new BindConfig;
                        
                        bnd->object = objectByName(win->widget, el.attribute("object"));
                        bnd->property = el.attribute("property");
                        bnd->configFile = el.attribute("configfile");
                        bnd->key = el.attribute("key");
                        bnd->defaultValue = el.attribute("default");
                        
                        bindscfg.append(bnd);
                }
                
                el = el.nextSiblingElement();
        }
}

void Runner::appendChildren(QObject *widget, QScriptValue &object)
{
        //Explorer les enfants du widget et les ajouter
        foreach(QObject *obj, widget->children())
        {
                object.setProperty(obj->objectName(), engine->newQObject(obj));
                
                appendChildren(obj, object);
        }
}

QObject *Runner::objectByName(QObject *object, const QString &name, bool isReverse)
{
        //Explorer les enfants et retourner celui qui a le bon nom
        QObject *rs;
        
        foreach(QObject *obj, object->children())
        {
                if (obj->objectName() == name)
                {
                        return obj;
                }
                
                rs = objectByName(obj, name, true);
                
                if (rs != 0) return rs;
        }
        
        if (!isReverse)
        {
                qFatal("Object %s not found", qPrintable(name));
        }
        
        return 0;
}

void Runner::saveConfig()
{
        applyConfig();
        
        emit preQuit();
        
        mainWindow->close();
}

void Runner::applyConfig()
{
        //Explorer les Binds et synchroniser
        foreach (BindConfig *bnd, bindscfg)
        {
                Config *cfg = configs.value(bnd->configFile);
                if (cfg != 0)
                {
                        QVariant defaultV(bnd->defaultValue);
                        QVariant prop = bnd->object->property(qPrintable(bnd->property));
                        
                        if (!prop.isValid())
                        {
                                prop = defaultV;
                        }
                        
                        cfg->setValue(bnd->key, prop);
                }
        }
}

void Runner::resetConfig()
{
        //Explorer les Binds et synchroniser
        foreach (BindConfig *bnd, bindscfg)
        {
                Config *cfg = configs.value(bnd->configFile);
                if (cfg != 0)
                {
                        //Vérifier que la propriété est writable
                        const QMetaObject *mobj = bnd->object->metaObject();
                        QMetaProperty prop = mobj->property(mobj->indexOfProperty(qPrintable(bnd->property)));
                        if (prop.isWritable())
                        {
                                QVariant defaultV(bnd->defaultValue);
                                QVariant value = cfg->value(bnd->key, defaultV);
                        
                                prop.write(bnd->object, value);
                        }
                }
        }
}
