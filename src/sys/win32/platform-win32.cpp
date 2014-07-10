// -----------------------------------------------
// platform-win32.cpp
// 
// Windows implementation of platform specific functions
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

///////////////////////////////////////////
//
//  NOTE: Make sure any code you put in Adaptor.cpp
//        is also put in here and vice versa to maintain
//        feature consistency between the standalone
//        and the stadium multiplayer games. -ADD
//
///////////////////////////////////////////

#define WinAPI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>

// #ifndef COMPILER_GCC
// 	#if (NTDDI_VERSION < NTDDI_WIN2K)
// 	#include <shfolder.h>
// 	#endif
// #endif // COMPILER_GCC

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include <iostream>
#include <string>
#include "internals.h"
#include "configResource.h"
#include "pdg/sys/os.h"
#include "graphics-win32.h"

#ifndef PDG_NO_SOUND
  #if !defined (PDG_DONT_USE_DIRECTSHOW_SOUND)
    #include "sound-mgr-directx.h"
    #include "sound-directx.h"
  #endif
#endif // PDG_NO_SOUND


#define WHITESPACE " \t\r\n\v"

int pow2(int n) {
	int x = 1;
	while(x < n) {
		x <<= 1;
	}
	return x;
}


namespace pdg {

// TODO: put this in a header file, perhaps internals.h or internals-win32
extern bool native2path(const char *inNativeFileName, char* outStdFileName, int len);

HINSTANCE gAppInstance = NULL;
HWND gMainHWND = NULL;

//#define DEBUG_DIRECTORY_SETUP

#ifndef DEBUG_DIRECTORY_SETUP
	#define DIR_SETUP_DEBUG_ONLY( exp )
#else
	#define DIR_SETUP_DEBUG_ONLY( exp ) exp
#endif

#pragma warning( disable : 4996 )

const char* platform_setupDirectories(int argc, const char* argv[]) {

    char* cmdLine = WinAPI::GetCommandLineA();  // force the ASCII version
    
    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("got windows command line: [%s]", cmdLine); )

    // extract the path to the application
    // skip any leading whitespace
    while (std::strchr(WHITESPACE, *cmdLine) != NULL) {
           ++cmdLine;
    }

    // sometimes Windows returns this as a unix path with forward slashes
    char* p;
    while ( (p = std::strchr(cmdLine, '/')) != NULL) {
        *p = '\\';
    }
    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("converted command line: [%s]", cmdLine); )

	char currWorkingDir[1024];
	static char stdWorkingDir[1024];
  	WinAPI::GetCurrentDirectoryA(1024, currWorkingDir);
	MAKE_STRING_BUFFER_SAFE(currWorkingDir, 1024);
    std::string workingDirStr = OS::makeCanonicalPath(currWorkingDir, false); // resolving symlinks doesn't actually work on windows, so why pretend?
	pdg::native2path(workingDirStr.c_str(), stdWorkingDir, 1024);

    std::string appPath = cmdLine;
    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("cmd line -> appPath: [%s]", appPath.c_str()); )

	// remove any quotes that occur before the first space
	size_t spaceIdx = appPath.find(' ');
	size_t index = appPath.find('\"');
    if (appPath.npos != index && ((spaceIdx == appPath.npos) || (index < spaceIdx)) ) {
        appPath.erase(index,1);
		index = appPath.find('\"');  // find the end quote
		if (appPath.npos != index) {
			appPath = appPath.substr(0, index); // truncate everything after ending quote
		}
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("dequoted appPath: [%s]", appPath.c_str()); )
/*
    	// escape the spaces that remain
    	index = 0;
    	do {
    		index = appPath.find(' ', index);
    		if (appPath.npos != index) {
    			appPath.replace(index, 1, "^ ");
    			index += 2;
    		}
    	} while (index != appPath.npos);
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("escaped appPath: [%s]", appPath.c_str()); )
*/
    } else {
    	// no quotes, truncate everything after the first space, since those are parameters
    	index = appPath.find(' ');
		if (appPath.npos != index) {
			appPath = appPath.substr(0, index);
		}
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("spaces removed appPath: [%s]", appPath.c_str()); )
    }
	// remove the application name
    index = appPath.rfind('\\');
    if (appPath.npos != index) {
        appPath = appPath.substr(0,index+1);
    } else {
    	// no backslashes means this is only an app name, dump it all
    	appPath = ".\\";
    }
    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("less app name appPath: [%s]", appPath.c_str()); )
    if (!os_isAbsolutePath(appPath.c_str())) {
    	std::string temp = currWorkingDir;
    	temp += '\\';
    	temp += appPath;
    	appPath = temp;
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("added to cwd for absolute path: [%s]", appPath.c_str()); )
    }
    appPath = OS::makeCanonicalPath(appPath.c_str(), false); // resolving symlinks doesn't actually work on windows, so why pretend?
	char stdAppPath[1024];
	pdg::native2path(appPath.c_str(), stdAppPath, 1024);
    pdg::os_setApplicationDirectory(stdAppPath);    // hold this in a global so it can be accessed by OS statics

#ifdef PDG_NODE_MODULE

  	// node modules just use the working directory for data and resources
    pdg::os_setApplicationDataDirectory(stdWorkingDir);
    pdg::os_setApplicationResourceDirectory(stdWorkingDir);    // hold this in a global so it can be accessed by OS statics

