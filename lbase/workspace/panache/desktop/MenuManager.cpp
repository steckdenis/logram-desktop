/*
 * MenuManager.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Denis Steckelmacher <steckdenis@yahoo.fr>
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#include <QSettings>
#include <QDir>
#include <QStringList>
#include <QtPlugin>
#include <QPluginLoader>
#include <QtDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QPainter>
#include <QBitmap>
#include <QPen>
#include <QBrush>
#include <QPolygon>
#include <QFont>
#include <QToolTip>

#include <App.h>
#include <StartMenu.h>
#include <MenuManager.h>

extern "C" {
#include <math.h>
}

MenuManager::MenuManager(App *mapp) : QWidget(0)
{
        app         = mapp;
        smenu       = new StartMenu(app, 0);
        smenuExpand = new QTimer(this);
        wTimer      = new QTimer(this);
        wTicks      = 0;
        activeEntry = 0;
        activeLayer = 0;

        position = (MenuPosition) LConfig::logramValue("RoundMenu/Position", MenuManager::Left, "Theme").toInt();
        theme    = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();

        connect(smenuExpand, SIGNAL(timeout()), this, SLOT(startMenuTimerTimeout()));
        connect(wTimer, SIGNAL(timeout()), this, SLOT(wTimerTimeout()));
        connect(smenu, SIGNAL(enterred()), this, SLOT(startMenuEnterred()));
        connect(smenu, SIGNAL(leaved()), this, SLOT(startMenuLeaved()));
        connect(smenu, SIGNAL(clicked()), this, SLOT(startMenuClicked()));
        connect(smenu, SIGNAL(posChanged(MenuManager::MenuPosition)), this, SLOT(posChanged(MenuManager::MenuPosition)));

        //Load configuration
        stagesize    = LConfig::logramValue("RoundMenu/StageSize", 40, "Theme").toInt();
        fontsize     = LConfig::logramValue("RoundMenu/FontSize", 8, "Theme").toInt();
        buttonsize   = LConfig::logramValue("RoundMenu/ButtonSize", 32, "Theme").toInt();
        entryspacing = LConfig::logramValue("RoundMenu/EntriesSpacing", 40, "Theme").toInt();

        activeItemBg = QPixmap(theme + LConfig::logramValue("RoundMenu/ActiveItemBackground", "activeitembackground.png", "Theme").toString()).scaled(buttonsize, buttonsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        bcolor      = QColor(LConfig::logramValue("RoundMenu/BorderColor", "#666666", "Theme").toString());
        fontcolor   = QColor(LConfig::logramValue("RoundMenu/FontColor", "#FFFFFF", "Theme").toString());
        brush       = QBrush(QPixmap(theme + LConfig::logramValue("RoundMenu/BackgroundTexture", "round_bgtexture.png", "Theme").toString()));
        brushactive = QBrush(QPixmap(theme + LConfig::logramValue("RoundMenu/ActiveBackgroundTexture", "round_bgtexture_a.png", "Theme").toString()));

        //Load plugins
        QSettings   pluginList(QDir::homePath() + "/.panache/plugins.list", QSettings::IniFormat);
        QStringList splugins = pluginList.childGroups();
        QString     pluginFileName;
        int         pluginLevel;

        foreach(QString splugin, splugins)
        {
                pluginList.beginGroup(splugin);

                pluginFileName = pluginList.value("FileName", "NIL").toString();
                pluginLevel    = pluginList.value("Level", 0).toInt();          //Niveau de l'anneau du plugin
                if (pluginFileName != "NIL")
                {
                        QPluginLoader      loader(pluginFileName);
                        QObject            *mplugin = loader.instance();
                        IPanacheMenuPlugin *nplugin = 0;
                        if (mplugin != 0)
                        {
                                nplugin = qobject_cast<IPanacheMenuPlugin *>(mplugin);
                                nplugin->init(this);
                                plugins.insert(pluginLevel, nplugin);
                        }
                }

                pluginList.endGroup();
        }

        //Once plugins loaded, update local plugins list
        updatePlugins();

        posChanged(position);

        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        setMouseTracking(true);

        show();

        //The menu should be at the forefront
        smenu->show();
        XRaiseWindow(QX11Info::display(), smenu->winId());
}

void MenuManager::updatePlugins()
{
        maxlevel = 1;

        //Browse plugins and add MenuEntry's (in round menu)
        entries.clear();
        levelCurrent.clear();
        foreach(IPanacheMenuPlugin * plugin, plugins.values())
        {
                int level = plugins.key(plugin);

                QList<MenuEntry *> *mentries = plugin->menuEntries();

                //Insert separator
                MenuEntry *eseparator = new MenuEntry();
                eseparator->separator = true;
                entries.insertMulti(level, eseparator);

                for (int i = 0; i < mentries->count(); i++)
                {
                        MenuEntry *entry = mentries->at(i);
                        entry->plugin = plugin;

                        if (!entry->icon.isNull())      //Add void Pixmap, give a bad image to user
                        {
                                entries.insertMulti(level, entry);
                        }
                }

                //Update maximum level
                maxlevel = level > maxlevel ? level : maxlevel;
        }

        //Update selected index in each level
        for (int i = 0; i <= maxlevel; i++)
        {
                int num = entries.values(i).count();

                levelCurrent.insert(i, num / 2);

                //Add separator in end-line, to balance
                if (i != 0)
                {
                        MenuEntry *eseparator = new MenuEntry();
                        eseparator->separator = true;
                        entries.insertMulti(i, eseparator);
                }
        }
}

void MenuManager::drawLevel(QPainter *painter, int num)
{
        bool lactive;

        //Draw level
        painter->setPen(QPen(bcolor));

        //Know if level is active
        QPoint pdistance = mapFromGlobal(QCursor::pos()) - circleCenterPos;
        int    distance  = sqrt(pow(pdistance.x(), 2) + pow(pdistance.y(), 2));

        if ((distance > smenu->width() + (stagesize * (num - 1))) && (distance < smenu->width() + (stagesize * num)))
        {
                lactive     = true;
                activeLayer = num;
        }

        painter->setBrush((lactive ? brushactive : brush));

        //Draw level
        int cx       = circleCenterPos.x();
        int cy       = circleCenterPos.y();
        int esize    = width() > height() ? width() : height();
        int mlevel   = maxlevel - num;                    //Nombre de niveaux en moins par rapport au maximum
        int sublevel = mlevel * stagesize;                //Taille à retirer
        int lsize    = esize - (2 * sublevel);
        painter->resetTransform();
        painter->drawEllipse(cx - (lsize / 2), cy - (lsize / 2), lsize, lsize);

        //Draw level entryes
        foreach(MenuEntry * entry, entries.values(num))
        {
                drawEntry(painter, entry);
        }

        //Draw the level below
        if (num != 0)
        {
                drawLevel(painter, num - 1);
        }
}

void MenuManager::drawEntry(QPainter *painter, MenuEntry *entry)
{
        //Build transformation
        QTransform trans;
        bool       toShow;

        trans = getTransform(entry, &toShow);

        if (!toShow)
        {
                return;
        }

        //Draw picture
        painter->setWorldTransform(trans);

        if (entry->separator)
        {
                painter->setPen(QPen(bcolor));
                int mx = 0 - (stagesize / 2);
                painter->drawLine(mx, 0, stagesize / 2, 0);
        }
        else
        {
                if (entry == activeEntry)
                {
                        painter->drawPixmap(0 - (buttonsize / 2), 0 - (buttonsize / 2), activeItemBg);
                }

                if ((entry->icon.width() != buttonsize) || (entry->icon.height() != buttonsize))
                {
                        entry->icon = entry->icon.scaled(buttonsize, buttonsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
                painter->drawPixmap(0 - (buttonsize / 2), 0 - (buttonsize / 2), entry->icon);
        }
}

void MenuManager::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);
        painter.setRenderHint(QPainter::Antialiasing);

        drawLevel(&painter, maxlevel);

        (void)event;
}

void MenuManager::startMenuEnterred()
{
        smenuE      = true;
        smenuETicks = 0;
        smenuExpand->start(20);

        XSetInputFocus(QX11Info::display(), winId(), RevertToParent, CurrentTime);
}

void MenuManager::startMenuLeaved()
{
}

void MenuManager::leaveEvent(QEvent *event)
{
        //Check we are not in the widget, for example not embarrased by tooltip
        int     cx    = circleCenterPos.x();
        int     cy    = circleCenterPos.y();
        int     esize = width() > height() ? width() : height();
        QRegion region(cx - (esize / 2), cy - (esize / 2), esize, esize, QRegion::Ellipse);

        if (region.contains(mapFromGlobal(QCursor::pos())))
        {
                return;
        }

        smenuE      = false;
        smenuETicks = 0;
        smenuExpand->start(20);

        QToolTip::showText(QPoint(), QString());

        (void)event;
}

void MenuManager::startMenuClicked()
{
}

void MenuManager::startMenuTimerTimeout()
{
        int madd;
        int smw = smenu->width() / 2;
        int smh = smenu->height() / 2;
        int smx = smenu->x();
        int smy = smenu->y();
        int dw  = QApplication::desktop()->width();
        int dh  = QApplication::desktop()->height();

        if (smenuE)
        {
                madd = 1;
        }
        else
        {
                madd = -1;
        }

        if (position == Left)
        {
                if ((smenuE && (smx <= 0)) || (!smenuE && (smx >= 0 - smw)))
                {
                        smenu->move(smenu->x() + madd, smenu->y());
                }
                move(x() + (madd * (width() / smw)), y());
        }
        else if (position == Right)
        {
                if ((smenuE && (smx >= dw - (smw*2))) || (!smenuE && (smx <= (dw-smw))))
                {
                        smenu->move(smenu->x() - madd, smenu->y());
                }
                move(x() - (madd * (width() / smw)), y());
        }
        else if (position == Top)
        {
                if ((smenuE && (smy <= 0)) || (!smenuE && (smy >= 0 - smh)))
                {
                        smenu->move(smenu->x(), smenu->y() + madd);
                }
                move(x(), y() + (madd * (height() / smh)));
        }
        else /* Bottom */
        {
                if ((smenuE && (smy >= dh - (smh * 2))) || (!smenuE && (smy <= (dh - smh))))
                {
                        smenu->move(smenu->x(), smenu->y() - madd);
                }
                move(x(), y() - (madd * (height() / smh)));
        }

        smenuETicks++;

        //Stop when round menu is totally unfolded
        if ((position == Left) && (((x() >= 0) && smenuE) || ((x() <= 0 - width()) && !smenuE)))
        {
                smenuExpand->stop();
                move(smenuE ? 0 : 0 - width(), y());
        }
        else if (position == Right && ((x() <= dw - width() && smenuE) || (x() >= dw && !smenuE)))
        {
                smenuExpand->stop();
                move(smenuE ? dw - width() : dw, y());
        }
        else if (position == Top && ((y() >= 0 && smenuE) || (y() <= 0 - height() && !smenuE)))
        {
                smenuExpand->stop();
                move(x(), smenuE ? 0 : 0 - height());
        }
        else if (position == Bottom && ((y() <= dh - height() && smenuE) || (y() >= dh && !smenuE)))
        {
                smenuExpand->stop();
                move(x(), smenuE ? dh - height() : dh);
        }
}

