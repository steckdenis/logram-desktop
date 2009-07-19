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

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
        : QMainWindow(parent, flags)
{
        ui.setupUi(this);

        tray = new QSystemTrayIcon(this);

        tray->setIcon(QIcon(":/icone/icone.png"));
        tray->show();

        connect(ui.buttonCapture, SIGNAL(clicked()), this, SLOT(capture()));

        connect(ui.actionQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
        connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
        connect(ui.actionLicence, SIGNAL(triggered()), this, SLOT(licence()));

        connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated()));
        connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

        connect(&full, SIGNAL(imageSaved()), this, SLOT(show()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::capture()
{
        int delai = ui.editDelai->value();

        ui.statusBar->showMessage("Capture en cours");

        if (ui.checkHide->isChecked())
        {
                this->hide();
        }

        if (delai == 0)
        {
                delai = 1;
        }

        //wid=QApplication::desktop()->winId();

#ifdef Q_OS_WIN
        delai *= 1000;
        Sleep(delai);
#else
        sleep(delai);
#endif

        QPixmap image;

        image = QPixmap::grabWindow(QApplication::desktop()->winId());

        if (!ui.checkEntiere->isChecked())
        {
                chooseSize(&image);
        }
        else
        {
                QString dest = QFileDialog::getSaveFileName(this, "Emplacement de sauvegarde", "", "Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xpm *.xbm)");

                if (dest != NULL)
                {
                        image.save(dest);
                }

                if (ui.checkHide->isChecked())
                {
                        this->show();
                }

                ui.statusBar->showMessage("Capture réalisée avec succès", 5000);
        }
}

void MainWindow::trayActivated()
{
        if (this->isHidden())
        {
                this->show();
        }
        else
        {
                this->hide();
        }
}

void MainWindow::chooseSize(QPixmap *image)
{
        ui.statusBar->showMessage("Capture réalisée avec succès", 5000);

        full.setPixmap(*image);
        full.move(0, 0);
        full.showFullScreen();

        /*#ifdef Q_WS_X11
           XRaiseWindows(QX11Info::display(), full.winId());
         #endif*/
}

void MainWindow::aboutQt()
{
        QMessageBox::aboutQt(this, "Bibliothèque Qt");
}

void MainWindow::about()
{
        QMessageBox::about(this, "A propos", "<h1><i>PixGrab</i></h1><br />"
                                             "Ce programme a été réalisé par Cleriot Simon &lt;<a href='mailto:malgon33@gmail.com'>malgon33@gmail.com</a>&gt;<br /><br />"
                                             "Vous trouverez la licence dans le fichier LICENCE.txt contenu dans le dossier du programme<br /><br />"
                                             " <u>Mises à jours</u> (pour plus de renseignements voir versions.txt)"
                                             "<ul>"
                                             "<li><u>Dimanche 23/11/2008 [14h]</u> : <b>Version 0.1</b></li>"
                                             "<li><u>Dimanche 23/11/2008 [16h30]</u> : <b>Version 0.2</b></li>"
                                             "<li><u>Dimanche 23/11/2008 [17h15]</u> : <b>Version 0.2.10</b></li>"
                                             "<li><u>Lundi 24/11/2008 [21h]</u> : <b>Version 0.3</b></li>"
                                             "<li><u>Dimanche 30/11/2008 [12h30]</u> : <b>Version 0.4</b></li>"
                                             "<li><u>Dimanche 30/11/2008 [17h40]</u> : <b>Version 0.5</b></li>"
                                             "<li><u>Jeudi 11/12/2008 [14h30]</u> : <b>Version 1.0</b></li>"
                                             "<li><u>Samedi 21/03/2009 [21h50]</u> : <b>Version 1.0.1</b></li>"
                                             "</ul>"

                                             "Remerciement à <b>rumpl</b> du chan #qt-fr@irc.freenode.net ainsi qu'au forum <a href=\"http://forum.qtfr.org/\">http://forum.qtfr.org/</a>");
}

void MainWindow::licence()
{
        QMessageBox::about(this, "Licence de PixGrab", "* Copyright (c) 2009, Cleriot Simon <malgon33@gmail.com><br />"
                                                       "* All rights reserved.<br />"
                                                       "* Redistribution and use in source and binary forms, with or without"
                                                       "* modification, are permitted provided that the following conditions are met:<br />"
                                                       "*<br/>"
                                                       "*     * Redistributions of source code must retain the above copyright<br />"
                                                       "*       notice, this list of conditions and the following disclaimer.<br />"
                                                       "*     * Redistributions in binary form must reproduce the above copyright<br />"
                                                       "*       notice, this list of conditions and the following disclaimer in the<br />"
                                                       "*       documentation and/or other materials provided with the distribution.<br />"
                                                       "*     * Neither the name of the University of California, Berkeley nor the<br />"
                                                       "*       names of its contributors may be used to endorse or promote products<br />"
                                                       "*       derived from this software without specific prior written permission.<br />"
                                                       "*<br />"
                                                       "* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY<br />"
                                                       "* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED<br />"
                                                       "* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE<br />"
                                                       "* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY<br />"
                                                       "* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES<br />"
                                                       "* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;<br />"
                                                       "* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND<br />"
                                                       "* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT<br />"
                                                       "* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS<br />"
                                                       "* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.<br />");
}
