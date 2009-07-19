/*
 * TabPluginContainer.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Denis Steckelmacher <steckdenis@yahoo.fr>
 *                      Many modifications by Cleriot Simon <malgon33@gmail.com>
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

#include <QCursor>
#include <QBitmap>
#include <QtDebug>

#include <TabPluginContainer.h>
#include <SubTab.h>
#include <App.h>

TabPluginContainer::TabPluginContainer(QString title, QString icon, QString id, IPanacheDesktopPlugin *plugin, App *mapp) : QWidget()
{
        //Initialize some variables
        app     = mapp;
        ax      = 0;
        ay      = 0;
        rcorner = None;
        onDrag  = false;
        micon   = icon;
        mtitle  = title;
        setMouseTracking(true);

        theme    = LConfig::logramValue("ImagePath", "/usr/share/logram/themes/default/", "Theme").toString();
        position = Left;
        tabInfos = new QSettings(QDir::homePath() + "/.panache/tabs.sav", QSettings::NativeFormat, this);

        //Load tab configuration
        tabName = id;

        tabInfos->setValue(tabName + "/IsTab", true);
        tabInfos->sync();

        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
        setAttribute(Qt::WA_NoSystemBackground, true);

        //Load pixmaps
        HG          = QPixmap(theme + LConfig::logramValue("Tab/TopLeftCornerImage", "desktop-topleftcorner.png", "Theme").toString());
        HD          = QPixmap(theme + LConfig::logramValue("Tab/TopRightCornerImage", "desktop-toprightcorner.png", "Theme").toString());
        BG          = QPixmap(theme + LConfig::logramValue("Tab/BottomLeftCornerImage", "desktop-bottomleftcorner.png", "Theme").toString());
        BD          = QPixmap(theme + LConfig::logramValue("Tab/BottomRightCornerImage", "desktop-bottomrightcorner.png", "Theme").toString());
        bgColor     = QColor(LConfig::logramValue("Tab/BackgroundColor", "#77a1ff", "Theme").toString());
        borderColor = QColor(LConfig::logramValue("Tab/BorderColor", "#000000", "Theme").toString());

        //Create new tab itself
        subtab = new SubTab(QPixmap(icon).scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), tabName, this, app);
        
        connect(subtab, SIGNAL(pullHided()), this, SLOT(pullHided()));

        //Load plugin
        resize(300, 50);
        nplugin = plugin;
        nplugin->linkParent(this);

        //each QWidget child must have normal cursor
        foreach(QObject * child, children())
        {
                QWidget *ch = qobject_cast<QWidget *>(child);

                if (ch != 0)
                {
                        ch->setCursor(Qt::ArrowCursor);
                }
        }

        //If we saved a size, restore
        //NOTE: This must be after nplugin-> Initialize, which can resize the plugin against the wishes of the user
        if (tabInfos->contains(tabName + "/width"))
        {
                resize(tabInfos->value(tabName + "/width").toInt(), tabInfos->value(tabName + "/height").toInt());
        }

        position = (tabPosition)tabInfos->value(tabName + "/position", Left).toInt();
        int offset = tabInfos->value(tabName + "/offset", 100).toInt();
        fixed = tabInfos->value(tabName + "/fixed", false).toBool();
        subtab->orientationChanged(position);

        if (position == Left)
        {
                move(0 - width(), offset);
        }
        else if (position == Right)
        {
                move(QApplication::desktop()->width(), offset);
        }
        else if (position == Top)
        {
                move(offset, 0 - height());
        }
        else if (position == Bottom)
        {
                move(offset, QApplication::desktop()->height());
        }

        if (tabInfos->value(tabName + "/showPull", true).toBool())
        {
                subtab->show();
        }
        else
        {
                subtab->hide();
        }

        //Create tab container
        addActions(subtab->listeActions);
        setContextMenuPolicy(Qt::ActionsContextMenu);

        //Glue content to tab
        arrange();

        //Create timer for tab's animations
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

        if (fixed)
        {
                //Tab fixed, unfolde
                deroule = true;
                calcPos();
                timer->start(17);
                arrange();
                subtab->listeActions.first()->setChecked(true);
        }
        else
        {
                subtab->show();
        }

        subtab->setToolTip(title);
        connect(subtab, SIGNAL(entered()), this, SLOT(buttonEntered()));
        connect(subtab, SIGNAL(mousePressed(bool)), this, SLOT(tabMousePressed(bool)));
        connect(subtab, SIGNAL(mouseMoved(int, int)), this, SLOT(tabMouseMoved(int, int)));
        connect(subtab, SIGNAL(block(bool)), this, SLOT(iconClicked(bool)));
        connect(subtab, SIGNAL(toDesktop()), this, SIGNAL(toDesktop()));
        connect(subtab, SIGNAL(closed()), this, SIGNAL(closed()));

        show();
}

TabPluginContainer::~TabPluginContainer()
{
        delete subtab;  //No need  strip

        //NOTE: No delete plugin, because it could be a desktop entry
}

void TabPluginContainer::arrange()
{
        if (position == Left)
        {
                subtab->move(x() + width(), y() + (height() / 2) - 10);
        }
        else if (position == Right)
        {
                subtab->move(x() - 30, y() + (height() / 2) - 10);
        }
        else if (position == Top)
        {
                subtab->move(x() + (width() / 2) - 10, y() + height());
        }
        else /* Bottom */
        {
                subtab->move(x() + (width() / 2) - 10, y() - 30);
        }
}

