// -----------------------------------------------
// platform-graphics-glfw.cpp
// 
// cross platform implementation of platform specific functions
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

#include "pdg/sys/platform.h"
#include "pdg/sys/os.h"
#include "pdg/sys/graphicsmanager.h"

#include "pdg-lib.h"
#include "pdg-main.h"
#include "internals.h"
#include "internals-glfw.h"
#include "graphics-opengl.h"

#ifndef PDG_MAX_DISPLAYS
  #define PDG_MAX_DISPLAYS 32
#endif

GLFWvidmode gOriginalDisplayModes[PDG_MAX_DISPLAYS];

#ifdef PLATFORM_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#define WinAPI
#endif // PLATFORM_WIN32

namespace pdg {

extern PortImpl* gMainPort;

struct PrivateWindowInfoT {
	bool	isFullscreen;
};

#ifdef PLATFORM_WIN32
extern HWND gMainHWND;
HDC graphics_getPortDC(Port* port) {
	PortImpl* portimpl = dynamic_cast<PortImpl*>(port);
	GLFWwindow* window = static_cast<GLFWwindow*>(portimpl->mPlatformWindowRef);
	HWND hwnd = glfwGetWin32Window(window);
	HDC hdc = WinAPI::GetDC(hwnd);
	return hdc;
}
#endif // PLATFORM_WIN32

void extraWindowSetup(GLFWwindow* window) {
  #ifdef PLATFORM_WIN32
    if (!gMainHWND) {
		gMainHWND = glfwGetWin32Window(window);
	}
  #endif
}

void handle_windowclose_callback(GLFWwindow* window) {
	if (gMainPort) {
		GLFWwindow* mainWind = static_cast<GLFWwindow*>(gMainPort->mPlatformWindowRef);
		if (window == mainWind) {
			pdg_LibQuit();
		}
	}
	// TODO: do something about other windows being closed
}

void handle_framebuffersize_callback(GLFWwindow* window, int width, int height) {
	// new width and height in pixels of frame buffer
	if (gMainPort) {
		GLFWwindow* mainWind = static_cast<GLFWwindow*>(gMainPort->mPlatformWindowRef);
		if (window == mainWind) {
			main_handleLiveResize(width, height);
		}
	}
	// TODO: do something about other windows being resized
}


void platform_startDrawing(void* windRef) {
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(windRef));
}

void platform_finishDrawing(void* windRef) {
	glfwSwapBuffers(static_cast<GLFWwindow*>(windRef));
}

int platform_getNumScreens(void) {
	glfwInitIfNeeded();
	int count = 0;
	glfwGetMonitors(&count);
	return count;
}

int platform_getPrimaryScreen() {
	return 0;
}

void platform_getScreenSize(long* outWidth, long* outHeight, int screenNum) {
	glfwInitIfNeeded();
	if (screenNum >= GraphicsManager::screenNum_PrimaryScreen && screenNum < platform_getNumScreens()) {
		GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);		
		*outWidth = mode->width;
		*outHeight = mode->height;
	} else {
		*outWidth = 0;
		*outHeight = 0;
	}
}

void platform_getMaxWindowSize(long* outWidth, long* outHeight, int screenNum) {
	long screenWidth, screenHeight;
	static bool measuredWindows = false;
	static long windFrameWidth = 0;
	static long windFrameHeight = 0;
	static long primaryDeltaWidth = 0;
	static long	primaryDeltaHeight = 0;
	platform_getScreenSize(&screenWidth, &screenHeight, screenNum);
	if (!measuredWindows) {
		measuredWindows = true;
		// figure out how big our window frame is
		int winWidth, winHeight, contentWidth, contentHeight;
		GLFWwindow* window = glfwCreateWindow(64, 64, "test window", NULL, NULL);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &contentWidth, &contentHeight);
		windFrameWidth = winWidth - contentWidth;
		windFrameHeight = winHeight - contentHeight;
		glfwDestroyWindow(window);
		// TODO: figure out how big the menu bar/dock and so forth on the primary screen are
	}
	*outWidth = screenWidth - windFrameWidth;
	*outHeight = screenHeight - windFrameHeight;
	// adjust for menu bar/dock
	if (screenNum == GraphicsManager::screenNum_PrimaryScreen 
	  || screenNumToGLFWmonitor(screenNum) == glfwGetPrimaryMonitor()) {
		*outWidth += primaryDeltaWidth;
		*outHeight += primaryDeltaHeight;
	}
}

