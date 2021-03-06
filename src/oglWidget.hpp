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
#include <QGLWidget> 
#include <QOpenGLFunctions_4_5_Core>

#include "utilities.hpp"
#include "appstate.hpp"

class OGLWidget : public QGLWidget, protected QOpenGLFunctions_4_5_Core {
	Q_OBJECT
public:
	OGLWidget(AppState& appState, QPushButton& zoomButton);

	void newAnimation(const int canvasWidth, const int imageHeight);

	void setFrame(int newFrame);
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
	AppState& 					appState;

	int							widgetWidth,		widgetHeight;
	int							canvasWidth,		canvasHeight;
	float						cameraPanX,			cameraPanY;
	QPointF						lastCursorPos;

	int							currentFrame;
	int							currentFrameLayerIndex;
	std::vector<int>			layers2frame;
	int							skinLevels;
	std::vector<Utils::Frame>	framesData;

	int							brushSize;

	float 						zoomFactor;

	QTime 						time;

	std::array<bool, 3> 		mouseButtonsPressed;
	bool						brushDown;
    bool                        isTabletStroke;
	bool						panning;

	QPushButton& 				zoomButton;

	GLuint						canvasesTexId;
	GLuint						strokeTexId;

	GLuint						fboId;

	QOpenGLVertexArrayObject	stroke_vao;
	GLuint						stroke_vtxBuf;
	GLuint						stroke_progId;
	GLuint						stroke_canvasSizeLocId;
	GLuint						stroke_brushSizeLocId;
	std::vector<GLfloat>		stroke_points;
	std::vector<GLuint>			stroke_points_indices;
	int							stroke_current_index;

	QOpenGLVertexArrayObject	showCanvas_vao;
	GLuint						showCanvas_progId;
	GLuint						showCanvas_matrixLocId;
	GLuint						showCanvas_strokeTexLocId;
	GLuint						showCanvas_canvasTexLocId;
	GLuint						showCanvas_onionActiveLocId;
	GLuint						showCanvas_currentFrameLayerIndexLocId;
	GLuint						showCanvas_lowerCurrentUpperFrameLocId;
	GLuint						showCanvas_eraserActiveLocId;

	QOpenGLVertexArrayObject	stroke2canvas_vao;
	GLuint						stroke2canvas_progId;
	GLuint						stroke2canvas_strokeTexLocId;
	GLuint						stroke2canvas_canvasTexLocId;
	GLuint						stroke2canvas_currentFrameLayerIndexLocId;
	GLuint						stroke2canvas_eraserActiveLocId;
	bool						stroke2canvas_doIt;

	void pan(const QPointF cursorPosOnScreen);

	// Brush is lifted from canvas
	void liftBush();
	// Brush dragged on canvas
    void dragBrush(const QPointF cursorPosOnWidget, const float pressure=0.999f);

	QPointF widget2canvasCoords(const QPointF& widgetPos);

	void strokeManagement();
	void transferStroke2Canvas();

	GLuint loadShader(std::string path, GLenum shaderType);
	GLuint linkShaderProgram(	GLuint vertShaderId, 
								GLuint fragShaderId,
								GLuint geomShaderId = 0,
								GLuint teseShaderId = 0);

	void testDraw();
};

#endif