/*
        Events
 */

void TabPluginContainer::timeout()
{
        int dw = QApplication::desktop()->width();
        int dh = QApplication::desktop()->height();

        if (deroule)
        {
                //Unfolde tab (le (7-d_i) make all fluid
                int diffy, diffx;
                if ((position == Left) || (position == Top))
                {
                        diffy = (((0 - y()) / 10) + 1) * d_y;
                        diffx = (((0 - x()) / 10) + 1) * d_x;
                }
                else
                {
                        diffx = ((((width() + x()) - dw) / 10) + 1) * d_x;
                        diffy = ((((height() + y()) - dh) / 10) + 1) * d_y;
                }
                move(x() + diffx, y() + diffy);

                if ((position == Left) && (x() >= 0))
                {
                        move(0, y());
                        timer->stop();
                }
                if ((position == Right) && (x() <= dw - width()))
                {
                        move(dw - width(), y());
                        timer->stop();
                }
                if ((position == Top) && (y() >= 0))
                {
                        move(x(), 0);
                        timer->stop();
                }
                if ((position == Bottom) && (y() <= dh - height()))
                {
                        move(x(), dh - height());
                        timer->stop();
                }
        }
        else
        {
                int diffy, diffx;
                if ((position == Left) || (position == Top))
                {
                        diffy = (((height() + y()) / 10) + 1) * d_y;
                        diffx = (((width() + x()) / 10) + 1) * d_x;
                }
                else
                {
                        diffx = (((dw - x()) / 10) + 1) * d_x;
                        diffy = (((dh - y()) / 10) + 1) * d_y;
                }

                move(x() - diffx, y() - diffy);

                if ((position == Left) && (x() <= 0 - width()))
                {
                        move(0 - width(), y());
                        timer->stop();
                }
                if ((position == Right) && (x() >= dw))
                {
                        move(dw, y());
                        timer->stop();
                }
                if ((position == Top) && (y() <= 0 - height()))
                {
                        move(x(), 0 - height());
                        timer->stop();
                }
                if ((position == Bottom) && (y() >= dh))
                {
                        move(x(), dh);
                        timer->stop();
                }
        }

        if (!timer->isActive())
        {
                //Stop timer, better moment to update tab offset
                if ((position == Left) || (position == Right))
                {
                        tabInfos->setValue(tabName + "/offset", y());
                }
                else
                {
                        tabInfos->setValue(tabName + "/offset", x());
                }
                tabInfos->sync();
        }

        arrange();
}

void TabPluginContainer::calcPos()
{
        if (position == Left)
        {
                d_x = 1;
                d_y = 0;
        }
        else if (position == Right)
        {
                d_x = -1;
                d_y = 0;
        }
        else if (position == Top)
        {
                d_x = 0;
                d_y = 1;
        }
        else /* Bottom */
        {
                d_x = 0;
                d_y = -1;
        }
}

