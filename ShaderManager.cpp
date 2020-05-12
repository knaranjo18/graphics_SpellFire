#include "ShaderManager.h"

ShaderManager::ShaderManager(){
		// Return the version of OpenGL you are running.
		program = 0;
		layer1 = NULL;
		layer2 = NULL;
}


ShaderManager::~ShaderManager(){
	resetShaders();
	// Delete each image if we allocated memory for it.
	if (layer1 != NULL) {
		delete layer1;
	}
	if (layer2 != NULL) {
		delete layer2;
	}
}


void ShaderManager::resetShaders() {
	// Delete the two shaders we compiled
	glDetachShader(program, vertexShaderID);
	glDetachShader(program, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glDeleteProgram(program);
}

/*Loads in a ppm file as a texture*/
void ShaderManager::loadImage(string fileName, int layer){
	if(layer <= 0){
		layer1 = new ppm(fileName);
		layer1ID = layer1->createAsTexture();
	}
	else if(layer == 1){
		layer2 = new ppm(fileName);
		layer2ID = layer2->createAsTexture();
	}
	else{
		cout << "Invalid layer -- Your video card may not support this many textures" << endl;
	}
}

/*Turns a shader file into a string so that it can be compiled*/
void ShaderManager::loadFile(const char* fileName, string& str){
	ifstream inputFile(fileName);
	// If our file does not open, print out an error
	// otherwise process it.
	if(!inputFile.is_open()){
		cout << "The file " << fileName << " cannot be open." << endl;
		return;
	}
	// Process a line of characters with a max size of 300 characters in a line.
	const int lineSize = 300;
	char buffer[lineSize];
	cout << "Processing shader file: " << fileName << endl;
	cout << "========================" << endl;
 	// Process each line of the shader and store it in a string.
	// Remember, that shaders are just strings that get compiled
	// at the runtime to be run within our program.
	// Because they get compiled at runtime, we do not need to compile
	// the rest of our program either!
	while(!inputFile.eof()){
		inputFile.getline(buffer,lineSize);
		str+=buffer;
		str+='\n';
	}
}


/*Desc: The source is a string that has been loaded from our
		loadFile function, or otherwise it is a complete OpenGL shader
		that has been compressed into a single string.

		The mode tells us if it is a vertex or fragment shader.
		The unsigned int values for mode are GL_VERTEX_SHADER or GL_FRAGMENT_SHADER

		The id that is returned, is a unique id that openGL assigns from
		the glCreateShader command.  OpenGL needs this in order to reference
		the correct shader.
		*/
unsigned int ShaderManager::loadShader(string& source, unsigned int mode){
	unsigned int id;
	id = glCreateShader(mode);

	const char* csource = source.c_str();

	/* The command 'glShaderSource' takes in the parameters in the order below.
	 shader - the handler to the shader
	 numOfStrings - Number of strings in the array
	 strings - the array of strings
	 lenOfStrings - An array with the length of each string, or NULL, if the strings
					  are NULL terminated. */
	glShaderSource(id, 1, &csource, NULL);

	// Once we have our shader, we have to compile it at runtime
	glCompileShader(id);

	// In case there are errors, we need to output them to the console.
	// Allocate memory for a buffer to hold error messages.
	char error[1024];
	glGetShaderInfoLog(id, 1024, NULL, error);
	cout << "Compiler errors: " << endl << error << endl;

	return id;
}

/*Initialize the vertex and fragment shaders*/
void ShaderManager::initShader(const char* vertexShaderName, const char* fragmentShaderName){
	string source;
	
	loadFile(vertexShaderName, source);

	// Compile the shader and assign it the unique id of our vertexShaderID
	vertexShaderID = loadShader(source, GL_VERTEX_SHADER);

	// We then repeat the process for the fragment shader
	// Load our source for a fragment shader
	// Because source is a reference, we need to clear it with the previous vertexShader
	// strings loaded from the previous 'loadFile' call.
	source="";
	loadFile(fragmentShaderName, source);
	fragmentShaderID = loadShader(source, GL_FRAGMENT_SHADER);

	// Now that we've compiled the actual shaders, we add them to our program.
	// We can create as many programs as we want and then switch between them.
	program = glCreateProgram();
	glAttachShader(program, vertexShaderID);
	glAttachShader(program, fragmentShaderID);

	// Finally we link them, this is the same as the linking process for C++
	// with a library file
	glLinkProgram(program);
	useShader();
}

void ShaderManager::useShader() {
	glUseProgram(program);
}