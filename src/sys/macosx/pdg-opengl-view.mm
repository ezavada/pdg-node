// -----------------------------------------------
// pdg-opengl-view.mm
// 
// Mac OS X implementation of NS View for OpenGL
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
#import "pdg/sys/events.h"

#import "internals-macosx.h"
#include "internals.h"
#include "pdg-main.h"
#include "pdg-lib.h"

#import <OpenGL/gl.h>

#ifndef PDG_MAX_DISPLAYS
  #define PDG_MAX_DISPLAYS 32
#endif

#define PDG_WINDOW_STYLE (NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask)

BOOL gHaveViewTimers = FALSE;

extern bool gPDG_IsInitialized;
extern BOOL gPortDirty;
extern SInt32 gOSversMajor;
extern SInt32 gOSversMinor;
extern bool gPDG_Quitting;


// ----------- FSWindow
@implementation FSWindow
- (BOOL)canBecomeKeyWindow
{
	return YES;
}
- (void)close
{
	printf("FSWindow close()\n");
	gPDG_Quitting = TRUE;
	[(PDGOpenGLView*)glview handleQuitting];
	[super close];
}
- (void) setGLView:(NSView *)view
{
	glview = view;
}
@end

#define SIZE_OF_KEY_MAP 64

unichar keyMap[SIZE_OF_KEY_MAP][6] = {
// Cocoa key				PDG key			modifiers (shift, control, alt/option, meta/cmd )
{ NSUpArrowFunctionKey        , IN_PDG_NAMESPACE(key_UpArrow),   0, 0, 0, 0},
{ NSDownArrowFunctionKey      , IN_PDG_NAMESPACE(key_DownArrow), 0, 0, 0, 0 },
{ NSLeftArrowFunctionKey      , IN_PDG_NAMESPACE(key_LeftArrow), 0, 0, 0, 0 },
{ NSRightArrowFunctionKey     , IN_PDG_NAMESPACE(key_RightArrow), 0, 0, 0, 0 },
{ NSF1FunctionKey             , IN_PDG_NAMESPACE(key_F1), 0, 0, 0, 0 },
{ NSF2FunctionKey             , IN_PDG_NAMESPACE(key_F2), 0, 0, 0, 0 },
{ NSF3FunctionKey             , IN_PDG_NAMESPACE(key_F3), 0, 0, 0, 0 },
{ NSF4FunctionKey             , IN_PDG_NAMESPACE(key_F4), 0, 0, 0, 0 },
{ NSF5FunctionKey             , IN_PDG_NAMESPACE(key_F5), 0, 0, 0, 0 },
{ NSF6FunctionKey             , IN_PDG_NAMESPACE(key_F6), 0, 0, 0, 0 },
{ NSF7FunctionKey             , IN_PDG_NAMESPACE(key_F7), 0, 0, 0, 0 },
{ NSF8FunctionKey             , IN_PDG_NAMESPACE(key_F8), 0, 0, 0, 0 },
{ NSF9FunctionKey             , IN_PDG_NAMESPACE(key_F9), 0, 0, 0, 0 },
{ NSF10FunctionKey            , IN_PDG_NAMESPACE(key_F10), 0, 0, 0, 0 },
{ NSF11FunctionKey            , IN_PDG_NAMESPACE(key_F11), 0, 0, 0, 0 },
{ NSF12FunctionKey            , IN_PDG_NAMESPACE(key_F12), 0, 0, 0, 0 },
{ NSInsertFunctionKey         , IN_PDG_NAMESPACE(key_Insert), 0, 0, 0, 0 },
{ NSDeleteFunctionKey         , IN_PDG_NAMESPACE(key_Delete), 0, 0, 0, 0 },
{ NSHomeFunctionKey           , IN_PDG_NAMESPACE(key_Home), 0, 0, 0, 0 },
{ NSBeginFunctionKey          , IN_PDG_NAMESPACE(key_Home), 0, 0, 0, 0 },
{ NSEndFunctionKey            , IN_PDG_NAMESPACE(key_End), 0, 0, 0, 0 },
{ NSPageUpFunctionKey         , IN_PDG_NAMESPACE(key_PageUp), 0, 0, 0, 0 },
{ NSPageDownFunctionKey       , IN_PDG_NAMESPACE(key_PageDown), 0, 0, 0, 0 },
{ NSPauseFunctionKey          , IN_PDG_NAMESPACE(key_Pause), 0, 0, 0, 0 },
{ NSSysReqFunctionKey         , IN_PDG_NAMESPACE(key_Break), 0, 0, 1, 0 },
{ NSBreakFunctionKey          , IN_PDG_NAMESPACE(key_Break), 0, 0, 0, 0 },
{ NSResetFunctionKey          , IN_PDG_NAMESPACE(key_Break), 0, 0, 1, 1 },
{ NSStopFunctionKey           , IN_PDG_NAMESPACE(key_Break), 0, 1, 0, 0 },
{ NSMenuFunctionKey           , IN_PDG_NAMESPACE(key_Escape), 0, 0, 0, 1 },
{ NSUserFunctionKey           , IN_PDG_NAMESPACE(key_Escape), 0, 0, 1, 0 },
{ NSSystemFunctionKey         , IN_PDG_NAMESPACE(key_Escape), 0, 1, 0, 0 },
{ NSClearLineFunctionKey      , IN_PDG_NAMESPACE(key_Clear), 1, 0, 0, 0 },
{ NSClearDisplayFunctionKey   , IN_PDG_NAMESPACE(key_Clear), 0, 0, 1, 0 },
{ NSInsertLineFunctionKey     , IN_PDG_NAMESPACE(key_Insert), 0, 0, 1, 0 },
{ NSDeleteLineFunctionKey     , IN_PDG_NAMESPACE(key_Delete), 0, 0, 1, 0 },
{ NSInsertCharFunctionKey     , IN_PDG_NAMESPACE(key_Insert), 0, 0, 0, 0 },
{ NSDeleteCharFunctionKey     , IN_PDG_NAMESPACE(key_Delete), 0, 0, 0, 0 },
{ NSPrevFunctionKey           , IN_PDG_NAMESPACE(key_Tab), 1, 0, 0, 0 },
{ NSNextFunctionKey           , IN_PDG_NAMESPACE(key_Tab), 0, 0, 0, 0 },
{ NSExecuteFunctionKey        , IN_PDG_NAMESPACE(key_Enter), 0, 0, 0, 0 },
{ NSUndoFunctionKey           , 'z', 0, 0, 0, 1 },
{ NSRedoFunctionKey           , 'Z', 1, 0, 0, 1 },
{ NSFindFunctionKey           , ' ', 0, 0, 0, 1 },  // cmd - space
{ NSHelpFunctionKey           , IN_PDG_NAMESPACE(key_Help), 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0 }
};

