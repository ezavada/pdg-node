// -----------------------------------------------
// events.h
// 
// definitions for events that the framework should generate
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


#ifndef PDG_EVENTS_H_INCLUDED
#define PDG_EVENTS_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"

#ifndef __cplusplus
  #define class typedef void
  #define PDG_SUBCLASS_OF(className)
  #define PDG_INHERITED_FIELDS_FROM(className) className its ## className;
  #define PDG_CLASS_TYPEDEF(className) typedef struct className className;
#else
  #define PDG_SUBCLASS_OF(className) : public className
  #define PDG_INHERITED_FIELDS_FROM(className)
  #define PDG_CLASS_TYPEDEF(className)
#endif

#ifdef PDG_USE_CHIPMUNK_PHYSICS
typedef struct cpConstraint cpConstraint;
typedef struct cpArbiter cpArbiter;
#endif
	

//! \defgroup Events
//! Collection of classes, types and constants used by the Event Manager

#ifdef __cplusplus
namespace pdg {
#endif // cplusplus	

class Sound;
class Sprite;
class SpriteLayer;
class Port;
	
// ===============
// Specific events 
// ===============

//! EventTypes
//! these are used by the EventManager to identify specific events
//! \ingroup Events
enum {
    eventType_Startup       = 1,        // application startup, data = startup info
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

    eventType_last
};

// ================
// Special keycodes 
// ================

//! Keycodes
//! these will be returned in KeyPressInfo->unicode
//! \ingroup Events
enum {

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

