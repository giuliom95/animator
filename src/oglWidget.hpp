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

	void newCanvas(const int imageWidth, const int imageHeight);
	void canvasManipulation();

	void setZoom(float zoomFactor, float centerX, float centerY);

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
	int		imageWidth,			imageHeight;
	float	cameraPanX,			cameraPanY;
	float	lastMouseX,			lastMouseY;
	int		mouseOnCanvasX,		mouseOnCanvasY;
	int		lastMouseOnCanvasX,	lastMouseOnCanvasY;

	float zoomFactor;

	std::array<bool, 3> mouseButtonsPressed;

	QPushButton& zoomButton;

	GLuint canvasTexId;
	GLuint strokeTexId;

	GLuint fboId;

	GLuint canvasVtxBuf;
	GLuint canvasUvBuf;
	QOpenGLVertexArrayObject vao;

	GLuint canvasManip_progId;
	GLuint canvasManip_canvasTexLocId;
	GLuint canvasManip_strokeTexLocId;
	GLuint canvasManip_mousePosLocId;
	GLuint canvasManip_lastMousePosLocId;

	GLuint showCanvas_progId;
	GLuint showCanvas_matrixLocId;
	GLuint showCanvas_canvasTexLocId;

	const GLuint loadShader(std::string path, GLenum shaderType);
	const GLuint linkShaderProgram(GLuint vertShaderId, GLuint fragShaderId);
};

#endif