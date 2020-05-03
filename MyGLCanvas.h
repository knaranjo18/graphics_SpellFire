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
	void initShaders();
	
	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);
	
	void moveSight();

	Player *player;
	ShaderManager* shader1;
	ShaderManager* shader2;
	ply* myPLY1;
	ply* myPLY2;
	
	glm::vec3 enemyPos, enemyLook, lightPos, lightDir;
	int prevX, prevY;
	float enemySpeed;
	bool firstTime;
};

#endif 