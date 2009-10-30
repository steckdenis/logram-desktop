/*
 * LRibbon.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2008 - Flavien Lefebvre <flavien-lefebvre@hotmail.fr>
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
 *
 * The Logram team decline any responsibility of the further use of this class which would go against the Microsoft copyright about the ribbon
 *
 */

#include <LRibbon.h>

LRibbon::LRibbon(QWidget *parent)
{
    this->setFixedHeight(140);
    QFont font;
    font.setFamily("Segoe UI Semibold");
    font.setPointSize(9);
    this->setFont(font);
    this->setStyleSheet(
                            "QTabWidget::pane\n"
                            "{\n"
                            "	font: 8pt \"Segoe UI\";\n"
                            "	color: rgb(21, 36, 134);\n"
                            "	border: 0px;\n"
                            "	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(251, 253, 255, 255), stop:1 rgba(220, 231, 245, 255));\n"
                            "	border-top: 1px solid rgb(186, 201, 219);\n"
                            "	border-bottom: 1px solid rgb(186, 201, 219);\n"
                            "}\n"
                            "\n"
                            "QTabWidget::pane:disabled\n"
                            "{\n"
                            "	color: gray;\n"
                            "	background-color: rgb(217, 217, 217);\n"
                            "}\n"
                            "\n"
                            "QTabWidget::tab-bar\n"
                            "{"
                            "	margin-left: 10px;\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab\n"
                            "{\n"
                            "	color: rgb(0, 58, 86);\n"
                            "	background-color: rgba(255, 255, 255, 0);\n"
                            "	border-top-right-radius: 3px;\n"
                            "	border-top-left-radius: 3px;\n"
                            "	padding: 3px;\n"
                            "	padding-right: 10px;\n"
                            "	padding-left: 10px;\n"
                            "	margin: 2px;\n"
                            "	margin-bottom: 1px;\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab:disabled\n"
                            "{\n"
                            "	color: gray;\n"
                            "	background-color: rgb(217, 217, 217);\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab:selected:disabled\n"
                            "{\n"
                            "	color: gray;\n"
                            "	back"
                                                    "ground-color: rgb(217, 217, 217);\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab:selected\n"
                            "{\n"
                            "	color: rgb(0, 85, 127);\n"
                            "	background-color: rgb(251, 253, 255);\n"
                            "	border-top: 1px solid rgb(186, 201, 219);\n"
                            "	border-right: 1px solid rgb(186, 201, 219);\n"
                            "	border-left: 1px solid rgb(186, 201, 219);\n"
                            "	margin-bottom: -1px;\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab:hover\n"
                            "{\n"
                            "	border-top: 1px solid rgb(255, 183, 0);\n"
                            "	border-right: 1px solid rgb(255, 183, 0);\n"
                            "	border-left: 1px solid rgb(255, 183, 0);\n"
                            "}\n"
                            "\n"
                            "QTabBar::tab:hover:!selected\n"
                            "{\n"
                            "	background-color: rgb(234, 240, 249);\n"
                            "	padding-bottom: 5px;\n"
                            "}\n"
                            "\n"
                            "QGroupBox\n"
                            "{\n"
                            "   margin-bottom: 5ex;\n"
                            "   padding: 0px;\n"
                            "}\n"
                            "\n"
                            "QGroupBox > QGridLayout, QGroupBox > QBoxLayout\n"
                            "{\n"
                            "   padding: 0px;\n"
                            "   margin: 0px;\n"
                            "}\n"
                            "\n"
                            "QGroupBox::title\n"
                            "{\n"
                            "	subcontrol-origin: margin;\n"
                            "	subcontrol-position: bottom center;\n"
                            "	padding: 0 3px;\n"
                            "	color: rgb(123, 138, 171);\n"
                            "}\n"
                            "\n"
                            "QGroupBox:focus\n"
                            "{\n"
                            "	border-color: rgb(255, 0, 0);\n"
                            "	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(247, 251, 255, 255), stop:1 rgba(234, 252, 255, 255));\n"
                            "}\n"
                            "\n"
                            "QPushButton\n"
                            "{\n"
                            "	background-color: rgba(255, 255, 255, 0);\n"
                            "	border: 1px solid rgba(194, 155, 41, 0);\n"
                            "	padding-top: 5px;\n"
                            "	padding-bottom: 5px;\n"
                            "	margin: 2px;\n"
                            "	padding: 5px;\n"
                            "	alignment: center\n"
                            "}\n"
                            "\n"
                            "QToolButton\n"
                            "{\n"
                            "	background-color: rgba(255, 255, 255, 0);\n"
                            "	border: 1px solid rgba(194, 155, 41, 0);\n"
                            "	padding-top: 5px;\n"
                            "	padding-bottom: 5px;\n"
                            "	margin: 0px;\n"
                            "	padding: 5px;\n"
                            "	alignment: center\n"
                            "}\n"
                            "\n"
                            "QPushButton:checked\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(255, 214, 127, 255), stop:0.630931 rgba(255, 196, 93, 255), stop:1 rgba(254, 219, 158, 255));\n"
                            "	border: 1px solid rgb(194, 155, 41);\n"
                            "	border-radius: 3px;\n"
                            "	padding: -5px;\n"
                            "}\n"
                            "\n"
                            "QToolButton:checked\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(255, 214, 127, 255), stop:0.630931 rgba(255, 196, 93, 255), stop:1 rgba(254, 219, 158, 255));\n"
                            "	border: 1px solid rgb(194, 155, 41);\n"
                            "	border-radius: 3px;\n"
                            "}\n"
                            "\n"
                            "QPushButton:hover\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(255, 255, 219, 255), stop:0.630931 rgba(255, 206, 104, 255), stop:1 rgba(253, 236, 219, 255));\n"
                            "	border: 1px solid rgb(255, 183, 0);\n"
                            "	border-radius: 3px;\n"
                            "   padding: -2px;\n"
                            "}\n"
                            "\n"
                            "QToolButton:hover\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(255, 255, 219, 255), stop:0.630931 rgba(255, 206, 104, 255), stop:1 rgba(253, 236, 219, 255));\n"
                            "	border: 1px solid rgb(255, 183, 0);\n"
                            "	border-radius: 3px;\n"
                            "}\n"
                            "\n"
                            "QPushButton:pressed\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(242, 242, 183, 255), stop:0.630931 rgba(245, 187, 86, 255), stop:1 rgba(244, 207, 147, 255));\n"
                            "	border: 1px solid rgb(194, 155, 41);\n"
                            "	border-radius: 3px;\n"
                            "	padding: -5px;\n"
                            "}"
                            "\n"
                            "QToolButton:pressed\n"
                            "{\n"
                            "	background-color: qradialgradient(spread:pad, cx:0.499, cy:1, radius:0.952, fx:0.497, fy:1, stop:0 rgba(242, 242, 183, 255), stop:0.630931 rgba(245, 187, 86, 255), stop:1 rgba(244, 207, 147, 255));\n"
                            "	border: 1px solid rgb(194, 155, 41);\n"
                            "	border-radius: 3px;\n"
                            "}\n"
                       );

}

void LRibbon::AddTab(QString name, QHBoxLayout *layout)
{
    QWidget *widget = new QWidget();
    layout->addStretch();
    layout->setMargin(2);
    widget->setLayout(layout);
    this->addTab(widget, name);
}
