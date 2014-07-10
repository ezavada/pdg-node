// -----------------------------------------------
// events.js
//
// pure javascript implementation of 
// core application/system interface
//
// Written by Ed Zavada, 2004-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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

// -----------------------------------------------------------------------------------
// EventHandler
// Wrapper for callbacks to handle events
// -----------------------------------------------------------------------------------

//! EventTypes
//! these are used by the EventManager to identify specific events
//! \ingroup Events
var eventType_Startup       = 1,        // application startup, data = startup info
    eventType_Shutdown      = 2,        // application shutdown, data = shutdown info
    eventType_Timer         = 3,        // a timer fired, data = timer info
    eventType_KeyDown       = 4,        // key went from unpressed to pressed, data = key info
    eventType_KeyUp         = 5,        // key went from pressed to released,  data = key info
    eventType_KeyPress      = 6,        // key was pressed & released, or is repeating, data = key press info
    eventType_MouseDown     = 7,        // mouse button went from up to down, data = mouse info
    eventType_MouseUp       = 8,        // mouse button went from down to up, data = mouse info
    eventType_MouseMove     = 9,        // the mouse was moved
    eventType_NetConnect    = 10,       // a connection was established, data = net connect info
    eventType_NetDisconnect = 11,       // a connection was closed, data = net disconnect info
    eventType_NetData       = 12,       // data was received on a connection, data = net data
    eventType_NetError      = 13,       // an error occurred on a connection, data = net error info
    eventType_SoundEvent    = 14,       // something happened with a sound that was playing, data = sound event info
    eventType_PortResized   = 15,       // a graphics port was resized, data = port info
	eventType_ScrollWheel   = 16,       // the scroll wheel on the mouse was used (or a scroll gesture on a touch device)
	eventType_SpriteAnimate	= 17,       // something happened to a sprite's animation
	eventType_SpriteLayer	= 18,       // something happened to a sprite layer
	eventType_SpriteTouch	= 19,		// the user did something directly to a sprite: a tap, mouse click, mouse over, etc...
	eventType_SpriteCollide = 20,		// a sprite was involved in a collision
    eventType_SpriteBreak   = 21,       // Chipmunk Physics Only: a joint on a sprite broke apart
	eventType_MouseEnter	= 22,		// the mouse entered a tracking area
	eventType_MouseLeave	= 23,		// the mouse left a tracking area
	eventType_PortDraw		= 24,		// a port wants to be redrawn

    eventType_last = eventType_PortDraw;

// ================
// Special keycodes 
// ================

//! Keycodes
//! these will be returned in KeyPressInfo->unicode
//! \ingroup Events
var

    // Do not change these values for a particular platform
    // Change the implementation of keypress handling for the
    // platform to map platform specific values to these
    
    key_Break      = 1,     // ASCII SOH -- start of heading
    
	key_Home	   = 2,     // ASCII STX -- start of text
	key_End		   = 3,     // ASCII ETX -- end of text
	
    key_Clear      = 4,     // ASCII EOT -- end of transmission

    key_Help       = 5,     // ASCII ENQ -- enquiry

    key_Pause      = 6,     // ASCII ACK -- acknowledge
    key_Mute       = 7,     // ASCII BEL -- bell

    key_Backspace  = 8,     // ASCII BS  -- backspace
    key_Delete     = 127,   // ASCII DEL -- delete

    key_Tab        = 9,     // ASCII TAB -- horizontal tab

    key_PageUp     = 11,    // ASCII VT  -- vertical tab
    key_PageDown   = 12,    // ASCII FF  -- form feed/new page
    
    key_Return     = 13,    // ASCII CR  -- carriage return
    key_Enter      = key_Return, // must use raw keycodes to distinguish
    
    key_F1         = 14,    // for these, we ignore which ASCII
    key_F2         = 15,    // they are, and just find a block
    key_F3         = 16,    // altogether
    key_F4         = 17,
    key_F5         = 18,
    key_F6         = 19,
    key_F7         = 20,
    key_F8         = 21,
    key_F9         = 22,
    key_F10        = 23,
    key_F11        = 24,
    key_F12        = 25,
    key_FirstF     = key_F1,
    key_LastF      = key_F12,

    key_Insert     = 26,    // ASCII SUB -- substitute

    key_Escape     = 27,    // ASCII ESC -- escape

    key_LeftArrow  = 28,    // ASCII FS  -- file separator
    key_RightArrow = 29,    // ASCII GS  -- group separator
    key_UpArrow    = 30,    // ASCII RS  -- record separator
    key_DownArrow  = 31,    // ASCII US  -- unit sepatator

    key_FirstPrintable = 32; // ASCII Space -- first printable character 


// for indicating how a device that can detect its orientation in
// real world space is actually positioned
var screenPos_Normal = 0,
    screenPos_Rotated180 = 1,				  // screen upside down
    screenPos_Rotated90Clockwise = 2,		  // screen rotated 90 degrees clockwise, ie: normal left side is up
    screenPos_Rotated90CounterClockwise = 4,  // screen rotated 90 degrees counter-clockwise, ie: normal right side is up
    screenPos_FaceUp = 4,				      // screen is face up
    screenPos_FaceDown = 5;					  // screen is face down

