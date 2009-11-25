#include "mwindow.h"

void mwindow::download(QNetworkReply *reply)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(request);
}

void mwindow::replyFinished(QNetworkReply *reply)
{
}
