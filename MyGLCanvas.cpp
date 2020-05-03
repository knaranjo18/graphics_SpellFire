 #include "MyGLCanvas.h"

#define SENSITIVITY 0.5



MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	srand(time(0));
	prevX = prevY = 0;
	firstTime = true;
	lightPos = glm::vec3(0.0, 10, 0.0);
	
	player = new Player();

	for (int i = 0; i < 2; i++) {
		spawnEnemy(COW);
		spawnEnemy(BUNNY);
	}

	arena = new Scenery(ARENA, glm::vec3(0.0, 1.1, 0.0), glm::vec3(9, 9, 9), 0.0);
}

MyGLCanvas::~MyGLCanvas() {
	deallocate();
}


void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		
		if (firstTime) {
			firstTime = false;
			setupShaders();
			startTime = time(0);
		}
		// needs to be after so that shaders can setup
		updateCamera(w(), h());
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene();
}

void MyGLCanvas::drawScene() {
	double deltaTime = difftime(time(0), startTime);
/*
	if (deltaTime >= 3) {
		startTime = time(0);
		if (cowList.size() != 0) {
			removeEnemy(COW, 0);
		}
		else if (bunnyList.size() != 0) {
			removeEnemy(BUNNY, 0);
		}
		else {
			for (int i = 0; i < 2; i++) {
				spawnEnemy(COW);
				spawnEnemy(BUNNY);
			}
		}
	}
	*/

	//setting up camera info
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();
	glm::vec3 playerPos = player->myCam->getEyePoint();

	/*-----------------------For the scenery----------------------------------------*/
	arena->draw(modelViewMatrix, shaderList[ARENA], plyList[ARENA]);

	/*--------------For the enemy---------------------------*/
	for (int i = 0; i < cowList.size(); i++) {
		cowList[i]->draw(modelViewMatrix, shaderList[COW], plyList[COW], playerPos);
	}

	for (int i = 0; i < bunnyList.size(); i++) {
		bunnyList[i]->draw(modelViewMatrix, shaderList[BUNNY], plyList[BUNNY], playerPos);
	}
}

void MyGLCanvas::spawnEnemy(shaderType enemyType) {
	float xPos = float(rand()) / float(RAND_MAX) * (ARENA_SIZE * 2) - ARENA_SIZE;
	float zPos = sqrt((ARENA_SIZE * ARENA_SIZE) - xPos * xPos);
	float random = rand();
	float(random) > (RAND_MAX / 2.0) ? zPos *= -1 : zPos;

	switch (enemyType) {
	case(COW):
		cowList.push_back(new Enemy(COW, glm::vec3(xPos, HEIGHT - 0.05, zPos)));
		break;
	case(BUNNY):
		bunnyList.push_back(new Enemy(BUNNY, glm::vec3(xPos, HEIGHT, zPos)));
		break;
	}
}


void MyGLCanvas::removeEnemy(shaderType enemyType, int index) {
	switch (enemyType) {
	case(COW):
		delete cowList[index];
		cowList.erase(cowList.begin() + index);
		break;
	case(BUNNY):
		delete bunnyList[index];
		bunnyList.erase(bunnyList.begin() + index);
		break;
	}
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	GLint projection_id;
	xy_aspect = (float)width / (float)height;

	player->myCam->setScreenSize(width, height);

	glm::mat4 perspectiveMatrix = player->myCam->getProjectionMatrix();
	int size = shaderList.size();
	for (int i = 0; i < size; i++) {
		shaderList[i]->useShader();
		projection_id = glGetUniformLocation(shaderList[i]->program, "myProjectionMatrix");
		glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));
	}
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
				setupShaders();
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
			deallocate();
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

void MyGLCanvas::setupShaders() {	
	shaderList.push_back(new ShaderManager());
	shaderList.push_back(new ShaderManager());
	shaderList.push_back(new ShaderManager());

	plyList.push_back(new ply("./data/cow.ply"));
	plyList.push_back(new ply("./data/bunny.ply"));
	
	plyList.push_back(new ply("./data/arena_4_tex_2.ply"));
	plyList[ARENA]->applyTexture("./data/arena_1024.ppm");

	for (int i = COW; i <= ARENA; i++) {
		if (i == ARENA) {
			shaderList[i]->initShader("./shaders/330/scene.vert", "./shaders/330/scene.frag");
		}
		else {
			shaderList[i]->initShader("./shaders/330/scene.vert", "./shaders/330/cowColor.frag");
		}

		GLint light_id = glGetUniformLocation(shaderList[i]->program, "lightPos");
		glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

		plyList[i]->buildArrays();
		plyList[i]->bindVBO(shaderList[i]->program);
	}
}

void printEvent(int e) {
	printf("Event was %s (%d)\n", fl_eventnames[e], e);
}

void MyGLCanvas::deallocate() {
	delete player;

	for (int i = 0; i < shaderList.size(); i++) {
		delete shaderList[i];
	}

	for (int i = 0; i < plyList.size(); i++) {
		delete plyList[i];
	}

	for (int i = 0; i < cowList.size(); i++) {
		delete cowList[i];
	}

	for (int i = 0; i < bunnyList.size(); i++) {
		delete bunnyList[i];
	}
}