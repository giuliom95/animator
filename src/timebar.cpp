#include "timebar.hpp"

TimeBar::TimeBar(OGLWidget& oglw) : QHBoxLayout(), 
									oglw{oglw},
									playing{false} {

	playButtonWidget = new QPushButton("Play");

	spinBoxWidget = new QSpinBox();
	spinBoxWidget->setMinimum(0);
	spinBoxWidget->setMaximum(99);
	spinBoxWidget->setPrefix("Frame: ");

	sliderWidget = new QSlider(Qt::Horizontal);
	sliderWidget->setMinimum(0);
	sliderWidget->setMaximum(99);
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
	spinBoxWidget->setValue(frame);
	sliderWidget->setValue(frame);
	oglw.setFrame(frame);
}

void TimeBar::nextFrame() {
	const auto thisFrame = spinBoxWidget->value();
	const auto nextFrame = thisFrame == 99 ? 0 : thisFrame + 1;
	changeFrame(nextFrame);
}