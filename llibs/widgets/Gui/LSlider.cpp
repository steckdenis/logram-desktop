#include <LSlider.h>
#include <QFile>
#include <QSettings>
#include <QTextStream>

LSlider::LSlider(QWidget *) {
	drawStyleSheet(); }
LSlider::LSlider(Qt::Orientation orientation, QWidget *) {
	setOrientation(orientation);
	drawStyleSheet(); }
void LSlider::drawStyleSheet() {
	QSettings set("Logram", "Theme");
	QString theme = set.value("Qt_theme").toString();
	if(theme.isNull())
		theme = "default";
	QFile f("/usr/share/logram/qt/" + theme + "/QSlider.css");
	if(!f.open(QIODevice::ReadOnly))
		return;
	QTextStream in(&f);
	QString styleSheet = in.readAll();
	setStyleSheet(styleSheet); }
