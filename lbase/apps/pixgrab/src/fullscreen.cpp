/* Copyright (c) 2009, Cleriot Simon <malgon33@gmail.com>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include "fullscreen.h"


fullDialog::fullDialog(QWidget *parent, Qt::WFlags flags) : QLabel(parent, flags)
{
        pressed = false;
        capture = false;
        setWindowFlags(Qt::X11BypassWindowManagerHint);
        setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
}

fullDialog::~fullDialog()
{
        //
}

void fullDialog::mousePressEvent(QMouseEvent *event)
{
        pressed = true;
        x       = event->x();
        y       = event->y();
}

void fullDialog::mouseMoveEvent(QMouseEvent *event)
{
        if (pressed)
        {
                x2 = event->x();
                y2 = event->y();

                repaint();
        }
}

void fullDialog::mouseReleaseEvent(QMouseEvent *event)
{
        (void)event;
        pressed = false;
        capture = true;

        repaint(); // pour ne pas avoir le cadre sur la capture

        int X, Y, width, height;
        if (x > x2)
        {
                X     = x2;
                width = x - x2;
        }
        else
        {
                X     = x;
                width = x2 - x;
        }

        if (y > y2)
        {
                Y      = y2;
                height = y - y2;
        }
        else
        {
                Y      = y;
                height = y2 - y;
        }

        //int wid=QApplication::desktop()->winId();

        image = QPixmap::grabWindow(QApplication::desktop()->winId(), X, Y, width, height);

        hide();

        QString dest = QFileDialog::getSaveFileName(this, "Emplacement de sauvegarde", "", "Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xpm *.xbm)");

        if (dest != NULL)
        {
                image.save(dest);
        }

        capture = false;

        emit imageSaved();
}

void fullDialog::paintEvent(QPaintEvent *event)
{
        QLabel::paintEvent(event);

        if (!capture)
        {
                QPainter painter(this);

                painter.setPen(QPen(QColor(Qt::red)));
                painter.setBrush(QColor(255, 255, 255, 120));
                painter.drawRect(x, y, x2 - x, y2 - y);
        }
}

void fullDialog::focusOutEvent(QFocusEvent *event)
{
        this->setFocus();
}
