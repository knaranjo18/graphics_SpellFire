#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>

#include "MyGLCanvas.h"

using namespace std;

class MyAppWindow;
MyAppWindow *win;

class MyAppWindow : public Fl_Window {
public:
	MyGLCanvas* canvas;

public:
	MyAppWindow(int W, int H, const char*L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}
};


MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	canvas = new MyGLCanvas(0, 0, w(), h());
}


/*---------------------------- main() ------------------------*/
// Makes the window and runs the game loop
int main(int argc, char **argv) {
	win = new MyAppWindow(600, 600, "Spell Fire");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	win->fullscreen();
	
	return(Fl::run());
}