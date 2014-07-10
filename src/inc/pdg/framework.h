// -----------------------------------------------
// framework.h
// 
// Include file that pulls in all the Pixel Dust
// framework includes
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2004-2012, Dream Rock Studios, LLC
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

#ifndef PDG_FRAMEWORK_H_INCLUDED
#define PDG_FRAMEWORK_H_INCLUDED

#define PDG_FRAMEWORK

#include "pdg_project.h"

// some defines you can use to disable features
// you should probably define them in your build
// environment rather than here. You can also
// define these on a per-project basis in your
// own copy of pdg_project.h

// Major Features:
//#define PDG_NO_NETWORK       		// disable networking
//#define PDG_NO_EVENT_QUEUE   		// disable multithread event queue
//#define PDG_NO_THREAD_SAFETY 		// disable thread safety (mutexes, semaphores and critical sections do nothing)
//#define PDG_NO_GUI           		// disable GUI stuff
//#define PDG_NO_SOUND         		// disable sound stuff
//#define PDG_USE_CHIPMUNK_PHYSICS	// use Chipmunk library for sprite physics
//#define PDG_HACK_GL_QUAD_SUPPORT	// emulate support for GL_QUADS when using OpenGL ES
//#define PDG_NO_MERSENNE_TWISTER	// disable built-in Mersenne Twister algorithm for OS::gameCriticalRandom()
//#define PDG_SCML_SUPPORT          // enable Spriter SCML animation support

// Build Environment/Target:
//#define PDG_NO_64BIT							// don't add support for int64, uint64
//#define PDG_LIBRARY		   					// we are building a library, so don't include main()
//#define PDG_INTERNAL_LIB	   					// pdg is internal to other code, such as a scripting language plugin
//#define PDG_COMPILING_FOR_SCRIPT_BINDINGS     // enable code that supports any script binding
//#define PDG_COMPILING_FOR_RUBY				// enable code that supports Ruby bindings
//#define PDG_COMPILING_FOR_JAVASCRIPT			// enable code that supports Javascript bindings
//#define PDG_DECORATE_GLOBAL_TYPES				// use pdg_{type} for int8, uint8, etc...
//#define PDG_SERIALIZE_NO_STD_STRING_SUPPORT  	// no built-in support for std::string to Serializer/Deserializer
//#define PDG_BUILDING_INTERFACE_FILES			// language binding interfaces --> c++ code ONLY
//#define PDG_USE_AUDIO_TOOLBOX_INTERNAL_THREAD	// sound without run loop (Mac OS X ONLY)

// Fine Tuning:
//#define PDG_BASE_COORD_TYPE float  			// what the base type for coordinate elements is
//#define MAX_FRAMES_PER_SPRITE 256				// individual animation frames (images)
//#define MAX_BREAKABLE_JOINTS_PER_SPRITE 16	// max joints that can have a breakingForce set
//#define PDG_NO_SERIALIZER_SANITY_CHECKS		// look for out of sync serialization
//#define SPRITE_IGNORE_ANIMATION_TIMER_DRIFT	// use fixed time step rather than actual elapsed time
//#define MAX_CACHED_STRING_TEXTURES 250		// largest number of strings that can be onscreen at once
//#define PDG_NO_SLEEP							// do not sleep in the PDG engine waiting for timers to fire
//#define PDG_DESERIALIZER_NO_THROW             // don't allow deserializer to throw C++ exceptions

// Application Debugging Support:
//#define PDG_SERIALIZATION_DEBUG		// dynamic debug mode in serializer and deserializer that dumps serialization info

// Library Debugging:
//#define PDG_DEBUG_MUTEX				// detailed info on mutex blocking
//#define PDG_DEBUG_EVENTS				// detailed info on event handling
//#define PDG_DEBUG_NETWORKING			// detailed info on networking
//#define PDG_DEBUG_TIMERS				// detailed info on timers
//#define PDG_DEBUG_RUN_LOOP			// detailed info on run loop
//#define PDG_DEBUG_SOUND				// detailed info on sounds
//#define DEBUG_MOUSE_EVENTS			// detailed info on mouse event handling (Mac OS X ONLY)
//#define SPRITE_INTERNAL_DEBUG			// draw sprite frame borders
//#define SPRITE_MOTION_DEBUG			// draw sprite motion vector lines
//#define SPRITE_DEBUG_COLLISIONS		// draw sprite collision boxes
//#define SPRITELAYER_INTERNAL_DEBUG	// draw sprite layer origin and center point
//#define TILING_INTERNAL_DEBUG			// visual aids for tile layer debugging
//#define DEBUG_PIXEL_COLLISIONS     	// only when compiling Parthenon
//#define CVTUTF_DEBUG					// debug UTF8 conversions
//#define PDG_GFX_POINTER_SAFETY_CHECKS	// runtime image manipulation checks for buffer overwrites 

#include "pdg/version.h"

// System Framework
#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/singleton.h"
#include "pdg/sys/events.h"
#include "pdg/sys/os.h"
#include "pdg/sys/core.h"
#include "pdg/sys/resource.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/network.h"
#include "pdg/sys/log.h"
#include "pdg/sys/config.h"
#include "pdg/sys/spline.h"
#include "pdg/sys/animated.h"
#include "pdg/sys/serializer.h"
#include "pdg/sys/deserializer.h"
#include "pdg/sys/serializable.h"

// Parts of the animation system that still work without a GUI
#include "pdg/sys/coordinates.h"
#include "pdg/sys/color.h"
#include "pdg/sys/image.h"  	// loading and dimensions only when non-gui
#include "pdg/sys/imagestrip.h"

// System Framework GUI
#ifndef PDG_NO_GUI
  #include "pdg/sys/font.h"
  #include "pdg/sys/port.h"
  #include "pdg/sys/graphicsmanager.h"
#endif

#ifndef PDG_NO_SOUND
  #include "pdg/sys/sound.h"
#endif

// Parts of the animation system that still work without a GUI
#include "pdg/sys/sprite.h"
#include "pdg/sys/spritelayer.h"
#include "pdg/sys/tilelayer.h"

#ifndef PDG_NO_APP_FRAMEWORK
  // Application Framework

  #include "pdg/app/Application.h"
  #include "pdg/app/Observer.h"
  #include "pdg/app/FloodLock.h"

  // Application Framework GUI
  #ifndef PDG_NO_GUI
    #include "pdg/app/Controller.h"
	#include "pdg/app/ModalController.h"
	#include "pdg/app/TouchController.h"
    #include "pdg/app/Dialog.h"
    #include "pdg/app/View.h"
	#include "pdg/app/ScrollingView.h"
	#include "pdg/app/ZoomingView.h"
    #include "pdg/app/Checkbox.h"
    #include "pdg/app/Button.h"
    #include "pdg/app/Scrollbar.h"
  #endif // PDG_NO_GUI

#endif // PDG_NO_APP_FRAMEWORK

#endif // PDG_FRAMEWORK_H_INCLUDED