void TabPluginContainer::iconClicked(bool toggled)
{
        //Si on a défixé la languette, s'assurer que la tirette est affichée
        if (!toggled)
        {
                subtab->showPull();
        }
        
        timer->start(17);
        fixed   = toggled;
        deroule = toggled;
        calcPos();

        tabInfos->setValue(tabName + "/fixed", fixed);
        tabInfos->sync();
}

void TabPluginContainer::buttonEntered()
{
        //Show part of tab
        //if (!fixed)
        //{
        deroule = true;
        timer->start(17);
        calcPos();
        //}

        //Give focus to tab
        setFocus(Qt::MouseFocusReason);
}

void TabPluginContainer::mousePressEvent(QMouseEvent *event)
{
        setFocus(Qt::MouseFocusReason);

        onDrag = true;
        a_x    = event->globalX();
        a_y    = event->globalY();

        QWidget::mousePressEvent(event);
}

void TabPluginContainer::mouseReleaseEvent(QMouseEvent *event)
{
        onDrag = false;

        QWidget::mouseReleaseEvent(event);
}

void TabPluginContainer::mouseMoveEvent(QMouseEvent *event)
{
        QWidget::mouseMoveEvent(event);

        if (!onDrag)
        {
                //Check mouse angle
                ResizeCorner mcorner = rcorner;
                int          mx      = event->x();
                int          my      = event->y();

                if ((mx < 30) && (my < 30))
                {
                        //Angle top-left
                        rcorner = TopLeft;
                }
                else if (mx < 30 && my > height() - 30)
                {
                        //Angle bottom-left
                        rcorner = BottomLeft;
                }
                else if (mx > width() - 30 && my < 30)
                {
                        //Angle top-right
                        rcorner = TopRight;
                }
                else if (mx > width() - 30 && my > height() - 30)
                {
                        //Angle bottom-right
                        rcorner = BottomRight;
                }
                else
                {
                        //Nowhere
                        rcorner = None;
                }

                //Check it's allowed angle
                if ((position == Left) && ((rcorner == TopLeft) || (rcorner == BottomLeft)))
                {
                        rcorner = None;
                }
                if ((position == Right) && ((rcorner == TopRight) || (rcorner == BottomRight)))
                {
                        rcorner = None;
                }
                if ((position == Top) && ((rcorner == TopLeft) || (rcorner == TopRight)))
                {
                        rcorner = None;
                }
                if ((position == Bottom) && ((rcorner == BottomLeft) || (rcorner == BottomRight)))
                {
                        rcorner = None;
                }

                //If angle changed, change cursor
                if (mcorner != rcorner)
                {
                        if (rcorner == TopLeft)
                        {
                                setCursor(Qt::SizeFDiagCursor);
                        }
                        if (rcorner == TopRight)
                        {
                                setCursor(Qt::SizeBDiagCursor);
                        }
                        if (rcorner == BottomLeft)
                        {
                                setCursor(Qt::SizeBDiagCursor);
                        }
                        if (rcorner == BottomRight)
                        {
                                setCursor(Qt::SizeFDiagCursor);
                        }
                        if (rcorner == None)
                        {
                                setCursor(Qt::ArrowCursor);
                        }
                }
        }
        else
        {
                //Mouse down, move/resize plugin
                int diffx = event->globalX() - a_x;
                int diffy = event->globalY() - a_y;

                if (rcorner == TopLeft)
                {
                        setGeometry(x() + diffx, y() + diffy, width() - diffx, height() - diffy);
                }
                else if (rcorner == TopRight)
                {
                        setGeometry(x(), y() + diffy, width() + diffx, height() - diffy);
                }
                else if (rcorner == BottomLeft)
                {
                        setGeometry(x() + diffx, y(), width() - diffx, height() + diffy);
                }
                else if (rcorner == BottomRight)
                {
                        resize(width() + diffx, height() + diffy);
                }

                a_x = event->globalX();
                a_y = event->globalY();
        }
}

void TabPluginContainer::leaveEvent(QEvent *event)
{
        //Don't leave because of tooltip or sub-widget
        if (geometry().contains(QCursor::pos()))
        {
                return;
        }

        if (!fixed)
        {
                deroule = false;
                timer->start(17);
                calcPos();
        }

        (void)event;
}

