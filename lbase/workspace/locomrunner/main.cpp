/*
 * main.cpp
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

#include <iostream>
using namespace std;

#include <LApplication.h>

#include <Runner.h>

#include <QStringList>

int main(int argc, char **argv)
{
        LApplication app(argc, argv);
        
        if (app.arguments().count() == 0)
        {
                cout << "LoCoM runner v0.1" << endl;
                cout << endl;
                cout << "LoCoM Runner is free software; you can redistribute it and/or modify" << endl;
                cout << "it under the terms of the GNU General Public License as published by" << endl;
                cout << "the Free Software Foundation; either version 2 of the License, or" << endl;
                cout << "(at your option) any later version." << endl;
                cout << endl;
                cout << "Usage :" << endl;
                cout << "        locomrunner [options] filename.locom" << endl;
                
                return 0;
        }
        else
        {
                QString fileName = app.arguments().last();
                
                Runner *runner = new Runner(fileName);
        }
        
        return app.exec();
}
