// -----------------------------------------------
// easing.h
// 
// easing functionality for animation
//
// Written by Ed Zavada, 2012
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


#ifndef PDG_EASING_H_INCLUDED
#define PDG_EASING_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"

namespace pdg {

// -----------------------------------------------------------------------------------
// Easing Functions
//
// timeMs: current time offset since start of animation, 
// beginVal: beginning value at start of animation, 
// change: complete change in value over entire animation,
// durationMs: duration for entire animation
// returns new value as of current time offset

typedef float (*EasingFunc)(uint32 timeOffsetMs, float beginVal, float change, uint32 durationMs);

// Predefined Easing Functions:

// --- LINEAR EASING: basic linear motion --------------------------------------------

float linearTween(uint32 ut, float b, float c, uint32 ud);
// easeInX - accelerating from zero velocity
// easeOutX - decelerating to zero velocity
// easeInOutX - acceleration until halfway, then deceleration

// --- QUADRATIC EASING: t^2 ---------------------------------------------------------
float easeInQuad(uint32 ut, float b, float c, uint32 ud);
float easeOutQuad(uint32 ut, float b, float c, uint32 ud);
float easeInOutQuad(uint32 ut, float b, float c, uint32 ud);

// --- CUBIC EASING: t^3 -------------------------------------------------------------
float easeInCubic(uint32 ut, float b, float c, uint32 ud);
float easeOutCubic(uint32 ut, float b, float c, uint32 ud);
float easeInOutCubic(uint32 ut, float b, float c, uint32 ud);

// --- QUARTIC EASING: t^4 -----------------------------------------------------------
float easeInQuart(uint32 ut, float b, float c, uint32 ud);
float easeOutQuart(uint32 ut, float b, float c, uint32 ud);
float easeInOutQuart(uint32 ut, float b, float c, uint32 ud);

// --- QUINTIC EASING: t^5 -----------------------------------------------------------
float easeInQuint(uint32 ut, float b, float c, uint32 ud);
float easeOutQuint(uint32 ut, float b, float c, uint32 ud);
float easeInOutQuint(uint32 ut, float b, float c, uint32 ud);

// --- SINUSOIDAL EASING: sin(t) -----------------------------------------------------
float easeInSine(uint32 ut, float b, float c, uint32 ud);
float easeOutSine(uint32 ut, float b, float c, uint32 ud);
float easeInOutSine(uint32 ut, float b, float c, uint32 ud);

// --- EXPONENTIAL EASING: 2^t -------------------------------------------------------
float easeInExpo(uint32 ut, float b, float c, uint32 ud);
float easeOutExpo(uint32 ut, float b, float c, uint32 ud);
float easeInOutExpo(uint32 ut, float b, float c, uint32 ud);

// --- CIRCULAR EASING: sqrt(1-t^2) --------------------------------------------------
float easeInCirc(uint32 ut, float b, float c, uint32 ud);
float easeOutCirc(uint32 ut, float b, float c, uint32 ud);
float easeInOutCirc(uint32 ut, float b, float c, uint32 ud);

// --- BOUNCE EASING: exponentially decaying parabolic bounce ------------------------
float easeInBounce(uint32 ut, float b, float c, uint32 ud);
float easeOutBounce(uint32 ut, float b, float c, uint32 ud);
float easeInOutBounce(uint32 ut, float b, float c, uint32 ud);

// --- BACK EASING: overshooting cubic easing: (s+1)*t^3 - s*t^2 ---------------------
//     backtracking slightly, then reversing direction and moving to target
float easeInBack(uint32 ut, float b, float c, uint32 ud);
//     moving towards target, overshooting it slightly, then reversing and coming back to target
float easeOutBack(uint32 ut, float b, float c, uint32 ud);
//     backtracking slightly, then reversing direction and moving to target,
//     then overshooting target, reversing, and finally coming back to target
float easeInOutBack(uint32 ut, float b, float c, uint32 ud);


#define NUM_BUILTIN_EASINGS		28
#define MAX_CUSTOM_EASINGS		10
#define NUM_EASING_FUNCTIONS 	NUM_BUILTIN_EASINGS + MAX_CUSTOM_EASINGS

#define BUILTIN_EASING_FUNC_LIST linearTween,    \
	easeInQuad, easeOutQuad, easeInOutQuad,		 \
	easeInCubic, easeOutCubic, easeInOutCubic,	 \
	easeInQuart, easeOutQuart, easeInOutQuart,	 \
	easeInQuint, easeOutQuint, easeInOutQuint,	 \
	easeInSine, easeOutSine, easeInOutSine,		 \
	easeInExpo, easeOutExpo, easeInOutExpo,		 \
	easeInCirc, easeOutCirc, easeInOutCirc,		 \
	easeInBounce, easeOutBounce, easeInOutBounce,\
	easeInBack, easeOutBack, easeInOutBack

#define EASING_FUNC_LIST   \
	BUILTIN_EASING_FUNC_LIST,       \
	customEasing0, customEasing1, customEasing2,  \
	customEasing3, customEasing4, customEasing5,  \
	customEasing6, customEasing7, customEasing8,  \
	customEasing9

namespace EasingFuncIds {  // no confusion in the global namespace
	enum {
		EASING_FUNC_LIST
	};
}

extern EasingFunc gEasingFunctions[NUM_EASING_FUNCTIONS];
extern int gNumCustomEasings;

uint8 easingFuncToId(EasingFunc func);
EasingFunc easingIdToFunc(uint8 id);

} // end namespace pdg

#endif // PDG_EASING_H_INCLUDED

