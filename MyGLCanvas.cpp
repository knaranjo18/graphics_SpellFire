 #include "MyGLCanvas.h"

#define MASTER_VOLUME 0.1f
#define MUSIC_VOLUME 0.5f
#define SENSITIVITY 0.5f
#define IFRAME_AFTER_HIT 60
#define MAX_ENEMIES 20

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

#define DEBUGMODE true



// Constructor to set everything up. Spawns some initial enemies
MyGLCanvas::MyGLCanvas() {
	setupWindow(800, 600);
	setupSound();
	srand(time(0));

	currState = LOADING;
	prevX = prevY = 0;
	firstTime = firstMouse = firstDeath = true;
	lightPos = glm::vec3(0.0, 10, 0.0);
	numBlob = numJad = numManaPot = numHealthPot = numFireball = 0;

	player = new Player();
	arena = new Scenery(ARENA, glm::vec3(0.0, 1.1, 0.0), glm::vec3(9, 9, 9), 0.0);

	healthBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(HEALTHBAR_START, mode->height - BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 1.0, 0.0), FOREGROUND));
	healthBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(HEALTHBAR_START, mode->height - BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND));

	manaBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width - MANABAR_START, mode->height - BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 0.0, 1.0), FOREGROUND));
	manaBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width - MANABAR_START, mode->height - BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND));

	expBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width / 2, mode->height - BAR_HEIGHT), glm::vec2(EXPBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(1.0, 1.0, 0.0), FOREGROUND));
	expBar.push_back(new Sprite(SPRITE_UNTEXTURED, glm::vec2(mode->width / 2, mode->height - BAR_HEIGHT), glm::vec2(EXPBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), FOREGROUND));

	glm::vec2 pos(mode->width / 2.0 - 2, mode->height / 2.0 + 40);
	crossHair.push_back(new Sprite(SPRITE_UNTEXTURED, pos, glm::vec2(2.0, 30.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));
	crossHair.push_back(new Sprite(SPRITE_UNTEXTURED, pos, glm::vec2(30.0, 2.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND));

	deathScreen.push_back(new Sprite(SPRITE_DEATH, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0 , 1.0), FOREGROUND));
	loadingScreen = new Sprite(SPRITE_LOADING, glm::vec2(mode->width / 2.0f, mode->height / 2.0f), glm::vec2(mode->width, mode->height), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND);
	
	// Initial Enemies
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			spawnEnemy(GOOP);
		spawnEnemy(JAD);
	}
}

// Makes sure to reclaim all memory taken by new
MyGLCanvas::~MyGLCanvas() {
	deallocate();
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

		stopSound(music);
		music = soundEngine->play2D("./audio/metal.mp3", true, false, true);
		music->setVolume(MUSIC_VOLUME);

		// needs to be after so that shaders can setup
		updateCamera(mode->width, mode->height);
		firstTime = false;
		currState = PLAYING;
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
		if (firstDeath) {
			stopSound(music);
			music = soundEngine->play2D("./audio/sad_dark.mp3", true, false, true);
			music->setVolume(MUSIC_VOLUME);
			firstDeath = false;
		}

		drawDeathScene();
		break;
	case START:
		//TODO
		break;
	default:
		printf("INVALID GAME STATE\n");
		exit(1);
	}
}


// Draws the death scene with the skull
void MyGLCanvas::drawDeathScene() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	shaderList[SPRITE_DEATH]->useShader();
	GLint modelView_id = glGetUniformLocation(shaderList[SPRITE_DEATH]->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelViewMatrix));

	deathScreen[0]->draw(shaderList[SPRITE_DEATH], plyList[SPRITE_DEATH]);
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

	soundEngine->setListenerPosition(vec3cov(playerPos), vec3cov(playerLook));


	handleMoveCollisions(playerPos);
	handlePlayerCollisions();
	
	handleProjectiles();
	handlePickups();

	handleManaBar();
	handleHealthBar();
	handleExpBar();

	if (player->isDead()) currState = DEAD;

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
			int random = rand() % 3;
			player->applyHit((*itE)->getHitFunc());
			player->setiFrames(IFRAME_AFTER_HIT);
			playerHurtSound();
			return;
		}
	}
}

void MyGLCanvas::playerHurtSound() {
	int random = rand() % 3;

	switch (random) {
	case 0:
		soundEngine->play2D("./audio/player_hurt1.mp3");
		break;
	case 1:
		soundEngine->play2D("./audio/player_hurt2.mp3");
		break;
	case 2:
		soundEngine->play2D("./audio/player_hurt3.mp3");
		break;
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
		enemyList.push_front(new Enemy(GOOP, glm::vec3(xPos, HEIGHT - 0.05, zPos)));
		numBlob++;
		break;
	case(JAD):
		enemyList.push_back(new Enemy(JAD, glm::vec3(xPos, HEIGHT, zPos)));
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
		pickupList.push_front(new Pickup(position, angle, type));
		numManaPot++;
		break;
	case(HEALTHPOT):
		pickupList.push_back(new Pickup(position, angle, type));
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
		
		if (i == SPRITE_UNTEXTURED || i == SPRITE_DEATH) {
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(orthoMatrix));
		} else{;
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));
		}
	}
}

