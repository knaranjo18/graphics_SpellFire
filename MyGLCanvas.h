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


class MyGLCanvas : public Fl_Gl_Window {
public:
	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();

private:
	void draw();
	void drawScene();
	
	void setupShaders();
	
	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);
	void deallocate();
	
	void moveSight();

	void spawnEnemy(shaderType enemyType);
	void removeEnemy(shaderType enemyType, int index);

	vector<ShaderManager *> shaderList;
	vector<Enemy *> cowList, bunnyList;
	vector<ply *> plyList;

	Player *player;
	Scenery *arena;

	glm::vec3 lightPos;
	int prevX, prevY;
	bool firstTime;
	time_t startTime;
};

#endif 