bool MenuManager::entryContains(MenuEntry *entry, int x, int y)
{
        //Return true if entry contain (x,y)
        QTransform trans;
        bool       toShow;

        trans = getTransform(entry, &toShow);

        if (!toShow)
        {
                return false;
        }

        QPolygon polygon(QRect(0 - (buttonsize / 2), 0 - (buttonsize / 2), buttonsize, buttonsize));
        if (trans.map(polygon).containsPoint(QPoint(x, y), Qt::OddEvenFill))
        {
                return true;
        }
        return false;
}

QTransform MenuManager::getTransform(MenuEntry *entry, bool *toShow, double *ma)
{
        QTransform trans;

        //Calculate angle required by widget
        int lnum = entries.key(entry);

        QList<MenuEntry *> levelentries = entries.values(lnum);
        int levelindex = levelentries.indexOf(entry);

        volatile double l     = (double)entryspacing;
        volatile double r     = (double)(smenu->width() + (stagesize * lnum) - (stagesize / 2));
        volatile double angle = l / r;

        //En fonction du levelindex et de levelCurrent(num), on peut calculer l'angle nécessaire
        int   cselect = levelCurrent.value(lnum);
        qreal a       = angle * (cselect - levelindex);

        if (ma != 0)
        {
                *ma = a;
        }

        //Don't draw hide entryes
        if ((a + levelAngle.value(lnum, 0.0) > 3.0) || (a + levelAngle.value(lnum, 0.0) < -3.0))
        {
                if (toShow != 0)
                {
                        *toShow = false;
                }
                return QTransform();
        }
        else
        {
                //Is going to be showed
                if (toShow != 0)
                {
                        *toShow = true;
                }
        }

        trans.translate(circleCenterPos.x(), circleCenterPos.y());                      //Se place au centre

        //Tourne encore du nécessaire pour que le demi-cercle soit toujours affiché
        if (position == Right)
        {
                a -= 3.14;
        }
        else if (position == Top)
        {
                a += 3.14 / 2;
        }
        else if (position == Bottom)
        {
                a -= 3.14 / 2;
        }

        a += levelAngle.value(lnum, 0.0);
        trans.rotateRadians(a);
        trans.translate(smenu->width() + (stagesize * lnum) - (stagesize / 2), 0);

        if (!entry->separator)
        {
                trans.rotateRadians(-a);        //Don't turn icon
        }
        return trans;
}

