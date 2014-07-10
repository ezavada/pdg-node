// -----------------------------------------------
// platform-graphics-macosx.mm
// 
// Mac OS X implementation of platform specific graphics functionality
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

#import "pdg-opengl-view.h"

#define PDG_DECORATE_GLOBAL_TYPES
#include "pdg/sys/platform.h"
#include "pdg/sys/os.h"
#import "pdg/sys/events.h"


#import "internals-macosx.h"
#include "internals.h"
#include "pdg-main.h"

#import <OpenGL/gl.h>

#ifndef PDG_MAX_DISPLAYS
  #define PDG_MAX_DISPLAYS 32
#endif

#define PDG_WINDOW_STYLE (NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask)

// from main-macosx.mm
extern SInt32 gOSversMajor;
extern SInt32 gOSversMinor;


CGDisplayModeRef gOriginalDisplayModes[PDG_MAX_DISPLAYS];
#if (__MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6)
CFDictionaryRef gOriginalScreenModes[PDG_MAX_DISPLAYS];
#endif

static CGDirectDisplayID screenNumToDisplayID(int screenNum);
static int displayIDToScreenNumTo(CGDirectDisplayID dispId);
static int bppFromDisplayMode(CGDisplayModeRef mode);
static void restoreScreenResolution(int screenNum);
static CGDisplayModeRef bestModeForParameters(CGDirectDisplayID dispId, int bpp, long width, long height);

CGDirectDisplayID screenNumToDisplayID(int screenNum) {
    if (screenNum == pdg::screenNum_PrimaryScreen) {
        return kCGDirectMainDisplay;
    }
    
    CGDirectDisplayID displays[PDG_MAX_DISPLAYS];
    CGDisplayCount numDisplays;
    
    CGGetActiveDisplayList (PDG_MAX_DISPLAYS, displays, &numDisplays);
    if ((CGDisplayCount)screenNum >= numDisplays) {
        return kCGDirectMainDisplay;
    }
    return displays[screenNum];
}

static int displayIDToScreenNumTo(CGDirectDisplayID dispId) {
	if (dispId == kCGDirectMainDisplay) {
		return pdg::platform_getPrimaryScreen();
	}
    CGDirectDisplayID displays[PDG_MAX_DISPLAYS];
    CGDisplayCount numDisplays;
    
    CGGetActiveDisplayList (PDG_MAX_DISPLAYS, displays, &numDisplays);
    for (int i = 0; i<numDisplays; i++) {
    	if (dispId == displays[i]) {
    		return i;
    	}
    }
    return pdg::platform_getPrimaryScreen();
}

namespace pdg {

// some routines to deal with the low level OS cursor

void platform_setHardwareCursorVisible(bool inVisible) {
#ifndef PLATFORM_IOS
    if (inVisible) {
        CGDisplayShowCursor(kCGDirectMainDisplay);
    } else {
        CGDisplayHideCursor(kCGDirectMainDisplay);
    }
#endif
}

void platform_setHardwareBusyCursor() {
/*    CursHandle busyH = MacAPI::GetCursor(watchCursor);
    MacAPI::HLock((Handle)busyH);
    MacAPI::MacSetCursor(*busyH);
    MacAPI::HUnlock((Handle)busyH);
    MacAPI::ReleaseResource((Handle)busyH); */
}

void platform_setHardwareNormalCursor() {
/*    Cursor arrow;
    MacAPI::GetQDGlobalsArrow(&arrow);
    MacAPI::MacSetCursor(&arrow); */
}

void platform_startDrawing(void* windRef) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    if (myView) {
    	[[myView openGLContext] makeCurrentContext];
    }
}

void platform_finishDrawing(void* windRef) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    if (myView) {
		[[myView openGLContext] flushBuffer];
	}
}

int platform_getNumScreens() {
    CGDirectDisplayID displays[PDG_MAX_DISPLAYS];
    CGDisplayCount numDisplays;
    CGGetActiveDisplayList (PDG_MAX_DISPLAYS, displays, &numDisplays);
    return numDisplays;
}

