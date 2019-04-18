#include "oglWidget.hpp"

OGLWidget::OGLWidget(QPushButton& zoomButton) :	QOpenGLWidget{},
												cameraPanX{0},
												cameraPanY{0},
												zoomFactor{1},
												mouseButtonsPressed{false, false, false},
												zoomButton(zoomButton),
												presentationCanvasVao{} {
	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(4,5);
	setFormat(format);
};


void OGLWidget::newCanvas(const int w, const int h) {
	imageWidth = w;
	imageHeight = h;

	const auto clearCanv = std::vector<GLubyte>(w*h*4, 255);

	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearCanv.data());
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
	
	presentationCanvasVao.create();
	presentationCanvasVao.bind();

	glGenTextures(1, &canvasTexId);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	newCanvas(400, 300);

	////////////////////////////////////////////
	// Shader program for canvas presentation //
	////////////////////////////////////////////
	const auto presentationCanvasVertShadId = loadShader("shaders/presentationcanvas.vert.glsl", GL_VERTEX_SHADER);
	const auto presentationCanvasFragShadId = loadShader("shaders/presentationcanvas.frag.glsl", GL_FRAGMENT_SHADER);
	
	presentationCanvasProgId = linkShaderProgram(presentationCanvasVertShadId, presentationCanvasFragShadId);

	glDeleteShader(presentationCanvasVertShadId);
	glDeleteShader(presentationCanvasFragShadId);

	presentationCanvasMatLocId = glGetUniformLocation(presentationCanvasProgId, "view");
	presentationCanvasTexLocId = glGetUniformLocation(presentationCanvasProgId, "image");

	///////////////////////////////////////
	// Shader program for canvas drawing //
	///////////////////////////////////////
	const auto canvasVertShadId = loadShader("shaders/canvas.vert.glsl", GL_VERTEX_SHADER);
	const auto canvasFragShadId = loadShader("shaders/canvas.frag.glsl", GL_FRAGMENT_SHADER);
	
	canvasProgId = linkShaderProgram(canvasVertShadId, canvasFragShadId);

	glDeleteShader(canvasVertShadId);
	glDeleteShader(canvasFragShadId);


	////////////////////////////////////////
	// Generate vertex buffers for canvas //
	////////////////////////////////////////
	glGenBuffers(1, &presentationCanvasUvBuf);
	glGenBuffers(1, &presentationCanvasVertBuf);
	
	const GLfloat vtxBufData[] = {
			  0.0f, 0.0f,
		imageWidth, 0.0f,
			  0.0f, imageHeight,
		imageWidth, imageHeight
	};
	glBindBuffer(GL_ARRAY_BUFFER, presentationCanvasVertBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtxBufData), vtxBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	const GLfloat uvBufData[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, presentationCanvasUvBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufData), uvBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	///////////////////
	// Generate FBOs //
	///////////////////
	glGenFramebuffers(1, &canvasFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, canvasFboId);

	// Attach the binded framebuffer to texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvasTexId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void OGLWidget::resizeGL(int w, int h) {
	widgetWidth = w;
	widgetHeight = h;
}

void OGLWidget::paintGL() {
	
	////////////////////
	// Draw on canvas //
	////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, canvasFboId);

	glViewport(0, 0, imageWidth, imageHeight);
	glUseProgram(canvasProgId);

	const GLenum buf[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buf);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	/////////////////
	// Draw canvas //
	/////////////////
	glViewport(0,0,widgetWidth,widgetHeight);
	glUseProgram(presentationCanvasProgId);

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
	glUniformMatrix4fv(presentationCanvasMatLocId, 1, GL_FALSE, mat.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, canvasTexId);
	glUniform1i(presentationCanvasTexLocId, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OGLWidget::mousePressEvent(QMouseEvent* event) {

	const auto mousePos = event->screenPos();
	lastMouseX = (int)mousePos.x();
	lastMouseY = (int)mousePos.y();

	mouseButtonsPressed[Utils::mapQtMouseBtn(event->button())] = true;
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

	// const auto mousePos = event->pos();
	// const int x = invZoom*mousePos.x() + cameraPanX;
	// const int y = invZoom*mousePos.y() + cameraPanY;
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