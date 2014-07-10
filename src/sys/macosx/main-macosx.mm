// -----------------------------------------------
// main-macosx.mm
// 
// Mac OS X implementation of main application entry point
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

#include "internals.h"
#include "pdg-main.h"

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

int gArgc;
const char** gArgv;
BOOL gIsInitialized;
BOOL gNeedInit;
BOOL gDidFinishLaunching = FALSE;
extern BOOL gHaveViewTimers;
extern BOOL gPortDirty;
SInt32 gOSversMajor = 0;
SInt32 gOSversMinor = 0;


int main(int argc, char *argv[])
{
	int result = 0;
	gIsInitialized = FALSE;
	gNeedInit = TRUE;
    
    int major;
    int minor;
    int bugfix;
    macosx_getSystemVersion(major, minor, bugfix);
    gOSversMajor = major;
	gOSversMinor = minor;
    
	bool noExit = pdg::main_getNoExitFromArgs(argc, (const char**) argv);
	do {
		gArgv = (const char**)argv;
		gArgc = argc;
		if (result == 0) {
			result = NSApplicationMain(argc,  (const char**) argv);
			if (noExit && result == 0) {
				result = pdg::main_cleanup(&noExit);
				gNeedInit = TRUE;
			}
		}
	} while(noExit);
    pdg::main_cleanup(&noExit);
	return result;
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
    BOOL result = (pdg::main_init(gArgc, gArgv, gIsInitialized) == 0);
    gIsInitialized = result;
}
-(void) applicationDidFinishLaunching:(NSNotification*) aNotification
{
}
@end


