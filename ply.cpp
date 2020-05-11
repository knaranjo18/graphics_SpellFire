#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include "ply.h"
#include <math.h>


using namespace std;


//Desc: Default constructor for a ply object
ply::ply(){
	vertexList = NULL;
	faceList = NULL;
	properties = 0;
	faceCount = 0;
	vertexCount = 0;
	texture = NULL;
	tex_id = -1;
}


//Desc: constructor for a ply object with a default path
ply::ply(string filePath) {
	vertexList = NULL;
	faceList = NULL;
	faceCount = 0;
	vertexCount = 0;
	properties = 0;
	reload(filePath);
}


//Desc: Destructor for a ply object
ply::~ply(){
	// Delete the allocated arrays
	if (vertexList != NULL)
		delete[] vertexList;

	for (int i = 0; i < faceCount; i++) {
		delete[] faceList[i].vertexList;
	}

	if (faceList != NULL)
		delete[] faceList;
	// Set pointers to NULL
	vertexList = NULL;
	faceList = NULL;


	if (vertex_vao != NULL) {
		delete[] vertex_vao;
		vertex_vao = NULL;
	}

	if (indicies_vao != NULL) {
		delete[] indicies_vao;
		indicies_vao = NULL;
	}

	if (normals_vao != NULL) {
		delete[] normals_vao;
		normals_vao = NULL;
	}

	// Destroy our vertex buffer object
	glDeleteBuffers(1, &vertexVBO_id);
	glDeleteBuffers(1, &indicesVBO_id);
	glDeleteBuffers(1, &normalVBO_id);
}


//Desc: reloads the geometry for a 3D object
void ply::reload(string _filePath){
	filePath = _filePath;

	// reclaim memory allocated in each array
	if (vertexList != NULL)
		delete[] vertexList;
	for (int i = 0; i < faceCount; i++) {
		delete[] faceList[i].vertexList;
	}

	if (faceList != NULL)
		delete[] faceList;

	vertexList = NULL;
	faceList = NULL;
	faceCount = 0;
	vertexCount = 0;

	// Call our function again to load new vertex and face information.
	loadGeometry();
}


// Reads a ply file and extracts the vertex and face information
void ply::loadGeometry(){
	ifstream myfile(filePath.c_str()); // load the file
	if (myfile.is_open()) { // if the file is accessable
		properties = -2; // set the properties because there are extras labeled

		string line;
		char * token_pointer;
		char * lineCopy = new char[256];
		int count;
		bool reading_header = true;
		// loop for reading the header 
		while (reading_header && getline(myfile, line)) {

			// get the first token in the line, this will determine which
			// action to take. 
			strcpy(lineCopy, line.c_str());
			token_pointer = strtok(lineCopy, " \r");
			// case when the element label is spotted:
			if (strcmp(token_pointer, "element") == 0){
				token_pointer = strtok(NULL, " \r");

				// When the vertex token is spotted read in the next token
				// and use it to set the vertexCount and initialize vertexList
				if (strcmp(token_pointer, "vertex") == 0){
					token_pointer = strtok(NULL, "  \r");
					vertexCount = atoi(token_pointer);
					vertexList = new vertex[vertexCount];
				}

				// When the face label is spotted read in the next token and 
				// use it to set the faceCount and initialize faceList.
				if (strcmp(token_pointer, "face") == 0){
					token_pointer = strtok(NULL, "  \r");
					faceCount = atoi(token_pointer);
					faceList = new face[faceCount];
				}
			}
			// if property label increment the number of properties.
			if (strcmp(token_pointer, "property") == 0) { properties++; }
			// if end_header break the header loop and move to reading vertices.
			if (strcmp(token_pointer, "end_header") == 0) { reading_header = false; }
		}

		// Read in exactly vertexCount number of lines after reading the header
		// and set the appropriate vertex in the vertexList.
		for (int i = 0; i < vertexCount; i++){

			getline(myfile, line);
			strcpy(lineCopy, line.c_str());

			// by convention the first three are x, y, z
			if (properties >= 0){
				vertexList[i].x = atof(strtok(lineCopy, " \r"));
			}
			if (properties >= 1){
				vertexList[i].y = atof(strtok(NULL, " \r"));
			}
			if (properties >= 2){
				vertexList[i].z = atof(strtok(NULL, " \r"));
			}
			// next 3 should be normals x, y, z
			if (properties >= 3) {
				vertexList[i].nx = atof(strtok(NULL, " \r"));
			}
			if (properties >= 4) {
				vertexList[i].ny = atof(strtok(NULL, " \r"));
			}
			if (properties >= 5) {
				vertexList[i].nz = atof(strtok(NULL, " \r"));
			}
			// next we expect texture u, v, then ignore the rest
			if (properties >= 6) {
				vertexList[i].u = atof(strtok(NULL, " \r"));
			}
			if (properties >= 7) {
				vertexList[i].v = atof(strtok(NULL, " \r"));
			}
		}

		// Read in the faces (exactly faceCount number of lines) and set the 
		// appropriate face in the faceList
		for (int i = 0; i < faceCount; i++){
			getline(myfile, line);

			strcpy(lineCopy, line.c_str());
			count = atoi(strtok(lineCopy, " \r"));
			faceList[i].vertexCount = count; // number of vertices stored 
			faceList[i].vertexList = new int[count]; // initialize the vertices

			// set the vertices from the input, reading only the number of 
			// vertices that are specified
			for (int j = 0; j < count; j++){
				faceList[i].vertexList[j] = atoi(strtok(NULL, " \r"));
			}
		}
		delete(lineCopy);
		myfile.close();
		scaleAndCenter();
	    cout << "completed loading: " << filePath.c_str() << "\n";
	}
	// if the path is invalid, report then exit.
	else {
	        cout << "cannot open file " << filePath.c_str() << "\n";
	}
};


