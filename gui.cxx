// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "gui.h"

void GUI::cb_Pt_i(Fl_Button*, void*) {
  DBG_cb();
}
void GUI::cb_Pt(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Pt_i(o,v);
}

void GUI::cb_Save_i(Fl_Button*, void*) {
  Save_cb();
}
void GUI::cb_Save(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_Save_i(o,v);
}
#include "scene.h"

GUI::GUI() {
  { uiw = new Fl_Double_Window(199, 94, "GUI");
    uiw->labeltype(FL_NORMAL_LABEL);
    uiw->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(10, 50, 80, 35, "Pt 1 demo");
      o->tooltip("Generates demonstrations of all required functionality.");
      o->selection_color(FL_DARK_RED);
      o->callback((Fl_Callback*)cb_Pt);
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(95, 50, 95, 35, "Save Video");
      o->tooltip("Saves the video under provided name to the mydbg dir.");
      o->callback((Fl_Callback*)cb_Save);
    } // Fl_Button* o
    { savepath = new Fl_Input(95, 10, 95, 35, "Video name:");
      savepath->labeltype(FL_SHADOW_LABEL);
    } // Fl_Input* savepath
    uiw->end();
  } // Fl_Double_Window* uiw
}

int main(int argc, char **argv) {
  scene = new Scene;
  return Fl::run();
}

void GUI::show() {
  uiw->show();
}

void GUI::DBG_cb() {
  scene->DBG();
}

void GUI::Save_cb() {
  scene->Save();
}
