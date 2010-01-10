#include <QPushButton>

class LPushButton : public QPushButton {

public:
	LPushButton(QWidget *parent = 0);
	LPushButton(QString text, QWidget *parent = 0);
	LPushButton(QIcon icon, QString text, QWidget *parent = 0);
	LPushButton(QString text, QObject*, const char*, QWidget *parent = 0);
	LPushButton(QIcon icon, QString text, QObject*, const char*, QWidget *parent = 0);
private:
	void drawStyleSheet();
};
