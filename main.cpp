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
	Fl_Slider* rotUSlider;
	Fl_Slider* rotVSlider;
	Fl_Slider* rotWSlider;

	Fl_Button* reloadButton;
	Fl_Button* animateLightButton;

	MyGLCanvas* canvas;

public:
	// APP WINDOW CONSTRUCTOR
	MyAppWindow(int W, int H, const char*L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}

private:
	// Someone changed one of the sliders
	static void rotateCB(Fl_Widget* w, void* userdata) {
		float value = ((Fl_Slider*)w)->value();
		*((float*)userdata) = value;
	}

	static void toggleCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Button*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
	}

	static void reloadCB(Fl_Widget* w, void* userdata) {
		win->canvas->reloadShaders();
	}
/*
	static void cameraRotateCB(Fl_Widget* w, void* userdata) {
		win->canvas->camera->setRotUVW(win->rotUSlider->value(), win->rotVSlider->value(), win->rotWSlider->value());
		glm::vec3 lookV = win->canvas->camera->getLookVector();
		lookV = glm::normalize(lookV);
	}
*/
};


MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	begin();
	// OpenGL window

	canvas = new MyGLCanvas(10, 10, w() - 110, h() - 20);
/*
	Fl_Pack* pack = new Fl_Pack(w() - 100, 30, 100, h(), "Control Panel");
	pack->box(FL_DOWN_FRAME);
	pack->labelfont(1);
	pack->type(Fl_Pack::VERTICAL);
	pack->spacing(0);
	pack->begin();
	*/
/*
	Fl_Box *rotUTextBox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateU");
	rotUSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotUSlider->align(FL_ALIGN_TOP);
	rotUSlider->type(FL_HOR_SLIDER);
	rotUSlider->bounds(-179, 179);
	rotUSlider->step(1);
	rotUSlider->value(canvas->camera->rotU);
	rotUSlider->callback(cameraRotateCB);

	Fl_Box *rotVTextBox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateV");
	rotVSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotVSlider->align(FL_ALIGN_TOP);
	rotVSlider->type(FL_HOR_SLIDER);
	rotVSlider->bounds(-720, 720);
	rotVSlider->step(1);
	rotVSlider->value(canvas->camera->rotV);
	rotVSlider->callback(cameraRotateCB);

	Fl_Box *rotWTextBox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateW");
	rotWSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotWSlider->align(FL_ALIGN_TOP);
	rotWSlider->type(FL_HOR_SLIDER);
	rotWSlider->bounds(-179, 179);
	rotWSlider->step(1);
	rotWSlider->value(canvas->camera->rotW);
	rotWSlider->callback(cameraRotateCB);

	pack->end();
	*/
	

	Fl_Pack* packShaders = new Fl_Pack(w() - 100, 30, 100, h(), "Shaders");
	packShaders->box(FL_DOWN_FRAME);
	packShaders->labelfont(1);
	packShaders->type(Fl_Pack::VERTICAL);
	packShaders->spacing(0);
	packShaders->begin();


	reloadButton = new Fl_Button(0, 0, packShaders->w() - 20, 20, "Reload");
	reloadButton->callback(reloadCB, (void*)this);

	packShaders->end();

	end();
}


/**************************************** main() ********************/
int main(int argc, char **argv) {
	win = new MyAppWindow(600, 500, "Spell Fire");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}