void TabPluginContainer::tabMousePressed(bool pressed)
{
        tabPressed = pressed;

        ax = QCursor::pos().x();
        ay = QCursor::pos().y();
}

void TabPluginContainer::tabMouseMoved(int globalx, int globaly)
{
        if (!tabPressed)
        {
                return;
        }

        int diffx = globalx - ax;
        int diffy = globaly - ay;

        ax = globalx;
        ay = globaly;

        //If we move tab on screen side, change orientation
        bool changed = false;
        if (globalx <= 30)
        {
                //Move right
                position = Left;
                fixed    = false;
                changed  = true;
                move(0 - width(), globaly - (height() / 2));
        }
        else if (globalx >= (QApplication::desktop()->width() - 30))
        {
                //Move left
                position = Right;
                fixed    = false;
                changed  = true;
                move(QApplication::desktop()->width(), globaly - (height() / 2));
        }
        else if (globaly <= 30)
        {
                //Move top
                position = Top;
                fixed    = false;
                changed  = true;
                move(globalx - (width() / 2), 0 - height());
        }
        else if (globaly >= (QApplication::desktop()->height() - 30))
        {
                //Move bottom
                position = Bottom;
                fixed    = false;
                changed  = true;
                move(globalx - (width() / 2), QApplication::desktop()->height());
        }

        if (changed)
        {
                tabInfos->setValue(tabName + "/position", position);
                tabInfos->setValue(tabName + "/fixed", fixed);
                tabInfos->sync();

                repaint(0, 0, width(), height());

                subtab->orientationChanged(position);
        }

        //Move tab itself
        if (position == Left)
        {
                move(x(), y() + diffy);
        }
        else if (position == Right)
        {
                move(x(), y() + diffy);
        }
        else if (position == Top)
        {
                move(x() + diffx, y());
        }
        else /* Bottom */
        {
                move(x() + diffx, y());
        }

        arrange();
}

void TabPluginContainer::resizeEvent(QResizeEvent *event)
{
        arrange();

        //Recalculate mask
        QBitmap mmask(width(), height());
        mmask.fill(Qt::color1);

        QPainter painter(&mmask);

        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.setBackground(QBrush(Qt::color0));

        //Angles
        painter.drawPixmap(0, 0, HG.mask());
        painter.drawPixmap(width() - HD.width(), 0, HD.mask());
        painter.drawPixmap(0, height() - BG.height(), BG.mask());
        painter.drawPixmap(width() - BD.width(), height() - BD.height(), BD.mask());

        setMask(mmask);

        //Save tab's size
        tabInfos->setValue(tabName + "/width", width());
        tabInfos->setValue(tabName + "/height", height());
        tabInfos->sync();

        QWidget::resizeEvent(event);
}

void TabPluginContainer::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        //Fill
        painter.fillRect(0, 0, width(), height(), QBrush(bgColor));

        //Draw edge
        painter.setPen(QPen(borderColor));
        painter.drawRect(0, 0, width() - 1, height() - 1);

        //Clear edge at tab's location
        if (subtab->isVisible())
        {
                painter.setPen(QPen(bgColor));
                if (position == Left)
                {
                        painter.drawLine(width() - 1, (height() / 2) - 10, width() - 1, (height() / 2) + 9);
                }
                else if (position == Right)
                {
                        painter.drawLine(0, (height() / 2) - 10, 0, (height() / 2) + 9);
                }
                else if (position == Top)
                {
                        painter.drawLine((width() / 2) - 10, height() - 1, (width() / 2) + 9, height() - 1);
                }
                else /* Bottom */
                {
                        painter.drawLine((width() / 2) - 10, 0, (width() / 2) + 9, 0);
                }
        }

        //Draw angles
        painter.drawPixmap(0, 0, HG);
        painter.drawPixmap(width() - HD.width(), 0, HD);
        painter.drawPixmap(0, height() - BG.height(), BG);
        painter.drawPixmap(width() - BD.width(), height() - BD.height(), BD);

        (void)event;
}

void TabPluginContainer::pullHided()
{
        //A tab with a hided pull must be fixed
        fixed = true;
        
        subtab->listeActions.first()->setChecked(true);
}

IPanacheDesktopPlugin *TabPluginContainer::getPlugin()
{
        return nplugin;
}