#else
//	strcat(stdAppPath, "resources/");
    pdg::os_setApplicationResourceDirectory(stdAppPath);    // hold this in a global so it can be accessed by OS statics

    // figure out where the application is supposed to save its files

  #ifdef COMPILER_GCC
    // currently can't use shfolder.h methods from gcc
    pdg::os_setApplicationDataDirectory(stdAppPath);
  #else
    std::string appDataPath( const_cast<char*>(appPath.c_str()) );
	HINSTANCE hDllInst = WinAPI::LoadLibraryA("shell32.dll");
	if (hDllInst) {
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)WinAPI::GetProcAddress(hDllInst, "DllGetVersion");
		DLLVERSIONINFO info;
		WinAPI::ZeroMemory(&info, sizeof(DLLVERSIONINFO));
		info.cbSize = sizeof(DLLVERSIONINFO);
		if (pDllGetVersion && SUCCEEDED((*pDllGetVersion)(&info))) {
			int folderId = 0;
			if (info.dwMajorVersion >= 5) { // make sure this is 5.0 or later (Win 2k or later)
				// for shell32.dll version 5.0. The file system directory containing
				// application data for all users. A typical path is C:\Documents and
				// Settings\All Users\Application Data.
				folderId = CSIDL_COMMON_APPDATA;
			} else if ( (info.dwMajorVersion == 4) && (info.dwMinorVersion >= 72) ) {   // v4.72 Win98 or later
				// for shell32.dll Version 4.71. The file system directory that serves as
				// a common repository for application-specific data. A typical path is
				// C:\Documents and Settings\username\Application Data. This CSIDL is
				// supported by the redistributable Shfolder.dll for systems that do
				// not have the Microsoft Internet Explorer 4.0 integrated Shell installed.
				folderId = CSIDL_APPDATA;
			}
			if (folderId != 0) {
				char szPath[1024];
              #if (NTDDI_VERSION < NTDDI_WIN2K)
				PFNSHGETFOLDERPATHA pSHGetFolderPath = (PFNSHGETFOLDERPATHA)WinAPI::GetProcAddress(hDllInst, "SHGetFolderPathA");
				if (pSHGetFolderPath && SUCCEEDED((*pSHGetFolderPath)(NULL,
								folderId|CSIDL_FLAG_CREATE,
								NULL, 0, szPath))) {
              #else
				if ( S_OK == SHGetFolderPathW(NULL, folderId, NULL, CSIDL_FLAG_CREATE, (TCHAR *)szPath) ) {
              #endif
            		char buffer[1024];
					buffer[0] = 0;
            		// we load the install directory string from a resource so it can be easily changed
            		WinAPI::LoadStringA(gAppInstance, IDS_INSTALLDIRSTR, buffer, 1024);
					std::strncat(szPath, "\\", 1024);
					std::strncat(szPath, buffer, 1024);
					MAKE_STRING_BUFFER_SAFE(szPath, 1024);
					appDataPath.assign(szPath);
				}
			}
		}
		WinAPI::FreeLibrary(hDllInst);
	}
	char stdAppDataPath[1024];
	pdg::native2path(appDataPath.c_str(), stdAppDataPath, 1024);
    pdg::os_setApplicationDataDirectory(stdAppDataPath);    // hold this in a global so it can be accessed by OS statics
  #endif // !COMPILER_GCC
#endif // !PDG_NODE_MODULE

    DEBUG_ONLY(
		std::cerr << "Using Application directory [" << OS::getApplicationDirectory() << "]" << std::endl;
		std::cerr << "Using Data directory [" << OS::getApplicationDataDirectory() << "]" << std::endl;
		std::cerr << "Using Resource directory [" << OS::getApplicationResourceDirectory() << "]" << std::endl;
	)

  	return stdWorkingDir;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifndef PDG_NO_GUI
    GraphicsManagerWin* winGfxMgr = static_cast<GraphicsManagerWin*>(GraphicsManager::getSingletonInstance());
	PortImplWin* thePortWin = static_cast<PortImplWin*>(winGfxMgr->getMainPort());
#endif // PDG_NO_GUI

	//DEBUG_ONLY( OS::_DOUT("winproc msg(%d) wparam(%d) lparam(%d)", msg, wparam, lparam); )

	switch(msg) {
		case WM_SETCURSOR:
			{
				if (winGfxMgr->privateMaintainWin32HardwareCursor() ) {
				    return true;  // TODO: why do we return true here when we return 0 everywhere else?
				}
			}
			break;
#ifndef PDG_NO_SOUND
  #if !defined (PDG_DONT_USE_DIRECTSHOW_SOUND)
		case WM_PDG_SOUND_NOTIFY:
			{
				// A Sound needs attention.
				SoundDirectX* sound = (SoundDirectX*) lparam;
				if(sound)
				{
					sound->handleGraphEvent();
				}
			}
			return 0;
  #endif // !PDG_DONT_USE_DIRECTSHOW_SOUND
#endif
		case WM_QUIT:
		    OS::exit(0);
		    break;
		default:
		    break;
	}

	// process any messages that we didn't take care of
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}


} // end namespace pdg


#if !defined(PDG_LIBRARY) || defined(PDG_STANDALONE_NODE_APP)

extern "C" int main(int argc, char *argv[]);

//void RedirectIOToConsole();  // in console-win32.cpp

// for gui versions, we have to parse the command line params ourselves
// from what WinMain gives us
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
	char **argv = 0;
	int argc = 0;

//	RedirectIOToConsole();

	pdg::gAppInstance = hInst;   // save the global application instance

    char* cmdLine = WinAPI::GetCommandLineA();  // force the ASCII version

    char* cmdLineCopy = (char*) std::malloc(std::strlen(cmdLine) + 2);
	char* buf = cmdLineCopy;
    argv = (char**) std::malloc(sizeof(char*)); // default allocation for empty command line

    while (*cmdLine) {

        // make space for additional arg
        ++argc;
        argv = (char**) std::realloc(argv, (argc+1) * sizeof(char*));

        // save this arg
        argv[argc-1] = buf;

        while (*cmdLine) {
            // look for break between args
            bool brk = false;
            while (*cmdLine && (std::strchr(WHITESPACE, *cmdLine) != NULL)) {
                ++cmdLine;
                brk = true;
            }
            if (brk) {
                // terminate this argument
                *buf++ = 0;
                break;
            } else {
                *buf++ = *cmdLine++;
            }
        }
    }

    *buf = 0;     // nul terminate final argument
    argv[argc] = 0; // terminate the argv[] list

   	/* Parse it into argv and argc */
    int result = main(argc, argv);

//    std::free(cmdLineCopy);     // clean up our allocations
//    std::free(argv);

    return result;
}

#endif // !defined(PDG_LIBRARY) || defined(PDG_STANDALONE_NODE_APP)


extern "C" {
#include <signal.h>
	void signal_handler(int signo);
	void signal_handler(int signo) {
		// do graceful shutdown so we close the database properly
		pdg::OS::exit(0);
		DEBUG_ONLY( pdg::OS::_DOUT("Quitting because we got signal [%d]", signo); )
	}
}




