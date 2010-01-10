/*
 * LImageButton.cpp
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

#include <LImageButton.h>

LImageButton::LImageButton(QPixmap mnorm, QPixmap mactive, QPixmap mpressed, QWidget *parent) : QPushButton(parent)
{
	isPressed = false;
        mhover    = false;
        norm      = mnorm;
       	active = mactive;	
	pressed = mpressed;	
		
        setCursor(Qt::ArrowCursor);
}

void LImageButton::enterEvent(QEvent *event)
{
        mhover = true;
        repaint(0, 0, width(), height());

        (void)event;
}

void LImageButton::leaveEvent(QEvent *event)
{
        mhover = false;
        repaint(0, 0, width(), height());

        (void)event;
}

void LImageButton::mousePressEvent(QEvent *event) 
{
	isPressed = true;
	repaint(0, 0, width(), height());

	(void)event;
}

void LImageButton::mouseReleaseEvent(QEvent *event)
{
	isPressed = false;	
	repaint(0, 0, width(), height());
	
	(void)event;
}
void LImageButton::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);

        if (isChecked() || mhover)
                painter.drawPixmap(0, 0, width(), height(), active);
	else if (isPressed)
		painter.drawPixmap(0, 0, width(), height(), pressed);
        else 
                painter.drawPixmap(0, 0, width(), height(), norm);

        (void)event;
}
