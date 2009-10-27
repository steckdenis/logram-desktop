#include "mwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	mwindow magellan;
	magellan.show();

	QTranslator translator;
	
	QString locale = QLocale::system().name();
	translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&translator);
  
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

	return app.exec();
}
