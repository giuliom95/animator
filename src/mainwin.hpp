#ifndef MAINWIN_HPP
#define MAINWIN_HPP

#include <QtWidgets>

#include "appstate.hpp"
#include "oglWidget.hpp"
#include "timebar.hpp"

class AnimatorMainWindow : public QWidget {
	Q_OBJECT
public:
	explicit AnimatorMainWindow(AppState& appState);

public slots:
	void handleZoomButton();
	void handleBrushSizeWidget(int value);
	void toggleOnionSkin();

private:
	AppState&		appState;
	QPushButton*	zoomButton;
	OGLWidget*		oglWidget;
};

#endif