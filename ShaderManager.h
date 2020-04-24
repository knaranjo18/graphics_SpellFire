/*  =================== File Information =================
	File Name: shaderManager.h
	Description:
	Author: Michael Shah
	Last Modified: 4/14/14

	Purpose: A Class for loading in shaders to use
	Usage:	

	Further Reading resources: 
	===================================================== */
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "ppm.h"

using namespace std;

class ShaderManager{

	public:

	ShaderManager();
	~ShaderManager();

	void resetShaders();
	void loadImage(string fileName, int layer);

	/*	===============================================
	Desc: 	Takes a file and essentially converts it into a 
			single string that can be compiled by OpenGL at runtime.
	Precondition: 
	Postcondition:
	=============================================== */ 
	void loadFile(const char* fileName, string& str);

	unsigned int loadShader(string& source, unsigned int mode);

	void initShader(const char* vertexShaderName, const char* fragmentShaderName);

	unsigned int getVertexShaderID() { return vertexShaderID; }
	unsigned int getFragmentShaderID() { return fragmentShaderID; }


	unsigned int layer1ID;
	unsigned int layer2ID;

		// When shaders get compiled, you can think of them as there own
		// program or library that gets shipped off to the GPU.
		// This integer holds that unique value.
		unsigned int program;


	private:
		// These are the unique id's of a vertex and fragment shader
		// that we load for our program.
		unsigned int vertexShaderID;
		unsigned int fragmentShaderID;

		// Stores images for our object in case we want to multi-texture it.
		// This limit could be increased in the future
		ppm* layer1; 
		ppm* layer2;


};


#endif