int platform_getPrimaryScreen() {
	if (platform_getNumScreens() == 1) {
		return 0;	// only one choice, easy
	}
    CGDirectDisplayID displays[PDG_MAX_DISPLAYS];
    CGDisplayCount numDisplays;
	CGDirectDisplayID dispId = CGMainDisplayID();
    CGGetActiveDisplayList (PDG_MAX_DISPLAYS, displays, &numDisplays);
    for (int i = 0; i<numDisplays; i++) {
    	if (dispId == displays[i]) {
    		return i;
    	}
    }
    return 0;	// couldn't find it, just use the first one
}

void platform_getScreenSize(long* outWidth, long* outHeight, int screenNum) {
	if (screenNum >= pdg::screenNum_PrimaryScreen && screenNum < platform_getNumScreens()) {
        CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
		*outWidth = CGDisplayPixelsWide(dispId);
		*outHeight = CGDisplayPixelsHigh(dispId);
	} else {
		*outWidth = 0;
		*outHeight = 0;
	}
}

void platform_getMaxWindowSize(long* outWidth, long* outHeight, int screenNum) {
    platform_getScreenSize(outWidth, outHeight, screenNum);
	if ( (screenNum == pdg::screenNum_PrimaryScreen) || (screenNum == platform_getPrimaryScreen()) ) {
        // the primary screen is the only one that would have the menu bar and dock on it
        // NScreen doesn't reliably stay up-to-date with mode changes through direct CG calls
        // so we can't trust the dimensions it gives, but we can trust the difference in
        // dimensions, that is, the difference between the full frame and the visible frame
        NSScreen* thePrimaryScreen = [NSScreen mainScreen];
        NSRect max = [thePrimaryScreen frame];
        NSRect visible = [thePrimaryScreen visibleFrame];
        NSRect constrained = [NSWindow contentRectForFrameRect:visible styleMask:PDG_WINDOW_STYLE];
        long wDelta = max.size.width - constrained.size.width;
        long hDelta = max.size.height - constrained.size.height;
        *outWidth -= wDelta;
        *outHeight -= hDelta;
	}
}


void* platform_createWindow(long width, long height, long x, long y, int bpp, const char* title) {
    if (bpp == 0) bpp = platform_getCurrentScreenDepth(pdg::screenNum_PrimaryScreen);
	DEBUG_ONLY( OS::_DOUT("Create Window at (%d, %d) [%d x %d @ %d]", x, y, width, height, bpp); ) 
    PDGOpenGLView* myView = [[PDGOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];

    NSWindow* myWindow = [[FSWindow alloc] initWithContentRect:NSMakeRect(x, y, width, height)
			styleMask:PDG_WINDOW_STYLE
			backing:NSBackingStoreBuffered
			defer:NO];

	[myWindow setTitle:[NSString stringWithUTF8String:title]];
	[myWindow setContentView:myView];
    myView->isFullscreen = FALSE;
    myView->onScreenNum = platform_getPrimaryScreen();
    [myView retain];    // we are passing it back to someone who is keeping a pointer to it

    [myWindow center];
    [myWindow makeKeyAndOrderFront:nil];
    return myView;
}


int platform_getNumSupportedScreenModes(int screenNum) {
	if ((screenNum < pdg::screenNum_PrimaryScreen) || (screenNum >= platform_getNumScreens())) return 0;
    CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
    CFArrayRef modeList = CGDisplayCopyAllDisplayModes(dispId, NULL);
	int count = CFArrayGetCount(modeList);
    CFRelease(modeList);
	return count;
}

void platform_getNthSupportedScreenMode(int screenNum, int n, long* outWidth, long* outHeight, int* outBpp) {
	*outWidth = 0;
	*outHeight = 0;
	*outBpp = 0;
	if ((screenNum < pdg::screenNum_PrimaryScreen) || (screenNum >= platform_getNumScreens())) return;
	if (n < 0) return;
    CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
    CFArrayRef modeList = CGDisplayCopyAllDisplayModes(dispId, NULL);
    int count = CFArrayGetCount(modeList);
    if (n < count) {
		CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex (modeList, n);
		*outBpp = bppFromDisplayMode(mode);
		*outHeight = CGDisplayModeGetHeight(mode);
		*outWidth = CGDisplayModeGetWidth(mode);
	}
    CFRelease(modeList);
}


bool platform_closestScreenMode(int screenNum, long* ioWidth, long* ioHeight, int* ioBpp) {
	if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) {
        *ioWidth = 0;
        *ioHeight = 0;
		*ioBpp = 0;
		return false;
	}
    CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
    long width = *ioWidth;
    long height = *ioHeight;
    long bpp = *ioBpp;
	DEBUG_ONLY( OS::_DOUT("Finding closest display mode to %d [%d x %d @ %d]", screenNum, width, height, bpp); ) 
    if ((gOSversMajor >= 10) && (gOSversMinor >= 6)) {
        CGDisplayModeRef mode = bestModeForParameters(dispId, bpp, width, height);
        *ioWidth = CGDisplayModeGetWidth(mode);
        *ioHeight = CGDisplayModeGetHeight(mode);
		*ioBpp = bppFromDisplayMode(mode);
  #if (__MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6)
    } else {
        CFDictionaryRef mode = CGDisplayBestModeForParameters(dispId, bpp, width, height, NULL);
        CFNumber* num;
        num = CFDictionaryGetValue(mode, kCGDisplayWidth);
		CFNumberGetValue(num, kCFNumberLongType, ioWidth);
        num = CFDictionaryGetValue(mode, kCGDisplayHeight);
		CFNumberGetValue(num, kCFNumberLongType, ioHeight);
		num = CFDictionaryGetValue(mode, kCGDisplayBitsPerPixel);
		CFNumberGetValue(num, kCFNumberIntType, ioBpp);
  #endif            
    }
	DEBUG_ONLY( OS::_DOUT("-- best mode [%d x %d @ %d]", *ioWidth, *ioHeight, bpp); )    
    // return true if mode was as good or better than asked for, false if closest mode was inferior
    return ( (*ioHeight >= height) && (*ioWidth >= width) && (*ioBpp >= bpp) );
}

