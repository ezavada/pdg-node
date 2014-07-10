// -----------------------------------------------
// timerids.h
// 
// Definitions for timer event ID's within PDG framework
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


#ifndef PDG_TIMERIDS_H_INCLUDED
#define PDG_TIMERIDS_H_INCLUDED

#define ILLEGAL_TIMER_ID_DONT_USE   0   // we can't use this id

// -----------------------------------------------------------------------------------
// predefined timers for things internal to the framework will track
// -----------------------------------------------------------------------------------

#define PDG_APP_STARTUP_TIMER       -1
#define PDG_FULL_SCREEN_ACTIVATE_REDRAW_TIMER -2    // used by Win32 framework implementation

#define PDG_START_CURSOR_BLINK	    -10  // used for Edit Text

#define PDG_SCROLLBAR_CLICK_PAUSE   -20  // waits after scroll bar button click until repeating behavior starts
#define PDG_SCROLLBAR_REPEATER      -21  // Timer to repeatedly scroll up or down when button is held down
#define PDG_SCROLLBAR_TRACKER       -22  // Timer to repeatedly query mouse position when button is held down on slider

#define PDG_DICE_DURATION_TIMER     -30  // used to time how long dice should roll
#define PDG_DICE_ANIM_FRAME_TIMER   -31  // used to generate animation frame intervals
#define PDG_DICE_ANIM_RESULT_TIMER  -32  // used to show dice after the roll has completed

#define PDG_FLICK_DECAY_TIMER       -40  // used by TouchController to continue a flick gesture after mouse up
#define PDG_DRAG_SNAPBACK_TIMER     -41  // used by TouchController to snap back to bounds after item dragged past end

#endif // PDG_TIMERIDS_H_INCLUDED



