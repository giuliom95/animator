#include "oglWidget.hpp"

OGLWidget::OGLWidget(QPushButton& zoomButton) :	QOpenGLWidget{},
												cameraPanX{0},
												cameraPanY{0},
												zoomFactor{1},
												mouseButtonsPressed{false, false, false},
												zoomButton(zoomButton),
												vao{} {
	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(4,5);
	setFormat(format);
};


void OGLWidget::newCanvas(const int w, const int h) {
	imageWidth = w;
	imageHeight = h;

	const auto clearCanv = std::vector<GLubyte>(w*h*3, 255);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, clearCanv.data());

	const auto clearAlpha = std::vector<GLubyte>(w*h, 0);
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, clearAlpha.data());

	update();
}


void OGLWidget::setZoom(float zf, float x, float y) {
	cameraPanX += x / zoomFactor;
	cameraPanY += y / zoomFactor;

	zoomFactor = zf;
	zoomFactor = std::max(0.5f, zoomFactor);
	zoomFactor = std::min(10.f, zoomFactor);

	cameraPanX -= x / zoomFactor;
	cameraPanY -= y / zoomFactor;

	std::stringstream stream;
	stream << std::fixed << std::setprecision(1);
	stream << (100 * zoomFactor) << "%";
	zoomButton.setText(stream.str().c_str());

	update();
}


void OGLWidget::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &canvasTexId);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	newCanvas(400, 300);

	////////////////////////////////////////////
	// Shader program for canvas presentation //
	////////////////////////////////////////////
	const auto showCanvas_vertShadId = loadShader("shaders/showCanvas.vert.glsl", GL_VERTEX_SHADER);
	const auto showCanvas_fragShadId = loadShader("shaders/showCanvas.frag.glsl", GL_FRAGMENT_SHADER);
	
	showCanvas_progId = linkShaderProgram(showCanvas_vertShadId, showCanvas_fragShadId);

	glDeleteShader(showCanvas_vertShadId);
	glDeleteShader(showCanvas_fragShadId);

	showCanvas_matrixLocId		= glGetUniformLocation(showCanvas_progId, "view");
	showCanvas_canvasTexLocId 	= glGetUniformLocation(showCanvas_progId, "canvas");

	////////////////////////////////////////////
	// Shader program for canvas manipulation //
	////////////////////////////////////////////
	const auto canvasManip_VertShadId = loadShader("shaders/canvasManip.vert.glsl", GL_VERTEX_SHADER);
	const auto canvasManip_FragShadId = loadShader("shaders/canvasManip.frag.glsl", GL_FRAGMENT_SHADER);
	
	canvasManip_progId = linkShaderProgram(canvasManip_VertShadId, canvasManip_FragShadId);

	glDeleteShader(canvasManip_VertShadId);
	glDeleteShader(canvasManip_FragShadId);

	canvasManip_canvasTexLocId		= glGetUniformLocation(canvasManip_progId, "canvas");
	canvasManip_strokeTexLocId		= glGetUniformLocation(canvasManip_progId, "stroke");
	canvasManip_mousePosLocId		= glGetUniformLocation(canvasManip_progId, "mousePos");
	canvasManip_lastMousePosLocId	= glGetUniformLocation(canvasManip_progId, "lastMousePos");

	/////////////////////////////
	// Generate vertex buffers //
	/////////////////////////////
	vao.create();
	vao.bind();

	glGenBuffers(1, &canvasUvBuf);
	glGenBuffers(1, &canvasVtxBuf);
	
	const GLfloat vtxBufData[] = {
			  0.0f, 0.0f,
		imageWidth, 0.0f,
			  0.0f, imageHeight,
		imageWidth, imageHeight
	};
	glBindBuffer(GL_ARRAY_BUFFER, canvasVtxBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtxBufData), vtxBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	const GLfloat uvBufData[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, canvasUvBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufData), uvBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//////////////////
	// Generate FBO //
	//////////////////
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	// Attach the binded framebuffer to texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvasTexId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void OGLWidget::resizeGL(int w, int h) {
	widgetWidth = w;
	widgetHeight = h;
}

