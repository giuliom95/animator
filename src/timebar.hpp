#ifndef TIMEBAR_HPP
#define TIMEBAR_HPP

#include <QtWidgets>
#include "oglWidget.hpp"

class TimeBar : public QHBoxLayout {
	Q_OBJECT
public:
	explicit TimeBar(OGLWidget& oglw);

public slots:
	void handlePlayButton();
	void changeFrame(int frame);
	void nextFrame();

private:
	int				topFrame;
	
	OGLWidget&		oglw;

	bool			playing;
	QTimer* 		nextFrameScheduler;

	QPushButton*	playButtonWidget;
	QSlider*		sliderWidget;
	QSpinBox*		spinBoxWidget;
};

#endif