#if 0

#include "pdg/sys/os.h"
#include "pdg/sys/events.h"
#include "pdg-main.h"

#include "timerids.h"

#include <iostream>
#include <cstdio>
#include <objbase.h>


using std::cout;
using std::endl;

}



// these should be able to go away
#include "pdg/sys/graphicsmanager.h"
#include "pdg/sys/timermanager.h"
#include "graphics-win32.h"
#define RUN_LOOP_DEBUG_ONLY( m ) m



// -----------------------------------------------
// Globals
// -----------------------------------------------

namespace pdg {

// app state tracking
bool gAppIsMinimized = false;
bool gAppIsActive = true;
// modifier key tracking
bool gShiftKeyDown = false;
bool gAltKeyDown = false;
bool gControlKeyDown = false;
bool gMetaKeyDown = false;
// Mouse button state before leaving client area or window
bool gMouseButtonDown = false;
// repeat key tracking
WPARAM gLastKeyDown = 0;

// Image* gHatchImage = 0;
std::string gPausedStr = "";
std::string gClickContStr = "";
bool gGameIsPaused = false;
bool gAlreadyPaused = false;
bool gResizingInProgress = false;
bool gIsMaximized = false;
Rect gCurrentScreenSize;

// -----------------------------------------------
// Platform specific local function declarations
// -----------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int pdg_main(int argc, const char* argv[]);

/*
void pauseGame()
{
	// Don't do anything if we are already paused. If we did this would ruin our gAlreadyPaused boolean below
	// that tells if the game timers were already paused through the pause key. If you want to see how pause can
	// get called twice look at bug #5884. -ADD
	if(gGameIsPaused)
	{
		DEBUG_ONLY( OS::_DOUT("main-win32::pauseGame() Returning...already paused."); )
		return;
	}

	gGameIsPaused = true;
	DEBUG_ONLY( OS::_DOUT("main-win32::pauseGame()"); )
#ifndef PDG_NO_SOUND
	// Mute Sounds
	if(SoundManager::getSingletonInstance())
	{
      #if !defined (PDG_DONT_USE_DIRECTSHOW_SOUND)
		SoundManagerDirectX* dXSndMgr = static_cast<SoundManagerDirectX*>(SoundManager::getSingletonInstance());
		dXSndMgr->mute(true);
      #endif // PDG_DONT_USE_DIRECTSHOW_SOUND
	}
#endif //ndef PDG_NO_SOUND

#ifndef PDG_NO_GUI
	Port* mainPort = GraphicsManager::instance().getMainPort();
	if(gHatchImage && mainPort)
	{
		Rect screenDim(mainPort->getDrawingArea());
		gHatchImage->drawTexture(screenDim);
		Point textPt(screenDim.width()/2, screenDim.height()/2);
		Graphics::Style style = (Graphics::Style)(Graphics::textStyle_Bold + Graphics::textStyle_Centered);
		const int PAUSE_FONT_SIZE = 45;
		int fontHeight = mainPort->getCurrentFont()->getFontHeight(PAUSE_FONT_SIZE, style);
		mainPort->drawText(gPausedStr.c_str(), textPt, PAUSE_FONT_SIZE, style, Color(0xff, 0xff, 0xff));
		textPt.y += fontHeight;
		mainPort->drawText(gClickContStr.c_str(), textPt, PAUSE_FONT_SIZE, style, Color(0xff, 0xff, 0xff));
		gAlreadyPaused = TimerManager::getSingletonInstance()->isPaused();
		if(!gAlreadyPaused)
		{
			TimerManager::instance().pause();        // pause all currently running timers
		}
	}
#endif // PDG_NO_GUI
}

void unpauseGame()
{
	gGameIsPaused = false;
	DEBUG_ONLY( OS::_DOUT("main-win32::unpauseGame()"); )
#ifndef PDG_NO_SOUND
	// UnMute Sounds
  #if !defined (PDG_DONT_USE_DIRECTSHOW_SOUND)
	SoundManagerDirectX* dXSndMgr = static_cast<SoundManagerDirectX*>(SoundManager::getSingletonInstance());
	dXSndMgr->mute(false);
  #endif // PDG_DONT_USE_DIRECTSHOW_SOUND
#endif //ndef PDG_NO_SOUND

	if(!gAlreadyPaused)
	{
		TimerManager::getSingletonInstance()->unpause();        // unpause all currently running timers
	}
}
*/
// pass 0,0 to use previous width and height as recorded in gOldWindWidth and gOldWindHeight


/*
void resizeWindow(long width, long height, bool fullscreen)
{
TODO("Consider whether this should be part of the graphics manager");
#ifndef PDG_NO_GUI
	DEBUG_ONLY( OS::_DOUT("resizeWindow to [%dx%d] [%s] mode", width, height, fullscreen ? "fullscreen":"window"); )
    // use saved screen res
	if ((width == 0) && (height ==0)) {
		DEBUG_ONLY( OS::_DOUT("Resizing window to old dimensions [%dx%d]", gOldWindWidth, gOldWindHeight); )
	    width = gOldWindWidth;
	    height = gOldWindHeight;
	}

	if(!fullscreen)
	{
		// This constrains the width and height to fit on the desktop work area if needed.
		sanitizeToWorkArea(width, height);
	}
	DEBUG_ONLY( OS::_DOUT("After sanitize, dimensions [%dx%d]", width, height); )

	PortImplWin* thePortWin = static_cast<PortImplWin*>(GraphicsManager::instance().getMainPort());

	// Save off old screen res and set new screen res
	gOldWindWidth = gWindWidth;
	gOldWindHeight = gWindHeight;
	gOldFullScreen = gFullScreen;
	gWindWidth = width;
	gWindHeight = height;
	gFullScreen = fullscreen;

	if( gFullScreen && ((thePortWin->mBackBuffer.getWidth() != gWindWidth) ||
		(thePortWin->mBackBuffer.getHeight() != gWindHeight)) ){
		PortImpl* thePort = static_cast<PortImpl*>(GraphicsManager::instance().getMainPort());
		Rect oldPortRect = thePort->getDrawingArea();
	    DEBUG_ONLY( OS::_DOUT("pre mode-swap port size adjustment to [%dx%d]", gWindWidth, gWindHeight); )
		gResizingInProgress = true;
		graphics_setHardwareBusyCursor();
		thePort->resizePort(gWindWidth, gWindHeight);
		Rect actualRect = thePort->getDrawingArea(); // no guarantee that resize port will give us
		gWindWidth = actualRect.width();             // exactly what we passed in
		gWindHeight = actualRect.height();
		PortInfo pi;
		pi.whichPort = thePort;
		pi.screenPos = graphics_getEffectiveScreenPos();
		pi.oldScreenPos = pi.screenPos;
		pi.oldWidth = oldPortRect.width();
		pi.oldHeight = oldPortRect.height();
		EventManager::instance().postEvent(eventType_PortResized, &pi);
	}	// end if fullscreen

    if ( gFullScreen ) {
		thePortWin->unlockDrawingSurface();
    	DEBUG_ONLY( OS::_DOUT("resizeWindow doing Mode Swap"); )
		if (!thePortWin->mBackBuffer.setBufferMode(gWindWidth, gWindHeight, gScreenDepth, gFullScreen)) {
			DEBUG_ONLY( OS::_DOUT("ERROR: SetBufferMode FAILED to switch to above screen mode."); )
			gWindWidth = gOldWindWidth;
			gWindHeight = gOldWindHeight;
			gFullScreen = gOldFullScreen;
		}
		thePortWin->lockDrawingSurface();
		graphics_prepOpenGLForDrawing();
	}	// end if fullscreen

	if( !gFullScreen) {
		PortImpl* thePort = static_cast<PortImpl*>(GraphicsManager::instance().getMainPort());
		Rect oldPortRect = thePort->getDrawingArea();
	    DEBUG_ONLY( OS::_DOUT("post mode-swap resizing port to [%dx%d]", gWindWidth, gWindHeight); )
		gResizingInProgress = true;
		graphics_setHardwareBusyCursor();
		thePort->resizePort(gWindWidth, gWindHeight);
		PortInfo pi;
		pi.whichPort = thePortWin;
		pi.screenPos = graphics_getEffectiveScreenPos();
		pi.oldScreenPos = pi.screenPos;
		pi.oldWidth = oldPortRect.width();
		pi.oldHeight = oldPortRect.height();
		EventManager::instance().postEvent(eventType_PortResized, &pi);
	}
	graphics_setHardwareNormalCursor();

	thePortWin->mNeedRedraw = true;
#endif // PDG_NO_GUI
}
*/

// 
// #define SIZE_OF_KEY_MAP 64
// 
// unichar keyMap[SIZE_OF_KEY_MAP][6] = {
// // Cocoa key				PDG key			modifiers (shift, control, alt/option, meta/cmd )
// { NSUpArrowFunctionKey        , key_UpArrow,   0, 0, 0, 0},
// { NSDownArrowFunctionKey      , key_DownArrow, 0, 0, 0, 0 },
// { NSLeftArrowFunctionKey      , key_LeftArrow, 0, 0, 0, 0 },
// { NSRightArrowFunctionKey     , key_RightArrow, 0, 0, 0, 0 },
// { NSF1FunctionKey             , key_F1, 0, 0, 0, 0 },
// { NSF2FunctionKey             , key_F2, 0, 0, 0, 0 },
// { NSF3FunctionKey             , key_F3, 0, 0, 0, 0 },
// { NSF4FunctionKey             , key_F4, 0, 0, 0, 0 },
// { NSF5FunctionKey             , key_F5, 0, 0, 0, 0 },
// { NSF6FunctionKey             , key_F6, 0, 0, 0, 0 },
// { NSF7FunctionKey             , key_F7, 0, 0, 0, 0 },
// { NSF8FunctionKey             , key_F8, 0, 0, 0, 0 },
// { NSF9FunctionKey             , key_F9, 0, 0, 0, 0 },
// { NSF10FunctionKey            , key_F10, 0, 0, 0, 0 },
// { NSF11FunctionKey            , key_F11, 0, 0, 0, 0 },
// { NSF12FunctionKey            , key_F12, 0, 0, 0, 0 },
// { NSInsertFunctionKey         , key_Insert, 0, 0, 0, 0 },
// { NSDeleteFunctionKey         , key_Delete, 0, 0, 0, 0 },
// { NSHomeFunctionKey           , key_Home, 0, 0, 0, 0 },
// { NSBeginFunctionKey          , key_Home, 0, 0, 0, 0 },
// { NSEndFunctionKey            , key_End, 0, 0, 0, 0 },
// { NSPageUpFunctionKey         , key_PageUp, 0, 0, 0, 0 },
// { NSPageDownFunctionKey       , key_PageDown, 0, 0, 0, 0 },
// { NSPauseFunctionKey          , key_Pause, 0, 0, 0, 0 },
// { NSSysReqFunctionKey         , key_Break, 0, 0, 1, 0 },
// { NSBreakFunctionKey          , key_Break, 0, 0, 0, 0 },
// { NSResetFunctionKey          , key_Break, 0, 0, 1, 1 },
// { NSStopFunctionKey           , key_Break, 0, 1, 0, 0 },
// { NSMenuFunctionKey           , key_Escape, 0, 0, 0, 1 },
// { NSUserFunctionKey           , key_Escape, 0, 0, 1, 0 },
// { NSSystemFunctionKey         , key_Escape, 0, 1, 0, 0 },
// { NSClearLineFunctionKey      , key_Clear, 1, 0, 0, 0 },
// { NSClearDisplayFunctionKey   , key_Clear, 0, 0, 1, 0 },
// { NSInsertLineFunctionKey     , key_Insert, 0, 0, 1, 0 },
// { NSDeleteLineFunctionKey     , key_Delete, 0, 0, 1, 0 },
// { NSInsertCharFunctionKey     , key_Insert, 0, 0, 0, 0 },
// { NSDeleteCharFunctionKey     , key_Delete, 0, 0, 0, 0 },
// { NSPrevFunctionKey           , key_Tab, 1, 0, 0, 0 },
// { NSNextFunctionKey           , key_Tab, 0, 0, 0, 0 },
// { NSExecuteFunctionKey        , key_Enter, 0, 0, 0, 0 },
// { NSUndoFunctionKey           , 'z', 0, 0, 0, 1 },
// { NSRedoFunctionKey           , 'Z', 1, 0, 0, 1 },
// { NSFindFunctionKey           , ' ', 0, 0, 0, 1 },  // cmd - space
// { NSHelpFunctionKey           , key_Help, 0, 0, 0, 0 },
// { 0, 0, 0, 0, 0, 0 }
// };
// 
// unichar remapKeyboardChar(unichar character, int modifierFlags, BOOL& shift, BOOL& control, BOOL& alt, BOOL& cmd) {
// 	shift = ((modifierFlags & NSShiftKeyMask) != 0);
// 	control = ((modifierFlags & NSControlKeyMask) != 0);
// 	alt = ((modifierFlags & NSAlternateKeyMask) != 0);
// 	cmd = ((modifierFlags & NSCommandKeyMask) != 0);
// 	// check for special keys that must be mapped
// 	if (character >= NSUpArrowFunctionKey) {
// 		for (int i = 0; i < SIZE_OF_KEY_MAP; i++) {
// 			if (keyMap[i][0] == character) {
// 				character = keyMap[i][1];  // replace the character
// 				shift |= keyMap[i][2];	   // add additional modifier keys if needed
// 				control |= keyMap[i][3];
// 				alt |= keyMap[i][4];
// 				cmd |= keyMap[i][5];
// 				break;
// 			} else if (keyMap[i][0] == 0) {  // at end of list
// 				break;
// 			}
// 		}
// 	}
// 	return character;
// }
// 


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifndef PDG_NO_GUI
    GraphicsManagerWin* winGfxMgr = static_cast<GraphicsManagerWin*>(GraphicsManager::getSingletonInstance());
	PortImplWin* thePortWin = static_cast<PortImplWin*>(winGfxMgr->getMainPort());
#endif // PDG_NO_GUI