void OGLWidget::canvasManipulation() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	glViewport(0, 0, imageWidth, imageHeight);
	glUseProgram(canvasManip_progId);

	const GLenum buf[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buf);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glUniform1i(canvasManip_canvasTexLocId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glUniform1i(canvasManip_strokeTexLocId, 1);

	glUniform2i(canvasManip_mousePosLocId,     mouseOnCanvasX,     mouseOnCanvasY);
	glUniform2i(canvasManip_lastMousePosLocId, lastMouseOnCanvasX, lastMouseOnCanvasY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	lastMouseOnCanvasX = mouseOnCanvasX;
	lastMouseOnCanvasY = mouseOnCanvasY;
}


void OGLWidget::paintGL() {

	if(mouseButtonsPressed[0])
		canvasManipulation();

	glViewport(0,0,widgetWidth,widgetHeight);
	glUseProgram(showCanvas_progId);

	const auto invZoom = 1 / zoomFactor;
	const auto left = cameraPanX;
	const auto right = cameraPanX + widgetWidth*invZoom;
	const auto bottom = cameraPanY + widgetHeight*invZoom;
	const auto top = cameraPanY;
	const auto A = 2.0f / (right - left);
	const auto B = 2.0f / (top - bottom);
	const auto C = -1;
	const auto tx = -(right + left) / (right - left);
	const auto ty = -(top + bottom) / (top - bottom);
	const auto tz = 0;
	const std::array<GLfloat, 16> mat{A, 0, 0, tx, 0, B, 0, ty, 0, 0, C, tz, 0, 0, 0, 1};
	glUniformMatrix4fv(showCanvas_matrixLocId, 1, GL_FALSE, mat.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glUniform1i(showCanvas_canvasTexLocId, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OGLWidget::mousePressEvent(QMouseEvent* event) {

	const auto mouseScreenPos = event->screenPos();
	lastMouseX = (int)mouseScreenPos.x();
	lastMouseY = (int)mouseScreenPos.y();

	mouseButtonsPressed[Utils::mapQtMouseBtn(event->button())] = true;

	const auto mouseWidgetPos = event->pos();
	const auto invZoom = 1 / zoomFactor;
	mouseOnCanvasX = invZoom*mouseWidgetPos.x() + cameraPanX;
	mouseOnCanvasY = invZoom*mouseWidgetPos.y() + cameraPanY;
	lastMouseOnCanvasX = mouseOnCanvasX;
	lastMouseOnCanvasY = mouseOnCanvasY;

	if(event->button() == Qt::LeftButton)
		update();
}

void OGLWidget::mouseReleaseEvent(QMouseEvent* event) {
	
	mouseButtonsPressed[Utils::mapQtMouseBtn(event->button())] = false;

	update();
}

void OGLWidget::mouseMoveEvent(QMouseEvent* event) {
	const auto invZoom = 1 / zoomFactor;

	if(mouseButtonsPressed[2]) {
		const auto mousePos = event->screenPos();
		const int curMouseX = mousePos.x();
		const int curMouseY = mousePos.y(); 

		cameraPanX -= invZoom*(curMouseX - lastMouseX);
		cameraPanY -= invZoom*(curMouseY - lastMouseY);
		lastMouseX = curMouseX;
		lastMouseY = curMouseY;

		update();
	}

	const auto mousePos = event->pos();
	mouseOnCanvasX = invZoom*mousePos.x() + cameraPanX;
	mouseOnCanvasY = invZoom*mousePos.y() + cameraPanY;

	if(mouseButtonsPressed[0])
		update();
}

void OGLWidget::wheelEvent(QWheelEvent *event) {
	const auto mousePos = event->pos();
	const auto scroll = event->angleDelta().y() / 120.f;
	const auto newZoomFactor = zoomFactor + 0.5*scroll;

	setZoom(newZoomFactor, mousePos.x(), mousePos.y());
}


const GLuint OGLWidget::loadShader(std::string path, GLenum shaderType) {
	// Create shader
	const GLuint shaderId = glCreateShader(shaderType);

	GLint result = GL_FALSE;
	int infoLogLen;

	// Compile shader
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	const auto vtxShaderCode = buffer.str();
	const auto* vtxShaderCodePtr = vtxShaderCode.c_str();
	glShaderSource(shaderId, 1, &vtxShaderCodePtr, NULL);
	glCompileShader(shaderId);

	// Check shader
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLen);
	if ( infoLogLen > 0 ){
		std::vector<char> msg(infoLogLen+1);
		glGetShaderInfoLog(shaderId, infoLogLen, NULL, msg.data());
		std::cout << std::string(msg.data()) << std::endl;
	}

	return shaderId;
}

const GLuint OGLWidget::linkShaderProgram(GLuint vertShaderId, GLuint fragShaderId) {
	// Link the program
	const GLuint progId = glCreateProgram();
	glAttachShader(progId, vertShaderId);
	glAttachShader(progId, fragShaderId);
	glLinkProgram(progId);

	// Check the program
	GLint result = GL_FALSE;
	int infoLogLen;
	glGetProgramiv(progId, GL_LINK_STATUS, &result);
	glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &infoLogLen);
	if ( infoLogLen > 0 ){
		std::vector<char> msg(infoLogLen+1);
		glGetProgramInfoLog(progId, infoLogLen, NULL, msg.data());
		std::cout << std::string(msg.data()) << std::endl;
	}
	
	glDetachShader(progId, vertShaderId);
	glDetachShader(progId, fragShaderId);

	return progId;
}