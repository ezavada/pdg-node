// -----------------------------------------------
// pdg-main.cpp
// 
// Main loop for PDG
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

#define PDG_DEBUG_KEYBOARD

#include "pdg_project.h"

#include "pdg/msvcfix.h" // fixes GCC too

#include "pdg/sys/core.h"
#include "pdg/sys/config.h"
#include "pdg/sys/events.h"
#include "pdg/sys/os.h"
#include "pdg/sys/resource.h"
#include "pdg/sys/semaphore.h"
#include "pdg/sys/log.h"

#include "log-impl.h"

#ifndef PDG_NO_NETWORK
  #include "pdg/sys/network.h"
  #include "network-posix.h"
#endif

#ifndef PDG_NO_GUI
  #include "pdg/sys/graphics.h"
  #include "graphics-opengl.h"
#endif

#ifndef PDG_NO_SOUND
  #include "pdg/sys/sound.h"
#endif

#include "pdg-main.h"
#include "pdg-lib.h"
#include "internals.h"

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>

using std::cout;
using std::endl;

//#define DEBUG_EVENTS
//#define DEBUG_MOUSE_EVENTS

//#define PDG_DEBUG_RUN_LOOP

#ifdef PDG_NO_GUI

  #define MAX_UI_SLEEP_INTERVAL 1000  // once per second should be fine for polling OS events when
                                        // a non-gui version has no timers and no network activity
#else

  #define MAX_UI_SLEEP_INTERVAL 25 // max interval (in milliseconds) for polling UI
                                     // this means that baring any other timers or anything
                                     // we will be checking mouse position 40 times per second
                                     // and our max frame rate will be 40 fps
                                     // other good values:  40 = 25 timer per sec
                                     //                     25 = 40 times per sec
                                     //                     20 = 50 times per sec
                                     //                     10 = 100 times per sec
  #define MAX_POINTERS 16    // enough for 4 people to each have 4 fingers down simultaneously
  #define MAX_MOUSE_BUTTONS  256
  #define MAX_RAW_KEYS 256
  #define MAX_CHAR_KEYS 0x7fff

#endif // PDG_NO_GUI


//#define DRAWING_DEBUG_ONLY(_expression) _expression
#define DRAWING_DEBUG_ONLY(_expression)


#ifndef PDG_DEBUG_RUN_LOOP
#define RUN_LOOP_DEBUG_ONLY(_expression)
#else
#define RUN_LOOP_DEBUG_ONLY DEBUG_ONLY
#endif

// -----------------------------------------------
// Platform & Compiler specific defines & includes
// -----------------------------------------------

#if defined (__GNUC__)
namespace std {
	using ::trunc;
}
#endif

// -----------------------------------------------
// Globals
// -----------------------------------------------

//#define HACK_TEST_SCML_PDG

#ifdef HACK_TEST_SCML_PDG
	#include "SCML_pdg.h"
	using namespace std;
	using namespace SCML_pdg;
	extern list<Entity*> entities;
	uint32 gLastMs = 0;
#endif