void* platform_createWindow(long width, long height, long xPos, long yPos, int bpp, const char* title) {
	glfwInitIfNeeded();
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetWindowPos(window, xPos, yPos);
	PrivateWindowInfoT* winInfo = new PrivateWindowInfoT();
	winInfo->isFullscreen = false;
	glfwSetWindowUserPointer(window, winInfo);
	setupGlfwCallbacks(window);
	extraWindowSetup(window);
	glfwShowWindow(window);
	return window;
}

int platform_getNumSupportedScreenModes(int screenNum) {
	if ((screenNum < GraphicsManager::screenNum_PrimaryScreen) || (screenNum >= platform_getNumScreens())) return 0;
	int count = 1;
	GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
	glfwGetVideoModes(monitor, &count);
	return count;
}

void platform_getNthSupportedScreenMode(int screenNum, int n, long* outWidth, long* outHeight, int* outBpp) {
	*outWidth = 0;
	*outHeight = 0;
	*outBpp = 0;
	if ((screenNum < GraphicsManager::screenNum_PrimaryScreen) || (screenNum >= platform_getNumScreens())) return;
	if (n < 0) return;
	int count = 1;
	GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
	const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
	if (n >= count) return;
	*outWidth = modes[n].width;
	*outHeight = modes[n].height;
	*outBpp = bppFromGLFWvidmode(&modes[n]);
}


bool platform_closestScreenMode(int screenNum, long* ioWidth, long* ioHeight, int* ioBpp) {
	long width = *ioWidth;
	long height = *ioHeight;
	int bpp = *ioBpp;
	int count = 1;
	GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
	const GLFWvidmode* modeList = glfwGetVideoModes(monitor, &count);

    int highestRank = 0;
    
    int* ranking = (int*)malloc(count * sizeof(int));
    
    bool equalOrBetter = false;
    for (int i = 0; i < count; i++) {
    	const GLFWvidmode* mode = &modeList[i];
        int d = bppFromGLFWvidmode(mode);
        long h = mode->height;
        long w = mode->width;
        if (w == width && h == height && d == bpp) {
			*ioWidth = w;
			*ioHeight = h;
			*ioBpp = d;
            free(ranking);
            return true; // exact match!!
        }
        // rank display mode: exact dimensions best, greater size and depth 
        // are better than reduced, dimensions more important
        // than depth
        ranking[i] = 0;
        ranking[i] += (w==width) ? 100 : ((w > width) ? 50 : 0);
        ranking[i] += (h==height) ? 100 : ((h > height) ? 50 : 0);
        ranking[i] += (d==bpp) ? 20 : ((d > bpp) ? 10 : 0);
        if (ranking[i] > highestRank) {
            highestRank = ranking[i];
			*ioWidth = w;
			*ioHeight = h;
			*ioBpp = d;
			equalOrBetter = ( (w>=width) && (h>=height) && (d>=bpp) );
        }
    }
    free(ranking);
	return equalOrBetter;
}

int platform_closestScreenTo(long width, long height, int bpp) {
    if (bpp == 0) bpp = 32;
    int numScreens = platform_getNumScreens();
    int ranking[PDG_MAX_DISPLAYS];
    int highestRankedScreen = GraphicsManager::screenNum_PrimaryScreen; // worst case is we just use main screen
    int highestRank = 0;
    for (int i = 0; i <  numScreens; i++) {
        long h, w;
        platform_getScreenSize(&w, &h, i);
        int d = platform_getCurrentScreenDepth(i);
        if (w == width && h == height && d == bpp) {
            return i; // exact match!!
        }
        // rank screen: exact dimensions best, reducing size and depth 
        // are better than increasing them, dimensions more important
        // than depth
        ranking[i] = 0;
        ranking[i] += (w==width) ? 100 : ((w > width) ? 50 : 0);
        ranking[i] += (h==height) ? 100 : ((h > height) ? 50 : 0);
        ranking[i] += (d==bpp) ? 20 : ((d > bpp) ? 10 : 0);
        if (ranking[i] > highestRank) {
            highestRankedScreen = i;
            highestRank = ranking[i];
        }
    }
    return highestRankedScreen;
}

