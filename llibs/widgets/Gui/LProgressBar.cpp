#include <LProgressBar.h>
#include <QFile>
#include <QSettings>
#include <QTextStream>

LProgressBar::LProgressBar(QWidget *) {
	drawStyleSheet(); }
void LProgressBar::drawStyleSheet() {
	QSettings set("Logram", "Theme");
	QString theme = set.value("Qt_theme").toString();
	if(theme.isNull())
		theme = "default";
	QFile f("/usr/share/logram/qt/" + theme + "/QProgressBar.css");
	if(!f.open(QIODevice::ReadOnly))
		return;
	QTextStream in(&f);
	QString styleSheet = in.readAll();
	setStyleSheet(styleSheet); }
