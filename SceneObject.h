/*  =================== File Information =================
	File Name: object.h
	Description:
	Author: Michael Shah
	Last Modified: 4/2/14

	Purpose: 
	Usage:	

	Further Reading resources: 
	===================================================== */
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
#include "ppm.h"

/*
	This object renders a piece of geometry ('a sphere by default')
	that has one texture that can be drawn on.

*/
class SceneObject {
	public:
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		SceneObject(int _id);
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */ 
		~SceneObject();
		/*	===============================================
		Desc:
		Precondition: 
		Postcondition:
		=============================================== */
		GLuint loadTexture(int width, int height, char* pixels);
		/*	===============================================
		Desc:	This instantiates an image to be rendered.
				
				If memory has already been allocated for this,
				then our contract is to delete the previous image,
				and overwrite it with the new one.
		Precondition: 
		Postcondition:
		=============================================== */ 
		void setTexture(int textureNumber,std::string _fileName);

		/*	===============================================
		Desc:	Draw the actual rendered spheres
		Precondition: 
		Postcondition:
		=============================================== */ 
		void drawTexturedSphere();
		/*	===============================================
		Desc:			Calls into this function modify a previously loaded ppm's
						color array.

						Note that this does NOT change the original ppm image at all.

						Also note, that a new OpenGL Texture is generated everytime, we
						paint on the texture.  There could probably be a buffer to improve
						the performance of this call.
		Precondition: 
		Postcondition:
		=============================================== */ 
		void paintTexture(int x, int y, char r, char g, char b);

		
		/*
			Object ID
		*/
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
		// I believe OpenGL will use these id's when we generate a texture
		// to uniquely identify which array of pixels to map to an image.
		GLuint baseTextureID;
		GLuint blendTextureID;

};

#endif