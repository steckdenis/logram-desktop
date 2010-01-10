#include <QGroupBox>

class LGroupBox : public QGroupBox {

public:
	LGroupBox(QWidget *parent = 0);
	LGroupBox(QString title, QWidget *parent = 0);
private:
	void drawStyleSheet();
};
