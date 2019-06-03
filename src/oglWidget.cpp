#include "oglWidget.hpp"

OGLWidget::OGLWidget(QPushButton& zoomButton) :	QOpenGLWidget{},
												cameraPanX{0},
												cameraPanY{0},
												currentFrame{0},
												currentFrameLayerIndex{0},
												skinLevels{3},
												zoomFactor{1},
												lastCursorPos{},
												mouseButtonsPressed{false, false, false},
												brushDown{false},
												panning{false},
												zoomButton(zoomButton),
												showCanvas_vao{},
												stroke_vao{},
												stroke_points{},
												stroke_points_indices{},
												stroke_current_index{0},
												stroke2canvas_doIt{true} {
	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(4,5);
	setFormat(format);

	auto* updateScheduler = new QTimer(this);
	connect(updateScheduler, SIGNAL(timeout()), this, SLOT(update()));
	updateScheduler->start(20);
};


void OGLWidget::newAnimation(const int w, const int h) {
	canvasWidth = w;
	canvasHeight = h;

	framesData = std::vector<Utils::Frame>(100, {w,h, {255,255,255}});

	const auto levels = 2*skinLevels + 1;
	layers2frame = std::vector<int>(levels);
	for(auto i = 0; i < levels; ++i) layers2frame[i] = i;

	glBindTexture(GL_TEXTURE_2D_ARRAY, canvasesTexId);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, w, h, levels);
	for(int i = 0; i < levels; i++) {
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY,
			0, 0, 0, i,
			w, h, 1, 
			GL_RGB, GL_UNSIGNED_BYTE, 
			framesData[i].data()
		);
	}

	const auto clearAlpha = std::vector<GLubyte>(w*h, 0);
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, clearAlpha.data());
}