unichar remapKeyboardChar(unichar character, int modifierFlags, BOOL& shift, BOOL& control, BOOL& alt, BOOL& cmd) {
	shift = ((modifierFlags & NSShiftKeyMask) != 0);
	control = ((modifierFlags & NSControlKeyMask) != 0);
	alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	cmd = ((modifierFlags & NSCommandKeyMask) != 0);
	// check for special keys that must be mapped
	if (character >= NSUpArrowFunctionKey) {
		for (int i = 0; i < SIZE_OF_KEY_MAP; i++) {
			if (keyMap[i][0] == character) {
				character = keyMap[i][1];  // replace the character
				shift |= keyMap[i][2];	   // add additional modifier keys if needed
				control |= keyMap[i][3];
				alt |= keyMap[i][4];
				cmd |= keyMap[i][5];
				break;
			} else if (keyMap[i][0] == 0) {  // at end of list
				break;
			}
		}
	}
	return character;
}

// ===================================

@implementation PDGOpenGLView

// pixel format definition
+ (NSOpenGLPixelFormat*) basicPixelFormat
{
	if (pdg::main_isFullscreen()) {
		NSOpenGLPixelFormatAttribute attributes [] = {
			// Specify that we want a full-screen OpenGL context.
			NSOpenGLPFAFullScreen,
			
			// We may be on a multi-display system (and each screen may be driven by a different 
			// renderer), so we need to specify which screen we want to take over.  For this 
			// demo, we'll specify the main screen.
			NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
			NSOpenGLPFANoRecovery,
			NSOpenGLPFADoubleBuffer,	// double buffered
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)0,//16, // 16 bit depth buffer
			NSOpenGLPFAAccelerated,
			(NSOpenGLPixelFormatAttribute)0
		};
		return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
	} else {
		NSOpenGLPixelFormatAttribute attributes [] = {
			// Specify that we want a windowed context.
			NSOpenGLPFAWindow,
			
			NSOpenGLPFANoRecovery, 
			NSOpenGLPFADoubleBuffer,	// double buffered
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)0, //16, // 16 bit depth buffer
			NSOpenGLPFAAccelerated,
			(NSOpenGLPixelFormatAttribute)0
		};
		return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
	}
}