namespace pdg {

bool gExit = false;
int  gExitCode = 0;

Semaphore gWakeupSemaphore; // general semaphore used to wake up the framework if something
                            // interesting happens, such as a network event

// -----------------------------------------------
// some globals only used for GUI builds
// -----------------------------------------------
#ifndef PDG_NO_GUI

bool gFullScreen = false;
long gWindWidth;
long gWindHeight;
long gOldWindWidth;
long gOldWindHeight;
uint8 gScreenDepth;

bool gHardwareCursorVisible = true;
bool gButtonStates[MAX_MOUSE_BUTTONS];
bool gRawKeyStates[MAX_RAW_KEYS];
bool gCharKeyStates[MAX_CHAR_KEYS];

// click tracking
Point   gMouseLoc[MAX_POINTERS];
Point   gLastClickPos;
uint32  gLastClickMillisec;
uint32  gFPSBaseMs = 0;
int gMaxAttachedMouse = 0;

int gFPSFrameCount = 0;
float gCurrentFPS = 0;
float gTargetFPS = 40; // fps
uint32 gNextRedrawMillisec = 0;
uint32 gFrameNum = 0;

#endif // PDG_NO_GUI

log gDebugLog;


#define LOG_LEVEL_NOT_SPECIFIED -2

#ifndef PDG_NO_GUI
bool main_isFullscreen() {
	return gFullScreen;
}

float  main_getCurrentFPS() {
	return gCurrentFPS;
}

void main_setTargetFPS(float fps) {
	if (fps <= 0) {
		fps = 30.0;
	}
	gTargetFPS = fps;
}

float main_getTargetFPS() {
	return gTargetFPS;
}
#endif //!PDG_NO_GUI

log& main_getDebugLog() {
	return gDebugLog;
}

int main_getLogLevelFromArgs(int argc, const char* argv[], int defaultLevel) {
    int loglevel = LOG_LEVEL_NOT_SPECIFIED;
    for (int i = 1; i<argc; i++) {
        if (std::strncmp(argv[i], "-loglevel=", 10) == 0) {
            const char* s = &argv[i][10];
            if (std::strcmp(s, "none") == 0) {
                loglevel = log::none;
            } else if (std::strcmp(s, "fatal") == 0) {
                loglevel = log::fatal;
            } else if (std::strcmp(s, "error") == 0) {
                loglevel = log::error;
            } else if (std::strcmp(s, "inform") == 0) {
                loglevel = log::inform;
            } else if (std::strcmp(s, "detail") == 0) {
                loglevel = log::detail;
            } else if (std::strcmp(s, "verbose") == 0) {
                loglevel = log::verbose;
            } else if (std::strcmp(s, "trace") == 0) {
                loglevel = log::trace;
            }
            if (loglevel == LOG_LEVEL_NOT_SPECIFIED) {
                loglevel = std::atol(&argv[i][10]);
            }
            DEBUG_ONLY( OS::_DOUT("Using log level specified by -loglevel=%d", loglevel); )
        }
    }
    if (loglevel == LOG_LEVEL_NOT_SPECIFIED) {
        loglevel = defaultLevel;
    }
    return loglevel;
}

// look to see if -noexit was passed on command line
bool main_getNoExitFromArgs(int argc, const char* argv[]) {
    for (int i = 1; i<argc; i++) {
        if (std::strcmp(argv[i], "-noexit") == 0) {
            DEBUG_ONLY( OS::_DOUT("Will automatically restart app as specified by -noexit"); )
            return true;
        }
    }
    return false;
}


#ifndef PDG_NO_GUI
	
void main_handleScreenUpdate(int screenPos) {
	int oldEffectiveScreenPos = graphics_getEffectiveScreenPos();
	graphics_setScreenPos(screenPos);
	if (main_isFullscreen()) {
		platform_getScreenSize(&gWindWidth, &gWindHeight, screenNum_PrimaryScreen);
	}
	PortImpl* mainPort = dynamic_cast<PortImpl*>(GraphicsManager::instance().getMainPort());
	if (mainPort && ( oldEffectiveScreenPos != graphics_getEffectiveScreenPos() )) {
		Rect oldPortRect = mainPort->getDrawingArea();
		mainPort->resizePort(gWindWidth, gWindHeight);
		PortResizeInfo pi;
		pi.port = mainPort;
		pi.screenPos = screenPos;
		pi.oldScreenPos = oldEffectiveScreenPos;
		pi.oldWidth = oldPortRect.width();
		pi.oldHeight = oldPortRect.height();
		EventManager::instance().postEvent(eventType_PortResized, &pi);
	}
}

void main_setInitialScreenState(int screenPos, bool fullscreen) {
	gFullScreen = fullscreen;
	graphics_getApplicationSupportedOrientations();
	graphics_setScreenPos(screenPos);
}

void main_handleLiveResize(int currWidth, int currHeight) {
	PortImpl* mainPort = dynamic_cast<PortImpl*>(GraphicsManager::instance().getMainPort());
	if (mainPort) {
		Rect oldPortRect = mainPort->getDrawingArea();
		gWindWidth = currWidth;
		gWindHeight = currHeight;
		mainPort->resizePort(currWidth, currHeight);
		PortResizeInfo pi;
		pi.port = mainPort;
		pi.screenPos = graphics_getEffectiveScreenPos();
		pi.oldScreenPos = pi.screenPos;
		pi.oldWidth = oldPortRect.width();
		pi.oldHeight = oldPortRect.height();
		EventManager::instance().postEvent(eventType_PortResized, &pi);
	}
}
#endif // ! PDG_NO_GUI

// -----------------------------------------------
// Main initialization
// -----------------------------------------------

int main_initManagers() {
    if (!LogManager::getSingletonInstance()) { return Initializer::initFail_LogMgr; }
    if (!ConfigManager::getSingletonInstance()) { return Initializer::initFail_ConfigMgr; }
    if (!ResourceManager::getSingletonInstance()) { return Initializer::initFail_ResourceMgr; }
    if (!EventManager::getSingletonInstance()) { return Initializer::initFail_EventMgr; }
    if (!TimerManager::getSingletonInstance()) { return Initializer::initFail_TimerMgr; }
  #ifndef PDG_NO_GUI
    if (!GraphicsManager::getSingletonInstance()) { return Initializer::initFail_GraphicsMgr; }
  #endif // PDG_NO_GUI
  #ifndef PDG_NO_SOUND
    if (!SoundManager::getSingletonInstance()) { return Initializer::initFail_SoundMgr; }
  #endif // PDG_NO_SOUND
  #ifndef PDG_NO_NETWORK
    if (!NetworkManager::getSingletonInstance()) { return Initializer::initFail_NetworkMgr; }
  #endif // PDG_NO_NETWORK
	return 0; // no error, everything is fine
}

void main_initKeyStates() {
  #ifndef PDG_NO_GUI
    for (int i = 0; i< MAX_RAW_KEYS; i++) { gRawKeyStates[i] = false; }	  
    for (int i = 0; i< MAX_CHAR_KEYS; i++) { gCharKeyStates[i] = false; }	  
    for (int i = 0; i< MAX_MOUSE_BUTTONS; i++) { gButtonStates[i] = false; }
  #endif //! PDG_NO_GUI
}

int main_init(int argc, const char* argv[], bool isInitialized) {

	pdg_LibInit();  // this initializes the managers, key states and directories

    // these will be not be reinitialized if isInitialized = true (ie: we are using -noexit from cmd line for a server)
	if (!isInitialized) {
	
		std::string logfilename = Initializer::getAppName(0);
	#ifdef DEBUG
		int defaultLogLevel = log::verbose;
	#else
		int defaultLogLevel = log::inform;
	#endif
        LogManager::instance().initialize(logfilename.c_str(), LogManager::init_AppendToExisting);
		LogManager::instance().setLogLevel(main_getLogLevelFromArgs(argc, argv, defaultLogLevel));
		gDebugLog.setLogManager(LogManager::getSingletonInstance());
		
		DEBUG_ONLY(OS::_DOUT("App Base dir [%s]",OS::getApplicationDirectory() ); )
		DEBUG_ONLY(OS::_DOUT("App Data dir [%s]", OS::getApplicationDataDirectory() ); )
		DEBUG_ONLY(OS::_DOUT("App Res dir [%s]", OS::getApplicationResourceDirectory() ); )
		
		// use appropriate config for this app
		std::string configname = Initializer::getAppName(0);
        ConfigManager::instance().useConfig(configname.c_str());

		// open the main resource file
	#ifndef PDG_NO_ZIP
		const char* resfilename = Initializer::getMainResourceFileName();
		if (resfilename) {
			ResourceManager::instance().openResourceFile(resfilename);
		}
	#endif
        // always open the Application Resource Directory
        ResourceManager::instance().openResourceFile("./");
        DEBUG_ONLY(
            std::string wdstr = OS::getApplicationDirectory();
            wdstr += "/resources";
            ResourceManager::instance().openResourceFile(wdstr.c_str());
        )
	}

	// call the static method implemented by the application that will setup 
	// the EventManager and install the application's event handlers
	if (!Initializer::installGlobalHandlers()) {
		std::cerr << "application failed to install global handlers at startup" << std::endl;
        return Initializer::initFail_TimerMgr;
    }
	
  #ifndef PDG_NO_GUI
 	if (!isInitialized) {

		long screenHeight;
		long screenWidth;
		platform_getScreenSize(&screenWidth, &screenHeight, screenNum_PrimaryScreen);
		platform_getMaxWindowSize(&gWindWidth, &gWindHeight, screenNum_PrimaryScreen);
		DEBUG_ONLY( OS::_DOUT("Current Main Device: %d x %d (%d bit)", screenWidth, screenHeight, gScreenDepth); )
		Rect maxFullScreenDim(screenWidth, screenHeight);
		Rect maxWindowDim(gWindWidth, gWindHeight);
		
      #ifndef PLATFORM_IOS
        gScreenDepth = platform_getCurrentScreenDepth(screenNum_PrimaryScreen);
 		gFullScreen = Initializer::getGraphicsEnvironmentDimensions(
                            maxWindowDim, maxFullScreenDim, gWindWidth, gWindHeight, gScreenDepth);
		if (gFullScreen) {
			gWindWidth = std::min(gWindWidth, (int32)maxFullScreenDim.width());
			gWindHeight = std::min(gWindHeight, (int32)maxFullScreenDim.height());
		} else {
			gWindWidth = std::min(gWindWidth, (int32)maxWindowDim.width());
			gWindHeight = std::min(gWindHeight, (int32)maxWindowDim.height());
		}
	  #else
		gScreenDepth = 32;		
		gFullScreen = true;
		long ignoreDim;
		uint8 ignoreDepth;
		Initializer::getGraphicsEnvironmentDimensions(
			maxWindowDim, maxFullScreenDim, ignoreDim, ignoreDim, ignoreDepth);
      #endif
		
		const char* windName = Initializer::getAppName(
			ResourceManager::getSingletonInstance());

		Rect drawingRect;
		drawingRect.setHeight(gWindHeight);
		drawingRect.setWidth(gWindWidth);

		// set up the graphics environment	
		Port* mainPort = 0;
		if (gFullScreen) {
			mainPort = static_cast<PortImpl*>( 
				GraphicsManager::instance().createFullScreenPort(
                    drawingRect, screenNum_BestFitScreen, gScreenDepth)
			); // on main screen
		}
		if (!mainPort) {
			mainPort = static_cast<PortImpl*>( 
				GraphicsManager::instance().createWindowPort(
					drawingRect, windName, gScreenDepth) 
			);
			gFullScreen = false;
		}
		if (!mainPort) { return Initializer::initFail_GraphicsMgr; }
	} // end !isInitialized block
  #endif // PDG_NO_GUI
	
    DEBUG_ONLY( OS::_DOUT("Posting Startup event to application layer"); )

    StartupInfo si;
    si.startupReason = 0;
    si.startupParamCount = argc;
    si.startupParam = argv;
    
    EventManager::instance().postEvent(eventType_Startup, &si);
		
	return 0; // no error, everything is fine
}


// -----------------------------------------------
// Main run loop
// -----------------------------------------------
void main_run() {

	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u - ENTERING main_run()", OS::getMilliseconds()); )

  #ifndef PDG_NO_GUI
	uint32 currMs = OS::getMilliseconds();

	DRAWING_DEBUG_ONLY(
		static int mainLoopCounter = 0;
		if (++mainLoopCounter % 100 == 0) {
			OS::_DOUT("main loop interation: %ld @%lu (%lu)", mainLoopCounter, currMs, 
				gNextRedrawMillisec);
		}
	)

	// check timers will almost certainly trigger some drawing to happen if any timers fire
	Port* mainPort = GraphicsManager::instance().getMainPort();
	if (mainPort) {
		// calc our frames per second
		if (currMs >= gNextRedrawMillisec) {
			if ((gFrameNum % 100) == 0) {
				// calculate our fps
				long fpsMsDelta = currMs - gFPSBaseMs;
				// it's been more than 1 second, take average
				gCurrentFPS = 1000.0 * (float)gFPSFrameCount / (float)fpsMsDelta;
				gFPSBaseMs = currMs;
				gFPSFrameCount = 0;
				DRAWING_DEBUG_ONLY(
                    long lag = currMs - gNextRedrawMillisec;
					OS::_DOUT("frame: %ld lag: %ld ms fps: %01.2f", gFrameNum, 
					lag, gCurrentFPS);
				)
				DEBUG_ONLY( 
					float warnFps = 0.8f * gTargetFPS;
					if (gCurrentFPS < warnFps) {
						OS::_DOUT("WARNING: FPS drop!!! FPS: %01.2f", gCurrentFPS);
					}
				)
			}
			gNextRedrawMillisec = currMs + std::floor(1000.0 / gTargetFPS);
			gFPSFrameCount++; gFrameNum++;

			RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    About to Draw", OS::getMilliseconds()); )
			graphics_startDrawing(mainPort);

			// post an event to the application so it can draw the contents of the main port
			// TODO: let the graphics manager do this instead
			PortDrawInfo pdi;
			pdi.port = mainPort;
			pdi.frameNum = gFrameNum;
			EventManager::instance().postEvent(eventType_PortDraw, &pdi);

			graphics_finishDrawing(mainPort);
			RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Drawing Event complete", OS::getMilliseconds()); )
	
		}
	} else {
		gCurrentFPS = 0.0;
	}
  #endif // !PDG_NO_GUI

	TimerManager::instance().checkTimers();

	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Timer check/fire complete", OS::getMilliseconds()); )

  #ifndef PDG_NO_SLEEP
	// now figure out how long we need to sleep and block for that long
	// but make it interruptable by a semaphore
	long maxSleepMs = TimerManager::instance().msTillNextFire();
  #ifndef PDG_NO_GUI
	long redrawDelay = gNextRedrawMillisec - OS::getMilliseconds();
	if (maxSleepMs > redrawDelay) {
		maxSleepMs = redrawDelay;
	}
  #endif // !PDG_NO_GUI
	if (maxSleepMs > MAX_UI_SLEEP_INTERVAL) { // don't go past max interval for polling UI
		maxSleepMs = MAX_UI_SLEEP_INTERVAL;
	}
//	if (maxSleepMs > 1) maxSleepMs -= 1; // adjust for 1 ms timer delay in Cocoa layer
	
    //            OS::_DOUT("Sleeping for %d ms", maxSleepMs);
	// most likely reason for a signal will be data on the network
	// network events will be on the event queue, so we don't need to do anything special
	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    About to sleep for %ld ms", OS::getMilliseconds(), maxSleepMs); )
	
	/* bool signaled = */ gWakeupSemaphore.awaitSignal( maxSleepMs );
	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Awakened from sleep", OS::getMilliseconds()); )
  #endif

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
	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Dequeued %d event(s) queued during sleep", OS::getMilliseconds(), eventCount); )
  #endif // NO_EVENT_QUEUE

  #ifndef PDG_NO_NETWORK
    NetworkManager::instance().idle();
    RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Network Idle complete", OS::getMilliseconds()); )
  #endif // PDG_NO_NETWORK

  #ifndef PDG_NO_SOUND
    SoundManager::instance().idle();
    RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u -    Sound Manager Idle complete", OS::getMilliseconds()); )
  #endif // PDG_NO_SOUND

	RUN_LOOP_DEBUG_ONLY(OS::_DOUT("%12u - EXITING main_run()", OS::getMilliseconds()); )
}

// -----------------------------------------------
// Main cleanup
// -----------------------------------------------
	
int main_cleanup(bool* dontExit) {
	DEBUG_ONLY( OS::_DOUT("Posting Shutdown event to application layer"); )
		
	ShutdownInfo xi;
	xi.exitReason = 0;
	xi.exitCode = gExitCode;
	EventManager::instance().postEvent(eventType_Shutdown, &xi);

	if ((dontExit != NULL) && *dontExit) {
		if (gExitCode == 0) {
			TimerManager::instance().cancelAllTimers();
			DEBUG_ONLY( OS::_DOUT("Waiting for clients to disconnect"); )
			// let all the clients disconnect
		  #ifndef PDG_NO_NETWORK
// TODO: abstract this out so it is not implementation specific
//             OpenPlayNetworkManager* opnm = 
//                 dynamic_cast<OpenPlayNetworkManager*>(NetworkManager::getSingletonInstance());
//             if (opnm) {
//                 opnm->doOrderlyShutdown();
//             }
//             PosixAPI::usleep(10 * 1000); // delay for 10 seconds while shutdown happens
		  #endif // PDG_NO_NETWORK
			gExit = false;
			DEBUG_ONLY( OS::_DOUT("Restarting app as required by -noexit"); )
			return 0;
		} else {
			DEBUG_ONLY( OS::_DOUT("App exiting with an error code [%d], ignoring -noexit", gExitCode); )
			*dontExit = false;
		}
	}

  #ifndef PDG_NO_NETWORK
    delete NetworkManager::getSingletonInstance(); // cleanup network manager
  #endif // PDG_NO_NETWORK

  #ifndef PDG_NO_SOUND
    delete SoundManager::getSingletonInstance();
  #endif
	
  #ifndef PDG_NO_GUI
    delete GraphicsManager::getSingletonInstance(); // this cleans up Graphics manager & main port
  #endif // PDG_NO_GUI

    delete TimerManager::getSingletonInstance();  // clean up timer manager
	delete EventManager::getSingletonInstance(); // clean up event manager
	delete ResourceManager::getSingletonInstance();  // clean up resource manager
	delete ConfigManager::getSingletonInstance();    // clean up config manager
	delete LogManager::getSingletonInstance();   // clean up the log manager

	return gExitCode;
}


// FUNCTIONS //////////////////////////////////////////////

#ifndef PDG_NO_GUI
void main_handleKeyPress(utf16char keyChar, bool repeat, bool shift, bool control, bool alt, bool cmd) {
	KeyPressInfo ki;
	ki.unicode = keyChar;
	ki.isRepeating = repeat;
	ki.shift = shift;  // Is Shift Key Down?
	ki.ctrl = control;  // Is Control Key Down?
	ki.alt = alt;  // Is Option Key Down?
	ki.meta = cmd;  // Is Cmd Key Down?
	EventManager::instance().postEvent(eventType_KeyPress, &ki);
}

void main_handleKeyDown(int keyCode, utf16char keyChar) {
  #ifdef PDG_DEBUG_KEYBOARD
    std::cout << "main_handleKeyDown("<<keyCode<<", "<<(void*)keyChar<<" ("<<(char)keyChar<<")\n";
  #endif
	if (keyCode >= 0 && keyCode < MAX_RAW_KEYS) {
		gRawKeyStates[keyCode] = true;
	}
	if (keyChar > 0 && keyChar < MAX_CHAR_KEYS) {  // ignore zero
		gCharKeyStates[keyChar] = true;
	}
	KeyInfo ki;
	ki.keyCode = keyCode;
	EventManager::instance().postEvent(eventType_KeyDown, &ki);
}
	
void main_handleKeyUp(int keyCode, utf16char keyChar) {
  #ifdef PDG_DEBUG_KEYBOARD
    std::cout << "main_handleKeyUp("<<keyCode<<", "<<(void*)keyChar<<" ("<<(char)keyChar<<")\n";
  #endif
	if (keyCode >= 0 && keyCode < MAX_RAW_KEYS) {
		gRawKeyStates[keyCode] = false;
	}
	if (keyChar > 0 && keyChar < MAX_CHAR_KEYS) {  // ignore zero
		gCharKeyStates[keyChar] = false;
	}
	KeyInfo ki;
	ki.keyCode = keyCode;
	EventManager::instance().postEvent(eventType_KeyUp, &ki);
}

bool main_getRawKeyState(int keyCode) {
	if (keyCode >= 0 && keyCode < MAX_RAW_KEYS) {
		return gRawKeyStates[keyCode];
	} else {
		return false;
	}
}

bool main_getKeyState(utf16char keyChar) {
	if (keyChar > 0 && keyChar < MAX_CHAR_KEYS) { // ignore zero
		return gCharKeyStates[keyChar];
	} else {
		return false;
	}
}

bool main_getButtonState(int buttonNumber) {
	if (buttonNumber >= 0 && buttonNumber < MAX_MOUSE_BUTTONS) {
		return gButtonStates[buttonNumber];
	} else {
		return false;
	}
}

Point main_getMousePos(int mouseNumber) {
	if (mouseNumber >= 0 && mouseNumber < MAX_POINTERS) {
		if (mouseNumber <= gMaxAttachedMouse) {
			return gMouseLoc[mouseNumber];
		} else {
			return gMouseLoc[0];
		}
	} else {
		return gMouseLoc[0];
	}
}
	
void main_handleMouse(int action, float x, float y, int button, bool shift, bool control, bool alt, bool cmd) {
	int screenPos = graphics_getEffectiveScreenPos();
	// update the button states
	if (button >= 0 && button < MAX_MOUSE_BUTTONS) {
		if (action == mouseEventType_MouseDown) {
			gButtonStates[button] = true;
		} else if (action == mouseEventType_MouseUp) {
			gButtonStates[button] = false;
		}
	}
	// figure out if this represents a new mouse or finger we weren't tracking before
	if ( (button > gMaxAttachedMouse) && (action == mouseEventType_MouseMoved) ) {
		if (button < MAX_POINTERS) {
			gMaxAttachedMouse = button;
		}
	}
	MouseInfo mi;
	unsigned long ms = OS::getMilliseconds();
	mi.lastClickElapsed = ms - gLastClickMillisec;
	Point pdgfMousePoint( (PDG_BASE_COORD_TYPE) std::floor(x), (PDG_BASE_COORD_TYPE) std::floor(y) );  
	if (screenPos == screenPos_Normal) {
		// nothing special
	} else if (screenPos == screenPos_Rotated180) {
		// flip the input coordinates because the screen is flipped
		pdgfMousePoint.x = gWindWidth - pdgfMousePoint.x;
		pdgfMousePoint.y = gWindHeight - pdgfMousePoint.y;
	} else if (screenPos == screenPos_Rotated90CounterClockwise) {
		// flip the input coordinates because the screen is flipped
		PDG_BASE_COORD_TYPE temp = pdgfMousePoint.x;
		pdgfMousePoint.x = pdgfMousePoint.y;
		pdgfMousePoint.y = gWindHeight - temp;
	} else if (screenPos == screenPos_Rotated90Clockwise) {
		// flip the input coordinates because the screen is flipped
		PDG_BASE_COORD_TYPE temp = pdgfMousePoint.x;
		pdgfMousePoint.x = gWindWidth - pdgfMousePoint.y;
		pdgfMousePoint.y = temp;
	}
	mi.mousePos = pdgfMousePoint;
	mi.lastClickPos = gLastClickPos;
	mi.leftButton = gButtonStates[0];
	mi.rightButton = gButtonStates[1];
	mi.buttonNumber = button;
	mi.shift = shift;
	mi.ctrl = control;
	mi.alt = alt;
	mi.meta = cmd;
	if (button >= 0 && button <= gMaxAttachedMouse) {
		gMouseLoc[button] = mi.mousePos;
	}
	long eventType;
	if (action == mouseEventType_MouseDown) {
		gLastClickMillisec = ms;
		gLastClickPos = mi.mousePos;
		eventType = eventType_MouseDown;
	} else if (action == mouseEventType_MouseUp) {
		eventType = eventType_MouseUp;
	} else if ( (action == mouseEventType_MouseDragged) || (action == mouseEventType_MouseMoved) ) {
		eventType = eventType_MouseMove;
	}
	EventManager::instance().postEvent(eventType, &mi);
}
#endif // !PDG_NO_GUI


} // end namespace pdg

