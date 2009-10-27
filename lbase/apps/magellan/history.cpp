#include "mwindow.h"

void mwindow::addToHistory(QString title, QString url)
{
	QSettings set("Logram", "Magellan_History");
	set.beginGroup(title);
	set.setValue("title", title);
	set.setValue("url", url);
	set.endGroup();
}