#ifndef TIMEBAR_HPP
#define TIMEBAR_HPP

#include <QtWidgets>
#include "oglWidget.hpp"
#include "appstate.hpp"

class TimeBar : public QHBoxLayout {
	Q_OBJECT
public:
	explicit TimeBar(AppState& appState, OGLWidget& oglw);

public slots:
	void handlePlayButton();
	void changeFrame(int frame);
	void nextFrame();

private:
	AppState&		appState;

	OGLWidget&		oglw;

	bool			playing;
	QTimer* 		nextFrameScheduler;

	QPushButton*	playButtonWidget;
	QSlider*		sliderWidget;
	QSpinBox*		spinBoxWidget;
};

#endif