void platform_switchScreenResolution(int screenNum, long width, long height, int bpp) {
// handled automatically by creating fullscreen window
}

void* platform_createFullscreenWindow(long width, long height, int bpp, int screenNum) {
	glfwInitIfNeeded();
	if (screenNum < GraphicsManager::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) {
		screenNum = GraphicsManager::screenNum_PrimaryScreen;
	}
    if (bpp == 0) bpp = platform_getCurrentScreenDepth(screenNum);
	GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
	GLFWwindow* window = glfwCreateWindow(width, height, "PDG", monitor, NULL);
	PrivateWindowInfoT* winInfo = new PrivateWindowInfoT();
	winInfo->isFullscreen = true;
	glfwSetWindowUserPointer(window, winInfo);
	setupGlfwCallbacks(window);
	extraWindowSetup(window);
	glfwShowWindow(window);
	return window;
}

void platform_destroyWindow(void* windRef) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	PrivateWindowInfoT* winInfo = (PrivateWindowInfoT*)glfwGetWindowUserPointer(window);
	delete winInfo;
	glfwSetWindowUserPointer(window, 0);
	glfwDestroyWindow(window);
}

void platform_resizeWindow(void* windRef, long width, long height, bool fullscreen) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	PrivateWindowInfoT* winInfo = (PrivateWindowInfoT*)glfwGetWindowUserPointer(window);
	glfwSetWindowSize(window, width, height);
	if (winInfo->isFullscreen != fullscreen) {
		// we are swapping between fullscreen and windowed mode
		OS::_DOUT("Can't swap between fullscreen and windowed mode in GLFW!!");
	}
//	winInfo->isFullscreen = fullscreen;
}	

int platform_getWindowScreen(void* windRef) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	return 0;
}

void platform_getWindowPosition(void* windRef, long* outXPos, long* outYPos) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	int xpos;
	int ypos;
	glfwGetWindowPos(window, &xpos, &ypos);
	*outXPos = xpos;
	*outYPos = ypos;
}

void platform_setWindowPosition(void* windRef, long xPos, long yPos) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	glfwSetWindowPos(window, xPos, yPos);
}

bool platform_isFullScreen(void* windRef) {
	GLFWwindow* window = static_cast<GLFWwindow*>(windRef);
	PrivateWindowInfoT* winInfo = (PrivateWindowInfoT*)glfwGetWindowUserPointer(window);
	return winInfo->isFullscreen;
}

int platform_getCurrentScreenDepth(int screenNum) {
	glfwInitIfNeeded();
	if (screenNum >= GraphicsManager::screenNum_PrimaryScreen && screenNum < platform_getNumScreens()) {
		GLFWmonitor* monitor = screenNumToGLFWmonitor(screenNum);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		return bppFromGLFWvidmode(mode);
	} else {
		return 0;
	}
}

void platform_captureScreen(int screenNum) {
	// glfw doesn't have a separate step to capture the screen
}

void platform_releaseScreen(int screenNum) {
}

void glfwInitIfNeeded() {
	static bool glfwInitialized = false;
	if (!glfwInitialized) {
		glfwInit();
		glfwWindowHint(GLFW_VISIBLE, 0);  // always start off with invisible windows]
		glfwInitialized = true;
	}
}

int bppFromGLFWvidmode(const GLFWvidmode* mode) {
	int bits = mode->redBits + mode->greenBits + mode->blueBits;
	if (bits <= 16) {
		return 16;
	} else {
		return 32;
	}
}

GLFWmonitor* screenNumToGLFWmonitor(int screenNum) {
	if (screenNum < 0) {
		return glfwGetPrimaryMonitor();
	} else {
		int count = 0;
		GLFWmonitor** monitorList = glfwGetMonitors(&count);
		if (screenNum >= count) {
			return glfwGetPrimaryMonitor();
		} else {
			return monitorList[screenNum];
		}
	}
}

} // end namespace pdg

