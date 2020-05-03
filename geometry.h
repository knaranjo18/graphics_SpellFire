#ifndef GEOMETRY_H
#define GEOMETRY_H

/*  ============== Vertex ==============
	Purpose: Stores properties of each vertex
	Use: Each vertex is used in face structure.

	Notes: You may not have all of the data available
	below.  In this case, you would want to make an
	optimization, and only store (x,y,z) cooridinates for example.
	==================================== */  
struct vertex{
	// position in 3D space
	float x,y,z;		

	float confidence, intensity;

	// Color values
	float r,g,b;	

	// surface normal values	
	float nx,ny,nz;		

	// texture coordinates
	float u,v,w;		
};

/*  ============== Face ==============
	Purpose: Store list of vertices that make up a polygon.
			In modern versions of OpenGL this value will always be 3(a triangle)
	Use: Used in Shape data structure.
	==================================== */  
struct face{
	// The number of vertices that make up a single face.
	// A face can have anywhere from 3 to n vertices.
	int vertexCount;
	// Stores an index(integer value) list of vertices. See struct 'vertex' for more information
	int* vertexList;

	// Default constructor
	face(){
		vertexCount = 0;
		vertexList = NULL;
	}
};

#endif