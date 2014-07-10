// -----------------------------------------------
// pdg-lib-macosx.mm
// 
// Mac OS X GUI implementation of library
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

#include "pdg_project.h"

#import <Cocoa/Cocoa.h>

#define PDG_DECORATE_GLOBAL_TYPES
#include "pdg/sys/platform.h"
#import "pdg/sys/events.h"
#include "pdg/sys/os.h"

#include "pdg-lib.h"
#include "pdg-main.h"
#include "internals.h"
#include "internals-macosx.h"

#import <OpenGL/gl.h>


// use a custom run loop
#define PDG_CUSTOM_RUN_LOOP

// ----------- AppDelegate
@interface AppDelegate : NSObject
{
}
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
- (void)applicationWillFinishLaunching:(NSNotification*) aNotification;
- (void)applicationDidFinishLaunching:(NSNotification*) aNotification;
@end

// int gArgc;
// const char** gArgv;
// BOOL gIsInitialized;
// BOOL gNeedInit;
// extern bool gPDG_Quitting;  // in pdg-opengl-view.mm
BOOL gDidFinishLaunching = FALSE;
BOOL gInCustomRunLoop = FALSE;
extern BOOL gHaveViewTimers;
extern BOOL gPortDirty;
SInt32 gOSversMajor = 0;
SInt32 gOSversMinor = 0;


extern "C" {

void pdg_LibAppInit();
void pdg_LibPoolInit();
void pdg_LibPoolCleanup();

}

#define MAX_ARGS 100

bool gPDG_IsInitialized = false;
bool gPDG_Quitting = false;
bool gPDG_InRunLoop = false;
int  gPDG_argc = 0;
const char* gPDG_argv[MAX_ARGS];
bool gPDG_haveArgs = false;

void pdg_LibSaveArgs(int argc, const char* argv[]) {
    gPDG_argc = (argc < MAX_ARGS) ? argc : MAX_ARGS;
    for (int i = 0; i<MAX_ARGS; i++) {
        if (i < argc) {
            gPDG_argv[i] = argv[i];
        } else {
            gPDG_argv[i] = 0;
        }
    }
    gPDG_haveArgs = true;
}

int  pdg_LibGetArgc() {
    return gPDG_argc;
}
const char** pdg_LibGetArgv() {
    return gPDG_argv;
}


bool pdg_LibNeedsInit() {
    return !gPDG_IsInitialized;
}

void pdg_LibInit() {
    // figure out the OS version we are running under
    int major;
    int minor;
    int bugfix;
    macosx_getSystemVersion(major, minor, bugfix);
    gOSversMajor = major;
	gOSversMinor = minor;
    if (!gPDG_haveArgs) {
		std::cerr << "Fatal Error: pdg_LibInit() called before pdg_LibSaveArgs()";
		exit(1);
    }
	int err = pdg::main_initManagers();
	if (err) {
		std::cerr << "Fatal Error: Could not initialize PDG Managers";
		exit(err);
	}
    pdg::main_initKeyStates();
	pdg::platform_setupDirectories(gPDG_argc, gPDG_argv);
	pdg::platform_init(gPDG_argc, gPDG_argv);
	pdg_LibPoolInit();
	pdg_LibAppInit();
	gPDG_IsInitialized = true;
	gPDG_Quitting = false;
	gPDG_InRunLoop = false;
}

void pdg_LibAppInit() {
	AppDelegate * delegate = [[AppDelegate alloc] init];
	[NSApplication sharedApplication];
	[NSApp setDelegate:delegate];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
}

// these are for managing the pool when this is a library
NSAutoreleasePool *gPool;
void pdg_LibPoolInit() {
     gPool = [[NSAutoreleasePool alloc] init];
}
void pdg_LibPoolCleanup() {
    [gPool drain];
}

void pdg_LibIdle() {
    if (!gPDG_IsInitialized) {
    	pdg_LibInit();
	}
    if (!gDidFinishLaunching) {
		[[NSNotificationCenter defaultCenter]
			postNotificationName:NSApplicationWillFinishLaunchingNotification
			object:NSApp];
		[[NSNotificationCenter defaultCenter]
			postNotificationName:NSApplicationDidFinishLaunchingNotification
			object:NSApp];
		gDidFinishLaunching = TRUE;
	}
	if (gPDG_IsInitialized && !gPDG_Quitting) {
		NSRunLoop* runloop = [NSRunLoop currentRunLoop];
		[runloop runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.001]];
		if (!gHaveViewTimers) {
			// no view timers means we have to call pdg_main_run() ourselves
			NSAutoreleasePool *mainPool = [[NSAutoreleasePool alloc] init];
			pdg::main_run();
			pdg::platform_pollEvents();
			[mainPool drain];
		}
		NSEvent *event =
			[NSApp
				nextEventMatchingMask:NSAnyEventMask
				untilDate:[NSDate dateWithTimeIntervalSinceNow:0.001]  // distantFuture
				inMode:NSDefaultRunLoopMode
				dequeue:YES];
		
		[NSApp sendEvent:event];
		[NSApp updateWindows];
	}
}

void pdg_LibRun() {
    if (!gPDG_IsInitialized) {
    	pdg_LibInit();
	}
	gPDG_InRunLoop = true;
  #ifndef PDG_CUSTOM_RUN_LOOP
	[NSApp run];
  #else
    if (!gDidFinishLaunching) {
		[[NSNotificationCenter defaultCenter]
			postNotificationName:NSApplicationWillFinishLaunchingNotification
			object:NSApp];
		[[NSNotificationCenter defaultCenter]
			postNotificationName:NSApplicationDidFinishLaunchingNotification
			object:NSApp];
		gDidFinishLaunching = TRUE;
	}
	gInCustomRunLoop = TRUE;
	do {
		NSEvent *event =
			[NSApp
				nextEventMatchingMask:NSAnyEventMask
				untilDate:[NSDate dateWithTimeIntervalSinceNow:0.001]  // distantFuture
				inMode:NSDefaultRunLoopMode
				dequeue:YES];
		
		[NSApp sendEvent:event];
		[NSApp updateWindows];
// FIXME: not sure if I need to call these or not
//		pdg::main_run();
//		pdg::platform_pollEvents();
	} while (!gPDG_Quitting);
  #endif
    pdg::main_cleanup();
    pdg::platform_cleanup();
    pdg_LibPoolCleanup();
}

void pdg_LibQuit() {
	gPDG_Quitting = true;
	if (!gPDG_InRunLoop) {	// run loop calls this itself
    	pdg::main_cleanup();
    	pdg::platform_cleanup();
    }
}

bool pdg_LibIsQuitting() {
	return gPDG_Quitting;
}



// ----------- AppDelegate
@implementation AppDelegate
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	return NSTerminateNow;
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}
-(void) applicationWillFinishLaunching:(NSNotification*) aNotification
{
	// library init via pdg_LibAppInit() and direct singleton creation
//	gIsInitialized = TRUE;
}
-(void) applicationDidFinishLaunching:(NSNotification*) aNotification
{
}
@end