int platform_closestScreenTo(long width, long height, int bpp) {
	DEBUG_ONLY( OS::_DOUT("Finding closest screen to [%d x %d @ %d]", width, height, bpp); ) 
    int numScreens = platform_getNumScreens();
    int ranking[PDG_MAX_DISPLAYS];
    int highestRankedScreen = platform_getPrimaryScreen(); // worst case is we just use main screen
    int highestRank = 0;
    for (int i = 0; i <  numScreens; i++) {
        long h, w;
        platform_getScreenSize(&w, &h, i);
        int d = platform_getCurrentScreenDepth(i);
        if (w == width && h == height && ((d == bpp) || (bpp == 0)) ) {
            return i; // exact match!!
        }
        // rank screen: exact dimensions best, reducing size and depth 
        // are better than increasing them, dimensions more important
        // than depth
        ranking[i] = 0;
        ranking[i] += (w==width) ? 100 : ((w > width) ? 50 : 0);
        ranking[i] += (h==height) ? 100 : ((h > height) ? 50 : 0);
        if (bpp) {
        	ranking[i] += (d==bpp) ? 20 : ((d > bpp) ? 10 : 0);
        }
        if (ranking[i] > highestRank) {
            highestRankedScreen = i;
            highestRank = ranking[i];
        }
    }
	DEBUG_ONLY( OS::_DOUT("-- best screen %d", highestRankedScreen); ) 
    return highestRankedScreen;
}


void platform_switchScreenResolution(int screenNum, long width, long height, int bpp) {
	DEBUG_ONLY( OS::_DOUT("Switching display mode for %d [%d x %d @ %d]", screenNum, width, height, bpp); ) 
	if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) return;
    CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
    if ((gOSversMajor >= 10) && (gOSversMinor >= 6)) {
        CGDisplayModeRef mode = bestModeForParameters(dispId, bpp, width, height);
        gOriginalDisplayModes[screenNum] = mode;
        CGDisplaySetDisplayMode (dispId, mode, NULL); 
  #if (__MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6)
    } else {
        CFDictionaryRef mode = CGDisplayBestModeForParameters(dispId, bpp, width, height, NULL);
        gOriginalScreenModes[screenNum] = CGDisplayCurrentMode(dispId);
        CGDisplaySwitchToMode(dispId, mode);
  #endif            
    }
}
void restoreScreenResolution(int screenNum) {
    CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
    if ((gOSversMajor >= 10) && (gOSversMinor >= 6)) {
        CGDisplayModeRef mode = gOriginalDisplayModes[screenNum];
		DEBUG_ONLY( OS::_DOUT("Restoring display mode for %d", screenNum); ) 
        CGDisplaySetDisplayMode (dispId, mode, NULL); 
  #if (__MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6)
    } else {
        CFDictionaryRef mode = gOriginalScreenModes[screenNum];
		DEBUG_ONLY( OS::_DOUT("Restoring display mode for %d", screenNum); ) 
        CGDisplaySwitchToMode(dispId, mode);
  #endif
    }
    CGDisplayRelease(dispId);
}