void OGLWidget::setFrame(int newFrame) {
	const auto dFrame = newFrame - currentFrame;
	if(dFrame == 0) return;
	const auto nFrames = 2*skinLevels + 1;

	// Go one step forward or backward
	currentFrameLayerIndex = currentFrame + (dFrame > 0 ? +1 : -1);
	// Handle circular array index
	currentFrameLayerIndex = Utils::cycle(currentFrameLayerIndex, nFrames);

	layers2frame[currentFrameLayerIndex] = newFrame;

	// Refresh GPU memory
	const auto w = canvasWidth;
	const auto h = canvasHeight;
	glBindTexture(GL_TEXTURE_2D_ARRAY, canvasesTexId);
	for(auto i = -skinLevels; i <= +skinLevels; ++i) {
		const auto ci = Utils::cycle(currentFrameLayerIndex + i, nFrames);
		const auto f = newFrame + i;
		if(f < 0 || f > 99) continue;
		if(layers2frame[ci] != f) {
			// Upload new frame to GPU
			glTexSubImage3D(
				GL_TEXTURE_2D_ARRAY,
				0, 0, 0, ci, 
				canvasWidth, canvasHeight, 1, 
				GL_RGB, GL_UNSIGNED_BYTE, 
				framesData[f].data()
			);
			layers2frame[ci] = f;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_ARRAY, canvasesTexId, 0, currentFrameLayerIndex);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	currentFrame = newFrame;
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
}

void OGLWidget::setBrushSize(int newSize) {
	brushSize = newSize;
}

void OGLWidget::initializeGL() {
	initializeOpenGLFunctions();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glGenTextures(1, &canvasesTexId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, canvasesTexId);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &strokeTexId);
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	newAnimation(1920, 1080);

	////////////////////////////////////////////
	// Shader program for canvas presentation //
	////////////////////////////////////////////
	const auto showCanvas_vertShadId = loadShader("shaders/showCanvas.vert.glsl", GL_VERTEX_SHADER);
	const auto showCanvas_fragShadId = loadShader("shaders/showCanvas.frag.glsl", GL_FRAGMENT_SHADER);
	showCanvas_progId = linkShaderProgram(showCanvas_vertShadId, showCanvas_fragShadId);
	glDeleteShader(showCanvas_vertShadId);
	glDeleteShader(showCanvas_fragShadId);

	showCanvas_matrixLocId					= glGetUniformLocation(showCanvas_progId, "view");
	showCanvas_strokeTexLocId				= glGetUniformLocation(showCanvas_progId, "stroke");
	showCanvas_canvasTexLocId				= glGetUniformLocation(showCanvas_progId, "canvas");
	showCanvas_currentFrameLayerIndexLocId	= glGetUniformLocation(showCanvas_progId, "currentFrameLayerIndex");

	////////////////////////////////////////////
	// Shader program for stroke manipulation //
	////////////////////////////////////////////
	const auto stroke_vertShadId = loadShader("shaders/stroke.vert.glsl", GL_VERTEX_SHADER);
	const auto stroke_fragShadId = loadShader("shaders/stroke.frag.glsl", GL_FRAGMENT_SHADER);
	const auto stroke_geomShadId = loadShader("shaders/stroke.geom.glsl", GL_GEOMETRY_SHADER);
	const auto stroke_teseShadId  = loadShader("shaders/stroke.tese.glsl", GL_TESS_EVALUATION_SHADER);
	stroke_progId = linkShaderProgram(stroke_vertShadId, stroke_fragShadId, stroke_geomShadId, stroke_teseShadId);
	glDeleteShader(stroke_vertShadId);
	glDeleteShader(stroke_fragShadId);
	glDeleteShader(stroke_geomShadId);
	glDeleteShader(stroke_teseShadId);

	stroke_canvasSizeLocId = glGetUniformLocation(stroke_progId, "canvasSize");
	stroke_brushSizeLocId = glGetUniformLocation(stroke_progId, "brushSize");
	glPatchParameteri(GL_PATCH_VERTICES, 2);
	const GLfloat outer_tess_lvl[] = {1, 16};
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outer_tess_lvl);

	//////////////////////////////////////////////////
	// Shader program for stroke to canvas transfer //
	//////////////////////////////////////////////////
	const auto stroke2canvas_vertShadId = loadShader("shaders/stroke2canvas.vert.glsl", GL_VERTEX_SHADER);
	const auto stroke2canvas_fragShadId = loadShader("shaders/stroke2canvas.frag.glsl", GL_FRAGMENT_SHADER);
	stroke2canvas_progId = linkShaderProgram(stroke2canvas_vertShadId, stroke2canvas_fragShadId);
	glDeleteShader(stroke2canvas_vertShadId);
	glDeleteShader(stroke2canvas_fragShadId);

	stroke2canvas_strokeTexLocId				= glGetUniformLocation(stroke2canvas_progId, "stroke");
	stroke2canvas_canvasTexLocId				= glGetUniformLocation(stroke2canvas_progId, "canvas");
	stroke2canvas_currentFrameLayerIndexLocId	= glGetUniformLocation(stroke2canvas_progId, "currentFrameLayerIndex");

	////////////////////////////////////
	// Generate canvas vertex buffers //
	////////////////////////////////////
	showCanvas_vao.create();
	showCanvas_vao.bind();

	GLuint showCanvas_vtxBuf;
	glGenBuffers(1, &showCanvas_vtxBuf);	
	const GLfloat canvas_vtxBufData[] = {
			  0.0f, 0.0f,
		canvasWidth, 0.0f,
			  0.0f, canvasHeight,
		canvasWidth, canvasHeight
	};
	glBindBuffer(GL_ARRAY_BUFFER, showCanvas_vtxBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvas_vtxBufData), canvas_vtxBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint showCanvas_uvBuf;
	glGenBuffers(1, &showCanvas_uvBuf);
	const GLfloat uvBufData[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, showCanvas_uvBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufData), uvBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//////////////////////////
	// Stroke vertex buffer //
	//////////////////////////
	stroke_vao.create();
	stroke_vao.bind();
	glGenBuffers(1, &stroke_vtxBuf);
	glBindBuffer(GL_ARRAY_BUFFER, stroke_vtxBuf);
	glEnableVertexAttribArray(0);

	////////////////////////////////////
	// Stroke to canvas vertex buffer //
	////////////////////////////////////
	stroke2canvas_vao.create();
	stroke2canvas_vao.bind();
	GLuint stroke2canvas_vtxBuf;
	glGenBuffers(1, &stroke2canvas_vtxBuf);	
	const GLfloat stroke2canvas_vtxBufData[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, stroke2canvas_vtxBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stroke2canvas_vtxBufData), stroke2canvas_vtxBufData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//////////////////
	// Generate FBO //
	//////////////////
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_ARRAY, canvasesTexId, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, strokeTexId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//testDraw();
}

