// -----------------------------------------------
// animated.h
// 
// animation functionality
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


#ifndef PDG_ANIMATED_H_INCLUDED
#define PDG_ANIMATED_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/ianimationhelper.h"
#include "pdg/sys/easing.h"
#include "pdg/sys/serializable.h"

#include <vector>

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

// -----------------------------------------------------------------------------------
// Animated
// An object that can be automatically moved, spun or resized over time 
// Not intended to be used directly, but rather as a base class for items that can
// be animated.
// -----------------------------------------------------------------------------------

class Animated : public ISerializable {
public:

	SERIALIZABLE_TAG( CLASSTAG_ANIMATED );
	SERIALIZABLE_METHODS();
	static pdg::ISerializable* CreateInstance() { return new Animated(); }

	enum {
		// durations
		duration_Constant = -1,
		duration_Instantaneous = 0,
	};
	
	// bounds
	Rect			getBoundingBox();
	RotatedRect		getRotatedBounds();

	// current location in container's coordinate system
    Animated&		setLocation(const Point& loc);
    Point			getLocation();
    Animated&		moveTo(float x, float y);
    Animated&		moveTo(const Point& where);
    Animated&		move(float deltaX, float deltaY);
    Animated&		move(const Offset& delta);

    // animate moving to new location, after wait() if applicable
	virtual void	moveTo(const Point& loc, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad);
	void			moveTo(float x, float y, int32 msDuration, 
                         EasingFunc easing = easeInOutQuad);
	void			move(float deltaX, float deltaY, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad);
	void			move(const Offset& delta, int32 msDuration, 
                         EasingFunc easing = easeInOutQuad);

	// constant movement in pixels per second, direction in 2 PI radians per circle, 
	// with 0 being toward right, 0.5 PI down, PI to left, and 1.5 PI up
    virtual Animated&	setVelocity(const Vector& deltaPerSec);
	virtual Vector  getVelocity();
	Animated&		setSpeed(float speed);
	float			getSpeed();
	Animated&		setVelocityInRadians(float speed, float direction);
	float			getMovementDirectionInRadians();
	Animated&		setVelocity(float deltaXPerSec, float deltaYPerSec);
	Animated&		stopMoving();

	// animate changing speed over time, after wait() if applicable
	virtual void	accelerateTo(float speed, int32 msDuration, 
                            EasingFunc easing = linearTween);
	void			accelerate(float deltaSpeed, int32 msDuration, 
                            EasingFunc easing = linearTween);

	// change in size
	Animated&		setSize(float width, float height);
	Animated&		setWidth(float width);
	float			getWidth();
	Animated&		setHeight(float height);
	float			getHeight();
	Animated&		grow(float factor);
	Animated&		stretch(float widthFactor, float heightFactor);

	// constant change in size 
    Animated&		startGrowing(float amountPerSecond);
	Animated&		stopGrowing();
	Animated&		startStretching(float widthPerSecond, float heightPerSecond);
	Animated&		stopStretching();

	// animate change in size over time, to specific size, after wait() if applicable
	virtual void	resizeTo(float width, float height, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad);

	// animate change in size over time, relative to current size, after wait() if applicable
	void			resize(float deltaWidth, float deltaHeight, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad);
	void			grow(float factor, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad); // from current size
	void			stretch(float widthFactor, float weightFactor, int32 msDuration, 
                            EasingFunc easing = easeInOutQuad); // from current size

	// rotation clockwise is postive, counter-clockwise negative, around centerpoint, in radians
	Animated&		setRotation(float radiansRotation);
	float			getRotation();
	Animated&		setCenterOffset(const Offset& offset);
	Offset          getCenterOffset();
	Animated&		rotateTo(float radiansRotation);
	Animated&		rotate(float radians);
	
	// constant change in direction
	Animated&		setSpin(float radiansPerSecond);
	float			getSpin();
	Animated&		stopSpinning();

	// animate changing direction over time, after wait() if applicable
	virtual void	rotateTo(float radiansRotation, int32 msDuration, 
                                EasingFunc easing = easeInOutQuad);
	void			rotate(float radians, int32 msDuration, 
                                EasingFunc easing = easeInOutQuad);
	void			changeCenterTo(float xOffset, float yOffset, int32 msDuration, 
                                EasingFunc easing = easeInOutQuad);
	void			changeCenterTo(const Offset& offset, int32 msDuration, 
                                   EasingFunc easing = easeInOutQuad);
	void			changeCenter(float deltaXOffset, float deltaYOffset, int32 msDuration, 
                                EasingFunc easing = easeInOutQuad);
	void			changeCenter(const Offset& offset, int32 msDuration, 
                                 EasingFunc easing = easeInOutQuad);