void platform_destroyWindow(void* windRef) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    if (myView->isFullscreen == TRUE) {
        restoreScreenResolution(myView->onScreenNum);
    }
    NSWindow* window = [myView window];
    [window close];
    [myView release];
}

void* platform_createFullscreenWindow(long width, long height, int bpp, int screenNum) {
	if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) {
		screenNum = platform_getPrimaryScreen();
	}
    if (bpp == 0) bpp = platform_getCurrentScreenDepth(screenNum);
	DEBUG_ONLY( OS::_DOUT("Create Fullscreen Window on %d [%d x %d @ %d]", screenNum, width, height, bpp); ) 
    NSScreen* targetScreen;
    targetScreen = [[NSScreen screens] objectAtIndex: screenNum];
    
	int windowLevel = CGShieldingWindowLevel();

    PDGOpenGLView* myView = [[PDGOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
    
    NSWindow* myWindow = [[FSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
			styleMask:NSBorderlessWindowMask
			backing:NSBackingStoreBuffered
			defer:NO 
			screen:targetScreen];

	[myWindow setContentView:myView];
    myView->isFullscreen = TRUE;
    myView->onScreenNum = screenNum;
    [myView retain];    // we are passing it back to someone who is keeping a pointer to it
    
    [myWindow setLevel:windowLevel];
    [myWindow makeKeyAndOrderFront:nil];
    return myView;
}


void platform_resizeWindow(void* windRef, long width, long height, bool fullscreen) {
	DEBUG_ONLY( OS::_DOUT("ResizeWindow %d x %d (%s)", width, height, fullscreen ? "fullscreen":"window"); ) 
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    NSWindow* window = [myView window];
	NSSize newSize;
	newSize.width = width;
	newSize.height = height;
	[window setContentSize:newSize];
    CGDirectDisplayID dispId;
	if ((myView->isFullscreen == TRUE) && !fullscreen) {
		// changing from fullscreen to window mode
    	[window setLevel:NSNormalWindowLevel];
    	[window setStyleMask:PDG_WINDOW_STYLE];
		myView->isFullscreen = FALSE;
	} else if ((myView->isFullscreen == FALSE) && fullscreen) {
		// changing from window mode to fullscreen
		// find out what screen window is currently on (it could have been moved)
		myView->onScreenNum = platform_getWindowScreen(windRef);
		int windowLevel = CGShieldingWindowLevel();
		[window setLevel:windowLevel];
    	[window setStyleMask:NSBorderlessWindowMask];
		myView->isFullscreen = TRUE;
	}
}

int platform_getWindowScreen(void* windRef) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    NSWindow* window = [myView window];
	NSScreen* screen = [window screen];
	NSDictionary* dict = [screen deviceDescription];
	NSNumber* num = [dict valueForKey:@"NSScreenNumber"];
	CGDirectDisplayID dispId = [num longValue];
	return displayIDToScreenNumTo(dispId);
}

void platform_getWindowPosition(void* windRef, long* outXPos, long* outYPos) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    NSWindow* window = [myView window];
    NSRect contents = [window contentRectForFrameRect:[window frame]];
    *outXPos = contents.origin.x;
    *outYPos = contents.origin.y;
	DEBUG_ONLY( OS::_DOUT("GetWindowPos (%d, %d)", *outXPos, *outYPos); ) 
}