#define max(a, b)    ((a) > (b) ? (a) : (b))

void MenuManager::mouseMoveEvent(QMouseEvent *event)
{
        //repaint(0, 0, width(), height());

        //Just draw if entry or layer change
        //Check if level is active
        QPoint pdistance = mapFromGlobal(QCursor::pos()) - circleCenterPos;
        int    distance  = sqrt(pow(pdistance.x(), 2) + pow(pdistance.y(), 2));

        for (int i = 0; i <= maxlevel; i++)
        {
                if ((distance > smenu->width() + (stagesize * (i - 1))) && (distance < smenu->width() + (stagesize * i)))
                {
                        if (i != activeLayer)
                        {
                                //Draw level
                                int cx       = circleCenterPos.x();
                                int cy       = circleCenterPos.y();
                                int esize    = width() > height() ? width() : height();
                                int mlevel   = maxlevel - max(activeLayer, i);                  //Number of levels lower than the maximum
                                int sublevel = mlevel * stagesize;                              //Size to take off
                                int lsize    = esize - (2 * sublevel);
                                repaint(cx - (lsize / 2), cy - (lsize / 2), lsize, lsize);

                                activeLayer = i;
                        }
                }
        }

        //Check if cursor is on another entry
        QPoint cpos = mapFromGlobal(QCursor::pos());
        foreach(MenuEntry * entry, entries.values())
        {
                if (entryContains(entry, cpos.x(), cpos.y()))
                {
                        if (activeEntry != entry)
                        {
                                MenuEntry *oldactive = activeEntry;

                                activeEntry = entry;

                                QToolTip::showText(QCursor::pos(), entry->title);

                                //Get transformations for the two entryes (old and new), in order to draw nothing but its
                                QRect rectIcon(0 - (buttonsize / 2), 0 - (buttonsize / 2), buttonsize, buttonsize);

                                QTransform transf = getTransform(entry);
                                QRect      r1     = transf.mapRect(rectIcon);

                                if (oldactive != 0)
                                {
                                        transf = getTransform(oldactive);
                                        QRect r2 = transf.mapRect(rectIcon);

                                        repaint(r1.united(r2));
                                }
                                else
                                {
                                        repaint(r1);
                                }
                        }
                }
        }

        (void)event;
}

