// -----------------------------------------------
// platform-events-glfw.cpp
// 
// multiplatform implementation of platform specific event handling functions
// using GLFW
//
// Written by Ed Zavada, 2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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

#include "pdg_project.h"

#include "pdg/sys/events.h"

#include "internals.h"
#ifndef PDG_NO_GUI
	#include "internals-glfw.h"
#endif
#include "pdg-lib.h"
#include "pdg-main.h"

namespace pdg {

#ifndef PDG_NO_GUI
static bool sShift = false;
static bool sAlt = false;
static bool sControl = false;
static bool sCmd = false;
static bool sRepeat = false;
#endif

void platform_init(int argc, const char* argv[]) {
  #ifndef PDG_NO_GUI
	glfwInitIfNeeded();
  #endif
}

void platform_pollEvents() {
  #ifndef PDG_NO_GUI
	glfwPollEvents();
  #endif
}

void platform_cleanup() {
  #ifndef PDG_NO_GUI
	glfwTerminate();
  #endif
}

void platform_getDeviceOrientation(float* outRoll, float* outPitch, float* outYaw, bool absolute) {
    *outPitch = 0.0f;
    *outRoll = 0.0f;
    *outYaw = 0.0f;
}

#ifndef PDG_NO_GUI
void setupGlfwCallbacks(GLFWwindow* window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);
	glfwSetCharCallback(window, handle_char_callback);
	glfwSetKeyCallback(window, handle_key_callback);
//	glfwSetCursorEnterCallback(window, handle_cursorenter_callback);
	glfwSetCursorPosCallback(window, handle_cursorpos_callback);
	glfwSetMouseButtonCallback(window, handle_mousebutton_callback);
	glfwSetScrollCallback(window, handle_scroll_callback);
	glfwSetFramebufferSizeCallback(window, handle_framebuffersize_callback);
//	glfwSetWindowIconifyCallback(window, handle_windowiconify_callback);
//	glfwSetWindowFocusCallback(window, handle_windowfocus_callback);
//	glfwSetWindowRefreshCallback(window, handle_windowrefresh_callback);
	glfwSetWindowCloseCallback(window, handle_windowclose_callback);
	glfwSetWindowSizeCallback(window, handle_windowsize_callback);
//	glfwSetWindowPosCallback(window, handle_windowpos_callback);
}

void handle_windowpos_callback(GLFWwindow* window, int xpos, int ypos) {
	// xpos & ypos in screen coords
}

void handle_windowsize_callback(GLFWwindow* window, int width, int height) {
	// height/width in screen pixels
}

void handle_windowrefresh_callback(GLFWwindow* window) {
}

void handle_windowfocus_callback(GLFWwindow* window, int focused) {
	// focused = GL_TRUE or GL_FALSE
}

void handle_windowiconify_callback(GLFWwindow* window, int iconified) {
	// iconified = GL_TRUE or GL_FALSE
}

void handle_mousebutton_callback(GLFWwindow* window, int button, int action, int mods) {
	sShift = ((mods & GLFW_MOD_SHIFT) != 0);
	sControl = ((mods & GLFW_MOD_CONTROL) != 0);
	sAlt = ((mods & GLFW_MOD_ALT) != 0);
	sCmd = ((mods & GLFW_MOD_SUPER) != 0);
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
    main_handleMouse((action == GLFW_PRESS) ? mouseEventType_MouseDown : mouseEventType_MouseUp, 
    	xpos, ypos, button, sShift, sControl, sAlt, sCmd);
}

void handle_cursorpos_callback(GLFWwindow* window, double xpos, double ypos) {
	// cursor moved
	int button;
	bool dragging = false;
	// check button states for first 3 mouse buttons to see if we are dragging or just moving
	for (button = 0; button<3; button++) {
		if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
			dragging = true;
			break;
		}
	}
	if (!dragging) button = 0;
    main_handleMouse(dragging ? mouseEventType_MouseDragged : mouseEventType_MouseMoved, 
    	xpos, ypos, button, sShift, sControl, sAlt, sCmd);
}

void handle_cursorenter_callback(GLFWwindow* window, int entered) {
	// got cursor enter or leave (entered = GL_TRUE)
}

void handle_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// got scrolling
}

void handle_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	unichar character = remapKeyboardChar(key, mods, sShift, sControl, sAlt, sCmd);
	if (action == GLFW_PRESS) {
		main_handleKeyDown(scancode, character ? character : key);
	}
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		sRepeat = (action == GLFW_REPEAT);
		if (character) {
			main_handleKeyPress(character, sRepeat, sShift, sControl, sAlt, sCmd);
		}
	}
	if (action == GLFW_RELEASE) {
		main_handleKeyUp(scancode, character ? character : key);
	}
}

void handle_char_callback(GLFWwindow* window, unsigned int character) {
	// got unicode char input. Repeat and mod key states are set by handle_key_callback()
	main_handleKeyPress(character, sRepeat, sShift, sControl, sAlt, sCmd);
}


#define SIZE_OF_KEY_MAP 64

