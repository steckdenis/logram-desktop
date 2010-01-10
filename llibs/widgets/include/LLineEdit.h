#include <QLineEdit>

class LLineEdit : public QLineEdit {

public:
	LLineEdit(QWidget *parent = 0);
	LLineEdit(QString contents, QWidget *parent = 0);
private:
	void drawStyleSheet();
};
