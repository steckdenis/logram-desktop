#include <QApplication>
#include <mwindow.h>

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);

	QPixmap splash("../icons/ghetto10");

	QSplashScreen splashScreen(splash);
	splashScreen.show();
	app.processEvents();
  
	mwindow ghetto;
	ghetto.show();

	splashScreen.finish(&ghetto);

	return app.exec();
}
	
	
  
	 