//Desc: Moves all the geometry so that the object is centered at 0, 0, 0 and scaled to be between 0.5 and -0.5
void ply::scaleAndCenter() {
	float avrg_x = 0.0;
	float avrg_y = 0.0;
	float avrg_z = 0.0;
	float max = 0.0;
	int i;

	//loop through each vertex in the given image
	for (i = 0; i < vertexCount; i++){
		// obtain the total for each property of the vertex
		avrg_x += vertexList[i].x;
		avrg_y += vertexList[i].y;
		avrg_z += vertexList[i].z;
	}

	// compute the average for each property
	avrg_x = avrg_x / vertexCount;
	avrg_y = avrg_y / vertexCount;
	avrg_z = avrg_z / vertexCount;

	// center each vertex 
	for (i = 0; i < vertexCount; i++){
		vertexList[i].x = (vertexList[i].x - avrg_x);
		vertexList[i].y = (vertexList[i].y - avrg_y);
		vertexList[i].z = (vertexList[i].z - avrg_z);
	}

	//find the range of the vertices
	for (i = 0; i < vertexCount; i++){
		// obtain the max dimension to find the furthest point from 0,0
		if (max < fabs(vertexList[i].x)) max = fabs(vertexList[i].x);
		if (max < fabs(vertexList[i].y))  max = fabs(vertexList[i].y);
		if (max < fabs(vertexList[i].z)) max = fabs(vertexList[i].z);
	}

	//max is doubled so that the range we get is from 0.5 to -0.5
	max *= 2.0f;

	//scale each vertex to fit within the bounds
	for (i = 0; i < vertexCount; i++){
		vertexList[i].x = vertexList[i].x / max;
		vertexList[i].y = vertexList[i].y / max;
		vertexList[i].z = vertexList[i].z / max;
	}
}

/*Desc: Prints some statistics about the file you have read in
This is useful for debugging information to see if we parse our file correctly.*/
void ply::printAttributes(){
	cout << "==== ply Mesh Attributes=====" << endl;
	cout << "vertex count:" << vertexCount << endl;
	cout << "face count:" << faceCount << endl;
	cout << "properties:" << properties << endl;
}


//Desc: Iterate through our array and print out each vertex.
void ply::printVertexList(){
	if (vertexList == NULL){
		return;
	}
	else{
		for (int i = 0; i < vertexCount; i++){
			cout << vertexList[i].x << "," << vertexList[i].y << "," << vertexList[i].z << endl;
		}
	}
}


//Desc: Iterate through our array and print out each face.
void ply::printFaceList(){
	if (faceList == NULL){
		return;
	}
	else{
		// For each of our faces
		for (int i = 0; i < faceCount; i++){
			// Get the vertices that make up each face from the face list
			for (int j = 0; j < faceList[i].vertexCount; j++){
				// Print out the vertex
				int index = faceList[i].vertexList[j];
				cout << vertexList[index].x << "," << vertexList[index].y << "," << vertexList[index].z << endl;
			}
		}
	}
}

