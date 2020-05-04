 #include "MyGLCanvas.h"

#define SENSITIVITY 1.0f
#define HEALTHBAR_START 260.0
#define HEALTHBAR_LENGTH 500.0

#define MANABAR_START 260
#define MANABAR_LENGTH 500
#define EXPBAR_LENGTH 500
#define BAR_HEIGHT 25
#define BAR_WIDTH 30

MyGLCanvas::MyGLCanvas(int _x, int _y, int _w, int _h, const char *l) : Fl_Gl_Window(_x, _y, _w, _h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	srand(time(0));
	prevX = prevY = 0;
	firstTime = true;
	lightPos = glm::vec3(0.0, 10, 0.0);
	alive = true;
	
	player = new Player();

	for (int i = 0; i < 2; i++) {
		spawnEnemy(COW);
		spawnEnemy(BUNNY);
	}
	float temp = 400 / 500;
	temp = 1 - temp;


	arena = new Scenery(ARENA, glm::vec3(0.0, 1.1, 0.0), glm::vec3(9, 9, 9), 0.0);
	healthBar.push_back(new Sprite(SPRITE, glm::vec2(HEALTHBAR_START, BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 1.0, 0.0), FOREGROUND));
	healthBar.push_back(new Sprite(SPRITE, glm::vec2(HEALTHBAR_START, BAR_HEIGHT), glm::vec2(HEALTHBAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND));

	manaBar.push_back(new Sprite());
	manaBar.push_back(new Sprite());

	crossHair.push_back(new Sprite());
	crossHair.push_back(new Sprite());

	deathScreen.push_back(new Sprite());
	deathScreen.push_back(new Sprite());
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

		if (!crossHair[0]->initComplete) {
			setupSprites();
		}

		// needs to be after so that shaders can setup
		updateCamera(w(), h());
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (alive) {
		drawScene();
	} else {
		drawDeathScene();
	}
}

void MyGLCanvas::drawScene() {
	double deltaTime = difftime(time(0), startTime);

	doGameLogic();

	//setting up camera info
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();
	/*-----------------------For the skybox----------------------------------------*/
	skybox->draw(modelViewMatrix, player->myCam->getProjectionMatrix());
	
	/*-----------------------For the scenery----------------------------------------*/
	arena->draw(modelViewMatrix, shaderList[ARENA], plyList[ARENA]);

	/*------------------------For GUI-----------------------------------------------*/
	
	for (int i = 0; i < 2; i++)
		healthBar[i]->draw(modelViewMatrix, shaderList[SPRITE], plyList[SPRITE]);

	for (int i = 0; i < 2; i++) 
		crossHair[i]->draw(modelViewMatrix, shaderList[SPRITE], plyList[SPRITE]);

	for (int i = 0; i < 2; i++)
		manaBar[i]->draw(modelViewMatrix, shaderList[SPRITE], plyList[SPRITE]);
		
	/*--------------For the enemy---------------------------*/
	for (int i = 0; i < cowList.size(); i++) {
		cowList[i]->draw(modelViewMatrix, shaderList[COW], plyList[COW]);
	}

	for (int i = 0; i < bunnyList.size(); i++) {
		bunnyList[i]->draw(modelViewMatrix, shaderList[BUNNY], plyList[BUNNY]);
	}

	for (int i = 0; i < projectileList.size(); i++) {
		projectileList[i]->draw(modelViewMatrix, shaderList[FIREBALL], plyList[FIREBALL]);
	}	
}

void MyGLCanvas::doGameLogic() {
	glm::vec3 playerPos = player->myCam->getEyePoint();
	
	vector<Enemy*> enemies;
	for (int i = 0; i < cowList.size(); i++) {
		enemies.push_back(cowList[i]);
	}
	for (int i = 0; i < bunnyList.size(); i++) {
		enemies.push_back(bunnyList[i]);
	}

	player->chargeMana();

	handleMoveCollisions(playerPos, enemies);
	hendleProjectiles(enemies);
	handleManaBar();
	handleHealthBar();
}

void MyGLCanvas::handleHealthBar() {
	float healthRatio = player->getHealth() / player->maxHealth;

	float length = HEALTHBAR_LENGTH * healthRatio;
	float offset = (HEALTHBAR_START * (1 - healthRatio));

	glm::vec2 pos(HEALTHBAR_START - offset, BAR_HEIGHT);
	glm::vec2 scale(length, BAR_WIDTH);

	healthBar[0]->setPosition(pos);
	healthBar[0]->setScale(scale);
}

void MyGLCanvas::handleManaBar() {
	float manaRatio = player->getMana() / player->maxMana;

	float length = MANABAR_LENGTH * manaRatio;
	float offset = (MANABAR_START * (1 - manaRatio));

	glm::vec2 pos(w() - MANABAR_START, BAR_HEIGHT);
	glm::vec2 scale(length, BAR_WIDTH);

	manaBar[0]->setPosition(pos);
	manaBar[0]->setScale(scale);
}

void MyGLCanvas::hendleProjectiles(vector<Enemy*>&enemies) {
	int hit;
	for (int i = 0; i < projectileList.size(); i++) {
		if (difftime(time(0), projectileList[i]->getSpawnTime()) >= double(projectileList[i]->getDuration())) {
			removeProjectile(FIREBALL, i);
			i--;
		}
		else if (projectileList[i]->hitFloor()) {
			removeProjectile(FIREBALL, i);
			i--;
		}
		else if ((hit = findEnemyCollision(projectileList[i], enemies)) != -1) {
			// deal damage to enemy here
			applyProjectile(projectileList[i], hit, enemies);
			removeProjectile(FIREBALL, i);
			i--;
		}
		else {
			projectileList[i]->moveProjectile();
		}
	}
}

// Handle all logic when enemy e is hit by projectile p
void MyGLCanvas::applyProjectile(Projectile* p, int i, vector<Enemy*>&enemies) {
	Enemy* e = enemies[i];
	printf("Before hit, enemy has %f health left\n", e->getHealth());
	e->applyHit(p->getHitfunc());

	// if the enemy is dead remove it TODO: make this better its jank
	// probably make it better by making the one list of enemies THE list and using
	// counts of enemy types to tell what pointer is what
	if (e->getHealth() <= 0) { // TODO: an isDead() func would be better
		if (e->enemyType == COW) { 
			removeEnemy(COW, i);
		}
		else if (e->enemyType == BUNNY) { 
			removeEnemy(BUNNY, i - cowList.size()); 
		}
		
		enemies.erase(enemies.begin() + i);
		printf("Enemy killed!\n");
	}
	printf("after hit, it has %f health left!\n", e->getHealth());
}

// Returns the index of the enemy which is currently colliding with the projectile
// or -1 if there is no collision
int MyGLCanvas::findEnemyCollision(Projectile* p, vector<Enemy*>& enemies) {
	const BoundingBox* pBox = p->getBox();
	for (int i = 0; i < enemies.size(); i++) {
		const BoundingBox* eBox = enemies[i]->getBox();
		if (pBox->doesCollide(*eBox)) {
			return i;
		}
	}
	return -1;
}

void MyGLCanvas::handleMoveCollisions(glm::vec3 playerPos, vector<Enemy*>&enemies) {
	// move either away from collision or towards player
	for (int i = 0; i < enemies.size(); i++) {
		bool moved = false;
		for (int j = i + 1; j < enemies.size(); j++) {
			const BoundingBox* box1 = enemies[i]->getBox();
			const BoundingBox* box2 = enemies[j]->getBox();
			if (box1->doesCollide(*box2) && !moved) {
				glm::vec3 dir = box1->getCenter() - box2->getCenter();
				// move enemies towards some faraway point in opposite directions
				enemies[i]->moveEnemy((dir * 5.0f) + glm::vec3(box1->getCenter()));
				enemies[j]->moveEnemy((-dir * 5.0f) + glm::vec3(box2->getCenter()));
				box1 = enemies[i]->getBox();
				box2 = enemies[j]->getBox();
				moved = true;
			}
		}
		if (!moved) {
			enemies[i]->moveEnemy(playerPos);
		}
	}
}

void MyGLCanvas::fireProjectile(shaderType projectileType, glm::vec3 originPoint, glm::vec3 directionFired) {
	glm::vec3 startPoint = originPoint + (0.1f * glm::normalize(directionFired));
	startPoint.y -= 0.08;

	projectileList.push_back(new Projectile(projectileType, startPoint, directionFired));
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
	default:
		printf("NOT A VALID ENEMY");
		exit(1);
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
	default:
		printf("NOT A VALID ENEMY");
		exit(1);
		break;
	} 
}