	//DEBUG_ONLY( OS::_DOUT("winproc msg(%d) wparam(%d) lparam(%d)", msg, wparam, lparam); )

	switch(msg) {
	    // Application switching
		case WM_SYSCOMMAND:
			{
				#ifndef PDG_NO_GUI
				switch(wparam)
				{
				case SC_KEYMENU:
    				if (VK_RETURN == lparam) {
						DEBUG_ONLY( OS::_DOUT("got ALT-ENTER"); )
						// treat alt-enter and ctrl-enter as mode swaps
						TODO("Handle ALT-ENTER");
		//				gFullScreen = winGfxMgr->setMainPortFullScreenMode(!gFullScreen);
						return 0; // We handled this message.
    				}
					break;
				};
				#endif // ndef PDG_NO_GUI
			}
			break;

	  #ifndef PDG_NO_GUI
		case WM_ACTIVATEAPP:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM_ActivateApp");
          #endif
			gAppIsActive = wparam;
			if (!gAppIsMinimized && thePortWin) {
				thePortWin->mNeedRedraw = true;
		    }

			break;
		case WM_ACTIVATE:
			{
				DEBUG_ONLY( OS::_DOUT("got WM_activate"); )
				gAppIsMinimized = HIWORD(wparam);
				if (thePortWin && !gAppIsMinimized)
				{
					thePortWin->mNeedRedraw = true;
				}

				// If we are not already paused then pause the game
				bool appIsActive = LOWORD(wparam) == WA_ACTIVE || LOWORD(wparam) == WA_CLICKACTIVE;
				TODO("Handle full screen task switch");
				if(/*gFullScreen &&*/ appIsActive)
				{
					// HACK HERE: When you are in full screen mode and you task switch
					// the memory card blows away the DD surface to reallocate memory elsewhere.
					// Thus after we come back we need to refill the back buffer after it restores
					// the DD surface which takes time. There might be a better way to do this. -ADD
					TimerManager::instance().startTimer(PDG_FULL_SCREEN_ACTIVATE_REDRAW_TIMER, 500);
				}
			#ifndef _ACTIVEXCTRL
				if(appIsActive)
				{
				// ERZ, removed this so that unpausing will not automatically happen with an
				// activate event, instead the user must click to unpause. If clicking in the
				// content area, this is ideal, because the click both activates and unpauses,
				// but if they activate somehow (such as clicking on title bar) that doesn't deliver
				// a click to the window, then the app will still be paused in the foreground
				// However, this was an acceptable compromise for to solve the problem of people
				// clicking in content to activate/unpause, but the click being processed as an
				// action, in some cases an irreversable one such as choosing where to steal from
//						DEBUG_ONLY( OS::_DOUT("Unpausing from WM_ACTIVATE"); )
//						unpauseGame();
				}
				else
				{
					DEBUG_ONLY( OS::_DOUT("Pausing from WM_ACTIVATE"); )
// #ifndef NO_LOSE_FOCUS_PAUSE
// 					pauseGame();
// #endif
				}
				#endif //_ACTIVEXCTRL
			}
			break;

		case WM_CREATE:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM_create");
          #endif
			if(thePortWin)
			{
				thePortWin->mNeedRedraw = true;
			}
			return 0;
	  #endif // PDG_NO_GUI

	    // key events
		case WM_KEYDOWN:
		case WM_KEYUP:
		  #ifdef DEBUG_EVENTS
            DEBUG_ONLY( OS::_DOUT("got WM_key up/down"); )
          #endif
		    {
				// Fix for bug no. 5869 [
				if(gGameIsPaused)
					break;
				// Fix for bug no. 5869 ]
		        bool keyState = (msg == WM_KEYDOWN);
				bool sendKeyPress = false;
		        if (wparam == VK_SHIFT) { // shift
		            gShiftKeyDown = keyState;
		        } else if (wparam == VK_CONTROL) { // control
		            gControlKeyDown = keyState;
//		        } else if (wparam == VK_MENU) {    // alt
//		            gAltKeyDown = keyState;
		        } else if ( (wparam == VK_LWIN) || (wparam == VK_RWIN) ) {      // meta ?
		            gMetaKeyDown = keyState;
		        }
		        if (!keyState) {
		            gLastKeyDown = 0;   // if this is a key up, clear the last keydown
		        }
				// wparam - The virtual-key code of the nonsystem key. See Virtual-Key Codes.
				// lparam bit fields from Win32 API docs
				// 0-15  - The repeat count for the current message. The value is the number of times the keystroke 
				//         is autorepeated as a result of the user holding down the key. If the keystroke is held long 
				//         enough, multiple messages are sent. However, the repeat count is not cumulative.
				// 16-23 - The scan code. The value depends on the OEM.
				// 24    - Indicates whether the key is an extended key, such as the right-hand ALT and CTRL 
				//         keys that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an 
				//         extended key; otherwise, it is 0.
				// 25-28 - Reserved; do not use.
                // 29    - The context code. The value is always 0 for a WM_KEYDOWN message.
                // 30    - The previous key state. The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
                // 31    - The transition state. The value is always 0 for a WM_KEYDOWN message.
                // ((lparam >> 16) & 0x1ff) would be physical key scan code + extended code
                // but we are just going to use the virtual key code
				int keyCode = wparam; 

		        if (wparam != gLastKeyDown) { // as long as this is not a repeat, send the event
		            if (keyState) {
		                gLastKeyDown = wparam;  // remember last keydown, so we can avoid resending it
		            }
    		        if ( keyState && (VK_RETURN == wparam) && (gAltKeyDown || gControlKeyDown) ) {
						// This had Alt Enter code in it that wasn't being executed.
    		        }
    		        KeyInfo ki;
    		        ki.keyCode = wparam;
    		        
    		        // TODO: more comprehensive key remapping using something like remapKeyboardChar above
					if (ki.keyCode == VK_DELETE) {
						wparam = key_Delete;
						sendKeyPress = true;
					} else if (ki.keyCode == VK_UP) {
						wparam = key_UpArrow;
						sendKeyPress = true;
					} else if (ki.keyCode == VK_DOWN) {
						wparam = key_DownArrow;
						sendKeyPress = true;
					} else if (ki.keyCode == VK_LEFT) {
						wparam = key_LeftArrow;
						sendKeyPress = true;
					} else if (ki.keyCode == VK_RIGHT) {
						wparam = key_RightArrow;
						sendKeyPress = true;
					}else if (ki.keyCode == VK_END) {
						wparam = key_End;
						sendKeyPress = true;
					}else if (ki.keyCode == VK_HOME) {
						wparam = key_Home;
						sendKeyPress = true;
					}else if (ki.keyCode == VK_PRIOR) {
						wparam = key_PageUp;
						sendKeyPress = true;
					}else if (ki.keyCode == VK_NEXT) {
						wparam = key_PageDown;
						sendKeyPress = true;
					}
    		        if (keyState) {
        				main_handleKeyDown(keyCode, wparam);
        			} else {
        				main_handleKeyDown(keyCode, wparam);
        			}
			// CATAN SPECIFIC STUFF
// 					else if((eventType == eventType_KeyDown) &&
// 						((gControlKeyDown && (ki.keyCode == (long)'P' || ki.keyCode == (long)'p'))
// 						|| ki.keyCode == VK_PAUSE))
// 					{
// 						// Catch the Ctrl-P and Pause key for pausing
// 						if(gGameIsPaused)
// 						{
// 							DEBUG_ONLY( OS::_DOUT("Unpausing from Keyboard"); )
// 							unpauseGame();
// 						}
// 						else
// 						{
// 							DEBUG_ONLY( OS::_DOUT("Pausing from Keyboard"); )
// 							pauseGame();
// 						}
// 					}
                }
				/*	10/4/04 ADD Commented out the CTRL-ESC because if you hit CTRL-ALT-DEL and then ESC then ESC
						on our app, windows thinks the CTRL key is still down for some reason and we exit.
		        if ( keyState && (VK_ESCAPE == wparam) && gControlKeyDown ) {
		            // treat control-esc as emergency exit
					OS::_DOUT("Got CTRL-ESC for emergency exit. Exiting...");
		            gExit = true;
		        }*/
				if (!sendKeyPress || (msg == WM_KEYUP)) {	// we sometimes want to send a keypress when windows only gives us a keydown
					break;
				} else {
					lparam = 1;  // for now any keypress we generate is not a repeat
			    }
			}
		case WM_CHAR:
		  #ifdef DEBUG_EVENTS
            DEBUG_ONLY( OS::_DOUT("got WM_char"); )
          #endif
		    {
				// Fix for bug no. 5869 [
// 				if(gGameIsPaused)
// 					break;
				// Fix for bug no. 5869 ]
		        // the isRepeating flag will not be 100% accurate on Windows, sometimes
		        // it will give false negatives
		        bool isRepeating = ( LOWORD(lparam) > 1 );
		        bool alt = WinAPI::GetKeyState(VK_MENU); // check the alt key separately
        		main_handleKeyPress(
        			wparam,   // Note: wparam is UTF16 on most Win32, UTF32 on WinXP
        			isRepeating, gShiftKeyDown, gControlKeyDown, alt, gMetaKeyDown);
		    }
			break;

        // mouse events
	  #ifndef PDG_NO_GUI
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM mouse button up/down");
          #endif
			if (gAppIsActive) {
// 				if(gGameIsPaused)
// 				{
// 					unpauseGame();
// 					break;
// 				}
				// figure out which button it was
				int button = ((msg==WM_LBUTTONDOWN)||(msg==WM_LBUTTONUP)) ? 0 : ((msg==WM_RBUTTONDOWN)||(msg==WM_RBUTTONUP)) ? 1 : 2;
		        long eventType;
				if ((msg==WM_LBUTTONDOWN) || (msg==WM_MBUTTONDOWN) || (msg==WM_RBUTTONDOWN)) {
					eventType = eventType_MouseDown;
				} else {
					eventType = eventType_MouseUp;
				}
				// update the button states
				if (button >= 0 && button < MAX_MOUSE_BUTTONS) {
					gButtonStates[button] = (eventType == eventType_MouseDown);
				}
				// figure out if this represents a new mouse or finger we weren't tracking before
				if ((button > gMaxAttachedMouse) && (button < MAX_POINTERS)) {
					gMaxAttachedMouse = button;
				}
		        MouseInfo mi;
		        unsigned long ms = OS::getMilliseconds();
		        mi.lastClickElapsed = ms - gLastClickMillisec;
		        mi.mousePos.x = GET_X_LPARAM(lparam);
		        mi.mousePos.y = GET_Y_LPARAM(lparam);

				// ADDED ON 5th Mar 2004 BY SURAJ GOGOI
				// This will retain the mouse events
				// irrespective of the window size
				//gBackBuffer.TransformPoint(mi.mousePos);

		        mi.lastClickPos = gLastClickPos;
				mi.leftButton = gButtonStates[0];
				mi.rightButton = gButtonStates[1];
				mi.buttonNumber = button;
		        mi.shift = gShiftKeyDown;
		        mi.ctrl = gControlKeyDown;
		        mi.alt = WinAPI::GetKeyState(VK_MENU); // check the alt key separately
		        mi.meta = gMetaKeyDown;
				if (button >= 0 && button <= gMaxAttachedMouse) {
					gMouseLoc[button] = mi.mousePos;
				}
		        if (eventType == eventType_MouseDown) {
    		        gLastClickMillisec = ms;
    		        gLastClickPos = mi.mousePos;
		        }
                EventManager::instance().postEvent(eventType, &mi);
		    }
        case WM_MOUSEMOVE:
			if (gAppIsActive && !gGameIsPaused) {
                MouseInfo mi;
				unsigned long ms = OS::getMilliseconds();
				bool isDown[3] = { (wparam & MK_LBUTTON), (wparam & MK_RBUTTON), (wparam & MK_MBUTTON) };
				// fix incorrect button states for events that happened outside our window
				for (int button = 0; button<3; button++) {
					if (gButtonStates[button] != isDown[button]) {
						// mouse button state changed after going out of game window
						gButtonStates[button] = isDown[button];
						mi.lastClickPos = gLastClickPos;
						mi.lastClickElapsed = ms - gLastClickMillisec;
						if (isDown[button]) {
							gLastClickMillisec = ms;
							gLastClickPos = mi.mousePos;
						}
						mi.mousePos.x = -1;
						mi.mousePos.y = -1;
						mi.shift = gShiftKeyDown;
						mi.ctrl = gControlKeyDown;
						mi.alt = WinAPI::GetKeyState(VK_MENU); // check the alt key separately
						mi.meta = gMetaKeyDown;
						gMouseLoc[button] = mi.mousePos;
						EventManager::instance().postEvent( isDown[button] ? eventType_MouseDown : eventType_MouseUp, &mi);
						ms = OS::getMilliseconds();
					}
				}

				if (thePortWin->getCursor()) {
					thePortWin->mNeedRedraw = true;
				}
		        mi.lastClickElapsed = ms - gLastClickMillisec;
		        mi.mousePos.x = GET_X_LPARAM(lparam);
		        mi.mousePos.y = GET_Y_LPARAM(lparam);
				mi.lastClickPos = gLastClickPos;
		        mi.leftButton = (wparam & MK_LBUTTON);
		        mi.rightButton = (wparam & MK_RBUTTON);
		        mi.shift = gShiftKeyDown;
		        mi.ctrl = gControlKeyDown;
		        mi.alt = WinAPI::GetKeyState(VK_MENU); // check the alt key separately
		        mi.meta = gMetaKeyDown;
		        gMouseLoc[0] = mi.mousePos;
		        EventManager::instance().postEvent(eventType_MouseMove, &mi);
		    }
            break;
		case WM_MOVING:
			{
				PAINTSTRUCT	ps;
				if (thePortWin && thePortWin->mWindow) {
					WinAPI::BeginPaint(thePortWin->mWindow, &ps);
    				thePortWin->mBackBuffer.updateFrontBuffer();
    				WinAPI::EndPaint(thePortWin->mWindow, &ps);
				}
				return true;  // TODO: why do we return true here when we return 0 everywhere else?
			}
			break;
		case WM_SIZE:
			{
/*				long newWidth  = LOWORD(lparam);
				long newHeight = HIWORD(lparam);
				Rect newSize(newWidth, newHeight);
				Rect screenSize( WinAPI::GetSystemMetrics(SM_CXSCREEN), WinAPI::GetSystemMetrics(SM_CYSCREEN) );

				if (wparam == SIZE_RESTORED) {
					DEBUG_ONLY( OS::_DOUT("got WM_SIZE -> SIZE_RESTORED [%dx%d]", newWidth, newHeight); )
					// 1. See if screen resolution changed
					// 2. See if we are in windowed mode
					// Then: Do sanitize. Resize if need to. Set new screen size.
					if(gCurrentScreenSize != screenSize && !gFullScreen)
					{
						DEBUG_ONLY( OS::_DOUT("Detected current screen size does not match cached size."); )
						// Sanitize area to what the Back Buffer wants
						sanitizeToWorkArea(newWidth, newHeight);
						Rect newSanSize(newWidth, newHeight);

						PortImpl* thePort = static_cast<PortImpl*>(gPort);
						if (newSanSize != thePort->getDrawingArea()) {	// only do this if port size actually changes
							DEBUG_ONLY( OS::_DOUT("Resizing because newSanSize[%dx%d] doesn't match port drawing area[%dx%d].",
								newSanSize.width(), newSanSize.height(), thePort->getDrawingArea().width(),
								thePort->getDrawingArea().height()); )
							DEBUG_ONLY( OS::_DOUT("Resizing to [%dx%d]", newWidth, newHeight); )

							resizeWindow(newWidth, newHeight, gFullScreen);

							// Save off our new width and height
							if(gConfigManager)
							{
								gConfigManager->setConfigLong(PDG_CONFIG_LAST_WINDOW_WIDTH, newWidth);
								gConfigManager->setConfigLong(PDG_CONFIG_LAST_WINDOW_HEIGHT, newHeight);
							}
						}
						// Set the new cached screen size.
						gCurrentScreenSize = screenSize;
					}
				} */
				return 0;
			}
			break;
			case WM_SIZING:
			{
				DEBUG_ONLY( OS::_DOUT("got WM_SIZING"); )
				RECT rc = *((RECT*)lparam);
				Rect dragArea(rc.right - rc.left, rc.bottom - rc.top);
				RECT adjRect;
				adjRect.top = adjRect.right = adjRect.bottom = adjRect.left = 0;
				WinAPI::AdjustWindowRect(&adjRect, WS_OVERLAPPEDWINDOW, false);
				Rect adjustRect(adjRect.right - adjRect.left, adjRect.bottom - adjRect.top);

				Rect clientArea = dragArea - adjustRect;
				// Make sure our client area doesn't shrink below minimum
			  #ifdef CATAN_SPECIFIC
				int clientWidth = (clientArea.width() < CATAN_MIN_WINDOW_WIDTH) ? CATAN_MIN_WINDOW_WIDTH : clientArea.width();
				int clientHeight = (clientArea.height() < CATAN_MIN_WINDOW_HEIGHT) ? CATAN_MIN_WINDOW_HEIGHT : clientArea.height();
				clientArea.setWidth(clientWidth);
				clientArea.setHeight(clientHeight);
			  #else
			    int clientWidth = clientArea.width();
			    int clientHeight = clientArea.height();
              #endif // CATAN_SPECIFIC

				switch(wparam)
				{
				case WMSZ_BOTTOMLEFT:
				case WMSZ_LEFT:
					rc.left = rc.right - clientWidth - adjustRect.width();
					rc.bottom = rc.top + clientHeight + adjustRect.height();
					break;
				case WMSZ_BOTTOMRIGHT:
				case WMSZ_RIGHT:
					rc.right = rc.left + clientWidth + adjustRect.width();
					rc.bottom = rc.top + clientHeight + adjustRect.height();
					break;
				case WMSZ_TOPLEFT:
				case WMSZ_TOP:
					rc.left = rc.right - clientWidth - adjustRect.width();
					rc.top = rc.bottom - clientHeight - adjustRect.height();
					break;
				case WMSZ_TOPRIGHT:
					rc.right = rc.left + clientWidth + adjustRect.width();
					rc.top = rc.bottom - clientHeight - adjustRect.height();
					break;
				case WMSZ_BOTTOM:
					rc.right = rc.left + clientWidth + adjustRect.width();
					rc.bottom = rc.top + clientHeight + adjustRect.height();
					break;
				};
				*((RECT*)lparam) = rc;

				PAINTSTRUCT	ps;
				if (thePortWin && thePortWin->mWindow) {
					WinAPI::BeginPaint(thePortWin->mWindow, &ps);
    				thePortWin->mBackBuffer.updateFrontBuffer();
    				WinAPI::EndPaint(thePortWin->mWindow, &ps);
				}

				if(thePortWin)
				{
					RECT winrect = *((RECT*)lparam);
					int resizeWidth = winrect.right - winrect.left;
					int resizeHeight = winrect.bottom - winrect.top;
					Rect newSize(resizeWidth, resizeHeight);
					if (newSize != thePortWin->getDrawingArea()) {	// only do this if port size actually changes
						//below line added to keep the opposite side anchored while dragged for resizing
						thePortWin->mBackBuffer.setWindowLeftTop(rc.left,rc.top);
						resizeWindow(newSize.width(), newSize.height(), false); // only happens when in windowed mode
					}

					// now tell windows what the window size will actually be
//					winrect = thePortWin->mBackBuffer.getWindowFrameRect();
//					*((RECT*)lparam) = winrect;

					// while live resizing, windows doesn't call the normal event dispatcher, so
					// we do a mini version of main drawing loop here
					TimerManager::instance().checkTimers();
				#ifndef PDG_NO_EVENT_QUEUE
					// check of the event queue in case someone put an event in the queue while the
					// main thread was sleeping
					long eventType;
					UserData* eventData;
					EventEmitter* eventEmitter;
					int eventCount = 0;
					while (EventManager::instance().getQueuedEvent(eventType, eventData, eventEmitter)) {
						EventManager::instance().postEventToEmitter(
							eventType, eventData->getData(), eventEmitter);
						eventData->release();
						eventData = 0;
						eventEmitter = 0;
						eventCount++;
					}
				#endif // NO_EVENT_QUEUE
 					thePortWin->unlockDrawingSurface();
					thePortWin->mBackBuffer.updateFrontBuffer();
					thePortWin->lockDrawingSurface();
					graphics_prepOpenGLForDrawing();
 

					bool buttonDown = (WinAPI::GetAsyncKeyState(VK_LBUTTON) & 0x8000);
					if (!buttonDown) {
						DEBUG_ONLY( OS::_DOUT("got WM_SIZING with mouse button up"); )
						// Save off our new width and height
						ConfigManager::instance().setConfigLong(PDG_CONFIG_LAST_WINDOW_WIDTH, newSize.width());
						ConfigManager::instance().setConfigLong(PDG_CONFIG_LAST_WINDOW_HEIGHT, newSize.height());
					}
				}
			}
			return 0;
			break;
		case WM_PAINT:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM_paint");
          #endif
			if(thePortWin)
			{
				thePortWin->mNeedRedraw = true;
			}
			return 0;
		case WM_CLOSE:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM_close");
          #endif
			thePortWin->resetCursor();
			WinAPI::DestroyWindow(thePortWin->mWindow);
			return 0;
		case WM_DESTROY:
		  #ifdef DEBUG_EVENTS
            OS::_DOUT("got WM_destroy");
          #endif
			thePortWin->resetCursor();
			thePortWin->mWindow = NULL;
			thePortWin->mBackBuffer.destroy();
			WinAPI::PostQuitMessage(0);
			return 0;
	  #endif // PDG_NO_GUI
		default:
		    break;
	}

}


} // end namespace pdg

// -----------------------------------------------
// Real Main entry point
// -----------------------------------------------
#ifdef PDG_NO_GUI

// non-gui, we can use the Win CUI entry point, which is basically
// a standard C/C++ entry point
int main(int argc, const char* argv[]) {
// install a signal handler for all signals
      signal(SIGABRT, signal_handler);
//      signal(SIGFPE, signal_handler);
//      signal(SIGLL, signal_handler);
	signal(SIGINT, signal_handler);  // SIGINT (ctrl-c)
//      signal(SIGSEGV, signal_handler);
      signal(SIGTERM, signal_handler);
//      signal(SIGBREAK, signal_handler);

	return pdg_main(argc, argv);
}

#else



#endif

#endif // 0