- (void) eraseToBlack
{
	// erase to black
    [[self openGLContext] makeCurrentContext];
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	[[self openGLContext] flushBuffer];	
}


- (void) handleQuitting
{
	printf("PDGOpenGLView handleQuitting()\n");
	gHaveViewTimers = FALSE; // so we know pdg_main_run() will no longer be called from the run loop 
	[timer invalidate];
}

- (void)keyDown:(NSEvent *)theEvent
{
    NSString *characters = [theEvent characters];
	BOOL repeat = [theEvent isARepeat];
	BOOL shift, control, alt, cmd;
	unichar character = 0;
    if ([characters length]) {
		character = remapKeyboardChar([characters characterAtIndex:0], [theEvent modifierFlags], shift, control, alt, cmd);
	}
	if (!repeat) {
		int keyCode = [theEvent keyCode];
        pdg::main_handleKeyDown(keyCode, character);
	}
	if (character) {
        pdg::main_handleKeyPress(character, repeat, shift, control, alt, cmd);
	}
}

- (void)keyUp:(NSEvent *)theEvent
{
    NSString *characters = [theEvent characters];
	BOOL shift, control, alt, cmd;
	unichar character = 0;
    if ([characters length]) {
		character = remapKeyboardChar([characters characterAtIndex:0], [theEvent modifierFlags], shift, control, alt, cmd);
	}
	int keyCode = [theEvent keyCode];
    pdg::main_handleKeyUp(keyCode, character);
}

// ---------------------------------

- (void)mouseDown:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
    pdg::main_handleMouse(pdg::mouseEventType_MouseDown, location.x, y, 0, shift, control, alt, cmd);
}

// ---------------------------------

- (void)rightMouseDown:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
    pdg::main_handleMouse(pdg::mouseEventType_MouseDown, location.x, y, 1, shift, control, alt, cmd);
}

// ---------------------------------

- (void)otherMouseDown:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
	int button = [theEvent buttonNumber];
    pdg::main_handleMouse(pdg::mouseEventType_MouseDown, location.x, y, button, shift, control, alt, cmd);
}

// ---------------------------------

- (void)mouseUp:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
    pdg::main_handleMouse(pdg::mouseEventType_MouseUp, location.x, y, 0, shift, control, alt, cmd);
}

// ---------------------------------

- (void)rightMouseUp:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
    pdg::main_handleMouse(pdg::mouseEventType_MouseUp, location.x, y, 1, shift, control, alt, cmd);
}

// ---------------------------------

- (void)otherMouseUp:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
	int button = [theEvent buttonNumber];
    pdg::main_handleMouse(pdg::mouseEventType_MouseUp, location.x, y, button, shift, control, alt, cmd);
}

// ---------------------------------

- (void)mouseMoved:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
    pdg::main_handleMouse(pdg::mouseEventType_MouseMoved, location.x, y, 0, shift, control, alt, cmd);
}

// ---------------------------------

- (void)mouseDragged:(NSEvent *)theEvent
{
	int modifierFlags = [theEvent modifierFlags];
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    NSRect r = [self convertRect:[self visibleRect] fromView:nil];
	float y =  (r.size.height - r.origin.y) - location.y;
	BOOL shift = ((modifierFlags & NSShiftKeyMask) != 0);
	BOOL control = ((modifierFlags & NSControlKeyMask) != 0);
	BOOL alt = ((modifierFlags & NSAlternateKeyMask) != 0);
	BOOL cmd = ((modifierFlags & NSCommandKeyMask) != 0);
	int button = [theEvent buttonNumber];
    pdg::main_handleMouse(pdg::mouseEventType_MouseDragged, location.x, y, button, shift, control, alt, cmd);
}

// ---------------------------------

