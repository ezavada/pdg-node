// -----------------------------------------------
// internals-glfw.h
// 
// internal functions used among GLFW implementations
//
// Written by Ed Zavada, 2009-2012
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


#ifndef INTERNALS_GLFW_H_INCLUDED
#define INTERNALS_GLFW_H_INCLUDED

#include "pdg_project.h"

#include <GLFW/glfw3.h>
#undef THIS // in case Windows defined it

namespace pdg {

// implemented in platform-graphics-glfw.cpp

void glfwInitIfNeeded();
int bppFromGLFWvidmode(const GLFWvidmode* mode);
GLFWmonitor* screenNumToGLFWmonitor(int screenNum);
void extraWindowSetup(GLFWwindow* window);

void handle_windowclose_callback(GLFWwindow* window);
void handle_framebuffersize_callback(GLFWwindow* window, int, int);


// implemented in platform-events-glfw.cpp

typedef unsigned int unichar;
unichar remapKeyboardChar(unichar character, int modifierFlags, bool& shift, bool& control, bool& alt, bool& cmd);
void setupGlfwCallbacks(GLFWwindow* window);

void handle_windowpos_callback(GLFWwindow* window, int, int);
void handle_windowsize_callback(GLFWwindow* window, int, int);
void handle_windowrefresh_callback(GLFWwindow* window);
void handle_windowfocus_callback(GLFWwindow* window, int);
void handle_windowiconify_callback(GLFWwindow* window, int);
void handle_mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
void handle_cursorpos_callback(GLFWwindow* window, double xpos, double ypos);
void handle_cursorenter_callback(GLFWwindow* window, int entered);
void handle_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) ;
void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void handle_char_callback(GLFWwindow* window, unsigned int character);

} // end namespace pdg

#endif // INTERNALS_GLFW_H_INCLUDED

