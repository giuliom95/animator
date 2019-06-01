#ifndef MAINWIN_HPP
#define MAINWIN_HPP

#include <QtWidgets>

#include "oglWidget.hpp"


class AnimatorMainWindow : public QWidget {
	Q_OBJECT
public:
	explicit AnimatorMainWindow();

public slots:
	void handleZoomButton();
	void handleBrushSizeWidget(int value);
	void handleFrameChange(int frame);

private:
	QPushButton*	zoomButton;
	OGLWidget*		oglWidget;
};

#endif