- (void)scrollWheel:(NSEvent *)theEvent
{
	float wheelDelta = [theEvent deltaX] +[theEvent deltaY] + [theEvent deltaZ];
	if (wheelDelta)
	{
		[self setNeedsDisplay: YES];
	}
}

// ---------------------------------

- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[self mouseDragged: theEvent];
}

// ---------------------------------

- (void)otherMouseDragged:(NSEvent *)theEvent
{
	[self mouseDragged: theEvent];
}

// ---------------------------------

// this can be a troublesome call to do anything heavyweight, as it is called on window moves, resizes, and display config changes.  So be
// careful of doing too much here.
- (void) update // window resizes, moves and display changes (resize, depth and display config change)
{
	[super update];
	if ([self inLiveResize])  {// doing live resize
		NSSize newSize = [ self bounds].size;
        pdg::main_handleLiveResize(newSize.width, newSize.height);
	} else {
        pdg::main_handleScreenUpdate(IN_PDG_NAMESPACE(screenPos_Normal));
	}
}

// ---------------------------------

-(id) initWithFrame: (NSRect) frameRect
{
	NSOpenGLPixelFormat * pf = [PDGOpenGLView basicPixelFormat];

	self = [super initWithFrame: frameRect pixelFormat: pf];
	self->onScreenNum = 0;
	self->isFullscreen = FALSE;

	[ [self window] setAcceptsMouseMovedEvents:YES];
	// start animation timer
	timer = [NSTimer timerWithTimeInterval:(1.0f/100.0f) target:self selector:@selector(animationTimer:) userInfo:nil repeats:YES];
	NSRunLoop* runloop = [NSRunLoop currentRunLoop];
	[runloop addTimer:timer forMode:NSDefaultRunLoopMode];
	[runloop addTimer:timer forMode:NSEventTrackingRunLoopMode]; // ensure timer fires during resize
	[runloop addTimer:timer forMode:NSModalPanelRunLoopMode];	 // ensure time fires during modal panels

	gHaveViewTimers = TRUE; // so we know pdg_main_run() will be called from the run loop 
	
	[self eraseToBlack];
//	[self eraseToBlack];
	
    return self;
}

// ---------------------------------
- (void)finalize 
{
	printf("glview finalize() removing view timers\n");
	gHaveViewTimers = FALSE; // so we know pdg_main_run() will no longer be called from the run loop 
    [super finalize];
}

// ---------------------------------
- (void)dealloc 
{
	gHaveViewTimers = FALSE; // so we know pdg_main_run() will no longer be called from the run loop 
	[super dealloc];
}

// ---------------------------------

- (BOOL)acceptsFirstResponder
{
	return YES;
}

// ---------------------------------

- (BOOL)becomeFirstResponder
{
	return  YES;
}

// ---------------------------------

- (BOOL)resignFirstResponder
{
	return YES;
}

// ---------------------------------

- (void) awakeFromNib
{	
}

// ---------------------------------

// per-window timer function, basic time based animation preformed here
- (void)animationTimer:(NSTimer *)timer
{
    pdg::main_run();
//	printf("ENTER glview animationTimer()\n");
//	NSRect rectView = [self bounds];
//	[self drawRect: rectView];
//	printf("EXIT glview animationTimer()\n");
}

// ---------------------------------
// this is sometimes called from other parts of the NSOpenGLView, so do everything main loop related here
- (void) drawRect:(NSRect)rect
{
	if (gPDG_Quitting) {
		printf("drawRect() detected quitting\n");
		[self handleQuitting];
		return;
	}

    [[self openGLContext] makeCurrentContext];
	
	if (gPDG_IsInitialized) {
        pdg::main_run();
        pdg::platform_pollEvents();
	
#ifdef PDG_NO_DOUBLE_BUFFER
		if (gPortDirty)
#endif
		{
			[[self openGLContext] flushBuffer];	
		}
	}
}

// ---------------------------------

// set initial OpenGL state (current context is set)
// called after context is created
- (void) prepareOpenGL
{
#ifndef PDG_NO_DOUBLE_BUFFER
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; // set to vbl sync
#endif
	
	// init GL stuff here
//	glEnable(GL_DEPTH_TEST);
	
//	glShadeModel(GL_SMOOTH);    
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
	glPolygonOffset (1.0f, 1.0f);
	
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
}


@end

