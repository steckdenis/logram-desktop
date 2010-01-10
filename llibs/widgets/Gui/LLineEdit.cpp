#include <LLineEdit.h>
#include <QFile>
#include <QSettings>
#include <QTextStream>

LLineEdit::LLineEdit(QWidget *) {
	drawStyleSheet(); }
LLineEdit::LLineEdit(QString contents, QWidget *) {
	setText(contents);
	drawStyleSheet(); }
void LLineEdit::drawStyleSheet() {
	QSettings set("Logram", "Theme");
	QString theme = set.value("Qt_theme").toString();
	if(theme.isNull())
		theme = "default";
	QFile f("/usr/share/logram/qt/" + theme + "/QLineEdit.css");
	if(!f.open(QIODevice::ReadOnly))
		return;
	QTextStream in(&f);
	QString styleSheet = in.readAll();
	setStyleSheet(styleSheet); }
