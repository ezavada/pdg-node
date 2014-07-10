// -----------------------------------------------
// graphics-manager.cpp
//
// Open GL based implementation of
// common graphics functions
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


#include "pdg_project.h"

#ifndef PDG_NO_GUI

#include "pdg/msvcfix.h"

#include "pdg/sys/graphics.h"

#include "pdg/sys/os.h"
#include "pdg/sys/eventmanager.h"
#include "pdg/sys/events.h"
#include "internals.h"
#include "pdg-main.h"
#include "graphics-opengl.h"

namespace pdg {

PortImpl* gMainPort = 0;

// ==================================================================
// GRAPHICS MANAGER
// implementation of publicly declared methods (ie, exposed outside
// system framework by framework/inc/graphics.h)
// ==================================================================

Port* 
GraphicsManager::getMainPort() {
	return gMainPort; 
}

void 
GraphicsManager::setMainPort(Port* port) {
	gMainPort = static_cast<PortImpl*>(port); 
}

Point 
GraphicsManager::getMouse(int mouseNumber) {
	return OS::getMouse(mouseNumber);
}

GraphicsManager::GraphicsManager() {
	gMainPort = 0;
	for (int i = 0; i < kMaxScreens; i++) {
		mOldHeight[i] = 0;
		mOldWidth[i] = 0;
		mOldDepth[i] = 0;
		mWasChanged[i] = false;
		mIsFullscreen[i] = false;
		mSavedWindowPos[i] = Rect();
		mSavedScreenWidth[i] = 0;
		mSavedScreenHeight[i] = 0;
	}
}

GraphicsManager::~GraphicsManager() {
	if (gMainPort) {  // clean up our port
		closeGraphicsPort(gMainPort);
	}
	gMainPort = 0;
	// restore previous screen settings
	for (int i = 0; i < kMaxScreens; i++) {
		if (mWasChanged[i] && (mOldDepth[i] > 0)) {
			DEBUG_ONLY( OS::_DOUT("Restoring screen %d [%d x %d @ %d]", i, mOldWidth[i], mOldHeight[i], mOldDepth[i]); ) 
			platform_switchScreenResolution(i, mOldWidth[i], mOldHeight[i], mOldDepth[i]);
		}
	}
}

void
GraphicsManager::setTargetFPS(float fps) {
	main_setTargetFPS(fps);
}
	
float
GraphicsManager::getTargetFPS() {
	return main_getTargetFPS();
}

float
GraphicsManager::getFPS() {
	return main_getCurrentFPS();
}

int        
GraphicsManager::getNumScreens() {
	return platform_getNumScreens();
}

GraphicsManager::ScreenMode 
GraphicsManager::getCurrentScreenMode(int screenNum, pdg::Rect* maxWindowRect) 
{
	ScreenMode mode;
	if (screenNum == GraphicsManager::screenNum_PrimaryScreen) {
		screenNum = platform_getPrimaryScreen();
	}
    mode.bpp = platform_getCurrentScreenDepth(screenNum);
	platform_getScreenSize(&mode.width, &mode.height, screenNum);
	if (maxWindowRect) {
		long winWidth, winHeight;
		platform_getMaxWindowSize(&winWidth, &winHeight, screenNum);
		*maxWindowRect = pdg::Rect(winWidth, winHeight);
	}
	return mode;
}

// returns the number of screens modes a particular screen supports
int   
GraphicsManager::getNumSupportedScreenModes(int screenNum) {
	if (screenNum == GraphicsManager::screenNum_PrimaryScreen) {
		screenNum = platform_getPrimaryScreen();
	}
	if ((screenNum < 0) || (screenNum >= kMaxScreens)) return 0;
	return platform_getNumSupportedScreenModes(screenNum);
}

// returns the details of a supported mode (width, height, depth) for a given screen
GraphicsManager::ScreenMode   
GraphicsManager::getNthSupportedScreenMode(int n, int screenNum) {
	ScreenMode mode;
	mode.bpp = 0;
	mode.height = 0;
	mode.width = 0;
	if (screenNum == GraphicsManager::screenNum_PrimaryScreen) {
		screenNum = platform_getPrimaryScreen();
	}
	if ((n >= 0) && (screenNum >= 0) && (screenNum < kMaxScreens) && (n < platform_getNumSupportedScreenModes(screenNum)) ) {
		platform_getNthSupportedScreenMode(screenNum, n, &mode.width, &mode.height, &mode.bpp);
	}
	return mode;
}

void       
GraphicsManager::setScreenMode(long width, long height, int screenNum, int bpp) {
	if (screenNum == GraphicsManager::screenNum_PrimaryScreen) {
		screenNum = platform_getPrimaryScreen();
	}
	if ((screenNum < 0) || (screenNum >= kMaxScreens)) return;
	if (bpp == 0) {
		bpp = platform_getCurrentScreenDepth(screenNum);
	}
	if (!mWasChanged[screenNum]) {  // first time we've changed this screen
		// save the old dimensions so we can reset them when we exit
		mOldDepth[screenNum] = platform_getCurrentScreenDepth(screenNum);
		platform_getScreenSize(&mOldWidth[screenNum], &mOldHeight[screenNum], screenNum);
		mWasChanged[screenNum] = true;
	}
	platform_switchScreenResolution(screenNum, width, height, bpp);
}

Port*   
GraphicsManager::createWindowPort(const Rect& rect, const char* windName, int bpp) {
    PortImpl* port = dynamic_cast<PortImpl*>(graphics_newPort(this));

    if (windName == 0) {
        // no window name given, leave it blank
        windName = "";
    }
    if (bpp == 0) {
        bpp = platform_getCurrentScreenDepth(screenNum_PrimaryScreen);
    }
    // create the window
    port->mPlatformWindowRef = platform_createWindow(rect.width(), rect.height(), 
    	rect.left, rect.top, bpp, windName);
	
	pdg::Rect r = rect;
	r.moveTo(0,0);
	port->setPortRects(r);

	// make this the main port if there isn't one already
	if (!gMainPort) {
	    gMainPort = port;
	}

    return port;
}

Port*   
GraphicsManager::createFullScreenPort(const Rect& rect, int screenNum, bool allowResChange, int bpp) {
    PortImpl* port = dynamic_cast<PortImpl*>(graphics_newPort(this));
	if ((screenNum == GraphicsManager::screenNum_PrimaryScreen) 
      || (screenNum < pdg::screenNum_BestFitScreen)
	  || (screenNum >= kMaxScreens)) {
		screenNum = platform_getPrimaryScreen();
	}

	long reqWidth = rect.width();
	long reqHeight = rect.height();
    if (screenNum == pdg::screenNum_BestFitScreen) {
        screenNum = platform_closestScreenTo(reqWidth, reqHeight, bpp);
    }
    if (bpp == 0) {
        bpp = platform_getCurrentScreenDepth(screenNum);
    }
 	// capture the screen so we own it completely and our resolution switch (if we do one) 
	// won't screw up other application's windows
	platform_captureScreen(screenNum); 

	Rect possibleRect = graphics_getScreenRectForPortSize(reqWidth, reqHeight, screenNum, allowResChange);
    long newWidth = possibleRect.width();
    long newHeight = possibleRect.height();
    if ((newWidth == 0) || (newHeight == 0)) return 0;

	bool needResChange = (reqWidth != newWidth) && (reqHeight != newHeight); // both dimensions changed
	if (needResChange && allowResChange) {
		platform_getScreenSize(&mSavedScreenWidth[screenNum], &mSavedScreenHeight[screenNum], screenNum);
		if (!mWasChanged[screenNum]) {  // first time we've changed this screen
			mOldWidth[screenNum] = mSavedScreenWidth[screenNum];
			mOldHeight[screenNum] = mSavedScreenHeight[screenNum];
			mOldDepth[screenNum] = platform_getCurrentScreenDepth(screenNum);
			mWasChanged[screenNum] = true;
		}
		platform_switchScreenResolution(screenNum, reqWidth, reqHeight, bpp);
	}

    // create the window
    port->mPlatformWindowRef = platform_createFullscreenWindow(rect.width(), rect.height(), bpp, screenNum);

	// reposition the window to fill the screen
	platform_setWindowPosition(port->mPlatformWindowRef, possibleRect.left, possibleRect.top);
	
	pdg::Rect r(reqWidth, reqHeight);
	port->setPortRects(r);

	// make this the main port if there isn't one already
	if (!gMainPort) {
	    gMainPort = port;
	}
    return port;
}

Font*   
GraphicsManager::createFont(const char* fontName, float scalingFactor) {
	return 0;  // must implement in subclass
}

void    
GraphicsManager::closeGraphicsPort(Port* port) {
    PortImpl* thePort = static_cast<PortImpl*>(port);
	if (!thePort) thePort = gMainPort;
    if (thePort == gMainPort) {
        gMainPort = 0;  // clear the main port if we just closed it
    }
    platform_destroyWindow(thePort->mPlatformWindowRef);
    delete thePort;
}

bool    
GraphicsManager::switchToFullScreenMode(bool allowResChange, Port* port) {
    PortImpl* thePort = static_cast<PortImpl*>(port);
	if (!thePort) thePort = gMainPort;
	if (!thePort) return false;
	bool prevFS = platform_isFullScreen(thePort->mPlatformWindowRef);
	if (prevFS) return true; // already in Full Screen Mode
	long height = thePort->getDrawingArea().height();
	long width = thePort->getDrawingArea().width();
	int screenNum = platform_getWindowScreen(thePort->mPlatformWindowRef);
	if (screenNum < 0 || screenNum >= kMaxScreens) return false;
	int bpp = platform_getCurrentScreenDepth(screenNum);
	if (bpp == 0) return false;

	// we were in windowed mode, so save our old window dimensions and location
	long xpos, ypos;
	platform_getWindowPosition(thePort->mPlatformWindowRef, &xpos, &ypos);
	mSavedWindowPos[screenNum] = Rect(Point(xpos, ypos), width, height);
	mIsFullscreen[screenNum] = true;

	// capture the screen so we own it completely and our resolution switch (if we do one) 
	// won't screw up other application's windows
	platform_captureScreen(screenNum); 

	Rect targetPortRect = graphics_getScreenRectForPortSize(width, height, screenNum, allowResChange);
    long newWidth = targetPortRect.width();
    long newHeight = targetPortRect.height();
    if ((newWidth == 0) || (newHeight == 0)) return false;

	bool needResChange = (width != newWidth) && (height != newHeight); // both dimensions changed
	if (needResChange && allowResChange) {
		platform_getScreenSize(&mSavedScreenWidth[screenNum], &mSavedScreenHeight[screenNum], screenNum);
		if (!mWasChanged[screenNum]) {  // first time we've changed this screen
			mOldWidth[screenNum] = mSavedScreenWidth[screenNum];
			mOldHeight[screenNum] = mSavedScreenHeight[screenNum];
			mOldDepth[screenNum] = platform_getCurrentScreenDepth(screenNum);
			mWasChanged[screenNum] = true;
		}
		platform_switchScreenResolution(screenNum, width, height, bpp);
	}

	// now resize the window to the requested size
	platform_resizeWindow(thePort->mPlatformWindowRef, width, height, true);

	// reposition the window to fill the screen
	platform_setWindowPosition(thePort->mPlatformWindowRef, targetPortRect.left, targetPortRect.top);

	return true;
}

//! make a port be in a window
//! returns true if succeeded, false if failed
//! if port is not passed it will default to the main port
bool    
GraphicsManager::switchToWindowMode(Port* port, const char* windName) {
    PortImpl* thePort = static_cast<PortImpl*>(port);
	if (!thePort) port = gMainPort;
	if (!thePort) return false;
    if (windName == 0) {
        // no window name given, leave it blank
        windName = "";
    }
	bool prevFS = platform_isFullScreen(thePort->mPlatformWindowRef);
	if (!prevFS) return true; // already in Window Mode
	long height = thePort->getDrawingArea().height();
	long width = thePort->getDrawingArea().width();
	int screenNum = platform_getWindowScreen(thePort->mPlatformWindowRef);
	if (screenNum < 0 || screenNum >= kMaxScreens) return false;
	int bpp = platform_getCurrentScreenDepth(screenNum);
	if (bpp == 0) return false;
	// were in fullscreen, now going back to windowed mode, so
	// restore the screen resolution back to whatever setting the user had it at
	platform_switchScreenResolution(screenNum, mSavedScreenWidth[screenNum], mSavedScreenHeight[screenNum], bpp);
	platform_releaseScreen(screenNum); 
	// clear out our saved screen resolution info since we have restored it to the user's settings
	mSavedScreenWidth[screenNum] = 0;
	mSavedScreenHeight[screenNum] = 0;
	// reposition the window
	platform_resizeWindow(thePort->mPlatformWindowRef, mSavedWindowPos[screenNum].width(), mSavedWindowPos[screenNum].height(), false);
	platform_setWindowPosition(thePort->mPlatformWindowRef, mSavedWindowPos[screenNum].left, mSavedWindowPos[screenNum].top);
	return true;
}


// return true if main port is in fullscreen mode
bool
GraphicsManager::inFullScreenMode() {
    if (!gMainPort) return false;
    return platform_isFullScreen(gMainPort->mPlatformWindowRef);
}

// IMPORTANT! This rect return is two different bits of data stuffed into a rect:
// the rectangle is the size of the required screen (which might or might not be the current screen size)
// the top left point is the offset within the screen at which to position the port with it's
// current ratio
Rect 
graphics_getScreenRectForPortSize(long width, long height, int screenNum, bool allowResChange) {
    long x = 0, y = 0;
    long h, w;
    platform_getScreenSize(&w, &h, screenNum);
    if (h != height || w != width) {
        // compare screen ratios
        float currRatio = (float)h/(float)w;
        float targetRatio = (float)height/(float)width;
        long nHeight, nWidth;

        // make alternate height and width that preserves ratio
        if (currRatio > targetRatio) {  
            // the current aspect ratio is taller than the target aspect ratio, 
            // so base our dimensions on the width and let the new height be
            // bigger than we need
            nWidth = width;
            nHeight = width / currRatio;
        } else {
            // the current aspect ratio is wider than the target aspect ratio, 
            // so base our dimensions on the height and let the new width be
            // bigger than we need
            nWidth = height / currRatio;
            nHeight = height;
        }
		// if both dimensions or the depth are different, then change screen mode 
		// to new height, width and depth
		if (nWidth != w && nHeight != h) {
			if (allowResChange) {
				int bpp = platform_getCurrentScreenDepth(screenNum);
				nWidth = width;
				nHeight = height;
				if (!platform_closestScreenMode(screenNum, &nWidth, &nHeight, &bpp)) {
					return Rect();  // couldn't find anything 
				} else {
					// change our screen values to reflect the change we can make
					w = nWidth;
					h = nHeight;
				}
			} else {
				if ( (nWidth > w) || (nHeight > h) ) {
					return Rect();
				}
			}
        }
        // then set x & y to center within the final size
        x = (w - width) / 2;
        y = (h - height) / 2;
    }
    return Rect(Point(x, y), w, h); // return offset in screen rect for port, and screen height & width
}

} // end namespace pdg

#endif // PDG_NO_GUI

