#ifndef OGL_WIDGET_HPP
#define OGL_WIDGET_HPP

#define GL_GLEXT_PROTOTYPES 1

#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>
#include <thread>

#include <fstream>

#include <QtWidgets>

#include "utilities.hpp"

class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	Q_OBJECT
public:
	OGLWidget(QPushButton& zoomButton);

	void newCanvas(const int canvasWidth, const int imageHeight);
	void strokeManagement();
	void transferStroke2Canvas();

	void setZoom(float zoomFactor, float centerX, float centerY);
	void setBrushSize(int newSize);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent	(QMouseEvent*	event);
	void mouseReleaseEvent	(QMouseEvent*	event);
	void mouseMoveEvent		(QMouseEvent*	event);
	void wheelEvent			(QWheelEvent*	event);
	void tabletEvent		(QTabletEvent*	event);

private:
	int							widgetWidth,		widgetHeight;
	int							canvasWidth,		canvasHeight;
	float						cameraPanX,			cameraPanY;
	QPointF						lastCursorPos;

	int							brushSize;

	float 						zoomFactor;

	QTime 						time;

	std::array<bool, 3> 		mouseButtonsPressed;
	bool						brushDown;
	bool						panning;

	QPushButton& 				zoomButton;

	GLuint						canvasTexId;
	GLuint						strokeTexId;

	GLuint						fboId;

	QOpenGLVertexArrayObject	stroke_vao;
	GLuint 						stroke_vtxBuf;
	GLuint						stroke_progId;
	GLuint						stroke_canvasSizeLocId;
	GLuint						stroke_brushSizeLocId;
	std::vector<GLfloat>		stroke_points;

	QOpenGLVertexArrayObject	showCanvas_vao;
	GLuint 						showCanvas_progId;
	GLuint 						showCanvas_matrixLocId;
	GLuint 						showCanvas_strokeTexLocId;
	GLuint 						showCanvas_canvasTexLocId;

	QOpenGLVertexArrayObject	stroke2canvas_vao;
	GLuint 						stroke2canvas_progId;
	GLuint 						stroke2canvas_strokeTexLocId;
	GLuint 						stroke2canvas_canvasTexLocId;
	bool						stroke2canvas_doIt;

	void pan(const QPointF cursorPosOnScreen);

	// Brush is lifted from canvas
	void liftBush();
	// Brush dragged on canvas
	void dragBrush(const QPointF cursorPosOnWidget, const float pressure=1);

	QPointF widget2canvasCoords(const QPointF& widgetPos);

	GLuint loadShader(std::string path, GLenum shaderType);
	GLuint linkShaderProgram(	GLuint vertShaderId, 
								GLuint fragShaderId,
								GLuint geomShaderId = 0,
								GLuint teseShaderId = 0);

	void testDraw();
};

#endif