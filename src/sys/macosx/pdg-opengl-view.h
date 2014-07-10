// -----------------------------------------------
// pdg-opengl-view.h
// 
// Mac OS X declaration of NS View for OpenGL
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

unichar remapKeyboardChar(unichar character, int modifierFlags, BOOL& shift, BOOL& control, BOOL& alt, BOOL& cmd);

// ------------ FSWindow
@interface FSWindow : NSWindow
{
	NSView* glview;
}
- (BOOL) canBecomeKeyWindow;
- (void) close;
- (void) setGLView:(NSView *)view;
@end


// ----------- PDGOpenGLView
@interface PDGOpenGLView : NSOpenGLView
{ 
	NSTimer* timer; 
    @public BOOL isFullscreen;
    @public int onScreenNum;
}

- (void)animationTimer:(NSTimer *)timer;
- (void) drawRect:(NSRect)rect;
- (void) prepareOpenGL;
- (void) handleQuitting;
- (void) eraseToBlack;

+ (NSOpenGLPixelFormat*) basicPixelFormat;

- (void) keyDown:(NSEvent *)theEvent;
- (void) keyUp:(NSEvent *)theEvent;
- (void) mouseDown:(NSEvent *)theEvent;
- (void) rightMouseDown:(NSEvent *)theEvent;
- (void) otherMouseDown:(NSEvent *)theEvent;
- (void) mouseUp:(NSEvent *)theEvent;
- (void) rightMouseUp:(NSEvent *)theEvent;
- (void) otherMouseUp:(NSEvent *)theEvent;
- (void) mouseMoved:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;
- (void) scrollWheel:(NSEvent *)theEvent;
- (void) rightMouseDragged:(NSEvent *)theEvent;
- (void) otherMouseDragged:(NSEvent *)theEvent;


- (void) update;		// moved or resized

- (BOOL) acceptsFirstResponder;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

- (id)   initWithFrame: (NSRect) frameRect;
- (void) finalize;
- (void) dealloc;
- (void) awakeFromNib;
@end