    // delay before starting next change over time operation (ie, one with easing)
    // no affect on constant motion operations
    Animated*       wait(int32 msDuration);

	// Simple Physics-based animation
	virtual Animated&	setMass(float mass);  // 1.0 is default, unit-less
	float			getMass();
	virtual Animated&	setFriction(float frictionCoefficient);  // 0.0 is default, 1.0 = perfect friction, 0.0 = no friction
	Animated&		setMoveFriction(float frictionCoefficient);
	Animated&		setSpinFriction(float frictionCoefficient);
	Animated&		setSizeFriction(float frictionCoefficient);
	float   		getMoveFriction();
	float   		getSpinFriction();
	float   		getSizeFriction();

	// multiple forces can be applied simultaneously, after wait() if applicable
	virtual void	applyForce(const Vector& force, int32 msDuration = duration_Instantaneous);
	virtual void	applyForce(float forceX, float forceY, int32 msDuration = duration_Instantaneous);
	virtual void	applyTorque(float forceSpin, int32 msDuration = duration_Instantaneous);
	virtual void	stopAllForces();  // removes all forces that were set by applyForce (but not friction)

	// objects that will be called to help with animation of this object
	// the animation helper(s) will be called in order they were added
	// after all other animation (from constant motion, change over time with easing,
	// and application of forces) has been calculated
	virtual void	addAnimationHelper(IAnimationHelper* helper);
	virtual void	removeAnimationHelper(IAnimationHelper* helper);
	virtual void	clearAnimationHelpers();

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mAnimatedScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
	Animated();
	virtual ~Animated();	

	// called by subclasses to do the actual work of animating
	// returns true if anything changed
	virtual bool	animate(int32 msElapsed);

	// ---------------------------------------
	// subclasses should override these when something needs to
	// be done to maintain the state of the object based on a
	// direct change to one of the basic attributes
	// mAnimating will be true when following called from animate()
	// or false if called from setX() calls

	bool			mAnimating;

	virtual void	locationChanged(const Offset& delta);
	virtual void	sizeChanged(float deltaW, float deltaH);
	virtual void	rotationChanged(float deltaRadians);
	virtual void	centerChanged(const Offset& delta);

	// ---------------------------------------
	// basic attributes

	Point           mLocation;
	float			mHeight;
	float			mWidth;
	float			mFacing;
	Offset          mCenterOffset;

	// ---------------------------------------
	// constant motion, subject to forces and friction

	float			mDeltaXPerMs;
	float			mDeltaYPerMs;
	float			mDeltaWidthPerMs;
	float			mDeltaHeightPerMs;
	float			mDeltaFacingPerMs;

	// ---------------------------------------
	// change over duration via easing
	// not subject to friction

	struct Animation {
		EasingFunc	easing;
        int32       delayMs;
        float       targetVal;  // only used with delay
		float*		value;
		uint32		currMs;
		uint32		durationMs;
		float		beginVal;
		float		deltaVal;
        Animation(float* valPtr, float val, EasingFunc func, int32 delay, uint32 duration) 
            : easing(func),
            delayMs(delay),
            targetVal(val),
            value(valPtr),
            currMs(0),
            durationMs(duration),
            beginVal(*valPtr),
            deltaVal(val - *valPtr)
            {};
        Animation()
            : easing(0),
            delayMs(0),
            targetVal(0),
            value(0),
            currMs(0),
            durationMs(0),
            beginVal(0),
            deltaVal(0)
            {};
	};

    int32           mDelayMs;

	std::vector<Animation> mAnimations;

    // subclasses override to do post-easing operations
    virtual void    easingCompleted(const Animation& a);

	// ---------------------------------------
	// physics model

	float			mMass;
	float			mMoveFriction;
	float			mSpinFriction;
	float			mSizeFriction;

	typedef struct {
        int32 delayRemaining;
		float xAccelerationPerMs2;
		float yAccelerationPerMs2;
		float radianAccelerationPerMs2;
		int32 milliRemaining;
	} Force;

