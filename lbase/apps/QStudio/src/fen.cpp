//test ^^

#include "fen.h"

Fen::Fen()
{
	setupUi(this);
	
	
	historyEnCours=-1;
	loadPage("http://www.logram-project.org/");
	
	buttonSuivant->setEnabled(false);
	buttonPrecedent->setEnabled(false);

	connect(okButton,SIGNAL(clicked()),this,SLOT(changePage()));
	connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(changeTitre()));
	connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(setStatusFini()));
	connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(chargementFini()));
	
	connect(webView,SIGNAL(loadProgress(int)),this,SLOT(setStatusCharge(int)));
	
	connect(buttonPrecedent,SIGNAL(clicked()),this,SLOT(precedent()));
			
	show();
}

void Fen::changePage()
{
	loadPage(adresseEdit->text());
}

void Fen::changeTitre()
{
	adresseEdit->setText(webView->url().toString());
}

void Fen::changeStatus(QString status)
{
	statutBar->showMessage(status);
}

void Fen::setStatusCharge(int charge)
{
	QString string;
	string.setNum(charge);
	changeStatus("chargement "+string+"%");
}

void Fen::setStatusFini()
{
	changeStatus("chargement termine");
}

void Fen::precedent()
{
	if(historyEnCours>0)
	{
		historyEnCours-=2;
		loadPage(history[historyEnCours+1]);
		if(historyEnCours+1==0)
		{
			buttonPrecedent->setEnabled(false);
		}
	}
}

void Fen::loadPage(QString url)
{
	if(url.startsWith("http://"))
		webView->load(QUrl(url));
	else
		webView->load(QUrl(url.prepend("http://")));
}

void Fen::chargementFini()
{
	historyEnCours++;
	history[historyEnCours]=webView->url().toString();
	
	if(historyEnCours>0)
	{
		buttonPrecedent->setEnabled(true);
	}
}

