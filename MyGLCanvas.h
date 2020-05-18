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
#include <list>
#include <time.h>
#include <irrKlang/irrKlang.h>

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

using namespace irrklang;


class MyGLCanvas {
public:
	MyGLCanvas();
	~MyGLCanvas();

	void run();

private:
	void draw();
	void drawScene();
	void drawDeathScene();
	void drawLoading();
	void drawMainMenu();
	
	void setupSound();
	void setupShaders();
	void setupSprites();
	void setupCursors();
	void restartGame();
	
	void updateCamera(int width, int height);
	void deallocate();
	void enforceFrameTime(GLint);
	void setupWindow(int w, int h);

	void fireProjectile(shaderType projectileType, glm::vec3 originPoint, glm::vec3 directionFired);
	void removeProjectile(list<Projectile *>::iterator &it);

	void spawnPickup(shaderType, glm::vec3);
	void removePickup(list<Pickup *>::iterator &it);

	void spawnEnemy(shaderType enemyType);
	void respawnEnemies();
	void removeEnemy(list<Enemy *>::iterator &it);

	void handleProjectiles();
	void applyProjectile(Projectile*, list<Enemy *>::iterator it);
	void handleMoveCollisions(glm::vec3);
	void handlePlayerCollisions(); 
	void handleEnemySound();
	list<Enemy *>::iterator findEnemyCollision(Projectile*);

	void handlePickups();

	void doGameLogic();

	void handleHealthBar();
	void handleManaBar();
	void handleExpBar();

	bool isExpired(time_t spawnTime, float duration);

	vector<ShaderManager *> shaderList;
	vector<ply *> plyList;
	vector<Sprite *> crossHair;
	vector<Sprite *> healthBar;
	vector<Sprite *> manaBar;
	vector<Sprite *> expBar;
	vector<Sprite *> deathScreen;
	vector<Sprite *> mainMenu;
	Sprite *loadingScreen;


	list<Projectile *> projectileList;
	list<Enemy *> enemyList;
	list<Pickup*> pickupList;

	int numJad, numBlob, numManaPot, numHealthPot, numFireball;

	Player *player;
	Scenery *arena;
	Skybox* skybox;
	ISoundEngine *soundEngine;
	ISound *music;

	glm::vec3 lightPos;
	int prevX, prevY;
	bool firstTime, firstMouse, fullscreen, cursorVisible, firstDeath;

	GameState currState;

	time_t startTime;

	GLFWwindow *window;
	GLFWmonitor *monitor;
	const GLFWvidmode *mode;
	GLFWcursor *regular;
	GLFWcursor *hover;
	static void cursor_position_callback(GLFWwindow* window, double currX, double currY);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void pollInput();
	void toggleFullScreen();
	void toggleCursor();

	void stopSound(ISound *sound);
};


#endif 