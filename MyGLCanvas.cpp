 #include "MyGLCanvas.h"


#define IFRAME_AFTER_HIT 60
#define MAX_ENEMIES 20

#define MAX_VOLUME 1.0f
#define MAX_SENSITIVITY 1.0f

#define HEALTHBAR_START 260.0
#define HEALTHBAR_LENGTH 500.0

#define MANABAR_START 260
#define MANABAR_LENGTH 500

#define EXPBAR_START 260
#define EXPBAR_LENGTH 500

#define BAR_HEIGHT 25
#define BAR_WIDTH 30

#define TARGETFPS 60
#define TIMEPERFRAME 1000 / TARGETFPS

#define NANOPERSEC 1000000000

#define DEBUGMODE false

float Projectile::volume = 1.0f;
float Enemy::volume = 1.0f;
float Player::volume = 1.0f;
float Pickup::volume = 1.0f;

// Constructor to set everything up. Spawns some initial enemies
MyGLCanvas::MyGLCanvas() {
	setupWindow(800, 600);
	setupSound();
	setupSprites();
	setupCursors();
	srand(time(0));

	sensitivity = 0.3f;
	prevState = currState = LOADING;
	prevX = prevY = 0;
	buttonSelected = SPRITE_MAIN;
	optionSelected = NONE;
	firstTime = firstMouse = true;
	lightPos = glm::vec3(0.0, 10, 0.0);
	numBlob = numJad = numManaPot = numHealthPot = numFireball = 0;

	player = new Player(soundEngine);
	arena = new Scenery(ARENA, glm::vec3(0.0, 1.1, 0.0), glm::vec3(9, 9, 9), 0.0);
}

// Makes sure to reclaim all memory taken by new
MyGLCanvas::~MyGLCanvas() {
	deallocate();
}

// Frees all memory that used 
void MyGLCanvas::deallocate() {
	delete player;
	delete arena;

	for (int i = 0; i < shaderList.size(); i++) delete shaderList[i];
	for (int i = 0; i < plyList.size(); i++) delete plyList[i];
	for (list<Enemy *>::iterator itE = enemyList.begin(); itE != enemyList.end(); itE++) delete (*itE);
	for (list<Projectile *>::iterator itP = projectileList.begin(); itP != projectileList.end(); itP++) delete (*itP);
	for (list<Pickup *>::iterator itPU = pickupList.begin(); itPU != pickupList.end(); itPU++) delete (*itPU);

	for (int i = 0; i < 2; i++) delete healthBar[i];
	for (int i = 0; i < 2; i++) delete manaBar[i];
	for (int i = 0; i < 2; i++) delete expBar[i];
	for (int i = 0; i < 2; i++) delete crossHair[i];
	for (int i = 0; i < 2; i++) delete deathScreen[i];
	for (int i = 0; i < mainMenu.size(); i++) delete mainMenu[i];
	delete loadingScreen;

	glfwDestroyCursor(regular);
	glfwDestroyCursor(hover);
	music->drop();
	soundEngine->drop();
}

// Main game loop
void MyGLCanvas::run() {
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
		pollInput();
		draw();
	}
	glfwTerminate();
}

// Chooses to draw the correct thing on the screen
void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (firstTime) { 
		printf("establishing GL context\n");

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);

		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err) {
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(1);
		} else {
			setupShaders();
		}

		// needs to be after so that shaders can setup
		updateCamera(mode->width, mode->height);
		firstTime = false;
		prevState = currState;
		currState = MAIN_MENU;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	switch (currState) {
	case PLAYING:
		// set up frame timing
		GLuint query;
		glGenQueries(1, &query);
		glBeginQuery(GL_TIME_ELAPSED, query);

		doGameLogic();
		drawScene();

		glEndQuery(GL_TIME_ELAPSED);
		enforceFrameTime(query);
		break;
	case DEAD:
		drawDeathScene();
		break;
	case MAIN_MENU:
		drawMainMenu();
		break;
	case PAUSE:
		drawPauseScreen();
		break;
	case OPTIONS:
		handleOptionBars();
		drawOptionScreen();
		break;
	case CONTROLS:
		drawControlScreen();
		break;
	default:
		printf("INVALID GAME STATE\n");
		exit(1);
	}
}

// Draw start screen
void MyGLCanvas::drawMainMenu() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	shaderList[BUTTON_START]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[BUTTON_START]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	mainMenu[0]->draw(shaderList[BUTTON_START], plyList[BUTTON_START]);

	shaderList[BUTTON_OPTIONS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_OPTIONS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	mainMenu[1]->draw(shaderList[BUTTON_OPTIONS], plyList[BUTTON_OPTIONS]);

	shaderList[BUTTON_QUIT]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_QUIT]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	mainMenu[2]->draw(shaderList[BUTTON_QUIT], plyList[BUTTON_QUIT]);

	shaderList[BUTTON_CONTROLS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_CONTROLS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	mainMenu[3]->draw(shaderList[BUTTON_CONTROLS], plyList[BUTTON_CONTROLS]);

	shaderList[SPRITE_MAIN]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_MAIN]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	mainMenu[4]->draw(shaderList[SPRITE_MAIN], plyList[SPRITE_MAIN]);
}

// Draws the death scene with the skull
void MyGLCanvas::drawDeathScene() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	shaderList[BUTTON_MAIN]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[BUTTON_MAIN]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	deathScreen[0]->draw(shaderList[BUTTON_MAIN], plyList[BUTTON_MAIN]);

	shaderList[BUTTON_RESTART]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_RESTART]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	deathScreen[1]->draw(shaderList[BUTTON_RESTART], plyList[BUTTON_RESTART]);

	shaderList[BUTTON_QUIT2]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_QUIT2]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	deathScreen[2]->draw(shaderList[BUTTON_QUIT2], plyList[BUTTON_QUIT2]);

	shaderList[SPRITE_DEATH]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_DEATH]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	deathScreen[3]->draw(shaderList[SPRITE_DEATH], plyList[SPRITE_DEATH]);
}

