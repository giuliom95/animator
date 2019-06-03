#include "timebar.hpp"

TimeBar::TimeBar(OGLWidget& oglw) : QHBoxLayout(), oglw{oglw} {

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

	connect(spinBoxWidget,	SIGNAL (valueChanged(int)),	this, SLOT (handleFrameChange(int)));
	connect(sliderWidget,	SIGNAL (valueChanged(int)),	this, SLOT (handleFrameChange(int)));
}

void TimeBar::handleFrameChange(int frame) {
	spinBoxWidget->setValue(frame);
	sliderWidget->setValue(frame);
	oglw.setFrame(frame);
}