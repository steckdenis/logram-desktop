#ifndef LADRESSBAR_H
#define LADRESSBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>

#include <QUrl>

class LAdressBar : public QWidget
{
    Q_OBJECT

public:
    LAdressBar();

    QUrl url();
    QString text();

signals:
    void triggered();

public slots:
    void setUrl(QUrl);
    void setText(QString);
    void setValue(int);
    void focusChanged(QWidget *, QWidget*);

private slots:
    void item_triggered();

private:
    QPushButton *go;
    QLineEdit *bar;

    QUrl currentUrl;
    QString currentText;
};

#endif // LADRESSBAR_H
