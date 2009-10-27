// in this file are declared all methods related to the personnalisation window
#include "mwindow.h"

void mwindow::defineStart(int index)
{
	settings->setValue("startup", index); 
}

void mwindow::defineHome(QString home)
{
	settings->setValue("home", home);
	homeLineEdit->setText(home);
}

void mwindow::defineHomeBlankPage()
{
	settings->setValue("home", "");
	homeLineEdit->setText("");
}

void mwindow::defineHomeDefaultPage()
{
	settings->setValue("home", "http://titouille56.hostei.com/html/2009/09/logram_welcome/");
	homeLineEdit->setText("http://titouille56.hostei.com/html/2009/09/logram_welcome/");
}

void mwindow::defineHomeCurrentPage()
{
	settings->setValue("home", currentPage()->url().toString());
	homeLineEdit->setText(currentPage()->url().toString());
}

void mwindow::fontDialog()
{
	QFontDialog *fontDialog = new QFontDialog();
}