#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class QClipboard;
class QComboBox;
class QFontComboBox;
class QLineEdit;
class QScrollArea;
class QCheckBox;

class CharacterWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QTextEdit *parentText);

public slots:
    void findStyles(const QFont &font);
    void findSizes(const QFont &font);
    void insertCharacter(const QString &character);
    void updateClipboard();

private:
    CharacterWidget *characterWidget;
    QClipboard *clipboard;
    QComboBox *styleCombo;
    QComboBox *sizeCombo;
    QFontComboBox *fontCombo;
    QLineEdit *lineEdit;
    QScrollArea *scrollArea;
    QCheckBox *fontMerging;
    QTextEdit *text;
};

#endif