void ply::applyTexture(string filename) {
	texture = new ppm(filename);
	tex_id = texture->createAsTexture();
}

unsigned int ply::getTextureID() {
	if (texture) {
		return tex_id;
	} else {
		fprintf(stderr, "texture not initialized, cannot return id!");
		exit(1);
	}
}

//Desc:   Finds the normal to three vertices(1 face making up a triangle)
void ply::computeNormal(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float* outputx, float* outputy, float*outputz)
{

	float v1x, v1y, v1z;
	float v2x, v2y, v2z;
	float cx, cy, cz;

	//find vector between x2 and x1
	v1x = x1 - x2;
	v1y = y1 - y2;
	v1z = z1 - z2;

	//find vector between x3 and x2
	v2x = x2 - x3;
	v2y = y2 - y3;
	v2z = z2 - z3;

	//cross product v1xv2

	cx = v1y * v2z - v1z * v2y;
	cy = v1z * v2x - v1x * v2z;
	cz = v1x * v2y - v1y * v2x;

	//normalize

	float length = sqrt(cx * cx + cy * cy + cz * cz);
	cx = cx / length;
	cy = cy / length;
	cz = cz / length;

	*outputx = cx;
	*outputy = cy;
	*outputz = cz;
}


/*Desc: Simple function that builds all of the arrays to be used
		in vertex buffer objects. */
void ply::buildArrays() {
	// allocate memory for our arrays
	vertex_vao = new GLfloat[vertexCount * 3];
	if (vertex_vao == NULL) {
		cout << "Ran out of memory(vertex_vao)!" << endl;
		return;
	}

	indicies_vao = new GLuint[faceCount * 3];
	if (indicies_vao == NULL) {
		cout << "Ran out of memory(indicies_vao)!" << endl;
		return;
	}

	normals_vao = new GLfloat[vertexCount * 3];
	if (normals_vao == NULL) {
		cout << "Ran out of memory(normals_vao)!" << endl;
		return;
	}
	for (int i = 0; i < vertexCount * 3; i++) {
		normals_vao[i] = 0.0;
	}

	int* numNormals = new int[vertexCount];
	if (numNormals == NULL) {
		cout << "Ran out of memory(normal Counter)!" << endl;
		return;
	}
	for (int i = 0; i < vertexCount; i++) {
		numNormals[i] = 0;
	}
	tex_coords_vao = new GLfloat[vertexCount * 2];
	if (tex_coords_vao == NULL) {
		cout << "Ran out of memory(tex_coords_vao)!" << endl;
		return;
	}
	// initialize texture coordinates in the case we don't have them so shader can tell not to use them
	for (int i = 0; i < vertexCount * 2; i++) {
		tex_coords_vao[i] = NAN;
	}


	// Compress all of the vertices into a simple array
	if (vertexList == NULL) {
		return;
	}
	else {
		for (int i = 0; i < vertexCount; i++) {
			vertex_vao[i * 3 + 0] = vertexList[i].x;
			vertex_vao[i * 3 + 1] = vertexList[i].y;
			vertex_vao[i * 3 + 2] = vertexList[i].z;
		}
	}

	// Compress everything into one array of indices
	unsigned int k = 0;
	if (faceList == NULL) {
		return;
	}
	else {
		// For each of our faces
		for (int i = 0; i < faceCount; i++) {
			// Get the vertices that make up each face from the face list
			int index0 = faceList[i].vertexList[0];
			int index1 = faceList[i].vertexList[1];
			int index2 = faceList[i].vertexList[2];

			indicies_vao[k] = index0;
			indicies_vao[k + 1] = index1;
			indicies_vao[k + 2] = index2;

			k += 3;
		}
	}

	// if we did not read normals we should calculate them
	if (properties <= 3) {
		/*This part is for building the vertex normals -- it looks complicated because
		// we are computing the "smooth" normals of a vertex, which is defined as the "average"
		// of all the normals at a vertex.
		//That is, let's say that a vertex is at the intersection of 5 triangles, each triangle
		// will have its own normal (computed using the cross product).
		//To determine the "smooth" normal at that vertex, we add up all 5 triangle normals and then
		// divide by 5.
		//In order to do this, there are two steps:
		//  1. we sum up all the normals.
		//  2. While we are doing that, we need to keep a counter for the number of normals at a vertex */
		for (int i = 0; i < faceCount * 3; i = i + 3) {
			int index0 = indicies_vao[i];
			int index1 = indicies_vao[i + 1];
			int index2 = indicies_vao[i + 2];

			float outputx, outputy, outputz;
			// using the setNormal function from below we normalize the vectors
			computeNormal(
				vertex_vao[index0 * 3 + 0], vertex_vao[index0 * 3 + 1], vertex_vao[index0 * 3 + 2],
				vertex_vao[index1 * 3 + 0], vertex_vao[index1 * 3 + 1], vertex_vao[index1 * 3 + 2],
				vertex_vao[index2 * 3 + 0], vertex_vao[index2 * 3 + 1], vertex_vao[index2 * 3 + 2],
				&outputx, &outputy, &outputz);

			numNormals[index0]++;
			numNormals[index1]++;
			numNormals[index2]++;

			normals_vao[index0 * 3 + 0] += outputx;
			normals_vao[index0 * 3 + 1] += outputy;
			normals_vao[index0 * 3 + 2] += outputz;

			normals_vao[index1 * 3 + 0] += outputx;
			normals_vao[index1 * 3 + 1] += outputy;
			normals_vao[index1 * 3 + 2] += outputz;

			normals_vao[index2 * 3 + 0] += outputx;
			normals_vao[index2 * 3 + 1] += outputy;
			normals_vao[index2 * 3 + 2] += outputz;
		}
		for (int i = 0; i < vertexCount; i++) {
			normals_vao[i * 3 + 0] = normals_vao[i * 3 + 0] / (float)(numNormals[i]);
			normals_vao[i * 3 + 1] = normals_vao[i * 3 + 1] / (float)(numNormals[i]);
			normals_vao[i * 3 + 2] = normals_vao[i * 3 + 2] / (float)(numNormals[i]);
		}
	} else { // populate normals with read-in values
		for (int i = 0; i < vertexCount; i++) {
			normals_vao[i * 3 + 0] = vertexList[i].nx;
			normals_vao[i * 3 + 1] = vertexList[i].ny;
			normals_vao[i * 3 + 2] = vertexList[i].nz;
		}
	}

	// if we have texture coordinates to encode
	if (properties >= 6) {
		for (int i = 0; i < vertexCount; i++) {
			tex_coords_vao[i * 2 + 0] = vertexList[i].u;
			tex_coords_vao[i * 2 + 1] = vertexList[i].v;
		}
	}

	delete[] numNormals;
}

