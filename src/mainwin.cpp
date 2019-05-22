#include "mainwin.hpp"

AnimatorMainWindow::AnimatorMainWindow() : QWidget() {
	resize(600, 600);
	setWindowTitle(" ");

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);

	auto* bottomBarLayoutContainer = new QWidget;	// This is used to give a fixed height to the layout
	bottomBarLayoutContainer->setFixedHeight(25);
	auto* bottomBarLayout = new QHBoxLayout;
	bottomBarLayout->setContentsMargins(0,0,5,0);
	zoomButton = new QPushButton("100.0%");
	zoomButton->setFixedWidth(70);
	brushSizeSlider = new QSlider(Qt::Orientation::Horizontal);
	brushSizeSlider->setMinimum(0);
	brushSizeSlider->setMaximum(100);
	brushSizeSlider->setValue(10);

	bottomBarLayout->addWidget(zoomButton);
	bottomBarLayout->addWidget(brushSizeSlider);
	bottomBarLayout->addStretch();
	bottomBarLayoutContainer->setLayout(bottomBarLayout);

	oglWidget = new OGLWidget(*zoomButton);
	oglWidget->setMouseTracking(true);

	mainLayout->addWidget(oglWidget);
	mainLayout->addWidget(bottomBarLayoutContainer);
	setLayout(mainLayout);

	connect(zoomButton,			SIGNAL (clicked()),					this, SLOT (handleZoomButton()));	
	connect(brushSizeSlider,	SIGNAL (valueChanged(int)),	this, SLOT (handleBrushSizeSlider()));
}

void AnimatorMainWindow::handleZoomButton() {
	const auto cx = oglWidget->width() / 2;
	const auto cy = oglWidget->height() / 2;
	oglWidget->setZoom(1.0f, cx, cy);
}

void AnimatorMainWindow::handleBrushSizeSlider() {
	std::cout << brushSizeSlider->value() << std::endl;
}