void MyGLCanvas::removeProjectile(shaderType projectileType, int index) {
	switch (projectileType) {
	case(FIREBALL):
		delete projectileList[index];
		projectileList.erase(projectileList.begin() + index);
		break;
	default:
		printf("NOT A VALID PROJECTILE");
		exit(1);
		break;
	}
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	GLint projection_id;
	xy_aspect = (float)width / (float)height;

	player->myCam->setScreenSize(width, height);

	glm::mat4 perspectiveMatrix = player->myCam->getProjectionMatrix();
	glm::mat4 orthoMatrix = glm::ortho(0.0f, float(w()), float(h()), 0.0f, -1.0f, 1.0f);
	int size = shaderList.size();
	for (int i = 0; i < size; i++) {
		shaderList[i]->useShader();
		projection_id = glGetUniformLocation(shaderList[i]->program, "myProjectionMatrix");
		
		if (i == SPRITE || i == DEATH) {
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(orthoMatrix));
		} else{
			glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(perspectiveMatrix));
		}
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
				startTime = time(0);
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
		// toggle debug mode
		case 'm':
			Enemy::debug_draw_hitbox = !Enemy::debug_draw_hitbox;
			break;
		case FL_Escape:
			deallocate();
			exit(0);
			break;
		case 'h':
			player->changeHealth(5);
			break;
		case 'p':
			player->changeHealth(-5);
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
		else if (button == FL_RIGHT_MOUSE) {
			shaderType spellAttempt = player->spellSelected;

			if (player->getSpellCost(spellAttempt) <= player->getMana()) {
				fireProjectile(player->spellSelected, player->myCam->getEyePoint(), player->myCam->getLookVector());
				player->changeMana(-player->getSpellCost(spellAttempt));
			} else {
				printf("Need to charge my mana!\n");
			}

		} else 
			return 0;

		return 1;
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
	shaderList.push_back(new ShaderManager());
	shaderList.push_back(new ShaderManager());
	shaderList.push_back(new ShaderManager());

	plyList.push_back(new ply("./data/cow.ply"));
	plyList.push_back(new ply("./data/bunny.ply"));
	
	plyList.push_back(new ply("./data/fireball.ply"));
	plyList[FIREBALL]->applyTexture("./data/fireball_256.ppm");

	plyList.push_back(new ply("./data/spriteTemplate.ply"));

	plyList.push_back(new ply("./data/spriteTemplate.ply"));
	plyList[DEATH]->applyTexture("./data/skull.ppm");

	plyList.push_back(new ply("./data/arena_4_tex_2.ply"));
	plyList[ARENA]->applyTexture("./data/arena_1024.ppm");

	for (int i = COW; i <= ARENA; i++) {
		if (i == ARENA || i == FIREBALL) {
			shaderList[i]->initShader("./shaders/330/scene.vert", "./shaders/330/scene.frag");
		} else if (i == SPRITE) {
			shaderList[i]->initShader("./shaders/330/sprite.vert", "./shaders/330/sprite.frag");
		} else if (i == DEATH) {
			shaderList[i]->initShader("./shaders/330/death.vert", "./shaders/330/death.frag");
		} else {
			shaderList[i]->initShader("./shaders/330/scene.vert", "./shaders/330/enemyColor.frag");
		}


		GLint light_id = glGetUniformLocation(shaderList[i]->program, "lightPos");
		glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

		if (i == SPRITE || i == DEATH) {
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
}

void MyGLCanvas::setupSprites() {
	glm::vec2 pos(w() / 2.0 - 2, h() / 2.0 + 40);

	crossHair[0]->setEverything(SPRITE, pos, glm::vec2(2.0, 30.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND);
	crossHair[1]->setEverything(SPRITE, pos, glm::vec2(30.0, 2.0), 0, glm::vec3(1.0, 1.0, 1.0), FOREGROUND);

	manaBar[0]->setEverything(SPRITE, glm::vec2(w() - MANABAR_START, BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.0, 0.0, 1.0), FOREGROUND);
	manaBar[1]->setEverything(SPRITE, glm::vec2(w() - MANABAR_START, BAR_HEIGHT), glm::vec2(MANABAR_LENGTH, BAR_WIDTH), 0, glm::vec3(0.5, 0.5, 0.5), BACKGROUND);

	deathScreen[0]->setEverything(DEATH, glm::vec2(767, 430), glm::vec2(w(), h()), 0, glm::vec3(0.7, 0.0, 0.1), FOREGROUND);
	//deathScreen[1]->setEverything(SPRITE, glm::vec2(767, 430), glm::vec2(w(), h()), 0, glm::vec3(0.7, 0.0, 0.1), BACKGROUND);
}

void printEvent(int e) {
	printf("Event was %s (%d)\n", fl_eventnames[e], e);
}

void MyGLCanvas::deallocate() {
	delete player;
	delete arena;

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

	for (int i = 0; i < projectileList.size(); i++) {
		delete projectileList[i];
	}
}


void MyGLCanvas::drawDeathScene() {
	glm::mat4 modelViewMatrix = player->myCam->getModelViewMatrix();

	deathScreen[0]->draw(modelViewMatrix, shaderList[DEATH], plyList[DEATH]);
	//deathScreen[1]->draw(modelViewMatrix, shaderList[SPRITE], plyList[SPRITE]);
}