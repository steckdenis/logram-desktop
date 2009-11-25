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

void mwindow::defineFont(QFont font)
{
}

void mwindow::fontDialog()
{
	QFontDialog *fontDialog = new QFontDialog();
}

void mwindow::openSettings()
{
    settingsWindow = new QMainWindow();
    settingsWindow->setWindowFlags(Qt::Tool);
    settingsWindow->setMaximumSize(650, 475);
    settingsWindow->setWindowTitle(tr("Préférences du Navigateur Web Magellan"));
    QVBoxLayout *layout = new QVBoxLayout();

    QGroupBox *start = new QGroupBox(tr("Démarrage"));
        QLabel *atStart = new QLabel(tr("Au lancement du navigateur : "));
        QComboBox *startComboBox = new QComboBox();
            startComboBox->addItem(tr("Afficher ma page d'accueil"));
            startComboBox->addItem(tr("Afficher une page blanche"));
            startComboBox->addItem(tr("Afficher les dernieres pages visitees"));
            startComboBox->setCurrentIndex(settings->value("startup").toInt());
            connect(startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(defineStart(int)));
        QLabel *homeLabel = new QLabel(tr("Page d'accueil : "));
        homeLineEdit = new QLineEdit();
            homeLineEdit->setText(settings->value("home").toString());
            connect(homeLineEdit, SIGNAL(textEdited(QString)), this, SLOT(defineHome(QString)));
        QPushButton *homeCurrentPage = new QPushButton(tr("Page actuelle"));
            connect(homeCurrentPage, SIGNAL(clicked()), this, SLOT(defineHomeCurrentPage()));
        QPushButton *homeBlankPage = new QPushButton(tr("Page blanche"));
            connect(homeBlankPage, SIGNAL(clicked()), this, SLOT(defineHomeBlankPage()));
        QPushButton *homeDefaultPage = new QPushButton(tr("Par defaut"));
        connect(homeDefaultPage, SIGNAL(clicked()), this, SLOT(defineHomeDefaultPage()));
    QGridLayout *startLayout = new QGridLayout();
    QFontComboBox *font = new QFontComboBox();
    connect(font, SIGNAL(currentFontChanged(QFont)), this, SLOT(defineFont(QFont)));

    startLayout->addWidget(atStart, 0, 0);
    startLayout->addWidget(startComboBox, 0, 2, 1, 3);
    startLayout->addWidget(homeLabel, 1, 0);
    startLayout->addWidget(homeLineEdit, 1, 1, 1, 3);
    startLayout->addWidget(homeCurrentPage, 1, 2);
    startLayout->addWidget(homeBlankPage, 2, 2);
    startLayout->addWidget(homeDefaultPage, 2, 2);
    startLayout->addWidget(font, 3, 0);

    start->setLayout(startLayout);

    layout->addWidget(start);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    settingsWindow->setCentralWidget(widget);
    settingsWindow->show();
}