void ply::bindVBO(unsigned int programID){

	// Use a Vertex Array Object -- think of this as a single ID that sums up all the following VBOs
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate a buffer that will be sent to the video memory
	// For each ply object, it will get its own vertex buffer object, but it
	// would be much more efficient to put everything into one Vertex Buffer Object and send it over
	// only once to video memory. 

	// Note: If this seg faults, then Glee or Glew (however OpenGL 2.0 extensions are mangaged)
	// has not yet been initialized.
	//tell openGL to generate a new VBO object
	glGenBuffers(1, &vertexVBO_id);
	// Once we know how many buffers to generate, then hook up the buffer to the vertexVBO_id.
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_id);
	// Now we finally copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount * 3, vertex_vao, GL_STATIC_DRAW);


	//tell openGL to generate a new VBO object
	glGenBuffers(1, &indicesVBO_id);
	// Transfer the data from indices to a VBO indicesVBO_id
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO_id);
	// Copy data into the buffer object. Note the keyword difference here -- GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*faceCount * 3, indicies_vao, GL_STATIC_DRAW);
	// Get the location of the attributes that enters in the vertex shader
	GLint position_attribute = glGetAttribLocation(programID, "myPosition");
	// Specify how the data for position can be accessed
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Enable the attribute
	glEnableVertexAttribArray(position_attribute);


	//Repeat the process as building a vertexVBO, but this time for the normals
	glGenBuffers(1, &normalVBO_id);
	// bind the newly generated buffer to the normalVBO_id.
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO_id);
	// Now we finally copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount * 3, normals_vao, GL_STATIC_DRAW);
	// Get the location of the attributes that enters in the vertex shader
	GLint normal_attribute = glGetAttribLocation(programID, "myNormal");
	// Specify how the data for position can be accessed
	glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_TRUE, 0, 0);
	// Enable the attribute
	glEnableVertexAttribArray(normal_attribute);

	//Build a VBO for texture coordinates
	glGenBuffers(1, &texVBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 2, tex_coords_vao, GL_STATIC_DRAW);
	GLint tex_coord_attribute = glGetAttribLocation(programID, "myTextureCoordinate");
	glVertexAttribPointer(tex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(tex_coord_attribute);

	cout << "Created vbo successfully" << endl;
}