void OGLWidget::resizeGL(int w, int h) {
	widgetWidth = w;
	widgetHeight = h;
}

void OGLWidget::paintGL() {

	if(stroke2canvas_doIt)
		transferStroke2Canvas();

	strokeManagement();
	
	showCanvas_vao.bind();
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
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glUniform1i(showCanvas_strokeTexLocId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, canvasesTexId);
	glUniform1i(showCanvas_canvasTexLocId, 1);

	glUniform1i(showCanvas_currentFrameLayerIndexLocId, currentFrameLayerIndex);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OGLWidget::mousePressEvent(QMouseEvent* event) {

	const auto mouseScreenPos = event->screenPos();
	lastCursorPos.setX(mouseScreenPos.x());
	lastCursorPos.setY(mouseScreenPos.y());

	mouseButtonsPressed[Utils::mapQtMouseBtn(event->button())] = true;

	if(mouseButtonsPressed[0]) {
		brushDown = true;
		dragBrush(event->pos());
		dragBrush(event->pos());
	}
}

void OGLWidget::mouseReleaseEvent(QMouseEvent* event) {
	
	mouseButtonsPressed[Utils::mapQtMouseBtn(event->button())] = false;
	
	if(brushDown) {
		dragBrush(event->pos());
		liftBush();
	}
}

void OGLWidget::mouseMoveEvent(QMouseEvent* event) {

	if(mouseButtonsPressed[2]) {
		pan(event->screenPos());
	}

	if(brushDown) {
		dragBrush(event->pos());
	}
}

void OGLWidget::wheelEvent(QWheelEvent *event) {
	const auto mousePos = event->pos();
	const auto scroll = event->angleDelta().y() / 120.f;
	const auto newZoomFactor = zoomFactor + 0.5*scroll;

	setZoom(newZoomFactor, mousePos.x(), mousePos.y());
}

void OGLWidget::tabletEvent(QTabletEvent* event) {
	const auto btn = event->button();
	const auto screenPos = event->globalPosF();
	if(btn == Qt::MiddleButton) {
		panning = !panning;
		if(panning) {
			lastCursorPos.setX(screenPos.x());
			lastCursorPos.setY(screenPos.y());
		}
	}
		
	if(panning) {
		pan(screenPos);
	}
	
	const auto pressure = event->pressure();
	if(pressure > 0.0001 && !brushDown)
		brushDown = true;

	if(pressure < 0.0001 && brushDown)
		liftBush();

	if(brushDown) {
		const QPointF p{event->hiResGlobalX(), event->hiResGlobalY()};
		const auto windowPos = mapToGlobal({0,0});
		const QPointF widgetPos = p - windowPos;
		dragBrush(widgetPos, pressure);
	}

}

void OGLWidget::pan(const QPointF cursorPosOnScreen) {
	const auto invZoom = 1 / zoomFactor;
	const auto x = cursorPosOnScreen.x();
	const auto y = cursorPosOnScreen.y();
	
	cameraPanX -= invZoom*(x - lastCursorPos.x());
	cameraPanY -= invZoom*(y - lastCursorPos.y());
	lastCursorPos.setX(x);
	lastCursorPos.setY(y);
}

void OGLWidget::liftBush() {
	stroke_points.clear();
	stroke_points_indices.clear();
	stroke_current_index = 0;
	brushDown = false;
	stroke2canvas_doIt = true;
}

void OGLWidget::dragBrush(const QPointF cursorPosOnWidget, const float pressure) {
	const auto canvasPos = widget2canvasCoords(cursorPosOnWidget);

	// Compute the tangent on the previous point
	const auto n = stroke_points.size();
	auto angle = 0.0f;
	if(n > 0) {
		const QPointF p{stroke_points[n - 4], stroke_points[n - 3]};
		const auto d = canvasPos - p;
		if(Utils::length(d) < 2) return;
		angle = atan2(d.y(), d.x());

		if(n == 4) {
			stroke_points[n-1] = angle;
		}

		if(n > 4) {
			const QPointF pOld{stroke_points[n - 8], stroke_points[n - 7]};
			const auto d0 = Utils::normalize(p - pOld);
			const auto d1 = Utils::normalize(d);
			const auto middle = Utils::normalize(d0 + d1);
			const auto newAngle = atan2(middle.y(), middle.x());
			stroke_points[n-1] = newAngle;
		}

		if(!std::isfinite(stroke_points[n-1]))
			std::cout << "CRONK" << std::endl;
	}

	stroke_points.push_back(canvasPos.x());
	stroke_points.push_back(canvasPos.y());
	stroke_points.push_back(pressure);
	stroke_points.push_back(angle);

	stroke_points_indices.push_back(stroke_current_index);
	++stroke_current_index;
	stroke_points_indices.push_back(stroke_current_index);
}


QPointF OGLWidget::widget2canvasCoords(const QPointF& widgetPos) {
	const auto invZoom = 1 / zoomFactor;
	return {
		invZoom*widgetPos.x() + cameraPanX,
		invZoom*widgetPos.y() + cameraPanY
	};
}

void OGLWidget::strokeManagement() {

	glBlendEquation(GL_MAX);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	stroke_vao.bind();
	glBindBuffer(GL_ARRAY_BUFFER, stroke_vtxBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*stroke_points.size(), stroke_points.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glViewport(0, 0, canvasWidth, canvasHeight);
	glUseProgram(stroke_progId);

	const GLenum buf[] = {GL_COLOR_ATTACHMENT1};
	glDrawBuffers(1, buf);

	glUniform2i(stroke_canvasSizeLocId, canvasWidth, canvasHeight);
	glUniform1i(stroke_brushSizeLocId, brushSize);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_PATCHES, 2*stroke_current_index - 1, GL_UNSIGNED_INT, stroke_points_indices.data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendEquation(GL_FUNC_ADD);
}

void OGLWidget::transferStroke2Canvas() {

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	stroke2canvas_vao.bind();
	glViewport(0, 0, canvasWidth, canvasHeight);
	glUseProgram(stroke2canvas_progId);

	const GLenum buf[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buf);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, strokeTexId);
	glUniform1i(stroke2canvas_strokeTexLocId, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, canvasesTexId);
	glUniform1i(stroke2canvas_canvasTexLocId, 1);

	glUniform1i(stroke2canvas_currentFrameLayerIndexLocId, currentFrameLayerIndex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Write frame to RAM
	glReadPixels(0,0, canvasWidth, canvasHeight, GL_RGB, GL_UNSIGNED_BYTE, framesData[currentFrame].data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	stroke2canvas_doIt = false;
	glEnable(GL_BLEND);
}

GLuint OGLWidget::loadShader(std::string path, GLenum shaderType) {
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

GLuint OGLWidget::linkShaderProgram(GLuint vertShaderId, 
									GLuint fragShaderId, 
									GLuint geomShaderId,
									GLuint teseShaderId) {
	// Link the program
	const GLuint progId = glCreateProgram();
	glAttachShader(progId, vertShaderId);
	glAttachShader(progId, fragShaderId);
	if(geomShaderId != 0) glAttachShader(progId, geomShaderId);
	if(teseShaderId != 0)  glAttachShader(progId, teseShaderId);
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


void OGLWidget::testDraw() {
	// dragBrush({1000, 500}, 1);
	// dragBrush({ 900, 500}, .5);
	// dragBrush({ 800, 500}, .75);
	// dragBrush({ 700, 500}, 1);
	// dragBrush({ 600, 500}, .75);
	// dragBrush({ 500, 500}, .5);
	// dragBrush({ 400, 500}, .75);
	// dragBrush({ 300, 500}, .75);
	// dragBrush({ 200, 500}, .75);
	dragBrush({100, 100}, 1);
	dragBrush({200, 100}, .5);
	dragBrush({200, 200}, .75);
	dragBrush({100, 200}, 1);
	dragBrush({150, 250}, .75);
	dragBrush({100, 250}, .5);
	dragBrush({100, 150}, .75);
	//liftBush();
}