#include <LPushButton.h>
#include <QFile>
#include <QSettings>
#include <QTextStream>

LPushButton::LPushButton(QWidget *) {
	drawStyleSheet(); }
LPushButton::LPushButton(QString text, QWidget *) {
	setText(text);
	drawStyleSheet(); }
LPushButton::LPushButton(QIcon icon, QString text, QWidget *) {
	setIcon(icon);
	setText(text);
	drawStyleSheet(); }
LPushButton::LPushButton(QString text, QObject* receiver, const char* member, QWidget *) {
	setText(text);
	drawStyleSheet(); 
	connect(this, SIGNAL(clicked()), receiver, member); } 
LPushButton::LPushButton(QIcon icon, QString text, QObject* receiver, const char* member, QWidget *) {
	setIcon(icon);
	setText(text);
	drawStyleSheet(); 
	connect(this, SIGNAL(clicked()), receiver, member); }
void LPushButton::drawStyleSheet() {
	QSettings set("Logram", "Theme");
	QString theme = set.value("Qt_theme").toString();
	if(theme.isNull())
		theme = "default";
	QFile f("/usr/share/logram/qt/" + theme + "/QPushButton.css");
	if(!f.open(QIODevice::ReadOnly))
		return;
	QTextStream in(&f);
	QString styleSheet = in.readAll();
	setStyleSheet(styleSheet); }
