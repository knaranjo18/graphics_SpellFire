#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

#include <FL/glut.h>
#include <FL/glu.h>
#include <FL/names.h>
#include <FL/FL_Input_.H>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include <iostream>
#include <vector>
#include <time.h>

#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "Scenery.h"
#include "BoundingBox.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include "Camera.h"
#include "Sprite.h"
#include "skybox.h"
#include "pickup.h"

#include <chrono>
#include <thread>


class MyGLCanvas : public Fl_Gl_Window {
public:
	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();

private:
	void draw();
	void drawScene();
	void drawDeathScene();
	
	void setupShaders();
	
	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);
	void deallocate();
	void enforceFrameTime(GLint);
	
	void moveSight();
	void fireProjectile(shaderType projectileType, glm::vec3 originPoint, glm::vec3 directionFired);
	void removeProjectile(shaderType projectileType, int index);

	void spawnPickup(shaderType, glm::vec3);
	void removePickup(int);
	void spawnEnemy(shaderType enemyType);
	void respawnEnemies();
	void removeEnemy(shaderType enemyType, int index);

	void doGameLogic();
	void hendleProjectiles(vector<Enemy*>&);
	void applyProjectile(Projectile*, int, vector<Enemy*>&);
	void handleMoveCollisions(glm::vec3, vector<Enemy*>&);
	void handlePlayerCollisions(vector<Enemy*>&);

	int findEnemyCollision(Projectile*, vector<Enemy*>&);

	void setupSprites();
	void handleHealthBar();
	void handleManaBar();
	void handleExpBar();

	vector<ShaderManager *> shaderList;
	vector<Enemy *> cowList, bunnyList;
	vector<ply *> plyList;
	vector<Projectile *> projectileList;
	vector<Pickup*> pickupList;
	vector<Sprite *> crossHair;
	vector<Sprite *> healthBar;
	vector<Sprite *> manaBar;
	vector<Sprite *> expBar;
	vector<Sprite *> deathScreen;

	Player *player;
	Scenery *arena;
	Skybox* skybox;
	

	glm::vec3 lightPos;
	int prevX, prevY;
	bool firstTime, alive;
	time_t startTime;
};

#endif 