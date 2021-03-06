#include "ppm.h"
#include <FL/gl.h>

#define PRINT_STATS false

//Desc:	Default constructor for a ppm
ppm::ppm(std::string _fileName){
  /* Algorithm
      Step 1: Parse header of PPM
      Step 2: Read in colors into array
      Step 3: Allocate memory for width and height dimensions
  */

  // Open an input file stream for reading a file
  std::ifstream ppmFile(_fileName.c_str());
  // If our file successfully opens, begin to process it.
  if (ppmFile.is_open()) {
	  // line will store one line of input
	  std::string line;
	  // Our loop invariant is to continue reading input until
	  // we reach the end of the file and it reads in a NULL character
	  std::cout << "Reading in PPM file: " << _fileName << std::endl;
	  // Our delimeter pointer which is used to store a single token in a given
	  // string split up by some delimeter(s) specified in the strtok function
	  char* delimeter_pointer;
	  int iteration = 0;
	  int pos = 0;
	  while (getline(ppmFile, line)) {
		  char* copy = new char[line.length() + 1];
		  strcpy(copy, line.c_str());
		  delimeter_pointer = strtok(copy, " ");

		  if (copy[0] == '#') {
			  continue;
		  }

		  // Read in the magic number
		  if (iteration == 0) {
			  magicNumber = delimeter_pointer;
			  if (PRINT_STATS) std::cout << "Magic Number: " << magicNumber << " " << std::endl;
		  }
		  // Read in dimensions
		  else if (iteration == 1) {
			  width = atoi(delimeter_pointer);
			  delimeter_pointer = strtok(NULL, " ");
			  height = atoi(delimeter_pointer);
			  if (PRINT_STATS) 	std::cout << "width: " << width << " " << "height: " << height << std::endl;
			  // Allocate memory for the color array
			  if (width > 0 && height > 0) {
				  color = new char[width*height * 3];
				  if (color == NULL) {
					  std::cout << "Unable to allocate memory for ppm" << std::endl;
					  exit(1);
				  }
			  }
			  else {
				  std::cout << "PPM not parsed correctly, width and height dimensions are 0" << std::endl;
				  exit(1);
			  }
		  }
		  else if (iteration == 2) {
			  if (PRINT_STATS) std::cout << "color range: 0-" << delimeter_pointer << std::endl;

			  int num = width * height * 3;
			  for (int i = 0; i < num; i++) {
				  int value;
				  ppmFile >> value;
                  //std::cout << i << ": " << value << std::endl;
				  color[i] = value;
			  }
		  }
		  delete copy;
		  iteration++;
	  }
	  ppmFile.close();
	  std::cout << "PPM file parsed correctly: " << _fileName << std::endl;
  }
  else{
      std::cout << "Unable to open ppm file: " << _fileName << std::endl;
  } 
}

// Desc:	Default destructor for a ppm
ppm::~ppm(){
  if(color!=NULL){
    delete[] color;
    color=0;
  }	
}



// Desc: Sets a pixel in our array a specific color
void ppm::setPixel(int x, int y, int r, int g, int b){
  if(x > width || y > height){
    return;
  }
  else{
    color[(x*3)+height*(y*3)] = r;
    color[(x*3)+height*(y*3)+1] = g;
    color[(x*3)+height*(y*3)+2] = b;
  }
}


// Turns ppm file into a texture that can be used with shaders
unsigned int ppm::createAsTexture() {
	unsigned int textureNum;
	
	glGenTextures(1, &textureNum);
	glBindTexture(GL_TEXTURE_2D, textureNum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color);
	
	return textureNum;
}
