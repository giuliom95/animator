#include <iostream>

#include <QtWidgets>

#include "mainwin.hpp"
#include "appstate.hpp"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	AppState appState;
	appState.currentFrame = 0;
	appState.upperFrame = 25;
	appState.lowerFrame = 0;

	AnimatorMainWindow win{appState};
	win.show();

	return app.exec();
}