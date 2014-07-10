// -----------------------------------------------
// graphicsmanager.h
//
// graphics functionality
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


#ifndef PDG_GRAPHICS_MANAGER_H_INCLUDED
#define PDG_GRAPHICS_MANAGER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/coordinates.h"
#include "pdg/sys/singleton.h"

namespace pdg {

//! \defgroup Graphics
//! Collection of classes, types and constants that are used for drawing

// -----------------------------------------------------------------------------------
// PortInfo
// Data for the eventType_PortResized event
// -----------------------------------------------------------------------------------

class Port;
class Font;

// -----------------------------------------------------------------------------------
//! Graphics Manager
//! Used to create and track ports
//! \ingroup Managers
// -----------------------------------------------------------------------------------
class GraphicsManager : public Singleton<GraphicsManager> {
friend class Singleton<GraphicsManager>;
public:
    enum {
        screenNum_PrimaryScreen = -1,
        screenNum_BestFitScreen = -2
    };
    
    struct ScreenMode {
    	long width;
    	long height;
    	int  bpp;
    };

    //! returns the number of screens available for display
    virtual int   getNumScreens();

    //! returns the current mode (width, height, depth) of a screen
    //! if a Rect* is passed in, will also return the maximum
    //! dimensions for a window that fits on that screen after
    //! discounting menu bars, docks, window frames, etc...
    virtual ScreenMode   getCurrentScreenMode(int screenNum = screenNum_PrimaryScreen,
    	pdg::Rect* maxWindowRect = 0);

    //! returns the number of screens modes a particular screen supports
    virtual int   getNumSupportedScreenModes(int screenNum = screenNum_PrimaryScreen);

    //! returns the details of a supported mode (width, height, depth) for a given screen
    virtual ScreenMode   getNthSupportedScreenMode(int n, int screenNum = screenNum_PrimaryScreen);

    //! set the display mode for a particular screen
    //! resulting mode might not be exactly what you asked for, but
    //! could instead be the closest viable mode for that screen
    virtual void   setScreenMode(long width, long height, int screenNum = screenNum_PrimaryScreen, int bpp = 0);

    //! create a port for drawing in a Window. The Window's size and location are given by the rectangle passed in.
    //! bpp is the bits per pixel used for the back buffer of the window. Use bpp of zero (0) to match current screen
    //! color depth settings
    virtual Port*   createWindowPort(const Rect& rect, const char* windName = 0, int bpp = 0);

    //! create a full screen graphics port.
    //! desired screen resolution is in rect, desired screen color depth is in
    //! if allowResChange is true, the screen resolution will be changed to the closest 
    //! possible resolution, and any excess screen real estate will be
    //! erased to black
    //! if bpp is non-zero and doesn't match the current screen settings, then
    //! the screen depth will be changed to match, even if allowResChange is false
    virtual Port*   createFullScreenPort(const Rect& rect, int screenNum = screenNum_PrimaryScreen, 
    						bool allowResChange = true, int bpp = 0);

    //! close a port and clean it up
    //! if port is not passed it will default to the main port, which will trigger
    //! the application to quit
    virtual void    closeGraphicsPort(Port* port = 0);

    //! create a font
    virtual Font*   createFont(const char* fontName, float scalingFactor = 1.0f);

    //! this is the main window port, or the full screen port if in full screen
    virtual Port*   getMainPort();

    //! application developers should rarely, if ever, need to call this.
    //! it is primarily included for cases were someone wants to use the graphics
    //! layer but nothing else
    //! you might also use this before closing the old main port if you are replacing 
    //! it with a new one, that will avoid triggering the app to quit
    virtual void    setMainPort(Port* port);

	//! make a port be fullscreen 
    //! if port is not passed it will default to the main port
	//! if allowResChange is true, the screen resolution will be changed to the closest
	//! possible resolution, and any excess screen real estate will be erased to black
	//! returns true if succeeded, false if failed
	virtual bool    switchToFullScreenMode(bool allowResChange = false, Port* port = 0);

	//! make a port be in a window
    //! if port is not passed it will default to the main port
	//! returns true if succeeded, false if failed
	virtual bool    switchToWindowMode(Port* port = 0, const char* windName = 0);

    //! return true if main port is in fullscreen mode
    virtual bool    inFullScreenMode();

	//! return the current average frames per second
	virtual float	getFPS();

	//! set the desired frames per second
	virtual void	setTargetFPS(float fps);
	
	//! return the current desired frames per second
	virtual float	getTargetFPS();

	// Get the position of the mouse. 
	// On a multi-touch interface, mouseNumber specifies which pointer/finger, in order they started touching
	// On a device with multiple pointing devices, mouseNumber represents individual pointers in an arbitrary fixed order
	// If mouseNumber is greater than the number of pointing devices attached (or fingers down), the
	// location of the primary pointing device is returned (ie: same as getMouse(0) )
    virtual Point   getMouse(int mouseNumber = 0);

// lifecycle
/// @cond C++
    virtual ~GraphicsManager();
/// @endcond

protected:
/// @cond INTERNAL
	static const int kMaxScreens = 32;
	static GraphicsManager* createSingletonInstance();
	GraphicsManager(); // call GraphicsManager::getSingletonInstance() instead
	// screen resolution to restore on program exit
	long  mOldHeight[kMaxScreens];
	long  mOldWidth[kMaxScreens];
	int   mOldDepth[kMaxScreens];
	bool  mWasChanged[kMaxScreens];
	// used when swapping ports between full screen and windowed mode
	bool  mIsFullscreen[kMaxScreens];
	Rect  mSavedWindowPos[kMaxScreens];
	long  mSavedScreenWidth[kMaxScreens];
	long  mSavedScreenHeight[kMaxScreens];
/// @endcond
};


} // end namespace pdg

#endif // PDG_GRAPHICS_MANAGER_H_INCLUDED

