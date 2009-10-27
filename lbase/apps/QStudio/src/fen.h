//autre test :D 
#ifndef __FEN_H__
#define __FEN_H__

#include <QtGui>
#include <QtWebKit>
#include "ui_Main.h"
#include <iostream>

using namespace std;


class Fen : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT
	public :
		Fen();
	
	private slots :
		void changePage();
		
		void changeTitre();
		
		void chargementFini();
		
		//Changement de la barre de status
		void changeStatus(QString status);
		void setStatusCharge(int charge);
		void setStatusFini();
		void precedent();
		
	private :
		void loadPage(QString url);
		
		QString history[10]; //historique
		int historyEnCours;
		//QStringList::const_iterator iteratorHistory;
		QString strPrecedent;
};



#endif // __FEN_H__