void MenuManager::wheelEvent(QWheelEvent *event)
{
        wTicks += 6;

        if (event->delta() > 0)
        {
                wDown = false;
        }
        else
        {
                wDown = true;
        }

        wTimer->start(40);

        event->accept();
}

void MenuManager::wTimerTimeout()
{
        //Add what we need to the angle (angle/5)
        volatile double l     = (double)entryspacing;
        volatile double r     = (double)(smenu->width() + (stagesize * activeLayer) - (stagesize / 2));
        volatile double angle = l / r;

        double sign = wDown ? 1 : -1;

        levelAngle[activeLayer] += sign * (angle / 5);

        wTicks--;

        if (wTicks == 0)
        {
                wTimer->stop();
        }

        //Draw level
        int cx       = circleCenterPos.x();
        int cy       = circleCenterPos.y();
        int esize    = width() > height() ? width() : height();
        int mlevel   = maxlevel - activeLayer;                  //Number of levels lower than the maximum
        int sublevel = mlevel * stagesize;                      //Size to take off
        int lsize    = esize - (2 * sublevel);
        repaint(cx - (lsize / 2), cy - (lsize / 2), lsize, lsize);
}

void MenuManager::keyPressEvent(QKeyEvent *event)
{
        if ((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Left))
        {
                wTicks += 6;
                wDown   = false;
                wTimer->start(40);
        }
        else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Right)
        {
                wTicks += 6;
                wDown   = true;
                wTimer->start(40);
        }
}

