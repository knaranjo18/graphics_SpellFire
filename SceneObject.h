#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

#include <FL/glu.h>
#include <iostream>
#include <cmath>
#include <glm/gtc/constants.hpp>

#include "ppm.h"
#include "gfxDefs.h"
/*
	This object renders a piece of geometry ('a sphere by default')
	that has one texture that can be drawn on.

*/
class SceneObject {
	public:

		SceneObject(int _id);
		~SceneObject();

		GLuint loadTexture(int width, int height, char* pixels);

		void setTexture(int textureNumber,std::string _fileName);

		void drawTexturedSphere();

		void paintTexture(int x, int y, char r, char g, char b);

		
		int id;	// This is a unique id that we can reference our object by
		float radius; // default radius of our sphere object
				
		// The first texture image
		// This should be a white, black, pink, or other solid image that
		// we can draw on.
		ppm* baseTexture;

		// A second texture image that can be loaded
		// This is a second texture that we can blend onto our sphere
		// This is demonstrating multiple textures or 'multitexturing' as it
		// is called in the graphics world.
		ppm* blendTexture;

	private:
		// Create a texture id that uniquly identifies each texture image
		GLuint baseTextureID;
		GLuint blendTextureID;
};

#endif