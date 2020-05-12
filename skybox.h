#ifndef SKYBOX_H
#define SKYBOX_H

#include "ShaderManager.h"
#include "ppm.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <math.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

using namespace std;

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

// This Skybox was built by following the cubemap tutorial at 
// https://learnopengl.com/Advanced-OpenGL/Cubemaps
// Thanks alot to whoever wrote this it seems real good!

#define N_FLOATS 108

class Skybox {
public:
	Skybox();
	Skybox(string[6]);
	~Skybox();

	void applyTexture(string[6]);

    void draw(glm::mat4, glm::mat4);

	unsigned int getTextureID();

	ShaderManager* shader;

	GLuint vao;

private:
    void bindVBO(unsigned int programID);
    void setupShader();

	ppm* texture[6];
	unsigned int tex_id;

    GLuint vertexVBO_id, indicesVBO_id;



    GLfloat skyboxVertices[N_FLOATS] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GLint indices_vao[36];
};


#endif
