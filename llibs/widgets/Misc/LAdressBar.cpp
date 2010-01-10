#include <LAdressBar.h>

LAdressBar::LAdressBar()
{
    currentUrl = "";
    currentText = "";
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    bar = new QLineEdit(this);
    bar->setStyleSheet("QLineEdit { \n"
                       "border-top-left-radius: 4px; \n"
                       "border-bottom-left-radius: 4px; \n"
                       "border: 1px solid gray; \n"
                       "border-right: none; \n"
                       "height: 22px; \n"
                       "background-color: rgb(240, 240, 240); \n"
                       "selection-background-color: darkgray; \n } \n"
                       "QLineEdit::hover {\n"
                       "background-color: rgb(250, 250, 250); \n } \n"
                       "QLineEdit::focus {\n"
                       "background-color: rgb(255, 255, 255);\n } \n" );
    layout->addWidget(bar);

    go = new QPushButton(QIcon("/usr/share/logram/magellan/icons/go.png"), tr(""));
    go->setToolTip(tr("Valider"));
    go->setStyleSheet("QPushButton { \n"
                      "border-top-right-radius: 4px; \n"
                      "border-bottom-right-radius: 4px; \n"
                      "background-color: rgb(240, 240, 240); \n"
                      "border: 1px solid gray; \n"
                      "border-left-width: 1px; \n"
                      "height: 26px; \n "
                      "margin: 3px; \n"
                      "margin-left: 0px; \n } \n"
                      "QPushButton::hover { \n"
                      "background-color: rgb(250, 250, 250);\n } \n"
                      "QPushButton::pressed {\n"
                      "background-color: rgbrgb(255, 255, 255);\n }");
    layout->addWidget(go);

    setLayout(layout);

    connect(go, SIGNAL(clicked()), this, SLOT(item_triggered()));
    connect(bar, SIGNAL(returnPressed()), this, SLOT(item_triggered()));
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(focusChanged(QWidget *, QWidget *)));
}

void LAdressBar::setValue(int value)
{
    if(value == 100) {
        setStyleSheet("QLineEdit { \n"
                      "height: 22px; \n"
                      "border-top-left-radius: 4px; \n"
                      "border-bottom-left-radius: 4px; \n"
                      "border: 1px solid gray; \n"
                      "border-right: none; \n"
                      "background-color: rgb(240, 240, 240); \n"
                      "selection-background-color: darkgray; \n } \n"
                      "QLineEdit::hover {\n"
                      "background-color: rgb(250, 250, 250); \n } \n"
                      "QLineEdit::focus {\n"
                      "background-color: rgb(255, 255, 255);\n } \n" );
        return; }
    double f_stop = value / 100.0; // ex : 0,64
    double s_stop = f_stop + 0.001; // ex : 0,65
    QString f_stop_str;
    f_stop_str.setNum(f_stop);
    QString s_stop_str;
    s_stop_str.setNum(s_stop);

    setStyleSheet(
            "QLineEdit { \n"
            "height: 22 px; \n"
            "border-top-left-radius: 4px; \n"
            "border-bottom-left-radius: 4px; \n"
            "border: 1px solid gray; \n"
            "border-right-width none; \n"
            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(99, 255, 109, 255), stop:" + f_stop_str + " rgba(99, 255, 109, 255), stop:" + s_stop_str + " rgba(255, 255, 255, 255), stop:1 rgba(255, 255, 255, 255)); \n"
            "selection-background-color: darkgray; \n } \n"
            "QLineEdit::hover {\n"
            "background-color: rgb(250, 250, 250); \n } \n"
            "QLineEdit::focus {\n"
            "background-color: rgb(255, 255, 255);\n } \n");
}

void LAdressBar::focusChanged(QWidget *old, QWidget *now)
{
    if(old == bar)
        bar->setText(currentText);
    else if(now == bar)
        bar->setText(currentUrl.toString());
    else
        return;
}

void LAdressBar::item_triggered()
{
    currentUrl = QUrl(bar->text());
    emit triggered();
}

void LAdressBar::setUrl(QUrl new_url)
{
    if(bar->text() == currentUrl.toString()) {
        currentUrl = new_url;
        bar->setText(currentUrl.toString()); }
    else
        currentUrl = new_url;
}

void LAdressBar::setText(QString new_text)
{
    if(bar->text() == currentText) {
        currentText = new_text;
        bar->setText(currentText); }
    else
        currentText = new_text;
}

QUrl LAdressBar::url()
{
   return currentUrl;
}

QString LAdressBar::text()
{
    return currentText;
}
