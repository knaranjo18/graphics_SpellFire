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

#include <GLFW\glfw3.h>

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


class MyGLCanvas {
public:
	MyGLCanvas();
	~MyGLCanvas();

	void run();

private:
	void draw();
	void drawScene();
	void drawDeathScene();

	void setupShaders();
	void restartGame();
	
	void updateCamera(int width, int height);
	void deallocate();
	void enforceFrameTime(GLint);
	void setupWindow(int w, int h);

	void fireProjectile(shaderType projectileType, glm::vec3 originPoint, glm::vec3 directionFired);
	void removeProjectile(shaderType projectileType, int index);

	void spawnPickup(shaderType, glm::vec3);
	void removePickup(int);
	void spawnEnemy(shaderType enemyType);
	void respawnEnemies();
	void removeEnemy(shaderType enemyType, int index);

	void doGameLogic();
	void handleProjectiles(vector<Enemy*>&);
	void applyProjectile(Projectile*, int, vector<Enemy*>&);
	void handleMoveCollisions(glm::vec3, vector<Enemy*>&);
	void handlePlayerCollisions(vector<Enemy*>&); 
	void handlePickups();

	int findEnemyCollision(Projectile*, vector<Enemy*>&);

	void setupSprites();
	void handleHealthBar();
	void handleManaBar();
	void handleExpBar();

	bool isExpired(time_t spawnTime, float duration);

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
	bool firstTime, firstMouse;

	GameState currState;

	time_t startTime;

	GLFWwindow *window;
	GLFWmonitor *monitor;
	const GLFWvidmode *mode;
	static void cursor_position_callback(GLFWwindow* window, double currX, double currY);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void pollInput();
};

#endif 