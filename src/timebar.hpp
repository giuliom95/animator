#ifndef TIMEBAR_HPP
#define TIMEBAR_HPP

#include <QtWidgets>
#include "oglWidget.hpp"

class TimeBar : public QHBoxLayout {
	Q_OBJECT
public:
	explicit TimeBar(OGLWidget& oglw);

public slots:
	void handleFrameChange(int frame);

private:
	OGLWidget&	oglw;

	QPushButton*	playButtonWidget;
	QSlider*		sliderWidget;
	QSpinBox*		spinBoxWidget;
};

#endif