void platform_setWindowPosition(void* windRef, long xPos, long yPos) {
	DEBUG_ONLY( OS::_DOUT("SetWindowPos (%d, %d)", xPos, yPos); ) 
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    NSWindow* window = [myView window];
    NSRect contents = [myView frame];
    contents.origin.x = xPos;
    contents.origin.y = yPos;
    NSRect frame = [window frameRectForContentRect:contents];
    [window setFrameOrigin:frame.origin];
}

bool platform_isFullScreen(void* windRef) {
    PDGOpenGLView* myView = (PDGOpenGLView*)windRef;
    return (myView->isFullscreen == TRUE);
}

int platform_getCurrentScreenDepth(int screenNum) {
    if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) return -1;
	CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
	int screenDepth = 0;
	if ((gOSversMajor >= 10) && (gOSversMinor >= 6)) {
		CGDisplayModeRef mode = CGDisplayCopyDisplayMode(dispId);
		CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
		if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			screenDepth = 32;
		} else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			screenDepth = 16;
		} else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
			screenDepth = 8;
		}
		CFRelease(pixEnc);
		CGDisplayModeRelease(mode);
  #if (__MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6)
	} else {
		screenDepth = CGDisplayBitsPerPixel(dispId);
  #endif
	}
	return screenDepth;
}

void platform_captureScreen(int screenNum) {
	if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) return;
	CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
	DEBUG_ONLY( OS::_DOUT("Capturing display for screen %d", screenNum); ) 
	CGDisplayCapture( dispId );
}

void platform_releaseScreen(int screenNum) {
	if (screenNum < pdg::screenNum_PrimaryScreen && screenNum >= platform_getNumScreens()) return;
	CGDirectDisplayID dispId = screenNumToDisplayID(screenNum);
	DEBUG_ONLY( OS::_DOUT("Releasing display for screen %d", screenNum); ) 
	CGDisplayRelease(dispId);
}


} // end namespace pdg



CGContextRef graphics_getCurrentCGContextRef() {
    NSGraphicsContext* theGraphicsContext = [NSGraphicsContext currentContext];
    CGContextRef theContextRef = (CGContextRef) [theGraphicsContext graphicsPort];
	return theContextRef;
}

int bppFromDisplayMode(CGDisplayModeRef mode) {
    int d;
    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
	if(CFStringCompare(pixEnc, CFSTR(kIO64BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		d = 64;
	} else if(CFStringCompare(pixEnc, CFSTR(kIO30BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		// 10 bits per channel (RGB), in a single 32 bit value. For our purposes
		// we just care that it is higher color range than we asked for
		d = 40;
	} else if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		d = 32;
	} else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		d = 16;
	} else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		d = 8;
	} else {
		d = 0;	// unknown
	}
    CFRelease(pixEnc);
	return d;
}

CGDisplayModeRef bestModeForParameters(CGDirectDisplayID dispId, int bpp, long width, long height) {
    CFIndex count;
    CFArrayRef modeList;
    CGDisplayModeRef highestRankedMode = CGDisplayCopyDisplayMode(dispId);
    int highestRank = 0;
    
    modeList = CGDisplayCopyAllDisplayModes(dispId, NULL);
    count = CFArrayGetCount(modeList);
    
    int* ranking = (int*)malloc(count * sizeof(int));
    
    for (int i = 0; i < count; i++) {
        CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex (modeList, i);
        int d = bppFromDisplayMode(mode);
        long h = CGDisplayModeGetHeight(mode);
        long w = CGDisplayModeGetWidth(mode);
        if (w == width && h == height && d == bpp) {
            free(ranking);
            return mode; // exact match!!
        }
        // rank display mode: exact dimensions best, greater size and depth 
        // are better than reduced, dimensions more important
        // than depth
        ranking[i] = 0;
        ranking[i] += (w==width) ? 100 : ((w > width) ? 50 : 0);
        ranking[i] += (h==height) ? 100 : ((h > height) ? 50 : 0);
        ranking[i] += (d==bpp) ? 20 : ((d > bpp) ? 10 : 0);
        if (ranking[i] > highestRank) {
            highestRankedMode = mode;
            highestRank = ranking[i];
        }
    }
    free(ranking);
    CFRelease(modeList);
    return highestRankedMode;
}


