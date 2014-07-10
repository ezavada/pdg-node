// -----------------------------------------------
// pdg-main.h
// 
// interface to main loop and event handling functionality
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2012, Dream Rock Studios, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// -----------------------------------------------

#ifndef PDG_MAIN_H_INCLUDED
#define PDG_MAIN_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/log.h"
#include "pdg/sys/coordinates.h"

namespace pdg {

#ifndef PDG_NO_GUI

enum { 
	mouseEventType_MouseDown, 
	mouseEventType_MouseUp, 
	mouseEventType_MouseDragged, 
	mouseEventType_MouseEntered, 
	mouseEventType_MouseExited, 
	mouseEventType_MouseMoved,
	numMouseEventTypes
};

enum {
	screenNum_PrimaryScreen = -1,
    screenNum_BestFitScreen = -2
};

bool main_getRawKeyState(int keyCode);
bool main_getKeyState(utf16char keyChar);
bool main_getButtonState(int buttonNumber);
Point main_getMousePos(int mouseNumber);

bool main_isFullscreen();

float  main_getCurrentFPS();
float  main_getTargetFPS();
void   main_setTargetFPS(float fps);

void main_handleKeyPress(utf16char keyChar, bool repeat, bool shift, bool control, bool alt, bool cmd);
void main_handleKeyDown(int keyCode, utf16char keyChar);
void main_handleKeyUp(int keyCode, utf16char keyChar);
void main_handleMouse(int action, float x, float y, int button, bool shift, bool control, bool alt, bool cmd);
void main_handleScreenUpdate(int screenPos);
void main_setInitialScreenState(int screenPos, bool fullscreen);
void main_handleLiveResize(int currWidth, int currHeight);

#endif // ! PDG_NO_GUI

log& main_getDebugLog();

bool main_getNoExitFromArgs(int argc, const char* argv[]);
int main_getLogLevelFromArgs(int argc, const char* argv[], int defaultLevel);


int main_initManagers();
void main_initKeyStates();
int main_init(int argc, const char* argv[], bool isInitialized);
void main_run();
int main_cleanup(bool* dontExit = NULL);

} // end namespace pdg
#endif