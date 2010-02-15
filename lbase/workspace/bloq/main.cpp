/*
 * main.cpp
 * This file is part of BLoQ (Logram)
 *
 * Copyright (C) 2009 - Denis Steckelmacher
 *               2010 - Flavien Lefebvre <flavien-lefebvre@hotmail.fr>
 *
 * BLoQ (Logram) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * BLoQ (Logram) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BLoQ (Logram); if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <Win.h>

#include <LApplication.h>

int main(int argc, char **argv)
{
        LApplication app(argc, argv);

        Win *win = new Win();

        win->show();

        return app.exec();
}
