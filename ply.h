#ifndef PLY_H
#define PLY_H

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <math.h>

#include "geometry.h"
#include "ppm.h"

using namespace std;

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

using namespace std;

/*  ============== ply ==============
	Purpose: Load a PLY File

	Note that the ply file inherits from a base class called 'entity'
	This class stores common transformations that can be applied to 3D entities(such as mesh files, lights, or cameras)

	Example usage: 

	1.) ply* myPLY = new ply (filenamePath);
	2.) myPLY->render();
	3.) delete myPLY;

	==================================== */ 
class ply {
public:
		ply();
		ply(string filePath);
		~ply();

		void reload(string _filePath);
		void buildArrays();
		void bindVBO(unsigned int programID);
		void renderVBO();
		
		void printAttributes();
		void printVertexList();
		void printFaceList();

		void applyTexture(string);
		unsigned int getTextureID();

		GLuint vao;

private:
		void loadGeometry();
		void scaleAndCenter();
		void computeNormal(float x1, float y1, float z1,
			float x2, float y2, float z2,
			float x3, float y3, float z3,
			float* outputx, float* outputy, float*outputz);


		string filePath;
		int vertexCount, faceCount, properties;

		vertex* vertexList;
		face* faceList;

		// Id for Vertex Buffer Object
		GLuint vertexVBO_id, indicesVBO_id, normalVBO_id, texVBO_id;
		
		// Special arrays that are used for vertex buffer objects
		GLfloat* vertex_vao;
		GLuint* indicies_vao;
		GLfloat* normals_vao;
		GLfloat* tex_coords_vao;

		ppm* texture;
		unsigned int tex_id;
};

#endif