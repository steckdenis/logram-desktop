#include <QSlider>

class LSlider : public QSlider {

public:
	LSlider(QWidget *parent = 0);
	LSlider(Qt::Orientation orientation, QWidget *parent = 0);
private:
	void drawStyleSheet();
};