// Initializes all the shaderes with their ply files and textures
void MyGLCanvas::setupShaders() {	
	#ifndef __APPLE__
		printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	#endif
	
	shaderList.push_back(new ShaderManager());  // loading
	drawLoading(); // We draw loading screen now to mask the time needed to load all the other resources

	shaderList.push_back(new ShaderManager()); // goop
	shaderList.push_back(new ShaderManager()); // jad
	shaderList.push_back(new ShaderManager()); // fireball
	shaderList.push_back(new ShaderManager()); // sprite
	shaderList.push_back(new ShaderManager()); // death
	shaderList.push_back(new ShaderManager()); // arena
	shaderList.push_back(new ShaderManager()); // health pot
	shaderList.push_back(new ShaderManager()); // mana pot
	plyList.push_back(new ply("./data/blob.ply"));
	plyList.push_back(new ply("./data/jad.ply"));
	
	plyList.push_back(new ply("./data/fireball.ply"));
	plyList[FIREBALL]->applyTexture("./data/fireball.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[SPRITE_DEATH]->applyTexture("./data/skull_medium.ppm");

	plyList.push_back(new ply("./data/arena.ply"));
	plyList[ARENA]->applyTexture("./data/arena_large.ppm");

	plyList.push_back(new ply("./data/potion.ply"));
	plyList[HEALTHPOT]->applyTexture("./data/healthPot.ppm");

	plyList.push_back(new ply("./data/potion.ply"));
	plyList[MANAPOT]->applyTexture("./data/manaPot.ppm");

	for (int i = 1; i < shaderList.size(); i++) {
		if (i == ARENA || i == FIREBALL || i == HEALTHPOT || i == MANAPOT) {
			shaderList[i]->initShader("./shaders/330/model_textured.vert", "./shaders/330/model_textured.frag");
		} else if (i == SPRITE_UNTEXTURED) {
			shaderList[i]->initShader("./shaders/330/sprite_untextured.vert", "./shaders/330/sprite_untextured.frag");
		} else if (i == SPRITE_DEATH) {
			shaderList[i]->initShader("./shaders/330/sprite_textured.vert", "./shaders/330/sprite_textured.frag");
		} else {
			shaderList[i]->initShader("./shaders/330/model_untextured.vert", "./shaders/330/model_untextured.frag");
		}

		GLint light_id = glGetUniformLocation(shaderList[i]->program, "lightPos");
		glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

		if (i == SPRITE_UNTEXTURED || i == SPRITE_DEATH) {
			plyList[i]->buildArraysSprite();
			plyList[i]->bindVBOsprites(shaderList[i]->program);
		} else {
			plyList[i]->buildArrays();
			plyList[i]->bindVBO(shaderList[i]->program);
		}
	}
	
	string filenames[6] = {
		"./data/red_right.ppm", "./data/red_left.ppm", "./data/red_top.ppm",
		"./data/red_bottom.ppm", "./data/red_front.ppm", "./data/red_back.ppm"
	};

	skybox = new Skybox(filenames);
	shaderList.push_back(skybox->shader); // Skybox
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
	delete loadingScreen;
	music->drop();
	soundEngine->drop();

}

// Callback for keyboard key
void MyGLCanvas::key_callback(GLFWwindow* _window, int key, int scancode, int action, int mods) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(_window, true);
		if (key == GLFW_KEY_M) Enemy::debug_draw_hitbox = !Enemy::debug_draw_hitbox;
		if (key == GLFW_KEY_R && c->currState == DEAD) c->restartGame();
		if (key == GLFW_KEY_F) c->toggleFullScreen();
		if (key == GLFW_KEY_TAB) c->toggleCursor();
	}
}

// Callback for cursor position
void MyGLCanvas::cursor_position_callback(GLFWwindow* _window, double currX, double currY) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (c->firstMouse) {
		c->prevX = currX;
		c->prevY = currY;
		c->firstMouse = false;
	}

	float x_offset = currX - c->prevX;
	float y_offset = c->prevY - currY;

	x_offset *= SENSITIVITY;
	y_offset *= SENSITIVITY;

	c->prevX = currX;
	c->prevY = currY;

	c->player->moveSight(x_offset, y_offset);
}

// Callback for mouse click
void MyGLCanvas::mouse_button_callback(GLFWwindow* _window, int button, int action, int mods) {
	MyGLCanvas *c = (MyGLCanvas *)glfwGetWindowUserPointer(_window);

	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (c->currState == PLAYING) {
				shaderType spellAttempt = c->player->spellSelected;

				if (c->player->getSpellCost(spellAttempt) <= c->player->getMana()) {
					c->fireProjectile(c->player->spellSelected, c->player->getPosition(), c->player->getLookVec());
					c->player->changeMana(-c->player->getSpellCost(spellAttempt));
				}
				else {
					// TODO: Add some visual cue
					c->soundEngine->play2D("./audio/fizzle.mp3");
				}
			}
		}
	}
}

// Callback for changing the size of the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	printf("Resizing window\n");
	glViewport(0, 0, width, height);
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player->moveForward();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player->moveLeft();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player->moveBackward();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player->moveRight();
}

// Removes all objects from scene and start initial enemies. Places player back at start.
void MyGLCanvas::restartGame() {
	printf("GAME RESTART\n");

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
	music->setVolume(MUSIC_VOLUME);
	
	firstMouse = firstDeath = true;
	currState = PLAYING;
	player->restartPlayer();
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
		glfwSetWindowMonitor(window, NULL, 300, 300, 800, 600, NULL);
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
	soundEngine = createIrrKlangDevice();
	if (!soundEngine) exit(1);


	music = soundEngine->play2D("./audio/epic.mp3", true, false, true);
	music->setVolume(MUSIC_VOLUME);
	soundEngine->setSoundVolume(MASTER_VOLUME);
}


// Helper function to convert from GLM vec3 to irrKlang vec3
vec3df MyGLCanvas::vec3cov(glm::vec3 input) {
	return vec3df(input.x, input.y, input.z);
}

// Combines to irrKlang function calls to stop the current sound and free up
// the sound pointer
void MyGLCanvas::stopSound(ISound *sound) {
	sound->stop();
	sound->drop();
}