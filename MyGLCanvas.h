#pragma once

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
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>

#include "SceneObject.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include "Camera.h"


class MyGLCanvas : public Fl_Gl_Window {
public:
	glm::vec3 lightPos;

	Camera* camera;

	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();

	void reloadShaders();

private:
	void draw();
	void drawScene();

	void initShaders();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	SceneObject* myObject;
	ShaderManager* myShaderManager;
	ply* myPLY;
	bool firstTime;

	int prevX, prevY, rotY, rotX;
	bool moveOn;
};

#endif // !MYGLCANVAS_H