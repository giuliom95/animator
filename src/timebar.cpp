#include "timebar.hpp"

TimeBar::TimeBar(AppState& appState, OGLWidget& oglw) : QHBoxLayout(),
														appState{appState},
														oglw{oglw},
														playing{false} {

	playButtonWidget = new QPushButton("Play");

	spinBoxWidget = new QSpinBox();
	spinBoxWidget->setMinimum(appState.lowerFrame);
	spinBoxWidget->setMaximum(appState.upperFrame);
	spinBoxWidget->setPrefix("Frame: ");

	sliderWidget = new QSlider(Qt::Horizontal);
	sliderWidget->setMinimum(appState.lowerFrame);
	sliderWidget->setMaximum(appState.upperFrame);
	sliderWidget->setSingleStep(1);
	sliderWidget->setPageStep(1);
	sliderWidget->setTickPosition(QSlider::TicksBothSides);
	sliderWidget->setTickInterval(1);

	setContentsMargins(0,0,0,0);
	addWidget(playButtonWidget);
	addWidget(sliderWidget);
	addWidget(spinBoxWidget);

	connect(playButtonWidget,	SIGNAL (clicked()),			this, SLOT (handlePlayButton()));
	connect(spinBoxWidget,		SIGNAL (valueChanged(int)),	this, SLOT (changeFrame(int)));
	connect(sliderWidget,		SIGNAL (valueChanged(int)),	this, SLOT (changeFrame(int)));

	nextFrameScheduler = new QTimer(this);
	connect(nextFrameScheduler, SIGNAL(timeout()), this, SLOT(nextFrame()));
}

void TimeBar::handlePlayButton() {
	if(!playing) {
		playing = true;
		playButtonWidget->setText("Pause");
		nextFrameScheduler->start(41);
	} else {
		playing = false;
		playButtonWidget->setText("Play");
		nextFrameScheduler->stop();
	}
}

void TimeBar::changeFrame(int frame) {
	appState.currentFrame = frame;
	spinBoxWidget->setValue(frame);
	sliderWidget->setValue(frame);
	oglw.setFrame(frame);
}

void TimeBar::nextFrame() {
	const auto nextFrame = appState.currentFrame == appState.upperFrame ? 
		appState.lowerFrame : appState.currentFrame + 1;
	changeFrame(nextFrame);
}