unichar keyMap[SIZE_OF_KEY_MAP][6] = {
// GLFW key				PDG key			modifiers (shift, control, alt/option, meta/cmd )
{ GLFW_KEY_UP        	 , key_UpArrow,   0, 0, 0, 0},
{ GLFW_KEY_DOWN       	 , key_DownArrow, 0, 0, 0, 0 },
{ GLFW_KEY_LEFT       	 , key_LeftArrow, 0, 0, 0, 0 },
{ GLFW_KEY_RIGHT      	 , key_RightArrow, 0, 0, 0, 0 },
{ GLFW_KEY_F1            , key_F1, 0, 0, 0, 0 },
{ GLFW_KEY_F2            , key_F2, 0, 0, 0, 0 },
{ GLFW_KEY_F3            , key_F3, 0, 0, 0, 0 },
{ GLFW_KEY_F4            , key_F4, 0, 0, 0, 0 },
{ GLFW_KEY_F5            , key_F5, 0, 0, 0, 0 },
{ GLFW_KEY_F6            , key_F6, 0, 0, 0, 0 },
{ GLFW_KEY_F7            , key_F7, 0, 0, 0, 0 },
{ GLFW_KEY_F8            , key_F8, 0, 0, 0, 0 },
{ GLFW_KEY_F9            , key_F9, 0, 0, 0, 0 },
{ GLFW_KEY_F10           , key_F10, 0, 0, 0, 0 },
{ GLFW_KEY_F11           , key_F11, 0, 0, 0, 0 },
{ GLFW_KEY_F12           , key_F12, 0, 0, 0, 0 },
{ GLFW_KEY_INSERT        , key_Insert, 0, 0, 0, 0 },
{ GLFW_KEY_DELETE        , key_Delete, 0, 0, 0, 0 },
{ GLFW_KEY_HOME          , key_Home, 0, 0, 0, 0 },
{ GLFW_KEY_END           , key_End, 0, 0, 0, 0 },
{ GLFW_KEY_PAGE_UP       , key_PageUp, 0, 0, 0, 0 },
{ GLFW_KEY_PAGE_DOWN     , key_PageDown, 0, 0, 0, 0 },
{ GLFW_KEY_PAUSE         , key_Pause, 0, 0, 0, 0 },
{ GLFW_KEY_ESCAPE    	 , key_Escape, 0, 0, 0, 0 },
{ GLFW_KEY_TAB  	  	 , key_Tab, 0, 0, 0, 0 },
{ GLFW_KEY_BACKSPACE     , key_Backspace, 0, 0, 0, 0 },
{ GLFW_KEY_ENTER         , key_Enter, 0, 0, 0, 0 },
{ GLFW_KEY_MENU          , key_Escape, 0, 0, 0, 1 },
{ 0, 0, 0, 0, 0, 0 }
};

// unichar shiftKeyMap[SIZE_OF_KEY_MAP][2] = {
// // GLFW key				PDG key when shifted
// { GLFW_KEY_APOSTROPHE		, '"' },
// { GLFW_KEY_COMMA			, '<' },
// { GLFW_KEY_MINUS			, '_' },
// { GLFW_KEY_PERIOD			, '>' },
// { GLFW_KEY_SLASH			, '?' },
// { GLFW_KEY_0				, ')' },
// { GLFW_KEY_1				, '!' },
// { GLFW_KEY_2				, '@' },
// { GLFW_KEY_3				, '#' },
// { GLFW_KEY_4				, '$' },
// { GLFW_KEY_5				, '%' },
// { GLFW_KEY_6				, '^' },
// { GLFW_KEY_7				, '&' },
// { GLFW_KEY_8				, '*' },
// { GLFW_KEY_9				, '(' },
// { GLFW_KEY_SEMICOLON		, ':' },
// { GLFW_KEY_EQUAL			, '+' },
// { GLFW_KEY_GRAVE_ACCENT		, '~' },
// { 0, 0 }
// };

unichar remapKeyboardChar(unichar character, int modifierFlags, bool& shift, bool& control, bool& alt, bool& cmd) {
	shift = ((modifierFlags & GLFW_MOD_SHIFT) != 0);
	control = ((modifierFlags & GLFW_MOD_CONTROL) != 0);
	alt = ((modifierFlags & GLFW_MOD_ALT) != 0);
	cmd = ((modifierFlags & GLFW_MOD_SUPER) != 0);
	// check for special keys that must be mapped
// 	if ( (character >= GLFW_KEY_KP_0) && (character <= GLFW_KEY_KP_9) ) {
// 		character = character - GLFW_KEY_KP_0 + 48; // ascii '0'
// 	} else if ( (character >= GLFW_KEY_A) && (character <= GLFW_KEY_RIGHT_BRACKET) ) {
// 		if (shift) {
// 			character = character - GLFW_KEY_A + 97; // ascii 'a'
// 		}
// 	} else 

	if (character < GLFW_KEY_ESCAPE) return 0;  // don't remap any normal keys

	if (character >= GLFW_KEY_ESCAPE) {
		for (int i = 0; i < SIZE_OF_KEY_MAP; i++) {
			if (keyMap[i][0] == character) {
				character = keyMap[i][1];  // replace the character
				shift |= (keyMap[i][2] != 0);	   // add additional modifier keys if needed
				control |= (keyMap[i][3] != 0);
				alt |= (keyMap[i][4] != 0);
				cmd |= (keyMap[i][5] != 0);
				break;
			} else if (keyMap[i][0] == 0) {  // at end of list
				return 0;	// not in map, ignore
			}
		}
//	} else if (shift) {
// 		for (int i = 0; i < SIZE_OF_KEY_MAP; i++) {
// 			if (keyMap[i][0] == character) {
// 				character = keyMap[i][1];	// replace the character with the shifted version
// 				break;
// 			} else if (keyMap[i][0] == 0) {  // at end of list
// 				break;
// 			}
// 		}
 	}
	return character;
}

#endif  // ! PDG_NO_GUI



} // end namespace pdg

