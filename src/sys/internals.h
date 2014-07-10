// -----------------------------------------------
// internals.h
// 
// internal functions declared in various system modules, but
// useful to other internal system modules
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


#ifndef INTERNALS_H_INCLUDED
#define INTERNALS_H_INCLUDED

#include "pdg_project.h"

#ifdef PDG_LIBRARY
extern "C" {
	void pdg_LibContainerDoIdle();
}
#endif

namespace pdg {

	class Port;
	class GraphicsManager;

	// implemented in os.cpp or one of its platform specific versions
	bool os_path2native(const char *inStdFileName, char* outNativeFileName, int len);
	bool os_isAbsolutePath(const char* path);
	void os_setApplicationDirectory(const char* dir);
	void os_setApplicationDataDirectory(const char* dir);
	void os_setApplicationResourceDirectory(const char* dir);
	const char* os_getPlatformErrorMessage(long err);

	// implemented in platform specific files
	// returns current working directory
	const char* platform_setupDirectories(int argc, const char* argv[]);

	void platform_init(int argc, const char* argv[]);
	void platform_pollEvents();
	void platform_cleanup();

	void platform_initImageData(unsigned char* imageData, long imageDataLen, 
			unsigned char** outDataPtr, long* outWidth, long* outHeight, 
			long* outBufferWidth, long* outBufferHeight, long* outBufferPitch, 
			int* outFormat);

    void platform_getDeviceOrientation(float* outRoll, float* outPitch, float* outYaw, bool absolute);

#ifndef PDG_NO_GUI

	// implemented by specific graphics subsystems, such as graphics-opengl.cpp
	bool graphics_allowHorizontalOrientation();
	bool graphics_allowVerticalOrientation();
	void graphics_startDrawing(Port* port);  // calls platform_startDrawing()
	void graphics_finishDrawing(Port* port);  // calls platform_finishDrawing()
	void graphics_setScreenPos(int screenPos); // screen pos values in pdg/sys/events.h
	int  graphics_getEffectiveScreenPos();
	void graphics_getApplicationSupportedOrientations();
	Port* graphics_newPort(GraphicsManager* mgr);  // factory a new port object
  #ifdef PDG_BASE_COORD_TYPE
	Rect graphics_getScreenRectForPortSize(long width, long height, int screenNum, bool allowResChange = false);
  #endif

	// implemented in platform specific files
	void platform_startDrawing(void* windRef);
	void platform_finishDrawing(void* windRef);

	void platform_setHardwareCursorVisible(bool inVisible);
	void platform_setHardwareBusyCursor();
	void platform_setHardwareNormalCursor();

    void* platform_createWindow(long width, long height, long xPos, long yPos, int bpp, const char* title);
    void* platform_createFullscreenWindow(long width, long height, int bpp, int screenNum);
    void platform_destroyWindow(void* windRef);
	void platform_resizeWindow(void* windRef, long width, long height, bool fullscreen);
	int platform_getWindowScreen(void* windRef);
	void platform_getWindowPosition(void* windRef, long* outXPos, long* outYPos);
	void platform_setWindowPosition(void* windRef, long xPos, long yPos);
	bool platform_isFullScreen(void* windRef);

    int platform_getNumScreens();
    int platform_getPrimaryScreen();
	void platform_getMaxWindowSize(long* outWidth, long* outHeight, int screenNum);
	void platform_getScreenSize(long* outWidth, long* outHeight, int screenNum);
    int platform_getCurrentScreenDepth(int screenNum);
    int platform_closestScreenTo(long width, long height, int bpp);
	int platform_getNumSupportedScreenModes(int screenNum);
	void platform_getNthSupportedScreenMode(int screenNum, int n, long* outWidth, long* outHeight, int* outBpp);
    bool platform_closestScreenMode(int screenNum, long* ioWidth, long* ioHeight, int* ioBpp);
	void platform_switchScreenResolution(int screenNum, long width, long height, int bpp);
    void platform_captureScreen(int screenNum);
    void platform_releaseScreen(int screenNum);
	
#endif // ! PDG_NO_GUI


} // end namespace pdg
#endif
