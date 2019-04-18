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
	int		widgetWidth,	widgetHeight;
	int		imageWidth,		imageHeight;
	float	cameraPanX,		cameraPanY;
	float	lastMouseX,		lastMouseY;

	float zoomFactor;

	std::array<bool, 3> mouseButtonsPressed;

	QPushButton& zoomButton;

	GLuint canvasTexId;

	GLuint canvasFboId;

	GLuint canvasProgId;

	GLuint presentationCanvasProgId;
	GLuint presentationCanvasVertBuf;
	GLuint presentationCanvasUvBuf;
	GLuint presentationCanvasMatLocId;
	GLuint presentationCanvasTexLocId;
	QOpenGLVertexArrayObject presentationCanvasVao;

	const GLuint loadShader(std::string path, GLenum shaderType);
	const GLuint linkShaderProgram(GLuint vertShaderId, GLuint fragShaderId);
};

#endif