void ply::renderVBO() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
}

void ply::buildArraysSprite() {
	// allocate memory for our arrays
	vertex_vao = new GLfloat[vertexCount * 3];
	if (vertex_vao == NULL) {
		cout << "Ran out of memory(vertex_vao)!" << endl;
		return;
	}

	indicies_vao = new GLuint[faceCount * 3];
	if (indicies_vao == NULL) {
		cout << "Ran out of memory(indicies_vao)!" << endl;
		return;
	}

	tex_coords_vao = new GLfloat[vertexCount * 2];
	if (tex_coords_vao == NULL) {
		cout << "Ran out of memory(tex_coords_vao)!" << endl;
		return;
	}
	// initialize texture coordinates in the case we don't have them so shader can tell not to use them
	for (int i = 0; i < vertexCount * 2; i++) {
		tex_coords_vao[i] = NAN;
	}

	// Compress all of the vertices into a simple array
	if (vertexList == NULL) {
		return;
	}
	else {
		for (int i = 0; i < vertexCount; i++) {
			vertex_vao[i * 3 + 0] = vertexList[i].x;
			vertex_vao[i * 3 + 1] = vertexList[i].y;
			vertex_vao[i * 3 + 2] = vertexList[i].z;
		}
	}

	// Compress everything into one array of indices
	unsigned int k = 0;
	if (faceList == NULL) {
		return;
	}
	else {
		// For each of our faces
		for (int i = 0; i < faceCount; i++) {
			// Get the vertices that make up each face from the face list
			int index0 = faceList[i].vertexList[0];
			int index1 = faceList[i].vertexList[1];
			int index2 = faceList[i].vertexList[2];

			indicies_vao[k] = index0;
			indicies_vao[k + 1] = index1;
			indicies_vao[k + 2] = index2;

			k += 3;
		}
	}

	// if we have texture coordinates to encode
	if (properties >= 6) {
		for (int i = 0; i < vertexCount; i++) {
			tex_coords_vao[i * 2 + 0] = vertexList[i].u;
			tex_coords_vao[i * 2 + 1] = vertexList[i].v;
		}
	}
}

void ply::bindVBOsprites(unsigned int programID) {
	// Use a Vertex Array Object -- think of this as a single ID that sums up all the following VBOs
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate a buffer that will be sent to the video memory
	// For each ply object, it will get its own vertex buffer object, but it
	// would be much more efficient to put everything into one Vertex Buffer Object and send it over
	// only once to video memory. 

	// Note: If this seg faults, then Glee or Glew (however OpenGL 2.0 extensions are mangaged)
	// has not yet been initialized.
	//tell openGL to generate a new VBO object
	glGenBuffers(1, &vertexVBO_id);
	// Once we know how many buffers to generate, then hook up the buffer to the vertexVBO_id.
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_id);
	// Now we finally copy data into the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexCount * 3, vertex_vao, GL_STATIC_DRAW);


	//tell openGL to generate a new VBO object
	glGenBuffers(1, &indicesVBO_id);
	// Transfer the data from indices to a VBO indicesVBO_id
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO_id);
	// Copy data into the buffer object. Note the keyword difference here -- GL_ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*faceCount * 3, indicies_vao, GL_STATIC_DRAW);
	// Get the location of the attributes that enters in the vertex shader
	GLint position_attribute = glGetAttribLocation(programID, "myPosition");
	// Specify how the data for position can be accessed
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Enable the attribute
	glEnableVertexAttribArray(position_attribute);

	glGenBuffers(1, &texVBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 2, tex_coords_vao, GL_STATIC_DRAW);
	GLint tex_coord_attribute = glGetAttribLocation(programID, "myTextureCoordinate");
	glVertexAttribPointer(tex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(tex_coord_attribute);
 
	cout << "Created vbo successfully" << endl;
}