// Draws the screen when the game is paused
void MyGLCanvas::drawPauseScreen() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	shaderList[BUTTON_RESUME]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[BUTTON_RESUME]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[0]->draw(shaderList[BUTTON_RESUME], plyList[BUTTON_RESUME]);

	shaderList[BUTTON_CONTROLS2]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_CONTROLS2]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[1]->draw(shaderList[BUTTON_CONTROLS2], plyList[BUTTON_CONTROLS2]);

	shaderList[BUTTON_OPTIONS2]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_OPTIONS2]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[2]->draw(shaderList[BUTTON_OPTIONS2], plyList[BUTTON_OPTIONS2]);

	shaderList[BUTTON_MAIN2]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_MAIN2]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[3]->draw(shaderList[BUTTON_MAIN2], plyList[BUTTON_MAIN2]);

	shaderList[BUTTON_QUIT3]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_QUIT3]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[4]->draw(shaderList[BUTTON_QUIT3], plyList[BUTTON_QUIT3]);

	shaderList[SPRITE_PAUSE]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_PAUSE]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	pauseScreen[5]->draw(shaderList[SPRITE_PAUSE], plyList[SPRITE_PAUSE]);
}


// Draws the screen for editing options
void MyGLCanvas::drawOptionScreen() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	// Draws Master volume option
	shaderList[SPRITE_UNTEXTURED]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[0]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[1]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[BUTTON_MINUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_MINUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[0]->draw(shaderList[BUTTON_MINUS], plyList[BUTTON_MINUS]);

	shaderList[BUTTON_PLUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_PLUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[1]->draw(shaderList[BUTTON_PLUS], plyList[BUTTON_PLUS]);

	// Draws Music volume option
	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[2]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[3]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[BUTTON_MINUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_MINUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[2]->draw(shaderList[BUTTON_MINUS], plyList[BUTTON_MINUS]);

	shaderList[BUTTON_PLUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_PLUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[3]->draw(shaderList[BUTTON_PLUS], plyList[BUTTON_PLUS]);



	// Draws Misc volume option
	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[4]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[5]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[BUTTON_MINUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_MINUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[4]->draw(shaderList[BUTTON_MINUS], plyList[BUTTON_MINUS]);

	shaderList[BUTTON_PLUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_PLUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[5]->draw(shaderList[BUTTON_PLUS], plyList[BUTTON_PLUS]);


	// Draws sensitivity option
	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[6]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[SPRITE_UNTEXTURED]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionBars[7]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	shaderList[BUTTON_MINUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_MINUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[6]->draw(shaderList[BUTTON_MINUS], plyList[BUTTON_MINUS]);

	shaderList[BUTTON_PLUS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_PLUS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionPlusMinus[7]->draw(shaderList[BUTTON_PLUS], plyList[BUTTON_PLUS]);


	// Draws buttons and background
	shaderList[BUTTON_FULLSCREEN]->useShader();
	modelView_id = glGetUniformLocation(shaderList[BUTTON_FULLSCREEN]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionScreen[0]->draw(shaderList[BUTTON_FULLSCREEN], plyList[BUTTON_FULLSCREEN]);

	shaderList[SPRITE_OPTIONS]->useShader();
	modelView_id = glGetUniformLocation(shaderList[SPRITE_OPTIONS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	optionScreen[1]->draw(shaderList[SPRITE_OPTIONS], plyList[SPRITE_OPTIONS]);
}


// Draws the screen that shows the controls for the game
void MyGLCanvas::drawControlScreen() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	shaderList[SPRITE_CONTROLS]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[SPRITE_CONTROLS]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));
	controlScreen->draw(shaderList[SPRITE_CONTROLS], plyList[SPRITE_CONTROLS]);
}

// Draws all the elements of the main game
void MyGLCanvas::drawScene() {
	//setting up camera info
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();
	

	/*-----------------------For the scenary----------------------------------------*/
	skybox->draw(modelViewMatrix, player->myCam->getProjectionMatrix());
	arena->draw(modelViewMatrix, shaderList[ARENA], plyList[ARENA]);

	/*------------------------For GUI-----------------------------------------------*/
	shaderList[SPRITE_UNTEXTURED]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[SPRITE_UNTEXTURED]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	for (int i = 0; i < 2; i++)
		healthBar[i]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	for (int i = 0; i < 2; i++) 
		crossHair[i]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	for (int i = 0; i < 2; i++)
		manaBar[i]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);

	for (int i = 0; i < 2; i++) 
		expBar[i]->draw(shaderList[SPRITE_UNTEXTURED], plyList[SPRITE_UNTEXTURED]);
		
	/*--------------For the enemy---------------------------*/
	shaderList[GOOP]->useShader();
	modelView_id = glGetUniformLocation(shaderList[GOOP]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	list<Enemy *>::iterator itE, endPointE;
	endPointE = enemyList.begin();
	advance(endPointE, numBlob);
	for (itE = enemyList.begin(); itE != endPointE; itE++) 
		(*itE)->draw(shaderList[GOOP], plyList[GOOP]);


	shaderList[JAD]->useShader();
	modelView_id = glGetUniformLocation(shaderList[JAD]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	advance(endPointE, numJad);
	for (itE; itE != endPointE; itE++) 
		(*itE)->draw(shaderList[JAD], plyList[JAD]);

	/*--------------draw projectiles---------------------------*/
	shaderList[FIREBALL]->useShader();
	modelView_id = glGetUniformLocation(shaderList[FIREBALL]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	list<Projectile *>::iterator itP, endPointP;
	endPointP = projectileList.begin();
	advance(endPointP, numFireball);
	for (itP = projectileList.begin(); itP != endPointP; itP++) 
		(*itP)->draw(shaderList[FIREBALL], plyList[FIREBALL]);

	/*--------------draw pickups---------------------------*/
	shaderList[MANAPOT]->useShader();
	modelView_id = glGetUniformLocation(shaderList[MANAPOT]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	list<Pickup *>::iterator itPU, endPointPU;
	endPointPU = pickupList.begin();
	advance(endPointPU, numManaPot);
	for (itPU = pickupList.begin(); itPU != endPointPU; itPU++) 
		(*itPU)->draw(shaderList[MANAPOT], plyList[MANAPOT]);


	shaderList[HEALTHPOT]->useShader();
	modelView_id = glGetUniformLocation(shaderList[HEALTHPOT]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	advance(endPointPU, numHealthPot);
	for (itPU; itPU != endPointPU; itPU++)
		(*itPU)->draw(shaderList[HEALTHPOT], plyList[HEALTHPOT]);
}

// Ensures that the game runs at the specified FPS regardless of machine
void MyGLCanvas::enforceFrameTime(GLint query) {
	GLint available = 0;
	while (!available) {
		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &available);
	}

	GLuint64 timeElapsed = 0;
	GLuint64 frameTarget = NANOPERSEC / TARGETFPS;
	
	glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);

	if (timeElapsed < frameTarget) {
		this_thread::sleep_for(std::chrono::nanoseconds(frameTarget - timeElapsed));
	}
}

// Handles all the game mechanics such as collisions, projectiles, movements, etc...
void MyGLCanvas::doGameLogic() {
	glm::vec3 playerPos = player->getPosition();
	glm::vec3 playerLook = player->getLookVec();

	soundEngine->setListenerPosition(TO_VEC3(playerPos), TO_VEC3(playerLook));

	handleEnemySound();
	handleMoveCollisions(playerPos);
	handlePlayerCollisions();
	
	handleProjectiles();
	handlePickups();

	handleManaBar();
	handleHealthBar();
	handleExpBar();

	if (player->isDead()) gameOver();

	player->chargeMana();
	player->tickHeal();

	respawnEnemies();
}

// Has a random chance to spawn new enemies
void MyGLCanvas::respawnEnemies() {
	double deltaTime = difftime(time(0), startTime);
	float temp;

	if (deltaTime > 1) {
		startTime = time(0);
		temp = rand() / float(RAND_MAX);

		if (temp < 0.5) spawnEnemy(JAD);
		if (temp < 0.75) spawnEnemy(GOOP);
	}
}

// refactor idea: replace pointers with NULL and periodically cull instead of
// deleting immediately
void MyGLCanvas::handlePlayerCollisions() {
	const BoundingBox* p_box = player->getBox();
	
	list<Pickup *>::iterator itPU = pickupList.begin();
	while(itPU != pickupList.end()) {
		const BoundingBox* pot_box = (*itPU)->getBox();
		if (p_box->doesCollide(*pot_box)) { // pick up the potion
			player->applyHit((*itPU)->getHitFunc());
			(*itPU)->usePickupSound();
			removePickup(itPU);
		} else {
			itPU++;
		}
	}

	// Don't damage player if invicible
	if (player->isInvincible()) {
		player->deciFrames();
		return;
	}
	
	list<Enemy *>::iterator itE;
	for (itE = enemyList.begin(); itE != enemyList.end(); itE++) {
		const BoundingBox* e_box = (*itE)->getBox();
		if (p_box->doesCollide(*e_box)) { // Gives invicibility frames after player is hit
			player->applyHit((*itE)->getHitFunc());
			player->setiFrames(IFRAME_AFTER_HIT);
			player->hurtSound();
			return;
		}
	}
}

// Returns the index of the enemy which is currently colliding with the projectile
// or end() if there is no collision since it is one more than the last element
list<Enemy *>::iterator MyGLCanvas::findEnemyCollision(Projectile* p) {
	const BoundingBox* pBox = p->getBox();

	list<Enemy *>::iterator itE;
	for (itE = enemyList.begin(); itE != enemyList.end(); itE++) {
		const BoundingBox* eBox = (*itE)->getBox();

		if (pBox->doesCollide(*eBox)) {
			return itE;
		}
	}
	return enemyList.end();
}

// Moves enemies towards players. If collision between enemies occurs
// they will move apart from each other.
void MyGLCanvas::handleMoveCollisions(glm::vec3 playerPos) {
	list<Enemy *>::iterator it1, it2;
	for (it1 = enemyList.begin(); it1 != enemyList.end(); it1++) {
		bool moved = false;
		it2 = it1;
		for (advance(it2, 1); it2 != enemyList.end(); it2++) {
			const BoundingBox* box1 = (*it1)->getBox();
			const BoundingBox* box2 = (*it2)->getBox();
			if (box1->doesCollide(*box2) && !moved) {
				glm::vec3 dir = box1->getCenter() - box2->getCenter();
				// move enemies towards some faraway point in opposite directions
				(*it1)->moveEnemy((dir * 5.0f) + glm::vec3(box1->getCenter()));
				(*it2)->moveEnemy((-dir * 5.0f) + glm::vec3(box2->getCenter()));
				box1 = (*it1)->getBox();
				box2 = (*it2)->getBox();
				moved = true;
			}
		}
		if (!moved) {
			(*it1)->moveEnemy(playerPos);
		}
	}
}

// Randomly determines if an enemy should make their call sound
void MyGLCanvas::handleEnemySound() {
	int random;

	list<Enemy *>::iterator itE;
	for (itE = enemyList.begin(); itE != enemyList.end(); itE++) {
		if ((rand() % 800) == 0) (*itE)->callSound();
	}
}

// Calculates health bar sprites
void MyGLCanvas::handleHealthBar() {
	glm::vec3 color;
	float healthRatio = player->getHealth() / player->maxHealth;

	if (healthRatio <= 1.0 / 3.0) {
		color = glm::vec3(1.0, 0.0, 0.0);
	} else if (healthRatio <= 2.0 / 3.0) {
		color = glm::vec3(0.8, 0.4, 0.2);
	} else {
		color = glm::vec3(0.0, 1.0, 0.0);
	}

	float length = HEALTHBAR_LENGTH * healthRatio;
	//float offset = (HEALTHBAR_START * (1 - healthRatio));

	glm::vec2 pos(HEALTHBAR_START, mode->height - BAR_HEIGHT);
	glm::vec2 scale(length, BAR_WIDTH);

	healthBar[0]->setPosition(pos);
	healthBar[0]->setScale(scale);
	healthBar[0]->setColor(color);
}

// Calculates experience bar sprites
void MyGLCanvas::handleExpBar() {
	float expRatio = player->getPoints() / player->maxPoints;

	float length = EXPBAR_LENGTH * expRatio;
	//float offset = (EXPBAR_START * (1 - expRatio));

	glm::vec2 pos(mode->width / 2.0, mode->height - BAR_HEIGHT);
	glm::vec2 scale(length, BAR_WIDTH);

	expBar[0]->setPosition(pos);
	expBar[0]->setScale(scale);
}

// Calculates mana bar sprites
void MyGLCanvas::handleManaBar() {
	float manaRatio = player->getMana() / player->maxMana;

	float length = MANABAR_LENGTH * manaRatio;
	float offset = (MANABAR_START * (1 - manaRatio));

	glm::vec2 pos(mode->width - MANABAR_START, mode->height - BAR_HEIGHT);
	glm::vec2 scale(length, BAR_WIDTH);

	manaBar[0]->setPosition(pos);
	manaBar[0]->setScale(scale);
}

// Calculate option bar sprites
void MyGLCanvas::handleOptionBars() {
	float ratio, length;
	Sprite *currSprite;

	for (int i = 0; i < 8; i += 2) {	
		switch ((optionType)(i / 2)) {
		case MASTER_VOL:
			ratio = masterVol / MAX_VOLUME;
			break;
		case MUSIC_VOL:
			ratio = musicVol / MAX_VOLUME;
			break;
		case MISC_VOL:
			ratio = miscVol / MAX_VOLUME;
			break;
		case SENSITIVITY:
			ratio = sensitivity / MAX_SENSITIVITY;
			break;
		}

		length = mode->width / 4.75;
		glm::vec2 scale(length * ratio, mode->height / 25.5);

		optionBars[i]->setScale(scale);
	}
}

// Move projectiles, delete if they expire, figure out collisions with enemies
void MyGLCanvas::handleProjectiles() {
	list<Enemy *>::iterator hit;

	list<Projectile *>::iterator itP = projectileList.begin();
	while(itP != projectileList.end()) {
		Projectile *p = (*itP);

		if (isExpired(p->getSpawnTime(), p->getDuration())) {
			removeProjectile(itP);
		} else if (p->hitFloor()) {
			removeProjectile(itP);
		} else if ((hit = findEnemyCollision(p)) != enemyList.end()) {
			// deal damage to enemy here
			applyProjectile(p, hit);
			removeProjectile(itP);
		}
		else {
			p->moveProjectile();
			itP++;
		}
	}
}

// Deletes pickup if they have expired
void MyGLCanvas::handlePickups() {
	list<Pickup *>::iterator itPU = pickupList.begin();

	while(itPU != pickupList.end()) {
		Pickup *p = (*itPU);
		
		if (isExpired(p->getSpawnTime(), p->getDuration())) {
			removePickup(itPU);
		} else {
			itPU++;
		}
	}
}

// Handle all logic when enemy e is hit by projectile p
void MyGLCanvas::applyProjectile(Projectile* p, list<Enemy *>::iterator itE) {
	Enemy* e = (*itE);
	e->applyHit(p->getHitfunc());

	if (e->isDead()) {
		player->changePoints(e->pointValue);
		e->deathSound();

		if (rand() % 3 == 0) { // enemies have a 1/3 chance to spawn a potion on death
			spawnPickup(rand() % 2 == 0 ? HEALTHPOT : MANAPOT, e->getPosition()); 
		}

		removeEnemy(itE);
	}
}

// Creates new projectile given an origin and a direction of travel
void MyGLCanvas::fireProjectile(shaderType projectileType, glm::vec3 originPoint, glm::vec3 directionFired) {
	glm::vec3 startPoint = originPoint + (0.1f * glm::normalize(directionFired));
	startPoint.y -= 0.08;

	switch (projectileType) {
	case FIREBALL:
		projectileList.push_front(new Projectile(projectileType, startPoint, directionFired, soundEngine));
		numFireball++;
		break;
	default:
		printf("NOT A VALID PROJECTILE");
		exit(1);
		break;
	}
}

// Creates a new enemy of a given type somewhere around the edge of the arena
void MyGLCanvas::spawnEnemy(shaderType enemyType) {
	float xPos = float(rand()) / float(RAND_MAX) * (ARENA_SIZE * 2) - ARENA_SIZE;
	float zPos = sqrt((ARENA_SIZE * ARENA_SIZE) - xPos * xPos);

	(rand() % 2 == 0) ? zPos *= -1 : zPos;

	switch (enemyType) {
	case(GOOP):
		enemyList.push_front(new Enemy(GOOP, glm::vec3(xPos, HEIGHT - 0.05, zPos), soundEngine));
		numBlob++;
		break;
	case(JAD):
		enemyList.push_back(new Enemy(JAD, glm::vec3(xPos, HEIGHT, zPos), soundEngine));
		numJad++;
		break;	
	default:
		printf("NOT A VALID ENEMY");
		exit(1);
		break;
	}
}

// Spawns a pickup of a given type in a given position
void MyGLCanvas::spawnPickup(shaderType type, glm::vec3 position) {
	float angle = sin(rand());
	position.y = HEIGHT - 0.05;

	switch (type) {
	case(MANAPOT):
		pickupList.push_front(new Pickup(position, angle, type, soundEngine));
		numManaPot++;
		break;
	case(HEALTHPOT):
		pickupList.push_back(new Pickup(position, angle, type, soundEngine));
		numHealthPot++;
		break;
	default:
		fprintf(stderr, "NOT A VALID PICKUP\n");
		exit(1);
	}
}

// Removes an enemy from game and reclaims memory
void MyGLCanvas::removeEnemy(list<Enemy *>::iterator &it) {
	switch ((*it)->getType()) {
	case(GOOP):
		numBlob--;
		break;
	case(JAD):
		numJad--;
		break;
	default:
		printf("NOT A VALID ENEMY");
		exit(1);
		break;
	} 

	delete (*it);
	it = enemyList.erase(it);
}

// Removes a projectile from game and reclaims memory
void MyGLCanvas::removeProjectile(list<Projectile *>::iterator &it) {
	switch ((*it)->getType()) {
	case(FIREBALL):
		numFireball--;
		break;
	}

	delete (*it);
	it = projectileList.erase(it);
}

// Removes a pickup from game and reclaims memory
void MyGLCanvas::removePickup(list<Pickup *>::iterator &it) {
	switch ((*it)->getType()) {
	case MANAPOT:
		numManaPot--;
		break;
	case HEALTHPOT:
		numHealthPot--;
		break;
	}

	delete (*it);
	it = pickupList.erase(it);
}

// Updates the camera based on the width and height of the screen
void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	GLint projection_id;
	xy_aspect = (float)width / (float)height;

	player->myCam->setScreenSize(width, height);

	glm::mat4 perspectiveMatrix = player->myCam->getProjectionMatrix();
	glm::mat4 orthoMatrix = glm::ortho(0.0f, float(mode->width), float(mode->height), 0.0f, -1.0f, 1.0f);
	
	int size = shaderList.size();
	for (int i = 1; i < size; i++) {
		shaderList[i]->useShader();
		projection_id = glGetUniformLocation(shaderList[i]->program, "myProjectionMatrix");
		
		if (i == GOOP || i == JAD || i == FIREBALL || i == ARENA || i == HEALTHPOT || i == MANAPOT || i == SKYBOX) {
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));
		} else {
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(orthoMatrix));
		}
	}
}

// Initializes all the shaderes with their ply files and textures
// They have to be in the same order as the shaderType enum in gfxDefs.h
void MyGLCanvas::setupShaders() {	
	#ifndef __APPLE__
		printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	#endif
	
	shaderList.push_back(new ShaderManager());  // loading
	drawLoading(); // We draw loading screen now to mask the time needed to load all the other resources

	shaderList.push_back(new ShaderManager()); // goop
	shaderList.push_back(new ShaderManager()); // jad
	shaderList.push_back(new ShaderManager()); // fireball
	shaderList.push_back(new ShaderManager()); // arena
	shaderList.push_back(new ShaderManager()); // health pot
	shaderList.push_back(new ShaderManager()); // mana pot

	shaderList.push_back(new ShaderManager()); // sprite untextured
	shaderList.push_back(new ShaderManager()); // death screen
	shaderList.push_back(new ShaderManager()); // main menu screen
	shaderList.push_back(new ShaderManager()); // start button for main menu
	shaderList.push_back(new ShaderManager()); // options button for main menu
	shaderList.push_back(new ShaderManager()); // quit button for main menu
	shaderList.push_back(new ShaderManager()); // controls button for main menu
	shaderList.push_back(new ShaderManager()); // restart button for death menu
	shaderList.push_back(new ShaderManager()); // main menu button for death menu
	shaderList.push_back(new ShaderManager()); // quit button for death menu
	shaderList.push_back(new ShaderManager()); // pause screen
	shaderList.push_back(new ShaderManager()); // controls screen
	shaderList.push_back(new ShaderManager()); // options screen
	shaderList.push_back(new ShaderManager()); // resume button for pause screen
	shaderList.push_back(new ShaderManager()); // main menu button for pause screen
	shaderList.push_back(new ShaderManager()); // options button for pause screen
	shaderList.push_back(new ShaderManager()); // quit button for pause screen
	shaderList.push_back(new ShaderManager()); // controls button for pause screen
	shaderList.push_back(new ShaderManager()); // fullscreen button for options screen
	shaderList.push_back(new ShaderManager()); // plus button for options screen
	shaderList.push_back(new ShaderManager()); // minus button for options screen

	plyList.push_back(new ply("./data/blob.ply"));
	plyList.push_back(new ply("./data/jad.ply"));
	
	plyList.push_back(new ply("./data/fireball.ply"));
	plyList[FIREBALL]->applyTexture("./data/fireball.ppm");

	plyList.push_back(new ply("./data/arena.ply"));
	plyList[ARENA]->applyTexture("./data/arena_large.ppm");

	plyList.push_back(new ply("./data/potion.ply"));
	plyList[HEALTHPOT]->applyTexture("./data/healthPot.ppm");

	plyList.push_back(new ply("./data/potion.ply"));
	plyList[MANAPOT]->applyTexture("./data/manaPot.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_DEATH]->applyTexture("./data/skull_medium.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_MAIN]->applyTexture("./data/startScreen_small.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_START]->applyTexture("./data/startButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_OPTIONS]->applyTexture("./data/optionsButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_QUIT]->applyTexture("./data/exitButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_CONTROLS]->applyTexture("./data/controlsButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_RESTART]->applyTexture("./data/restartButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_MAIN]->applyTexture("./data/mainMenuButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_QUIT2]->applyTexture("./data/exitButton2.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_PAUSE]->applyTexture("./data/pauseScreen_small.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_CONTROLS]->applyTexture("./data/controlScreen_medium.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_OPTIONS]->applyTexture("./data/optionScreen_small.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_RESUME]->applyTexture("./data/resumeButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_MAIN2]->applyTexture("./data/mainMenuButton2.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_OPTIONS2]->applyTexture("./data/optionsButton2.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_QUIT3]->applyTexture("./data/exitButton3.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_CONTROLS2]->applyTexture("./data/controlsButton2.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_FULLSCREEN]->applyTexture("./data/fullscreenButton.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_PLUS]->applyTexture("./data/plusButton.ppm");
	
	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[BUTTON_MINUS]->applyTexture("./data/minusButton.ppm");

	for (int i = 1; i < shaderList.size(); i++) {
		if (i == ARENA || i == FIREBALL || i == HEALTHPOT || i == MANAPOT) {
			shaderList[i]->initShader("./shaders/330/model_textured.vert", "./shaders/330/model_textured.frag");
			plyList[i]->buildArrays();
			plyList[i]->bindVBO(shaderList[i]->program);
		} else if (i == SPRITE_UNTEXTURED) {
			shaderList[i]->initShader("./shaders/330/sprite_untextured.vert", "./shaders/330/sprite_untextured.frag");
			plyList[i]->buildArraysSprite();
			plyList[i]->bindVBOsprites(shaderList[i]->program);
		} else if (i == GOOP || i == JAD) {
			shaderList[i]->initShader("./shaders/330/model_untextured.vert", "./shaders/330/model_untextured.frag");
			plyList[i]->buildArrays();
			plyList[i]->bindVBO(shaderList[i]->program);
		} else {
			shaderList[i]->initShader("./shaders/330/sprite_textured.vert", "./shaders/330/sprite_textured.frag");
			plyList[i]->buildArraysSprite();
			plyList[i]->bindVBOsprites(shaderList[i]->program);
		}

		GLint light_id = glGetUniformLocation(shaderList[i]->program, "lightPos");
		glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);
	}
	
	string filenames[6] = {
		"./data/red_right.ppm", "./data/red_left.ppm", "./data/red_top.ppm",
		"./data/red_bottom.ppm", "./data/red_front.ppm", "./data/red_back.ppm"
	};

	skybox = new Skybox(filenames);
	shaderList.push_back(skybox->shader); // Skybox
}

// Initializes all the 2D sprites
void MyGLCanvas::setupSprites() {
	healthBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(HEALTHBAR_START, mode->height - BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 1.0, 0.0), FOREGROUND));
	healthBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(HEALTHBAR_START, mode->height - BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND));

	manaBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width - MANABAR_START, mode->height - BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 0.0, 1.0), FOREGROUND));
	manaBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width - MANABAR_START, mode->height - BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND));

	expBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width / 2, mode->height - BAR_HEIGHT), glm::vec2(EXPBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(1.0, 1.0, 0.0), FOREGROUND));
	expBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width / 2, mode->height - BAR_HEIGHT), glm::vec2(EXPBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));

	glm::vec2 pos(mode->width / 2.0 - 2, mode->height / 2.0 + 40);
	crossHair.push_back(new Sprite(SPRITE_UNTEXTURED, pos, glm::vec2(2.0, 30.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	crossHair.push_back(new Sprite(SPRITE_UNTEXTURED, pos, glm::vec2(30.0, 2.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));

	deathScreen.push_back(new Sprite(BUTTON_MAIN, glm::vec2(4 * mode->width / 16.0f, 14 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	deathScreen.push_back(new Sprite(BUTTON_RESTART, glm::vec2(8 * mode->width / 16.0f, 14 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	deathScreen.push_back(new Sprite(BUTTON_QUIT2, glm::vec2(12 * mode->width / 16.0f, 14 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	deathScreen.push_back(new Sprite(SPRITE_DEATH, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), BACKGROUND));

	loadingScreen = new Sprite(SPRITE_LOADING, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND);

	mainMenu.push_back(new Sprite(BUTTON_START, glm::vec2(mode->width / 2.0f, 7 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	mainMenu.push_back(new Sprite(BUTTON_OPTIONS, glm::vec2(mode->width / 2.0f, 11 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	mainMenu.push_back(new Sprite(BUTTON_QUIT, glm::vec2(mode->width / 2.0f, 13 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	mainMenu.push_back(new Sprite(BUTTON_CONTROLS, glm::vec2(mode->width / 2.0f, 9 * mode->height / 15.0f), glm::vec2(mode->width / 5.0f, mode->height / 10.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	mainMenu.push_back(new Sprite(SPRITE_MAIN, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), BACKGROUND));

	pauseScreen.push_back(new Sprite(BUTTON_RESUME, glm::vec2(5 * mode->width / 18.0f, 9 * mode->height / 15.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	pauseScreen.push_back(new Sprite(BUTTON_CONTROLS2, glm::vec2(9 * mode->width / 18.0f, 9 * mode->height / 15.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	pauseScreen.push_back(new Sprite(BUTTON_OPTIONS2, glm::vec2(13 * mode->width / 18.0f, 9 * mode->height / 15.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	pauseScreen.push_back(new Sprite(BUTTON_MAIN2, glm::vec2(6 * mode->width / 16.0f, 11 * mode->height / 15.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	pauseScreen.push_back(new Sprite(BUTTON_QUIT3, glm::vec2(10 * mode->width / 16.0f, 11 * mode->height / 15.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	pauseScreen.push_back(new Sprite(SPRITE_PAUSE, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), BACKGROUND));

	controlScreen = new Sprite(SPRITE_CONTROLS, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), BACKGROUND);

	// Master volume bar
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.8, 0.0, 0.0), FOREGROUND));
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_MINUS, glm::vec2(4.1 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_PLUS, glm::vec2(9.5 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));

	// Music volume bar
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 11.55 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.8, 0.0, 0.0), FOREGROUND));
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 11.55 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_MINUS, glm::vec2(4.1 * mode->width / 20.0, 11.55 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_PLUS, glm::vec2(9.5 * mode->width / 20.0, 11.55 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));

	// Misc volume bar
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 15 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.8, 0.0, 0.0), FOREGROUND));
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(6.8 * mode->width / 20.0, 15 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_MINUS, glm::vec2(4.1 * mode->width / 20.0, 15 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_PLUS, glm::vec2(9.5 * mode->width / 20.0, 15 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));

	// Sensitivity bar
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(13.5 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.8, 0.0, 0.0), FOREGROUND));
	optionBars.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(13.5 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 4.75f, mode->height / 25.5f), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_MINUS, glm::vec2(10.8 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	optionPlusMinus.push_back(new Sprite(BUTTON_PLUS, glm::vec2(16.2 * mode->width / 20.0, 8 * mode->height / 20.0), glm::vec2(mode->width / 40.0f, mode->height / 25.0f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));


	optionScreen.push_back(new Sprite(BUTTON_FULLSCREEN, glm::vec2(13.5 * mode->width / 20.0f, 12 * mode->height / 20.0f), glm::vec2(mode->width / 5.25f, mode->height / 10.5f), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	optionScreen.push_back(new Sprite(SPRITE_OPTIONS, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), BACKGROUND));
}

// Sets up everything required to draw a single image of the loading screen
void MyGLCanvas::drawLoading() {
	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_LOADING]->applyTexture("./data/loading_tiny.ppm");
	shaderList[SPRITE_LOADING]->initShader("./shaders/330/sprite_textured.vert", "./shaders/330/sprite_textured.frag");
	plyList[SPRITE_LOADING]->buildArraysSprite();
	plyList[SPRITE_LOADING]->bindVBOsprites(shaderList[SPRITE_LOADING]->program);
	shaderList[SPRITE_LOADING]->useShader();

	glm::mat4 orthoMatrix = glm::ortho(0.0f, float(mode->width), float(mode->height), 0.0f, -1.0f, 1.0f);
	GLint projection_id = glGetUniformLocation(shaderList[SPRITE_LOADING]->program, "myProjectionMatrix");
	glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(orthoMatrix));
	
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();
	GLint modelView_id = glGetUniformLocation(shaderList[SPRITE_LOADING]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	loadingScreen->draw(shaderList[SPRITE_LOADING], plyList[SPRITE_LOADING]);
	glfwSwapBuffers(window);
}

// Callback for keyboard key
void MyGLCanvas::key_callback(GLFWwindow* _window, int key, int scancode, int action, int mods) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (action == GLFW_PRESS) {
		switch(c->currState) {
		case PLAYING:
			if (key == GLFW_KEY_ESCAPE) c->pauseGame();
			break;
		case PAUSE:
			if (key == GLFW_KEY_ESCAPE) c->unpauseGame();
			break;
		case OPTIONS:
			if (key == GLFW_KEY_ESCAPE) {
				if (c->prevState == PAUSE) c->pauseGame(); 
				else if (c->prevState == MAIN_MENU) c->menuReturn(); 
			} 
			break;
		case CONTROLS:
			if (key == GLFW_KEY_ESCAPE) {
				if (c->prevState == PAUSE) c->pauseGame();
				else if (c->prevState == MAIN_MENU) c->menuReturn();
			}
			break;
		}
	}
}

// Callback for cursor position
void MyGLCanvas::cursor_position_callback(GLFWwindow* _window, double currX, double currY) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (c->currState == PLAYING) {
		if (c->firstMouse) {
			c->prevX = currX;
			c->prevY = currY;
			c->firstMouse = false;
		}

		float x_offset = currX - c->prevX;
		float y_offset = c->prevY - currY;

		x_offset *= c->sensitivity;
		y_offset *= c->sensitivity;

		c->prevX = currX;
		c->prevY = currY;

		c->player->moveSight(x_offset, y_offset);
	} else if (c->currState == MAIN_MENU) {
		c->handleButtons(c->mainMenu, c->mainMenu.size() - 1, 0.1, 0.045, currX, currY);
	} else if (c->currState == DEAD) {
		c->handleButtons(c->deathScreen, c->deathScreen.size() - 1, 0.1, 0.045, currX, currY);
	} else if (c->currState == PAUSE) {
		c->handleButtons(c->pauseScreen, c->pauseScreen.size() - 1, 0.095, 0.05, currX, currY);
	} else if (c->currState == OPTIONS) {
		c->handleButtons(c->optionPlusMinus, c->optionPlusMinus.size(), 0.0125, 0.02, currX, currY);
		if (c->optionSelected == NONE) c->handleButtons(c->optionScreen, c->optionScreen.size() - 1, 0.095, 0.05, currX, currY);
	}

}

// Callback for mouse click
void MyGLCanvas::mouse_button_callback(GLFWwindow* _window, int button, int action, int mods) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (action == GLFW_PRESS) {
		switch(c->currState) {
		case PLAYING:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				shaderType spellAttempt = c->player->spellSelected;

				if (c->player->getSpellCost(spellAttempt) <= c->player->getMana()) {
					c->fireProjectile(c->player->spellSelected, c->player->getPosition(), c->player->getLookVec());
					c->player->changeMana(-c->player->getSpellCost(spellAttempt));
				}
				else {
					ISound *sound;
					// TODO: Add some visual cue
					sound = c->soundEngine->play2D("./audio/fizzle.mp3", false, false, true);
					sound->setVolume(c->miscVol);
					sound->drop();
				}
			}
			break;
		case MAIN_MENU:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				ISound *sound;
				switch (c->buttonSelected) {
				case BUTTON_START:
					c->playClick();
					c->startGame();
					break;
				case BUTTON_CONTROLS:
					c->playClick();
					c->showControls();
					break;
				case BUTTON_OPTIONS:
					c->playClick();
					c->showOptions();
					break;
				case BUTTON_QUIT:
					c->playClick();
					glfwSetWindowShouldClose(_window, true);
					break;
				default:
					break;
				}
			}
			break;
		case DEAD:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				switch (c->buttonSelected) {
				case BUTTON_MAIN:
					c->playClick();
					c->showMenu();
					break;
				case BUTTON_RESTART:
					c->playClick();
					c->startGame();
					break;
				case BUTTON_QUIT2:
					c->playClick();
					glfwSetWindowShouldClose(_window, true);
					break;
				default:
					break;
				}
			}
			break;
		case PAUSE:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				switch (c->buttonSelected) {
				case BUTTON_RESUME:
					c->playClick();
					c->unpauseGame();
					break;
				case BUTTON_CONTROLS2:
					c->playClick();
					c->showControls();
					break;
				case BUTTON_OPTIONS2:
					c->playClick();
					c->showOptions();
					break;
				case BUTTON_MAIN2:
					c->playClick();
					c->showMenu();
					break;
				case BUTTON_QUIT3:
					c->playClick();
					glfwSetWindowShouldClose(_window, true);
					break;
				}
			}
			break;
		case OPTIONS:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				switch (c->buttonSelected) {
				case BUTTON_FULLSCREEN:
					c->playClick();
					c->toggleFullScreen();
					break;
				case BUTTON_PLUS:
					c->playClick();
					switch (c->optionSelected) {
					case MASTER_VOL:
						c->masterVol = min(MAX_VOLUME, c->masterVol + 0.05);
						c->soundEngine->setSoundVolume(c->masterVol);
						break;
					case MUSIC_VOL:
						c->musicVol = min(MAX_VOLUME, c->musicVol + 0.01);
						c->music->setVolume(c->musicVol);
						c->pauseMusic->setVolume(c->musicVol);
						break;
					case MISC_VOL:
						c->miscVol = min(MAX_VOLUME, c->miscVol + 0.05);
						Projectile::volume = c->miscVol;
						Enemy::volume = c->miscVol;
						Player::volume = c->miscVol;
						Pickup::volume = c->miscVol;
						break;
					case SENSITIVITY:
						c->sensitivity = min(MAX_SENSITIVITY, c->sensitivity + 0.025);
						break;
					}
					break;
				case BUTTON_MINUS:
					c->playClick();
					switch (c->optionSelected) {
					case MASTER_VOL:
						c->masterVol = max(0, c->masterVol - 0.05);
						c->soundEngine->setSoundVolume(c->masterVol);
						break;
					case MUSIC_VOL:
						c->musicVol = max(0, c->musicVol - 0.01);
						c->music->setVolume(c->musicVol);
						c->pauseMusic->setVolume(c->musicVol);
						break;
					case MISC_VOL:
						c->miscVol = max(0, c->miscVol - 0.05);
						Projectile::volume = c->miscVol;
						Enemy::volume = c->miscVol;
						Player::volume = c->miscVol;
						Pickup::volume = c->miscVol;
						break;
					case SENSITIVITY:
						c->sensitivity = max(0, c->sensitivity - 0.025);
						break;
					}
					break;
				}
			}
			break;
		} 
	}
}

// Callback for changing the size of the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	printf("Resizing window\n");
	glViewport(0, 0, width, height);
}

// Sets the default and hover cursor icons
void MyGLCanvas::setupCursors() {
	regular = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	hover = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

// Sets up the GLFW window 
void MyGLCanvas::setupWindow(int w, int h) {
	glfwInit();

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);

	// Specifies OpenGL must be compatible with version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Removes outdated functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  //Uncomment for MacOS devices

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	if (DEBUGMODE) {
		window = glfwCreateWindow(mode->width / 1.5, mode->height / 1.5, "Spellfire", NULL, NULL);
		fullscreen = false;
		cursorVisible = true;
	}
	else {
		window = glfwCreateWindow(mode->width, mode->height, "Spellfire", monitor, NULL);
		fullscreen = true;
		cursorVisible = false;
	}

	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);

	if (!DEBUGMODE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glViewport(0, 0, mode->width, mode->height);
	}
	else {
		glViewport(0, 0, mode->width / 1.5, mode->height / 1.5);
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
}

// Checks keyboard input. Works best for movement using WASD
void MyGLCanvas::pollInput() {
	if (currState == PLAYING) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player->moveForward();
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player->moveLeft();
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player->moveBackward();
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player->moveRight();
	}
}

// Removes all objects from scene and start initial enemies. Places player back at start.
void MyGLCanvas::startGame() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	list<Enemy *>::iterator itE = enemyList.begin();
	while(itE != enemyList.end()) removeEnemy(itE);

	list<Projectile *>::iterator itP = projectileList.begin();
	while(itP != projectileList.end()) removeProjectile(itP);

	list<Pickup *>::iterator itPU = pickupList.begin();
	while(itPU != pickupList.end()) removePickup(itPU);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			spawnEnemy(GOOP);
		spawnEnemy(JAD);
	}

	stopSound(music);
	music = soundEngine->play2D("./audio/metal.mp3", true, false, true);
	music->setVolume(musicVol);
	
	firstMouse = true;
	prevState = currState;
	currState = PLAYING;
	player->restartPlayer();
}

// Removes all objects from scene and goes back to main menu
void MyGLCanvas::showMenu() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	list<Enemy *>::iterator itE = enemyList.begin();
	while (itE != enemyList.end()) removeEnemy(itE);

	list<Projectile *>::iterator itP = projectileList.begin();
	while (itP != projectileList.end()) removeProjectile(itP);

	list<Pickup *>::iterator itPU = pickupList.begin();
	while (itPU != pickupList.end()) removePickup(itPU);

	pauseMusic->setIsPaused(true);

	stopSound(music);
	music = soundEngine->play2D("./audio/epic.mp3", true, false, true);
	music->setVolume(musicVol);

	prevState = currState;
	currState = MAIN_MENU;
}

// Checks if an item is expired based on it's duration and spawn time
bool MyGLCanvas::isExpired(time_t spawnTime, float duration) {
	return (difftime(time(0), spawnTime) >= duration);
}

// Changes fullscreen option
void MyGLCanvas::toggleFullScreen() {
	fullscreen = !fullscreen;

	if (fullscreen) {
		glfwSetWindowMonitor(window, monitor, NULL, NULL, mode->width, mode->height, mode->refreshRate);
	} else {
		glfwSetWindowMonitor(window, NULL, 200, 100, 1000, 800, NULL);
	}
}

// Changes cursor visibility
void MyGLCanvas::toggleCursor() {
	cursorVisible = !cursorVisible;

	if (cursorVisible) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else {
	    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

// Setups up sound engine and initial sound options
void MyGLCanvas::setupSound() {
	masterVol = 1.0f;
	musicVol = 0.1f;
	miscVol = 0.7f;

	Projectile::volume = miscVol;
	Enemy::volume = miscVol;
	Player::volume = miscVol;
	Pickup::volume = miscVol;
	
	soundEngine = createIrrKlangDevice();
	if (!soundEngine) exit(1);

	music = soundEngine->play2D("./audio/epic.mp3", true, false, true);
	music->setVolume(musicVol);
	soundEngine->setSoundVolume(masterVol);

	pauseMusic = soundEngine->play2D("./audio/pause.mp3", true, true, true);
	pauseMusic->setVolume(musicVol);
}

// Combines to irrKlang function calls to stop the current sound and free up
// the sound pointer
void MyGLCanvas::stopSound(ISound *sound) {
	sound->stop();
	sound->drop();
}

// Plays the click sound for a button
void MyGLCanvas::playClick() {
	ISound *sound;
	sound = soundEngine->play2D("./audio/click.mp3", false, false, true);
	sound->setVolume(miscVol);
	sound->drop();
}

// Checks to see if the cursor is over a button
void MyGLCanvas::handleButtons(std::vector<Sprite *> buttonList, int numButtons, double offX, double offY, double currX, double currY) {
	glm::vec3 pos;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	currX /= double(width);
	currY /= double(height);

	optionSelected = NONE;
	buttonSelected = SPRITE_MAIN;  // Acts as a NULL value TODO: Find a better value

	//printf("----------------------------------------\n");
	for (int i = 0; i < numButtons; i++) {
		pos = buttonList[i]->getPosition();
		pos.x /= mode->width;
		pos.y /= mode->height;

		if (overButton(currX, currY, offX, offY, pos)) {
			buttonSelected = buttonList[i]->spriteType;
			glfwSetCursor(window, hover);

			if (buttonSelected == BUTTON_PLUS || buttonSelected == BUTTON_MINUS) optionSelected = (optionType)(i / 2);
			break;
		}
		else {
			glfwSetCursor(window, regular);
		}

	//printf("CurrX: %f, CurrY: %f\n LeftPos: %f, RightPos: %f, TopPos: %f, BottomPos: %f\n\n", currX, currY, pos.x - offX, pos.x + offX, pos.y - offY, pos.y + offY);
	}


}

// Helper function to handleButtons
bool MyGLCanvas::overButton(double x, double y, double offX, double offY, glm::vec3 pos) {
	return  (pos.x - offX) <= x && x <= (pos.x + offX) && (pos.y - offY) <= y && y <= (pos.y + offY);
}

// Changes to pause screen
void MyGLCanvas::pauseGame() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	prevState = currState;
	currState = PAUSE;

	music->setIsPaused(true);
	pauseMusic->setIsPaused(false);
}

// Changes to game screen
void MyGLCanvas::unpauseGame() {
	prevState = currState;
	currState = PLAYING;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	pauseMusic->setIsPaused(true);
	music->setIsPaused(false);

	firstMouse = true;
}

// Changes to death screen
void MyGLCanvas::gameOver() {
	stopSound(music);
	music = soundEngine->play2D("./audio/sad_dark.mp3", true, false, true);
	music->setVolume(musicVol);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	prevState = currState;
	currState = DEAD;
}

// Changes to control screen
void MyGLCanvas::showControls() {
	prevState = currState;
	currState = CONTROLS;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	music->setIsPaused(true);
	pauseMusic->setIsPaused(false);
}

// Changes to options screen
void MyGLCanvas::showOptions() {
	prevState = currState;
	currState = OPTIONS;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	music->setIsPaused(true);
	pauseMusic->setIsPaused(false);
}

// Returns to the main menu from one of the option screens
void MyGLCanvas::menuReturn() {
	prevState = currState;
	currState = MAIN_MENU;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	pauseMusic->setIsPaused(true);
	music->setIsPaused(false);
}