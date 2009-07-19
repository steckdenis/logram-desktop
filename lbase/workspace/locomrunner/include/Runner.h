/*
 * Runner.h
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
 
#ifndef __RUNNER_H__
#define __RUNNER_H__

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QtScript>
#include <QList>
#include <QVariant>

class QWidget;
class QUiLoader;
class Config;

/**
 * \struct Window
 * \brief Représente une fenêtre au sens Locom du terme
 */
struct Window
{
        QWidget *widget;                                /*!< Widget de la fenêtre, retourné par QUiLoader */
        QHash <QString, QScriptValue> functions;        /*!< Tableau associatif entre le nom des fonctions et leur contenu (pour associer plusieurs signaux à une fonction */
        QScriptValue scriptObject;                      /*!< Objet Script qui représente la fenêtre */
};

/**
 * \struct BindConfig
 * \brief Lien entre une propriété et une clef de configuration
 */
struct BindConfig
{
        QObject *object;                                /*!< Object qui a la propriété */
        QString property;                               /*!< Nom de la propriété (text, icon, etc) */
        QString configFile;                             /*!< Nom du fichier de configuration */
        QString key;                                    /*!< Clef de configuration (Desktop/Background par exemple) */
        QString defaultValue;                           /*!< Valeur par défaut si la propriété n'existe pas dans le fichier de configuraiton */
};

/**
 * \class Runner
 * \brief Lance et gère un script Locom Runner
 *
 * Cette classe s'occupe de créer la GUI, de gérer les signaux et le moteur de script, et de <em>binder</em> les propriétés
 *
 */
class Runner : public QObject
{
        Q_OBJECT
        public:
                /*
                 * \brief Constructeur
                 * \param fileName nom du fichier .locom à lancer
                 */
                Runner(const QString &fileName);
                
                /**
                 * \brief Parse un fichier
                 * \param element Élément principal du fichier
                 */
                void parse(const QDomElement &element);
                
                /**
                 * \brief Ajoute une fenêtre
                 * 
                 * Lis le contenu en element et en fait une fenêtre graphique. Cette fonction s'occupe aussi des signaux et des slots
                 *
                 * \param element Élément contenant le XML de la fenêtre
                 */
                void addWindow(const QDomElement &element);
                
                /**
                 * \brief Relie les propriétés aux clef de configuration
                 * \param element Élément qui contient le XML des bindings
                 * \param win Fenêtre qui contient les bindings
                 */
                void binds(QDomElement &element, Window *win);                  //Bind des slots/signaux
                
                /**
                 * \brief Ajoute des enfants à l'objet script
                 *
                 * Parcourt récursivement les enfants de widget et les ajoute comme propriétés de object. Ainsi, widget.enfant dans un
                 * script permet d'accéder directement à un enfant, même s'il n'est pas directement l'enfant d'un widget
                 *
                 * \param widget Widget qui contient les enfants
                 * \param object Objet Script qui va contenir les propriétés
                 */
                void appendChildren(QObject *widget, QScriptValue &object);
                
                
                /**
                 * \brief Retourne l'objet qui a le nom demandé
                 *
                 * Explore les enfants de object et retourne celui qui a le nom name.
                 *
                 * \param object Objet qui contient les enfants
                 * \param name Nom de l'enfant recherché
                 * \param usReverse True si appelé en récursion. Laissez toujours ce paramètre à false
                 */
                QObject *objectByName(QObject *object, const QString &name, bool isReverse = false);
                
        public slots:
                //Les slots sont exposés au Script
                
                void saveConfig();      /*!< Enregistre la configuration (bindings de configuration) et ferme la fenêtre principale */
                void applyConfig();     /*!< Enregistre la configuration mais ne ferme pas la fenêtre */
                void resetConfig();     /*!< Remet dans les contrôles les précédantes valeurs qu'il y avait dans le fichier de configuration, ou les valeurs par défaut */
                
        signals:
                void preLoad();         /*!< Appelé juste avant d'afficher la fenêtre */
                void preQuit();         /*!< Appelé juste avant de quitter */
                
        private:
                QString path;
                QDomDocument doc;
                
                QUiLoader *uiloader;
                
                QHash<QString, Window *> windows;       //Liste des fenêtres (nom, widget)
                QHash<QString, Config *> configs;       //Liste des fichiers de configuration
                QList<BindConfig *> bindscfg;           //Bindings de configuration
                QWidget *mainWindow;
                QScriptEngine *engine;
};

#endif /* __RUNNER_H__ */

