// -----------------------------------------------
// animated.cpp
// 
// animation functionality
//
// Written by Ed Zavada, 2010-2012
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

#include "pdg/sys/animated.h"

#include <cmath>  // for sin() and cos()

#ifndef PI
#define PI       3.141592f        /* the venerable pi */
#endif
#ifndef M_PI
#define M_PI PI
#endif
#ifndef TWO_PI
#define TWO_PI    6.283184f      /* handy for dealing with circles */
#endif

#ifndef PI_DIV2
#define PI_DIV2   1.570796f
#endif

namespace pdg {


uint32 Animated::getSerializedSize(pdg::ISerializer* serializer) const { 
#ifndef COMPILER_MSVC
	#warning implement Animated::getSerializedSize()
#endif
	return 0; 
}


void Animated::serialize(pdg::ISerializer* serializer) const {
#ifndef COMPILER_MSVC
	#warning implement Animated::serialize()
#endif
}


void Animated::deserialize(pdg::IDeserializer* deserializer) {
#ifndef COMPILER_MSVC
	#warning implement Animated::deserialize()
#endif
}



// animate moving to new location
void
Animated::moveTo(const Point& loc, int32 msDuration, EasingFunc easing) {
    if (loc.x != mLocation.x) {
        Animation a(&mLocation.x, loc.x, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    if (loc.y != mLocation.y) {
        Animation a(&mLocation.y, loc.y, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    mDelayMs = 0;
}



Animated&
Animated::setVelocity(const Vector& delta) {
    mDeltaXPerMs = delta.x / 1000.0f;
    mDeltaYPerMs = delta.y / 1000.0f;
    return *this;
}

Vector
Animated::getVelocity() {
    float deltaX = mDeltaXPerMs * 1000.0f;
    float deltaY = mDeltaYPerMs * 1000.0f;
    return Vector(deltaX, deltaY);
}

// animate changing speed over time
void
Animated::accelerateTo(float speed, int32 msDuration, EasingFunc easing) {
	float targetDeltaXPerMs = speed * cos(mFacing) / 1000.0f;
	float targetDeltaYPerMs = speed * sin(mFacing) / 1000.0f;
	Animation a(&mDeltaXPerMs, targetDeltaXPerMs, easing, mDelayMs, msDuration);
	mAnimations.push_back(a);
	Animation b(&mDeltaYPerMs, targetDeltaYPerMs, easing, mDelayMs, msDuration);
	mAnimations.push_back(b);
    mDelayMs = 0;
}


// animate change in size over time, to specific size relative to original size
void
Animated::resizeTo(float width, float height, int32 msDuration, EasingFunc easing) {
    if (width != mWidth) {
        Animation a(&mWidth, width, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    if (height != mHeight) {
        Animation a(&mHeight, height, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    mDelayMs = 0;
}


Animated&
Animated::setRotation(float radiansRotation) {
	float oldFacing = mFacing;
	mFacing = radiansRotation;
	rotationChanged(radiansRotation - oldFacing);
	return *this;
}


// constant change in direction
Animated&
Animated::setSpin(float radiansPerSecond) {
	float radPerMs = radiansPerSecond / 1000.0f;  
	mDeltaFacingPerMs = radPerMs;
	return *this;
}


float
Animated::getSpin() {
	float radPerSec = mDeltaFacingPerMs * 1000.0f;  
	return radPerSec;
}


// animate changing direction over time
void
Animated::rotateTo(float radians, int32 msDuration, EasingFunc easing) {
	Animation a(&mFacing, radians, easing, mDelayMs, msDuration);
	mAnimations.push_back(a);
    mDelayMs = 0;
}


void
Animated::changeCenterTo(const Offset& offset, int32 msDuration, EasingFunc easing) {
    if (offset.x != mCenterOffset.x) {
        Animation a(&mCenterOffset.x, offset.x, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    if (offset.y != mCenterOffset.y) {
        Animation a(&mCenterOffset.y, offset.y, easing, mDelayMs, msDuration);
        mAnimations.push_back(a);
    }
    mDelayMs = 0;
}


// multiple forces can be applied simultaneously
void
Animated::applyForce(const Vector& force, int32 msDuration) {
	if (msDuration == duration_Instantaneous && mDelayMs == 0) {			// apply the full force once
		mDeltaXPerMs += (force.x*0.001f) / mMass;
		mDeltaYPerMs += (force.y*0.001f) / mMass;
	}
	else {
		Force newForce;
        newForce.delayRemaining = mDelayMs;
		newForce.xAccelerationPerMs2 = (force.x*0.000001f)/mMass; // acceleration using millisecond SQUARED
		newForce.yAccelerationPerMs2 = (force.y*0.000001f)/mMass;
		newForce.radianAccelerationPerMs2 = 0.0f;
		newForce.milliRemaining = msDuration;
		mForces.push_back(newForce);
	}
    mDelayMs = 0;
}


void
Animated::applyTorque(float forceSpin, int32 msDuration) {
	if (msDuration == duration_Instantaneous && mDelayMs == 0) {			// apply the full force once
		mDeltaFacingPerMs += (forceSpin*0.001f) / mMass;
	}
	else {
		Force newForce;
        newForce.delayRemaining = mDelayMs;
		newForce.xAccelerationPerMs2 = 0.0f;
		newForce.yAccelerationPerMs2 = 0.0f;
		newForce.radianAccelerationPerMs2 = (forceSpin*0.000001f)/mMass; // acceleration using millisecond SQUARED
		newForce.milliRemaining = msDuration;
		mForces.push_back(newForce);
	}
    mDelayMs = 0;
}


// removes all forces that were set by applyForce (but not friction)
void
Animated::stopAllForces() {
	mForces.clear();
    mDelayMs = 0;
}


// objects that will be called to help with animation of this object
// the animation helper(s) will be called in order they were added
// after all other animation (from constant motion, change over time with easing,
// and application of forces) has been calculated
void
Animated::addAnimationHelper(IAnimationHelper* helper) {
	mHelpers.push_back(helper);
}


void
Animated::removeAnimationHelper(IAnimationHelper* helper) {
	for (unsigned int i=0; i<mHelpers.size(); i++) {
		if (mHelpers.at(i) == helper) {
			mHelpers.erase(mHelpers.begin()+i);
			break;
		}
	}
}


void
Animated::clearAnimationHelpers() {
	// delete animation helpers if we own them
	for (unsigned int i = 0; i < mHelpers.size(); i++) {
		IAnimationHelper* helper = mHelpers.at(i);
        if (helper->ownedByAnimated()) {
            delete helper;
        }
	}
	mHelpers.clear();
}

// called by subclasses to do the actual work of animating
// returns true if anything changed
bool
Animated::animate(int32 msElapsed) {

	// inside the animate call
	mAnimating = true;	

	// apply timed and constant forces
	for (unsigned int i = 0; i < mForces.size(); i++) {
		Force& force = mForces.at(i);
        if (force.delayRemaining > 0) {
            force.delayRemaining -= msElapsed;
        }
        if (force.delayRemaining <= 0) {
            mDeltaXPerMs += force.xAccelerationPerMs2*msElapsed;
            mDeltaYPerMs += force.yAccelerationPerMs2*msElapsed;
            mDeltaFacingPerMs += force.radianAccelerationPerMs2*msElapsed;
            // reduce duration of timed forces
            if (force.milliRemaining > 0) {
                force.milliRemaining -= msElapsed;
                // erase those that have expired 
                if (force.milliRemaining <= 0) {
                    mForces.erase(mForces.begin() + i);
                    i--;  // go back one so we won't skip the next force
                }
            }
        }
	}

	// apply movement friction
	if ( mMoveFriction > 0) {
		float frictionX = 0.5f*mMoveFriction*mDeltaXPerMs*mDeltaXPerMs;
		float frictionY = 0.5f*mMoveFriction*mDeltaYPerMs*mDeltaYPerMs;
		if (mDeltaXPerMs < 0 && mDeltaXPerMs < frictionX) {
			mDeltaXPerMs += frictionX;
		} else if (mDeltaXPerMs > 0 && mDeltaXPerMs > frictionX) {
			mDeltaXPerMs -= frictionX;
		} else {
			mDeltaXPerMs = 0;
		}
		if (mDeltaYPerMs < 0 && mDeltaYPerMs < frictionY) {
			mDeltaYPerMs += frictionY;
		} else if (mDeltaYPerMs > 0 && mDeltaYPerMs > frictionY) {
			mDeltaYPerMs -= frictionY;
		} else {
			mDeltaYPerMs = 0;
		}	
	}

	// apply spin friction
	if ( mSpinFriction > 0) {
		float frictionSpin = 0.5f*mSpinFriction*mDeltaFacingPerMs*mDeltaFacingPerMs;
		if (mDeltaFacingPerMs < 0 && mDeltaFacingPerMs < frictionSpin) {
			mDeltaFacingPerMs += frictionSpin;
		} else if (mDeltaFacingPerMs > 0 && mDeltaFacingPerMs > frictionSpin) {
			mDeltaFacingPerMs -= frictionSpin;
		} else {
			mDeltaFacingPerMs = 0;
		}
	}

	// apply size friction
	if ( mSizeFriction > 0) {
		float frictionW = 0.5f*mSizeFriction*mDeltaWidthPerMs*mDeltaWidthPerMs;
		float frictionH = 0.5f*mSizeFriction*mDeltaHeightPerMs*mDeltaHeightPerMs;
		if (mDeltaWidthPerMs < 0 && mDeltaWidthPerMs < frictionW) {
			mDeltaWidthPerMs += frictionW;
		} else if (mDeltaWidthPerMs > 0 && mDeltaWidthPerMs > frictionW) {
			mDeltaWidthPerMs -= frictionW;
		} else {
			mDeltaWidthPerMs = 0;
		}
		if (mDeltaHeightPerMs < 0 && mDeltaHeightPerMs < frictionH) {
			mDeltaHeightPerMs += frictionH;
		} else if (mDeltaHeightPerMs > 0 && mDeltaHeightPerMs > frictionH) {
			mDeltaHeightPerMs -= frictionH;
		} else {
			mDeltaHeightPerMs = 0;
		}	
	}

	// save current info
	PointT<float> savedLocation = mLocation;
	float savedFacing = mFacing;
	float savedHeight = mHeight;
	float savedWidth = mWidth;
	PointT<float> savedCenterOffset = mCenterOffset;

	// change position by the velocity
	mLocation.x += (mDeltaXPerMs * (float)msElapsed);
	mLocation.y += (mDeltaYPerMs * (float)msElapsed);

	// apply spin
	mFacing += (mDeltaFacingPerMs * (float)msElapsed);

	// adjust size
	mHeight += (mDeltaHeightPerMs * (float)msElapsed);	
	mWidth += (mDeltaWidthPerMs * (float)msElapsed);	

	// invoke easing animations
	for (unsigned int i = 0; i < mAnimations.size(); i++) {
		Animation& animation = mAnimations.at(i);
        if (animation.delayMs > 0) {
            animation.delayMs -= msElapsed;
            animation.beginVal = *animation.value;
            animation.deltaVal = animation.targetVal - animation.beginVal;
            animation.currMs = 0;
        }
        if (animation.delayMs <= 0) {
            bool erase = false;
            int32 elapsedMs = (msElapsed - animation.delayMs);
            animation.delayMs = 0;
            if (elapsedMs < 0) {
                elapsedMs = 0;
            }
            animation.currMs += elapsedMs;
            if (animation.currMs >= animation.durationMs) {
                animation.currMs = animation.durationMs;
                erase = true; // we are done with this animation
            }
            *animation.value = animation.easing(animation.currMs, animation.beginVal, 
                animation.deltaVal, animation.durationMs);
            if (erase) {
                easingCompleted(animation);
                // this helper doesn't want to
                mAnimations.erase(mAnimations.begin() + i);
                i--;  // go back one so we won't skip the next animation
            }
        }
	}

	// invoke animation helpers in order added
	for (unsigned int i = 0; i < mHelpers.size(); i++) {
		IAnimationHelper* helper = mHelpers.at(i);
		if (!helper->animate(this, msElapsed)) {
			// this helper doesn't want to do any more
			mHelpers.erase(mHelpers.begin() + i);
            if (helper->ownedByAnimated()) {
                delete helper;
            }
			i--;  // go back one so we won't skip the next helper
		}
	}

	bool changes = false;

	// notify subclasses of changes from inside the animate call
	if (savedLocation != mLocation) {
		locationChanged(mLocation - savedLocation);
		changes = true;
	}
	if (savedFacing != mFacing) {
		rotationChanged(mFacing - savedFacing);
		changes = true;
	}
	if (savedHeight != mHeight || savedWidth != mWidth) {
		sizeChanged(mWidth - savedWidth, mHeight - savedHeight);
		changes = true;
	}
	if (savedCenterOffset != mCenterOffset) {
		centerChanged(mCenterOffset - savedCenterOffset);
		changes = true;
	}

	mAnimating = false;
	return changes;
}

void
Animated::locationChanged(const Offset& delta) {
}
    
void
Animated::sizeChanged(float deltaW, float deltaH) {
}


void
Animated::rotationChanged(float deltaRadians) {
}


void
Animated::centerChanged(const Offset& delta) {
}


void    
Animated::easingCompleted(const Animation& a) {
}


Animated::Animated() {
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mAnimatedScriptObj);
#endif
	mLocation = Point(0,0);
	mCenterOffset = Point(0,0);
	mHeight = 0;
	mWidth = 0;
	mFacing = 0;
	mDeltaXPerMs = 0;
	mDeltaYPerMs = 0;
	mDeltaWidthPerMs = 0;
	mDeltaHeightPerMs = 0;
	mDeltaFacingPerMs = 0;
    mDelayMs = 0;
	mMass = 1;
	mMoveFriction = 0;
	mSpinFriction = 0;
	mSizeFriction = 0;
    mAnimating = false;
}


Animated::~Animated() {
	stopAllForces();
	clearAnimationHelpers();
	mAnimations.clear();
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupAnimatedScriptObject(mAnimatedScriptObj);
#endif
}

/*
C++ Easing Equations based on:

Easing Equations v1.5
May 1, 2003
(c) 2003 Robert Penner, all rights reserved. 
  
TERMS OF USE - EASING EQUATIONS

Open source under the BSD License. 

Copyright © 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// simple linear tweening - no easing
// t: current time, b: beginning value, c: change in value, d: duration
float linearTween(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return c*t/d + b;
};


 ///////////// QUADRATIC EASING: t^2 ///////////////////

// quadratic easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be in frames or seconds/milliseconds
float easeInQuad(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return c*t*t + b;
};

// quadratic easing out - decelerating to zero velocity
float easeOutQuad(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return -c *t*(t-2) + b;
};

// quadratic easing in/out - acceleration until halfway, then deceleration
float easeInOutQuad(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	if ((t/2.0f) < 1) return c/2.0f*t*t + b;
	t--;
	return -c/2.0f * (t*(t-2.0f) - 1.0f) + b;
};


 ///////////// CUBIC EASING: t^3 ///////////////////////

// cubic easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be frames or seconds/milliseconds
float easeInCubic(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return c*t*t*t + b;
};

// cubic easing out - decelerating to zero velocity
float easeOutCubic(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d - 1.0f);
	return c*(t*t*t + 1.0f) + b;
};

// cubic easing in/out - acceleration until halfway, then deceleration
float easeInOutCubic(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d/2.0f);
	if (t < 1.0f) return c/2.0f*t*t*t + b;
	t -= 2.0f;
	return c/2.0f* (t*t*t + 2.0f) + b;
};


 ///////////// QUARTIC EASING: t^4 /////////////////////

// quartic easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be frames or seconds/milliseconds
float easeInQuart(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return c*t*t*t*t + b;
};

// quartic easing out - decelerating to zero velocity
float easeOutQuart(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d-1.0f);
	return -c * (t*t*t*t - 1.0f) + b;
};

// quartic easing in/out - acceleration until halfway, then deceleration
float easeInOutQuart(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d/2.0f);
	if (t < 1.0f) return c/2.0f*t*t*t*t + b;
	t -= 2.0f;
	return -c/2.0f * (t*t*t*t - 2.0f) + b;
};


 ///////////// QUINTIC EASING: t^5  ////////////////////

// quintic easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in value, d: duration
// t and d can be frames or seconds/milliseconds
float easeInQuint(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return c*t*t*t*t*t + b;
};

// quintic easing out - decelerating to zero velocity
float easeOutQuint(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d-1.0f);
	return c*(t*t*t*t*t + 1.0f) + b;
};

// quintic easing in/out - acceleration until halfway, then deceleration
float easeInOutQuint(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d/2.0f);
	if (t < 1.0f) return c/2.0f*t*t*t*t*t + b;
	t -= 2.0f;
	return c/2.0f*(t*t*t*t*t + 2.0f) + b;
};



 ///////////// SINUSOIDAL EASING: sin(t) ///////////////

// sinusoidal easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in position, d: duration
float easeInSine(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return -c * cos(t/d * PI_DIV2) + c + b;
};

// sinusoidal easing out - decelerating to zero velocity
float easeOutSine(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return c * sin(t/d * PI_DIV2) + b;
};

// sinusoidal easing in/out - accelerating until halfway, then decelerating
float easeInOutSine(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return -c/2.0f * (cos(PI*t/d) - 1) + b;
};


 ///////////// EXPONENTIAL EASING: 2^t /////////////////

// exponential easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in position, d: duration
float easeInExpo(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return (ut==0) ? b : c * pow(2.0f, 10.0f * (t/d - 1.0f)) + b;
};

// exponential easing out - decelerating to zero velocity
float easeOutExpo(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return (ut==d) ? b+c : c * (-pow(2.0f, -10.0f * t/d) + 1.0f) + b;
};

// exponential easing in/out - accelerating until halfway, then decelerating
float easeInOutExpo(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	if (ut==0) return b;
	if (ut==ud) return b+c;
	t/=(d/2.0f);
	if (t < 1.0f) return c/2.0f * pow(2.0f, 10.0f * (t - 1.0f)) + b;
	return c/2.0f * (-pow(2.0f, -10.0f * --t) + 2.0f) + b;
};


 /////////// CIRCULAR EASING: sqrt(1-t^2) //////////////

// circular easing in - accelerating from zero velocity
// t: current time, b: beginning value, c: change in position, d: duration
float easeInCirc(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	return -c * (sqrt(1.0f - t*t) - 1.0f) + b;
};

// circular easing out - decelerating to zero velocity
float easeOutCirc(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d-1.0f);
	return c * sqrt(1.0f - t*t) + b;
};

// circular easing in/out - acceleration until halfway, then deceleration
float easeInOutCirc(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d/2.0f);
	if (t < 1.0f) return -c/2.0f * (sqrt(1.0f - t*t) - 1.0f) + b;
	t -= 2.0f;
	return c/2.0f * (sqrt(1.0f - t*t) + 1.0f) + b;
};


//  /////////// ELASTIC EASING: exponentially decaying sine wave  //////////////
// 
// // t: current time, b: beginning value, c: change in value, d: duration, a: amplitude (optional), p: period (optional)
// // t and d can be in frames or seconds/milliseconds
// 
// float easeInElastic = function (t, b, c, d, a) {
// 	if (t==0) return b;  if ((t/=d)==1) return b+c;  float p=d*.3;
// 	if (a < Math.abs(c)) { a=c; var s=p/4; }
// 	else var s = p/(TWO_PI) * Math.asin (c/a);
// 	return -(a*Math.pow(2,10*(t-=1)) * Math.sin( (t*d-s)*(TWO_PI)/p )) + b;
// };
// 
// float easeOutElastic = function (t, b, c, d, a) {
// 	if (t==0) return b;  if ((t/=d)==1) return b+c;  float p=d*.3;
// 	if (a < Math.abs(c)) { a=c; var s=p/4; }
// 	else var s = p/(TWO_PI) * Math.asin (c/a);
// 	return a*Math.pow(2,-10*t) * Math.sin( (t*d-s)*(TWO_PI)/p ) + c + b;
// };
// 
// float easeInOutElastic = function (t, b, c, d, a) {
// 	if (t==0) return b;  if ((t/=d/2)==2) return b+c;  float p=d*(.3*1.5);
// 	if (a < Math.abs(c)) { a=c; var s=p/4; }
// 	else var s = p/(TWO_PI) * Math.asin (c/a);
// 	if (t < 1) return -.5*(a*Math.pow(2,10*(t-=1)) * Math.sin( (t*d-s)*(TWO_PI)/p )) + b;
// 	return a*Math.pow(2,-10*(t-=1)) * Math.sin( (t*d-s)*(TWO_PI)/p )*.5 + c + b;
// };
// 

 /////////// BACK EASING: overshooting cubic easing: (s+1)*t^3 - s*t^2  //////////////

// back easing in - backtracking slightly, then reversing direction and moving to target
// t: current time, b: beginning value, c: change in value, d: duration, s: overshoot amount (optional)
// t and d can be in frames or seconds/milliseconds
// s controls the amount of overshoot: higher s means greater overshoot
// s has a default value of 1.70158, which produces an overshoot of 10 percent
// s==0 produces cubic easing with no overshoot
float easeInBack(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	float s = 1.70158f; 
	return c*t*t*((s+1.0f)*t - s) + b;
};

// back easing out - moving towards target, overshooting it slightly, then reversing and coming back to target
float easeOutBack(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d-1.0f);
	float s = 1.70158f;
	return c*(t*t*((s+1.0f)*t + s) + 1.0f) + b;
};

// back easing in/out - backtracking slightly, then reversing direction and moving to target,
// then overshooting target, reversing, and finally coming back to target
float easeInOutBack(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / (d/2.0f);
	float s = 1.70158f * 1.525f; 
	if (t < 1.0f) { 
//		s*=1.525f;
		return c/2.0f*(t*t*((s+1.0f)*t - s)) + b;
	}
	t -= 2.0f;
//	s*=1.525f;
	return c/2.0f*(t*t*((s+1.0f)*t + s) + 2.0f) + b;
};

 /////////// BOUNCE EASING: exponentially decaying parabolic bounce  //////////////

// bounce easing in
// t: current time, b: beginning value, c: change in position, d: duration
float easeInBounce(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	return c - easeOutBounce ((uint32)(d-t), 0.0f, c, d) + b;
};

// bounce easing out
float easeOutBounce(uint32 ut, float b, float c, uint32 ud) {
	float d = (float)ud;
	float t = (float)ut / d;
	if (t < (0.36363636f)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < 0.72727272f) {
		t-=0.54545454f;
		return c*(7.5625f*t*t + 0.75f) + b;
	} else if (t < 0.90909090f) {
		t-=0.81818181f;
		return c*(7.5625f*t*t + 0.9375f) + b;
	} else {
		t-=0.95454545f;
		return c*(7.5625f*t*t + 0.984375f) + b;
	}
};

// bounce easing in/out
float easeInOutBounce(uint32 ut, float b, float c, uint32 ud) {
	float t = (float)ut;
	float d = (float)ud;
	if (t < d/2) return easeInBounce (t*2.0f, 0.0f, c, d) * 0.5f + b;
	return easeOutBounce (t*2.0f-d, 0.0f, c, d) * 0.5f + c*0.5f + b;
};

EasingFunc gEasingFunctions[NUM_EASING_FUNCTIONS] = {
	BUILTIN_EASING_FUNC_LIST,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

int gNumCustomEasings = 0;

uint8 easingFuncToId(EasingFunc func) {
	for (int i = 0; i < (NUM_BUILTIN_EASINGS + gNumCustomEasings); i++) {
		if (gEasingFunctions[i] == func) {
			return i;
		}
	}
	if (gNumCustomEasings < MAX_CUSTOM_EASINGS) {
		gEasingFunctions[NUM_BUILTIN_EASINGS + gNumCustomEasings] = func;
		gNumCustomEasings++;
		return NUM_BUILTIN_EASINGS + gNumCustomEasings - 1;
	}
	return 0; // after too many we just use linear tween
}

EasingFunc easingIdToFunc(uint8 id) {
	if (id > NUM_EASING_FUNCTIONS) {
		return 0;
	} else {
		return gEasingFunctions[id];
	}
}


} // end namespace pdg
