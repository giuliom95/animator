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

	void setZoom(float zoomFactor, float centerX, float centerY);
	void switchBlur();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent *event);

private:
	int		widgetWidth,		widgetHeight;
	int		canvasWidth,		canvasHeight;
	float	cameraPanX,			cameraPanY;
	float	lastMouseX,			lastMouseY;
	int		mouseOnCanvasX,		mouseOnCanvasY;
	int		lastMouseOnCanvasX,	lastMouseOnCanvasY;

	float zoomFactor;

	QTime time;

	std::array<bool, 3> mouseButtonsPressed;

	QPushButton& zoomButton;

	GLuint canvasTexId;
	GLuint strokeTexId;

	GLuint fboId;

	QOpenGLVertexArrayObject	stroke_vao;
	GLuint 						stroke_vtxBuf;
	GLuint						stroke_progId;
	std::vector<GLfloat>		stroke_points;

	QOpenGLVertexArrayObject	showCanvas_vao;
	GLuint 						showCanvas_progId;
	GLuint 						showCanvas_matrixLocId;
	GLuint 						showCanvas_strokeTexLocId;
	GLuint 						showCanvas_canvasTexLocId;
	GLuint 						showCanvas_blurSwitchLocId;

	GLuint loadShader(std::string path, GLenum shaderType);
	GLuint linkShaderProgram(	GLuint vertShaderId, 
								GLuint fragShaderId,
								GLuint geomShaderId = 0);
};

#endif