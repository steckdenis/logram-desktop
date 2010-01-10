#include <LGroupBox.h>
#include <QFile>
#include <QSettings>
#include <QTextStream>

LGroupBox::LGroupBox(QWidget *) {
	drawStyleSheet(); }
LGroupBox::LGroupBox(QString title, QWidget *) {
	setTitle(title);
	drawStyleSheet(); }
void LGroupBox::drawStyleSheet() {
	QSettings set("Logram", "Theme");
	QString theme = set.value("Qt_theme").toString();
	if(theme.isNull())
		theme = "default";
	QFile f("/usr/share/logram/qt/" + theme + "/QGroupBox.css");
	if(!f.open(QIODevice::ReadOnly))
		return;
	QTextStream in(&f);
	QString styleSheet = in.readAll();
	setStyleSheet(styleSheet); }
