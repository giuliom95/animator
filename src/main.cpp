#include <iostream>

#include <QtWidgets>

#include "mainwin.hpp"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	AnimatorMainWindow win;	
	win.show();

	return app.exec();
}