	std::vector<Force> mForces;

	// ---------------------------------------
	// helpers

	std::vector<IAnimationHelper*> mHelpers;
/// @endcond
};

// bounds
inline Rect
Animated::getBoundingBox() {
	return getRotatedBounds().getBounds();
}

inline RotatedRect
Animated::getRotatedBounds() {
	Rect r(mWidth, mHeight);
	r.center(mLocation);
	RotatedRect rr(r, mFacing, mCenterOffset);
	return rr;
}


// current location in container's coordinate system
inline Animated&		
Animated::setLocation(const Point& loc) {
	Point delta = loc - mLocation;
	mLocation.x = loc.x;
	mLocation.y = loc.y;
	locationChanged(Offset(delta.x, delta.y));
	return *this;
}

inline Point
Animated::getLocation() {
	return mLocation;
}

inline Animated&		
Animated::moveTo(float x, float y) {
    moveTo(Point(x,y));
    return *this;
}

inline Animated&		
Animated::moveTo(const Point& where) {
    setLocation(where);
    return *this;
}

inline Animated&		
Animated::move(float deltaX, float deltaY) {
    move(Offset(deltaX, deltaY));
    return *this;
}

inline Animated&		
Animated::move(const Offset& delta) {
    setLocation(getLocation()+delta);
    return *this;
}


inline void
Animated::moveTo(float x, float y, int32 msDuration, EasingFunc easing) {
    moveTo(Point(x, y), msDuration, easing);
}

inline void
Animated::move(float deltaX, float deltaY, int32 msDuration, EasingFunc easing) {
	move(Offset(deltaX, deltaY), msDuration, easing);
}

inline void
Animated::move(const Offset& delta, int32 msDuration, EasingFunc easing) {
    moveTo(mLocation + delta, msDuration, easing);
}

// constant movement in pixels per second, direction in 2 PI radians per circle, 
// with 0 being toward right, 0.5 PI down, PI to left, and 1.5 PI up
inline Animated&
Animated::setSpeed(float speed) { 
	setVelocityInRadians( speed, getMovementDirectionInRadians() );
	return *this;
}


inline float
Animated::getSpeed() {
	return getVelocity().vectorLength();
}


inline Animated&		
Animated::setVelocityInRadians(float speed, float direction) { 
	setVelocity( speed * cos(direction), speed * sin(direction) );
	return *this;
}

inline float
Animated::getMovementDirectionInRadians() { 
	return getVelocity().vectorAngle();
}

inline Animated&		
Animated::setVelocity(float deltaX, float deltaY) {
	setVelocity(Vector(deltaX, deltaY));
	return *this;
}

inline Animated&		
Animated::stopMoving() {
    setVelocity(Vector(0.0f, 0.0f));
    return *this;
}

inline void
Animated::accelerate(float deltaSpeed, int32 msDuration, EasingFunc easing) {
	accelerateTo(getSpeed() + deltaSpeed, msDuration, easing);
}

// change in size
inline Animated&		
Animated::setSize(float width, float height) {
	setWidth(width);
	setHeight(height);
	return *this;
}


inline Animated&		
Animated::setHeight(float height) {
	float deltaH = height - mHeight;
	mHeight = height;
	mDeltaHeightPerMs = 0.0f;
	sizeChanged(0.0f, deltaH);
	return *this;
}


inline Animated&		
Animated::setWidth(float width) {
	float deltaW = width - mWidth;
	mWidth = width;
	mDeltaWidthPerMs = 0.0f;
	sizeChanged(deltaW, 0.0f);
	return *this;
}

inline float
Animated::getHeight() {
	return mHeight;
}

inline float
Animated::getWidth() {
	return mWidth;
}

inline Animated&		
Animated::grow(float factor) {
	stretch(factor, factor);
	return *this;
}


inline Animated&		
Animated::stretch(float widthFactor, float heightFactor) {
	PointT<float> saved(mWidth, mHeight);
	mWidth *= widthFactor;
	mHeight *= heightFactor;
	sizeChanged(mWidth - saved.x, mHeight - saved.y);
	stopStretching(); // stop the animation if there is one
	return *this;	
}


// constant change in size 
inline Animated&		
Animated::startGrowing(float amountPerSecond) {
	startStretching(amountPerSecond, amountPerSecond);
	return *this;
}


