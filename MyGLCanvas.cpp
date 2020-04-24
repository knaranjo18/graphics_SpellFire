 #include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	prevX = 0;
	prevY = 0;
	rotX = 0;
	rotY = 0;
	glm::vec3 eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;

	firstTime = true;

	myObject = new SceneObject(175);
	myShaderManager = new ShaderManager();
	myPLY = new ply("./data/bunny.ply");
	camera = new Camera();
	camera->orientLookAt(eyePosition, lookatPoint, glm::vec3(0, 1, 0));
}

MyGLCanvas::~MyGLCanvas() {
	delete myObject;
	delete myShaderManager;
	delete myPLY;

	if (camera != NULL)
		delete camera;
}


void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		if (firstTime == true) {
			firstTime = false;
			initShaders();
		}
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene();
}

void MyGLCanvas::drawScene() {
	//setting up camera info
	glm::mat4 modelViewMatrix = camera->getModelViewMatrix();
	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myModelviewMatrix"), 1, false, glm::value_ptr(modelViewMatrix));


	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	myPLY->renderVBO();
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	camera->setScreenSize(width, height);



	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glm::mat4 perspectiveMatrix = camera->getProjectionMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myProjectionMatrix"), 1, false, glm::value_ptr(perspectiveMatrix));
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err)	{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		else {
			//SHADER: initialize the shader manager and loads the two shader programs
			initShaders();
		}
	}
#endif	

	int button;
	printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_MOVE:
	case FL_RELEASE:
	case FL_KEYUP:
	case FL_MOUSEWHEEL:
		break;
	case FL_PUSH:
		button = Fl::event_button();
		if (button == FL_RIGHT_MOUSE) {
			prevX = Fl::event_x();
			prevY = Fl::event_y();
			printf("pushing\n");
			return 1;
		}
		break;
	case FL_DRAG:
		rotX += (Fl::event_x() - prevX);
		rotY += (Fl::event_y() - prevY);
		camera->setRotUVW(0, rotX / 10.0, 0);

		prevX = Fl::event_x();
		prevY = Fl::event_y();
		printf("x position is %d\n", Fl::event_x());
		printf("y position is %d\n", Fl::event_y());
		break;
}
	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::initShaders() {
	myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

	myPLY->buildArrays(); 
	myPLY->bindVBO(myShaderManager->program);
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

	myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

	myPLY->bindVBO(myShaderManager->program);
	invalidate();
}
