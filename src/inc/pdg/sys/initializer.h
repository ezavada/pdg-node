// -----------------------------------------------
// initializer.h
//
// core application/system interface
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


#ifndef PDG_INITIALIZER_H_INCLUDED
#define PDG_INITIALIZER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"

namespace pdg {

class EventManager;
class ResourceManager;
class ConfigManager;

// -----------------------------------------------------------------------------------
// Initializer
// class which will implement application specific initialization
// consists completely of static methods that the application must implement
// this class is needed because the application main() is inside the framework
// -----------------------------------------------------------------------------------
/// @cond C++

class Initializer {
public:

    // failure codes returned by app at startup
    enum {
        initFail_LogMgr         = -1,
        initFail_ConfigMgr      = -2,
        initFail_ResourceMgr    = -3,
        initFail_EventMgr       = -4,
        initFail_GraphicsMgr    = -5,
        initFail_SoundMgr       = -6,
        initFail_TimerMgr       = -7,
        initFail_NetworkMgr     = -8,
        initFail_InstallHandlers = -9
    };

	// on platforms like iPad that support device orientation, return true
	// or false to report which orientation(s) you support
	static bool allowHorizontalOrientation() throw();
	static bool allowVerticalOrientation() throw();
	
	// this static method will called multiple times to help build window
    // and log file names, and anything else that expects to have the
    // application name. If the resource manager has already opened the main
    // resource file then haveMainResourceFile will be true.
    // You MUST return a valid string. This will be used for log file
    // This is the first call your application will get from the framework
    static const char* getAppName(bool haveMainResourceFile) throw();

    // this static method will be called from the framework before the
    // resource manager opens the main resource file.
    // return NULL if you don't want a resource file opened automatically
    static const char* getMainResourceFileName() throw();

    // this static method will be called from the framework just before the
    // eventType_Startup is sent to the inEventMgr, so that a handler
    // for that event (and others) can be installed in the inEventMgr
    // it is expected that all other app specific initialization can be done
    // from when the startup event is received
    // inEventMgr is the currently initialized Event
    // return false if the handler setup failed so the app can exit
    // this will be called after getMainResourceFileName() is called, so
    // it is safe to use resources if necessary here
    static bool installGlobalHandlers() throw();

    // this static method will be called from the framework before the GUI is
    // started. The inputs will be the current screen resolution
    // inResourceMgr contains the currrent ResourceManager in case you need to
    // read some of this info from resources
    // return true to switch screen to fullscreen resolution returned in ioHeight and ioWidth
    // return false to open a window centered on the screen with a content are of ioHeight and ioWidth
	static bool getGraphicsEnvironmentDimensions(Rect maxWindowDim, Rect maxFullScreenDim,
	                                             long& ioWidth, long& ioHeight, uint8& ioDepth) throw();
};
/// @endcond


} // end namespace pdg


#endif // PDG_INITIALIZER_H_INCLUDED

