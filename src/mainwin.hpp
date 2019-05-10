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
	void handleBlurSwitch();

private:
	QPushButton*	zoomButton;
	QCheckBox*		blurSwitch;
	OGLWidget*		oglWidget;
};

#endif