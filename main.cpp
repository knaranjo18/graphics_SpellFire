#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "MyGLCanvas.h"

using namespace std;


// Makes the window and runs the game loop
int main(int argc, char **argv) {
	MyGLCanvas *myCanvas = new MyGLCanvas();
	myCanvas->run();
	return 0;
}