#include <QProgressBar>

class LProgressBar : public QProgressBar {

public:
	LProgressBar(QWidget *parent = 0);
private:
	void drawStyleSheet();
};