    key_FirstPrintable = 32 // ASCII Space -- first printable character 
};

// for indicating how a device that can detect its orientation in
// real world space is actually positioned
enum {
    screenPos_Normal,
    screenPos_Rotated180,				  // screen upside down
    screenPos_Rotated90Clockwise,		  // screen rotated 90 degrees clockwise, ie: normal left side is up
    screenPos_Rotated90CounterClockwise,  // screen rotated 90 degrees counter-clockwise, ie: normal right side is up
    screenPos_FaceUp,				      // screen is face up
    screenPos_FaceDown					  // screen is face down
};

	
//! Event Data for eventType_Startup.
//! This is the first event your game will receive from the Pixel Dust Games Framework. 
//! It provides all the managers pre-initialized and ready to use. Your game should save the
//! references to them in your main game application class.
//! \ingroup Events
struct StartupInfo {
	//! the reason the application is starting up
    long        startupReason;
	//! the number of parameters that were passed to the app on startup
    long        startupParamCount;
	//! the actual parameters as an array of strings
    const char** startupParam;
};
PDG_CLASS_TYPEDEF(StartupInfo)

//! Event Data for eventType_Shutdown.
//! This is the last event your game will receive from the Pixel Dust Games Framework. 
//! \ingroup Events
struct ShutdownInfo {
	//! the reason the application is being told to shutdown
    long    exitReason;   
	//! the exit code that will be returned to the OS
    long    exitCode; 
};
PDG_CLASS_TYPEDEF(ShutdownInfo)

//! Event Data for eventType_Timer.
//! This is the event that will be received whenever a timer fires 
//! \ingroup Events
struct TimerInfo {
	//! this id tells us which timer fired
    long            id;           
	//! the current millisecond timestamp, as if you had called OS::getMilliseconds()
    unsigned long   millisec;     
	//! how long (excluding pause time) since last firing, or since started if first firing
	uint32			msElapsed;
	//! the data associated with this timer, passed in by app when timer was created
    void*           userData;
};
PDG_CLASS_TYPEDEF(TimerInfo)

//! Event Data for eventType_KeyDown and eventType_KeyUp. 
//! On some platforms controller/joystick buttons might be represented with key codes also
//! \ingroup Events
struct KeyInfo {
	//! virtual key codes to represent the physical key (leftshift, enter, etc..)
    long        keyCode;      
};
PDG_CLASS_TYPEDEF(KeyInfo)

//! Base structure for any event that might care about modifier keys. 
//! \ingroup Events
struct ModifierKeyInfo {
	//! true if shift key is down
	bool        shift;    
	//! true if control key is down
	bool        ctrl;     
	//! true if alt key is down
	bool        alt;     
	//! true if some other modifier key is down
	bool        meta;     
};
PDG_CLASS_TYPEDEF(ModifierKeyInfo)
	

//! Event Data for eventType_KeyPress.
//! This event will be received for each key press and also for repeat key events when a key is held down 
//! \ingroup Events
struct KeyPressInfo PDG_SUBCLASS_OF( ModifierKeyInfo ) {
	PDG_INHERITED_FIELDS_FROM(ModifierKeyInfo)
	//! the unicode char code for the key, or 0 if no unicode equivalent
	utf16char   unicode;      
	//! true if this is a repeat key event because the key is held down
    bool        isRepeating;  
};
PDG_CLASS_TYPEDEF(KeyPressInfo)

//! Event Data for eventType_MouseDown, eventType_MouseUp, and eventType_MouseMoved.
//! On multitouch platforms touches and gestures are represented by a series of mouse events
//! where buttonNumber represents a unique finger by order it was placed on the touch surface
//! \ingroup Events
struct MouseInfo PDG_SUBCLASS_OF( ModifierKeyInfo ) {
	PDG_INHERITED_FIELDS_FROM(ModifierKeyInfo)
	//! location of the mouse at the time of the event
    Point       mousePos;
	//! state of the left mouse button, true means pressed
    bool        leftButton;
	//! state of the right mouse button, true means pressed
    bool        rightButton;
	//! button number of the mouse button for mouseUp, mouseDown: 0 = left, 1 = right, 2 = middle button, etc...
	//! or for touch gestures this is the unique finger by order placed on the touch surface
    uint8       buttonNumber;
	//! mouse position where the last mouse *down* happened
    Point       lastClickPos;
	//! how many milliseconds since the last mouse *down* event
    unsigned long lastClickElapsed;
};
PDG_CLASS_TYPEDEF(MouseInfo)

//! Event Data for eventType_MouseEnter, and eventType_MouseLeave.
//! \ingroup Events
struct MouseTrackingInfo PDG_SUBCLASS_OF( MouseInfo ) {
	PDG_INHERITED_FIELDS_FROM(MouseInfo)
	//! true if the mouse is entering the rectangle, false if it is leaving
	bool        entering;
	//! the tracking reference for this tracking rectangle, pass to port->stopTrackingMouse() to stop tracking
    int         trackingRef;   
	//! the data associated with this tracking rectangle, passed in by app when tracking was started
    void*       userData;
	//! the area being tracked, in port coordinates
    Rect        trackingArea;
};
PDG_CLASS_TYPEDEF(MouseTrackingInfo)

//! Event Data for the eventType_PortResized event
//! \ingroup Events
//! \ingroup Graphics
struct PortResizeInfo {
	//! the port that was resized
    Port*   port;
    //! orientation of the screen in real space
	int		screenPos; 
	//! the old port size
	long	oldWidth;
	long	oldHeight;
	//! the old orientation
	int		oldScreenPos;
};
PDG_CLASS_TYPEDEF(PortResizeInfo)

//! Event Data for the eventType_PortDraw event
//! \ingroup Events
//! \ingroup Graphics
struct PortDrawInfo {
	//! the port that needs to be redrawn
    Port*   port;
    //! sequential count of draw events since app startup
	uint32	frameNum;
};
PDG_CLASS_TYPEDEF(PortDrawInfo)

//! Event Data for eventType_ScrollWheel.
//! \ingroup Events
struct ScrollWheelInfo PDG_SUBCLASS_OF( ModifierKeyInfo ) {
	PDG_INHERITED_FIELDS_FROM(ModifierKeyInfo)
	//! the amount of movement horizontally, with positive movement being to the right, negative to the left
	int         horizDelta;
	//! the amount of movement vertically, with positive movement being down, negative up
	int         vertDelta;
};
PDG_CLASS_TYPEDEF(ScrollWheelInfo)

//! \ingroup Sound
//! \ingroup Events
enum {
    soundEvent_DonePlaying,         //! a non-looping sound finished playing
    soundEvent_Looping,             //! a looping sound finished playing and is restarting
    soundEvent_FailedToPlay         //! a sound failed to play
};

//! \ingroup Sound
//! \ingroup Events
struct SoundEventInfo {
 	//! what type of event occurred, one of the SoundEvent_Xxx enum values
    long        eventCode;
    //! what sound did the event happen to
    Sound*      sound;              
};
PDG_CLASS_TYPEDEF(SoundEventInfo)

//! Event Data for eventType_SpriteAnimate.
//! \ingroup Events
//! \ingroup Animation
struct SpriteAnimateInfo {
	//! what action is being taken, one of the pdg::Sprite::action_Xxxx enumeration values
	uint16		action;
	//! which sprite is doing the animation
    Sprite*   	actingSprite;
	//! what layer the action happened in
	SpriteLayer* inLayer;
};
PDG_CLASS_TYPEDEF(SpriteAnimateInfo)

//! Event Data for eventType_SpriteCollide.
//! \ingroup Events
//! \ingroup Animation
//! \ingroup Sprites
//! \ingroup Physics
struct SpriteCollideInfo PDG_SUBCLASS_OF( SpriteAnimateInfo ) {
	PDG_INHERITED_FIELDS_FROM(SpriteAnimateInfo)
	//! the other sprite in the collision (cpBody/Shape B in the Chipmunk Arbiter)
	Sprite*		targetSprite;
	//! which sprite is doing the animation
    Vector      normal;    
    //! the impulse (instantaneous force) applied to resolve the collision
    Vector      impulse;
    //! the force of the collision
    float		force;
    //! the energy lost in the collision
    float       kineticEnergy;
#ifdef PDG_USE_CHIPMUNK_PHYSICS
    //! the Chipmunk Collision Arbiter (only when PDG is compiled with Chipmunk physics support)
    cpArbiter*  arbiter;
#endif
};
PDG_CLASS_TYPEDEF(SpriteCollideInfo)

#ifdef PDG_USE_CHIPMUNK_PHYSICS
//! Event Data for eventType_SpriteBreak.
//! \ingroup Events
//! \ingroup Animation
//! \ingroup Sprites
//! \ingroup Physics
struct SpriteJointBreakInfo PDG_SUBCLASS_OF( SpriteAnimateInfo ) {
	PDG_INHERITED_FIELDS_FROM(SpriteAnimateInfo)
	//! the other sprite we were connected to
    Sprite*		targetSprite;
    //! the magnitude of the impulse (instantaneous force) that broke the joint
    float       impulse;
    //! the amount of force that was actually applied
    float       force;
    //! the force that was required to break the joint
    float       breakForce; 
    //! the joint that broke (pointer to a Chipmunk structure)
    cpConstraint*  joint;  
};
PDG_CLASS_TYPEDEF(SpriteJointBreakInfo)
#endif

//! Event Data for eventType_SpriteLayer.
//! \ingroup Events
//! \ingroup Animation
//! \ingroup Sprites
struct SpriteLayerInfo {
	//! what layer action is being taken, one of the pdg::SpriteLayer::action_Xxxx enumeration values
	uint16			action;			
    //! which layer is doing the action
	SpriteLayer*    actingLayer;
    //! the time stamp at which the action occurred
	uint32          millisec;  
};
PDG_CLASS_TYPEDEF(SpriteLayerInfo)

//! Event Data for eventType_SpriteTouch.
//! \ingroup Events
//! \ingroup Sprites
struct SpriteTouchInfo PDG_SUBCLASS_OF(  MouseInfo ) {
	PDG_INHERITED_FIELDS_FROM(MouseInfo)
	//! what kind of touch occurred, one of the pdg::Sprite::touch_Xxxx enumeration values
	uint16			touchType;			
    //! which sprite was touched
	Sprite*			touchedSprite;
    //! what layer the Sprite was in when touched
	SpriteLayer*	inLayer;
};
PDG_CLASS_TYPEDEF(SpriteTouchInfo)


#ifdef __cplusplus
} // end namespace pdg
#endif

#endif //PDG_EVENTS_H_INCLUDED
