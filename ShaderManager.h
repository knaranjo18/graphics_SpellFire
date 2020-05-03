#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <FL/glut.h>
#include <FL/glu.h>

#include "ppm.h"

using namespace std;

class ShaderManager{
public:
	ShaderManager();
	~ShaderManager();

	void resetShaders();
	
	void loadImage(string fileName, int layer);
	void loadFile(const char* fileName, string& str);
	unsigned int loadShader(string& source, unsigned int mode);

	void initShader(const char* vertexShaderName, const char* fragmentShaderName);
	void useShader();

	unsigned int getVertexShaderID() { return vertexShaderID; }
	unsigned int getFragmentShaderID() { return fragmentShaderID; }

	unsigned int layer1ID;
	unsigned int layer2ID;
	unsigned int program;

private:
		unsigned int vertexShaderID;
		unsigned int fragmentShaderID;

		// Stores images for our object in case we want to multi-texture it.
		ppm* layer1; 
		ppm* layer2;
};
#endif