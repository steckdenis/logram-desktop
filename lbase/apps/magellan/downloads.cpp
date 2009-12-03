#include "mwindow.h"

void mwindow::download(QNetworkReply *reply)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void mwindow::replyFinished(QNetworkReply *reply)
{
}