inline Animated&		
Animated::stopGrowing() {
	stopStretching();
	return *this;
}


inline Animated&		
Animated::startStretching(float widthPerSecond, float heightPerSecond) {
	mDeltaWidthPerMs = widthPerSecond * 1000.0f;
	mDeltaHeightPerMs = heightPerSecond * 1000.0f;
	return *this;
}


inline Animated&		
Animated::stopStretching() {
	mDeltaWidthPerMs = 0.0f;
	mDeltaHeightPerMs = 0.0f;
	return *this;
}


// animate change in size over time, relative to current size
inline void
Animated::resize(float deltaWidth, float deltaHeight, int32 msDuration, EasingFunc easing) {
	resizeTo(mWidth + deltaWidth, mHeight + deltaHeight, msDuration, easing);
}


inline void
Animated::grow(float factor, int32 msDuration, EasingFunc easing) {
	stretch(factor, factor, msDuration, easing);
}

 // from current size
inline void
Animated::stretch(float widthFactor, float heightFactor, int32 msDuration, EasingFunc easing) {
	resizeTo(mWidth * widthFactor, mHeight * heightFactor, msDuration, easing);
}

// rotation clockwise is postive, counter-clockwise negative, around centerpoint, in radians
inline Animated&		
Animated::rotate(float radians) {
	setRotation(mFacing + radians);
	return *this;
}

inline Animated&			
Animated::rotateTo(float radiansRotation) {
	setRotation(radiansRotation);
	return *this;
}

inline float
Animated::getRotation() {
	return mFacing;
}


inline Animated&		
Animated::setCenterOffset(const Offset& offset) {
    Offset saved = mCenterOffset;
    mCenterOffset = offset;
    centerChanged(mCenterOffset - saved);
    return *this;
}
    

inline Offset
Animated::getCenterOffset() {
	return mCenterOffset;
}

inline Animated&		
Animated::stopSpinning() {
	mDeltaFacingPerMs = 0.0f;
	return *this;
}

inline void
Animated::rotate(float radians, int32 msDuration, EasingFunc easing) {
	rotateTo(mFacing + radians, msDuration, easing);
}

 // relative to real center
inline void
Animated::changeCenterTo(float xOffset, float yOffset, int32 msDuration, EasingFunc easing) {
	changeCenterTo(Offset(xOffset, yOffset), msDuration, easing);
}

inline void
Animated::changeCenter(float deltaXOffset, float deltaYOffset, int32 msDuration, EasingFunc easing) {
    changeCenter(Offset(deltaXOffset, deltaYOffset), msDuration, easing);
}

inline void
Animated::changeCenter(const Offset& offset, int32 msDuration, EasingFunc easing) {
    changeCenterTo(mCenterOffset + offset, msDuration, easing);
}

inline Animated* 
Animated::wait(int32 msDuration) {
    mDelayMs = (msDuration > 0) ? msDuration : 0;
    return this;
}

// Simple Physics-based animation
inline Animated&
Animated::setMass(float mass) {
	mMass = mass;
	return *this;
}

  // 1.0 is default, unit-less
inline float
Animated::getMass() {
	return mMass;
}


inline void	
Animated::applyForce(float forceX, float forceY, int32 msDuration) {
    applyForce(Vector(forceX, forceY), msDuration);
}

inline Animated&
Animated::setFriction(float frictionCoefficient) {
	setMoveFriction(frictionCoefficient);
	setSpinFriction(frictionCoefficient);
	setSizeFriction(frictionCoefficient);
	return *this;
}

inline Animated&
Animated::setMoveFriction(float frictionCoefficient) {
	mMoveFriction = frictionCoefficient;
	return *this;
}

inline float
Animated::getMoveFriction() {
	return mMoveFriction;
}

inline Animated&
Animated::setSpinFriction(float frictionCoefficient) {
	mSpinFriction = frictionCoefficient;
	return *this;
}

inline float
Animated::getSpinFriction() {
	return mSpinFriction;
}

inline Animated&
Animated::setSizeFriction(float frictionCoefficient) {
	mSizeFriction = frictionCoefficient;
	return *this;
}

inline float
Animated::getSizeFriction() {
	return mSizeFriction;
}


} // end namespace pdg

#endif // PDG_SPRITE_H_INCLUDED