void MenuManager::mousePressEvent(QMouseEvent *event)
{
        //Find wich entry we clicked
        foreach(MenuEntry * entry, entries.values())
        {
                if (entryContains(entry, event->x(), event->y()) && !entry->separator)
                {
                        //Find entry's plugin and index in plugin
                        IPanacheMenuPlugin *plugin = entry->plugin;
                        int eindex = plugin->menuEntries()->indexOf(entry);

                        //Call plugin's event
                        if (event->button() == Qt::LeftButton)
                        {
                                if (entry->togglable)
                                {
                                        entry->toggled = !entry->toggled;
                                }
                                plugin->itemClicked(eindex, entry->toggled);
                        }
                        else if (event->button() == Qt::RightButton)
                        {
                                plugin->itemContextMenu(eindex, event->globalX(), event->globalY());
                        }
                }
        }

        XRaiseWindow(QX11Info::display(), smenu->winId());
}

void MenuManager::appEvent(XEvent *event)
{
        //Send event to plugins
        foreach(IPanacheMenuPlugin * mplugin, plugins.values())
        {
                mplugin->appEvent(event);
        }
}

void MenuManager::posChanged(MenuManager::MenuPosition pos)
{
        position = pos;

        //Update size
        if ((position == Left) || (position == Right))
        {
                resize(smenu->width() + (maxlevel * stagesize),
                       (smenu->height() * 2) + (2 * maxlevel * stagesize));
        }
        else
        {
                resize((smenu->width() * 2) + (2 * maxlevel * stagesize),
                       smenu->height() + (maxlevel * stagesize));
        }
        pcache = QPixmap(width(), height());

        //Move widget behind menu
        int dw, dh;
        dw = QApplication::desktop()->width();
        dh = QApplication::desktop()->height();
        if (position == Left)
        {
                move(0 - width(), (dh / 2) - (height() / 2));
                circleCenterPos = QPoint(0, height() / 2);
        }
        else if (position == Right)
        {
                move(dw, (dh / 2) - (height() / 2));
                circleCenterPos = QPoint(width(), height() / 2);
        }
        else if (position == Top)
        {
                move((dw / 2) - (width() / 2), 0 - height());
                circleCenterPos = QPoint(width() / 2, 0);
        }
        else /* Bottom */
        {
                move((dw / 2) - (width() / 2), dh);
                circleCenterPos = QPoint(width() / 2, height());
        }

        //Update mask
        int mx = 0, my = 0;

        if (position == Left)
        {
                mx = 0 - width();
        }
        if (position == Top)
        {
                my = 0 - height();
        }

        int     masksize = width() > height() ? width() : height();     //The greatest value is the diameter of the circle
        QBitmap mmask(width(), height());
        mmask.fill(Qt::color0);
        QPainter painter(&mmask);

        //Draw the ellipse
        painter.setBrush(QBrush(Qt::color1));
        painter.setPen(QPen(Qt::color1));
        painter.drawEllipse(mx, my, masksize, masksize);

        //Assign mask
        setMask(mmask);

        repaint(0, 0, width(), height());

        //Save new position
        QSettings set("Logram", "Theme", 0);
        set.setValue("RoundMenu/Position", position);
}

void MenuManager::updateMenu()
{
        //Find the level of the plugin and redraw it. We also must to update the list of the entrie
        updatePlugins();
        repaint(0, 0, width(), height());
}
