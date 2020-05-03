 #include "MyGLCanvas.h"

#define SENSITIVITY 0.3

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	prevX = prevY = 0;
	firstTime = true;
	lightPos = glm::vec3(0.0, 10, 0.0);

	enemyPos = glm::vec3(1.5, -0.20, 0.4);
	enemySpeed = 0.001;
	enemyLook = glm::vec3(1.0, 0.0, 0.0);



	shader1 = new ShaderManager();
	myPLY1 = new ply("./data/arena.ply");
	myPLY1->applyTexture("./data/red_bricks.ppm");
	
	shader2 = new ShaderManager();
	myPLY2 = new ply("./data/cow.ply");

	player = new Player();
}

MyGLCanvas::~MyGLCanvas() {
	delete shader1;
	delete shader2;
	delete myPLY1;
	delete myPLY2;
	delete player;
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
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	/*-----------------------For the scenery----------------------------------------*/
	shader1->useShader();
	GLint modelView_id = glGetUniformLocation(shader1->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	//Places the arena in the correct location
	glm::mat4 transMat4(1.0f);
    transMat4 = glm::scale(transMat4, glm::vec3(6.5, 4, 6.5));

	GLint trans_id = glGetUniformLocation(shader1->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	// Pass scenery texture to the shader
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, myPLY1->getTextureID());
	glUniform1i(glGetUniformLocation(shader1->program, "texture"), 0);

	GLint light_id = glGetUniformLocation(shader1->program, "lightPos");
	glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPLY1->renderVBO();

	/*--------------For the enemy---------------------------*/
	shader2->useShader();
	modelView_id = glGetUniformLocation(shader2->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	transMat4 = glm::mat4(1.0f);

	// Calculates the location that enemy should move towards
	glm::vec3 enemyDir = glm::normalize(player->myCam->getEyePoint() -enemyPos) * enemySpeed;
	enemyPos += enemyDir;

	// Used for 2D angle calculations
	glm::vec2 enemyDir2 = glm::normalize(glm::vec2(enemyDir.x, enemyDir.z));
	glm::vec2 enemyLook2 = glm::vec2(enemyLook.x, enemyLook.z);

	// Calculates angle that the enemy should rotate to face the player
	float angle_offset = acos(glm::dot(glm::normalize(enemyDir2), glm::normalize(enemyLook2)));
	if (enemyDir.z > 0) {
		angle_offset = 2 * PI - angle_offset;  //Deals with some stupid loop in the angle
	}

	// Places enemy in the right position
	transMat4 = glm::translate(transMat4, enemyPos);
	transMat4 = glm::scale(transMat4, glm::vec3(0.3, 0.3, 0.3));
	transMat4 = glm::rotate(transMat4, angle_offset, glm::vec3(0.0, 1.0, 0.0));

	trans_id = glGetUniformLocation(shader2->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

    light_id = glGetUniformLocation(shader2->program, "lightPos");
	glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPLY2->renderVBO(); 
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	player->myCam->setScreenSize(width, height);


	shader1->useShader();
	glm::mat4 perspectiveMatrix = player->myCam->getProjectionMatrix();
	GLint projection_id = glGetUniformLocation(shader1->program, "myProjectionMatrix");
	glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));

	shader2->useShader();
	projection_id = glGetUniformLocation(shader2->program, "myProjectionMatrix");
	glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));
}


int MyGLCanvas::handle(int e) {
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

	int button, key;

	switch (e) {
	case FL_MOVE:
		if (player->canMoveSight) 
			moveSight();
		else 
			cursor(FL_CURSOR_CROSS);
		break;
	case FL_KEYDOWN:
		key = Fl::event_key();
		
		switch (key) {
		case 'w':
			player->moveForward();
			break;
		case 'a':
			player->moveLeft();
			break;
		case 's':
			player->moveBackward();
			break;
		case 'd':
			player->moveRight();
			break;
		case FL_Escape:
			exit(0);
			break;
		}

		return 1;
	case FL_PUSH:
		button = Fl::event_button();
		
		if (button == FL_LEFT_MOUSE) {
			prevX = Fl::event_x();
			prevY = Fl::event_y();
			player->canMoveSight = !(player->canMoveSight);
		}

		break;
	case FL_FOCUS:
		return 1;
	}

	return Fl_Gl_Window::handle(e);
}


void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}


void MyGLCanvas::initShaders() {
	shader1->initShader("./shaders/330/scene.vert", "./shaders/330/scene.frag");
	myPLY1->buildArrays(); 
	myPLY1->bindVBO(shader1->program);

	shader2->initShader("./shaders/330/scene.vert", "./shaders/330/cowColor.frag");
	myPLY2->buildArrays();
	myPLY2->bindVBO(shader2->program);
}


void MyGLCanvas::moveSight() {
	float currX = Fl::event_x(), currY = Fl::event_y();
	float x_offset = currX - prevX;
	float y_offset = prevY - currY;

	cursor(FL_CURSOR_NONE);

	x_offset *= SENSITIVITY;
	y_offset *= SENSITIVITY;

	prevX = currX;
	prevY = currY;

	player->moveSight(x_offset, y_offset);
}

void printEvent(int e) {
	printf("Event was %s (%d)\n", fl_eventnames[e], e);
}