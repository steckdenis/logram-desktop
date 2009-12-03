#ifndef GOOGLECOMPLETER_H
#define GOOGLECOMPLETER_H

#include <QObject>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QNetworkAccessManager>

class GoogleCompleter : public QObject
{
    Q_OBJECT

 public:
     GoogleCompleter(QLineEdit *parent = 0);
     ~GoogleCompleter();
     bool eventFilter(QObject *obj, QEvent *ev);
     void showCompletion(const QStringList &choices, const QStringList &hits);

 public slots:
     void doneCompletion();
     void preventSuggest();
     void autoSuggest();
     void handleNetworkData(QNetworkReply *networkReply);

 private:
     QLineEdit *edit;
     QTreeWidget *popup;
     QTimer *timer;
     QNetworkAccessManager networkManager;
};

#endif // GOOGLECOMPLETER_H
