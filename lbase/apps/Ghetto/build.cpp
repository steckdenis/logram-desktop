#include <mwindow.h>

void mwindow::createActions()
{
	iconPath = "/usr/share/logram/ghetto_blaster/icons/";

	nwindow    = new QAction(QIcon(iconPath + "tab-new-background.png"), tr("Nouvelle fenêtre"), this);
	quit       = new QAction(QIcon(iconPath + "application-exit.png"), tr("Quitter ghetto"), this);
	back       = new QAction(QIcon(iconPath + "media-seek-backward.png"), tr("Piste precedente"), this);
	playpause  = new QAction(QIcon(iconPath + "media-playback-start.png"), tr("Lecture"), this);
	stop       = new QAction(QIcon(iconPath + "media-playback-stop.png"), tr("Arreter"), this);
	forward    = new QAction(QIcon(iconPath + "media-seek-forward.png"), tr("Piste suivante"), this);
	addCD      = new QAction(QIcon(iconPath + "media-optical-audio.png"), tr("Importer un CD"), this);
	about      = new QAction(QIcon(iconPath + ""), tr("A propos de Ghetto Blaster"), this);
	qabout     = new QAction(QIcon(iconPath + "qt.png"), tr("A propos de Qt"), this);
	labout     = new QAction(QIcon(iconPath + ""), tr("A propos de Logram"), this);

	connect(nwindow, SIGNAL(triggered()), this, SLOT(newWindow()));
	connect(playpause, SIGNAL(triggered()), this, SLOT(play()));
	connect(stop, SIGNAL(triggered()), this, SLOT(stopReading()));
	connect(qabout, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void mwindow::createMenus()
{
	menuBar = new QMenuBar;
		file  = menuBar->addMenu(tr("&Fichier"));
			file->addAction(nwindow);			
			file->addAction(quit);
		edit  = menuBar->addMenu(tr("&Edition"));
		view  = menuBar->addMenu(tr("&Affichage"));
		media = menuBar->addMenu(tr("&Media"));
			media->addAction(back);
			media->addAction(playpause);
			media->addAction(stop);
			media->addAction(forward);
		bibli = menuBar->addMenu(tr("&Bibliotheque"));
			bibli->addAction(addCD);
		help  = menuBar->addMenu(tr("&Aide"));		
			help->addAction(about);
			help->addAction(qabout);
			help->addAction(labout);
	
	setMenuBar(menuBar);
}

void mwindow::createToolBar()
{
	mediaBar = addToolBar("media");
	timeSlider = new Phonon::SeekSlider();
		mediaBar->addAction(back);
		mediaBar->addAction(playpause);
		mediaBar->addAction(stop);
		mediaBar->addAction(forward);
		mediaBar->addWidget(timeSlider);

	mediaBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	mediaBar->setIconSize(QSize(32, 32));
}
