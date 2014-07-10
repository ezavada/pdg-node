// -----------------------------------------------
// sprite.h
// 
// sprite functionality
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


#ifndef PDG_SPRITE_H_INCLUDED
#define PDG_SPRITE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/image.h"
#include "pdg/sys/core.h"
#include "pdg/sys/events.h"
#include "pdg/sys/eventemitter.h"
#include "pdg/sys/animated.h"
#include "pdg/sys/ispritecollidehelper.h"
#include "pdg/sys/serializable.h"

#ifndef PDG_NO_GUI
  #include "pdg/sys/ispritedrawhelper.h"
#endif

#include <cmath>  // for sin() and cos()
#include <limits> // for infinity()

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

#ifdef PDG_USE_CHIPMUNK_PHYSICS
#include "chipmunk.h"
#define CP_COLLIDE_TYPE_SPRITE 1111
#define CP_COLLIDE_TYPE_WALL   1212
#endif

#ifdef PDG_SCML_SUPPORT
#include "SCML_pdg.h"
#endif

#define MAX_FRAMES_PER_SPRITE 256
#define MAX_BREAKABLE_JOINTS_PER_SPRITE 16

namespace pdg {

class ImageImpl;  // internal implementation class

// -----------------------------------------------------------------------------------
// Sprite
// An animated, moving image that is automatically moved, animated, and blitted onto 
// the screen
// Things that happen to a sprite that might require intervention, such as collisions
// completed animations, going offscreen, etc... will generate events 
// -----------------------------------------------------------------------------------

class Sprite : public EventEmitter, public Animated {
    friend class SpriteLayer;
    friend class TileLayer;
    friend class SpriteManager;
public:

	SERIALIZABLE_TAG( CLASSTAG_SPRITE );
	SERIALIZABLE_METHODS();
	static pdg::ISerializable* CreateInstance() { return new Sprite; }

	enum {
		// animation flags, add starting direction + uni/bi-directional + looping/not
		// starting direction
		animate_StartToEnd =		0,
		animate_EndToStart =		1,

		// one direction or bidirectional
		animate_Unidirectional =	0,
		animate_Bidirectional =		2,
		
		// looping or not
		animate_NoLooping =			0,
		animate_Looping =			4,
		
		// frame markers
		start_FromFirstFrame = -1,
		start_FromLastFrame = -2,
		
		all_Frames = 0,
		
		// sprite actions for SprintInfo
		action_CollideSprite = 0,
		action_CollideWall = 1, // hit the boundary of the sprite layer
		action_Offscreen = 2, // is completely offscreen
		action_Onscreen = 3, // has moved from being offscreen to onscreen
		action_ExitLayer = 4, // has moved completely outside the boundries of the sprite layer
		action_AnimationLoop = 8,
		action_AnimationEnd = 9,
		action_FadeComplete = 10,
		action_FadeInComplete = 11,
		action_FadeOutComplete = 12,
        action_JointBreak = 13,     // only available with chipmunk physics

		// touch types for SpritTouchInfo
		touch_MouseEnter = 20, /** NOT IMPLEMENTED **/
		touch_MouseLeave = 21, /** NOT IMPLEMENTED **/
		touch_MouseDown = 22,
		touch_MouseUp = 23,
		touch_MouseClick = 24,

		// collision type
		collide_None = 0,
		collide_Point = 1,
		collide_BoundingBox = 2,
		collide_CollisionRadius = 3,
		collide_AlphaChannel = 4,
		collide_Last = collide_AlphaChannel
	};

    // get the bounds for the current frame including rotation
    RotatedRect	getFrameRotatedBounds(int frame);

    // sets current frame of Sprite to a given frame number
    Sprite& setFrame(int frame);
	int		getCurrentFrame();
    
    // report how many frames are in the sprite
    int     getFrameCount();
	
	// set how fast we animate through the frames
	// startingFrame and number of frames can be used to only animate through a subset of the
	// Sprite's frames.
	// when animating frames, you 
	void	startFrameAnimation(float fps, int startingFrame = start_FromFirstFrame, 
                            int numFrames = all_Frames, int animateFlags = animate_Looping);
	
	// stops all frame animation, the current frame becomes the sprite's image
	// does not send action_AnimationEnd sprite events
	void	stopFrameAnimation();
	
	// controls whether the sprite should generate action_AnimationLoop or action_AnimationEnd events, default is no
	bool	setWantsAnimLoopEvents(bool wantsThem = true);
	bool	setWantsAnimEndEvents(bool wantsThem = true);

    // adds an image that is used for one or more frames
    // since an image itself can have multiple frames, all frames of the image are added
    // to the Sprite, unless the numFrames is passed in.
    // the frames are added to the end of the frame list, unless startingFrame is passed in 
    void	addFramesImage(Image* image, int startingFrame = start_FromFirstFrame, int numFrames = all_Frames);
	
  #ifdef PDG_SCML_SUPPORT
  	bool	hasAnimation(const char* animationName);
  	bool	hasAnimation(int animationId);
  	void	startAnimation(const char* animationName);
  	void	startAnimation(int animationId);
  	Sprite& setEntityScale(int xScale, int yScale);
  #endif // PDG_SCML_SUPPORT

  #ifndef PDG_NO_GUI
	// set a drawing helper that will be called before the sprite is drawn
	// it can do whatever drawing it likes, then return true if it wants to let the
	// sprite draw itself, or false if the sprite should skip it's normal drawing
	// if a post draw helper is set, it will always be called, even if false is returned
	// pass null to remove the current helper
	void    setDrawHelper(ISpriteDrawHelper* helper);
	
	// set a drawing helper that will be called after a sprite is drawn
	// it can do whatever drawing it likes. The return value is ignored since
	// the sprite has already drawn. If set, it is called even if the primary draw 
	// helper tells the Sprite to skip normal drawing.
	// pass null to remove the current helper
	void    setPostDrawHelper(ISpriteDrawHelper* helper);	
  #endif

	// replace an image with another one for all the frames that reference that image
	void	changeFramesImage(Image* oldImage, Image* newImage);

    // set offset of centerpoint of sprite (rotation and location are all relative to centerpoint)
	// this can be set for the whole sprite, if image = 0, or for an individual frame or group of
	// frames by specifying the image or by specifying the range of frames
	// if image is 0, the startingFrame and numFrames parameters are frame numbers for the Sprit
	// if image is specified, the startingFrame and numFrames paramaters are Image frame numbers
	// this offset is relative to the absolute center of the image or frame
    void	offsetFrameCenters(int offsetX, int offsetY, Image* image = 0, 
    						int startingFrame = start_FromFirstFrame, int numFrames = all_Frames);
	// fetch the offsets set above, but only for a single frame
    void	getFrameCenterOffset(int &offsetX, int &offsetY, Image* image = 0, int frameNum = 0);

	// fading, with 1.0 being complete opaque and 0.0 being completely transparent
	Sprite& setOpacity(float opacity);
	float	getOpacity();
	void	fadeTo(float targetOpacity, int32 msDuration, 
                            EasingFunc easing = linearTween);  // fadeComplete notification when done
	void	fadeIn(int32 msDuration, 
                            EasingFunc easing = linearTween);  // fadeInComplete notification when done
	void	fadeOut(int32 msDuration, 
                            EasingFunc easing = linearTween);  // fadeOutComplete notification when done

	// arrange sprites within the layer
	Sprite& moveBehind(Sprite* sprite);
	Sprite& moveInFrontOf(Sprite* sprite);
	Sprite& moveToFront();
	Sprite& moveToBack();
	int		getZOrder();
	bool	isBehind(Sprite* sprite);
	
	// collisions
	Sprite& setWantsCollideWallEvents(bool wantsThem = true); // collisions for hitting bounds of sprite layer
	Sprite& enableCollisions(int collisionType = collide_AlphaChannel);
	Sprite& disableCollisions();
	// calling setCollisionRadius > 0 when collision are off is same as calling enableCollisions(collide_CollisionRadious)
	Sprite& setCollisionRadius(float pixelRadius);
	float	getCollisionRadius();	
	// define a collision mask image for all the frames that use the given image
	void	useCollisionMask(Image* frameImage, Image* maskImage);
	// define a helper that decides if two sprites that just met all other collision criteria actually collide or not
	void    setCollisionHelper(ISpriteCollideHelper* helper);

	// 1.0 perfectly elastic collisions, 0.0 perfectly inelastic (no bounce)
	// for chipmunk physics, using 1.0 is not recommended
	Sprite& setElasticity(float elasticity);
	float	getElasticity();

  #ifndef PDG_NO_GUI
	// controls whether the sprite wants to get mouse events
	bool    setWantsMouseOverEvents(bool wantsThem = true);
	bool    setWantsClickEvents(bool wantsThem = true);
	int     setMouseDetectMode(int collisionType = collide_BoundingBox); // how we detect when the mouse is over a sprite
	bool    setWantsOffscreenEvents(bool wantsThem = true); // events for when sprite moves offscreen
  #endif // ! PDG_NO_GUI

  #ifdef PDG_USE_CHIPMUNK_PHYSICS
	// override of what is provided by Animated
	virtual void	applyForce(const Vector& force, int32 msDuration = duration_Instantaneous);
	virtual void	applyTorque(float forceSpin, int32 msDuration = duration_Instantaneous);
	virtual void	stopAllForces();  // removes all forces that were set by applyForce (but not friction)
    virtual Animated&    setVelocity(const Vector& deltaPerSec);
	virtual Vector  getVelocity();
  #endif

	void setUserData(UserData* userData);
	void freeUserData();
    
    SpriteLayer* getLayer() { return mLayer; }

	long spriteId;
	
	bool wantsMouseOver;
	bool wantsClicks;
	bool wantsAnimLoop;
	bool wantsAnimEnd;
	bool wantsOffscreen;
	bool wantsWallCollide;
	
	UserData* userData; // any data the user may have associated with this sprite

  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mSpriteScriptObj;
  #endif

  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    cpBody* mBody;
    cpShape* mCollideShape;
    
    // this sprite becomes a static body that isn't affected by physics, though
    // non-static objects can collide with it. This would be used for walls or platforms.
    // This should be done before setting anything else about the sprite
    // returns itself so you can call Sprite* sprite = layer->createSprite().makeStatic();
    Sprite&         makeStatic();

	virtual Animated&	setMass(float mass);  // override
    virtual Animated&   setFriction(float friction); // override
    float           getFriction();

    // used to mark things that are in the same group and so shouldn't collide with one another
    // mainly used with sprites that are joined together
    Sprite&         setCollideGroup(long group) { mCollideGroup = group; return *this; }
    long            getCollideGroup() { return mCollideGroup; }

    // JOINTS:

    // pin sprites together, at a particular anchor point (offset from center) on each
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it

    cpConstraint*   pinJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float breakingForce = 0.0f);

    // join sprites together via a slider between anchor points that has a min/max distance
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   slideJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float minDist, float maxDist, float breakingForce = 0.0f);

    // join sprites together at a particular location in layer coordinates
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   pivotJoint(Sprite* otherSprite, Point pivot, float breakingForce = 0.0f);

    // join sprites together via a groove on this sprite to an anchor point on another
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   grooveJoint(Offset grooveStart, Offset grooveEnd, Sprite* otherSprite, Offset otherAnchor, float breakingForce = 0.0f);

    // join sprites together via a spring between anchor points
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   springJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float restLength, float stiffness, float damping, float breakingForce = 0.0f);

    // keep sprites at a particular angle relative to one another via a rotary spring
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   rotarySpring(Sprite* otherSprite, float restAngle, float stiffness, float damping, float breakingForce = 0.0f);

    // limit the angle another sprite can have relative to this one
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   rotaryLimit(Sprite* otherSprite, float minAngle, float maxAngle, float breakingForce = 0.0f);

    // like a socket wrench. ratchetInterval is the distance between “clicks”, phase is the initial 
    // angular offset to use when deciding where the ratchet angles are.
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   ratchet(Sprite* otherSprite, float rachetInterval, float phase = 0.0f, float breakingForce = 0.0f);

    // keep another sprite's rotation relative to this one at a particular gear ratio
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   gear(Sprite* otherSprite, float gearRatio, float initialAngle = 0.0f, float breakingForce = 0.0f);

    // keep spin of another sprite at a constant rate compared to this one
    // optional breaking force at which the joint (and and any other connections to that sprite) are broken
    // returns the cpConstraint pointer in case you want to do anything special with it
    
    cpConstraint*   motor(Sprite* otherSprite, float spin, float maxTorque = std::numeric_limits<float>::infinity());
    
    // remove a single joint
    void            removeJoint(cpConstraint* joint);
    
    // remove all connections. Optionally only remove all conncetions to a specific other sprite
    void            disconnect(Sprite* otherSprite = 0);

    void            makeJointBreakable(cpConstraint* joint, float breakingForce, Sound* breakSound = 0);
    void            makeJointUnbreakable(cpConstraint* joint);

  #endif // PDG_USE_CHIPMUNK_PHYSICS

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
    Sprite();

    virtual void easingCompleted(const Animation& a);
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    
    cpSpace*        getSpace();
    
    // override these to set values in chipmunk
	virtual void	locationChanged(const Offset& delta);
	virtual void	sizeChanged(float deltaW, float deltaH);
	virtual void	rotationChanged(float deltaRadians);
	virtual void	centerChanged(const Offset& delta);
    
    // hide these since they don't do what is expected
	Animated&     	setMoveFriction(float frictionCoefficient) { return *this; }
	Animated&    	setSpinFriction(float frictionCoefficient) { return *this; }
	Animated&    	setSizeFriction(float frictionCoefficient) { return *this; }
	float   		getMoveFriction() { return 0; }
	float   		getSpinFriction() { return 0; }
	float   		getSizeFriction() { return 0; }
    
    void            setupCollideGroup(Sprite* otherSprite);
    
    void			initCpBody();
    void			freeCpBody();

    cpConstraint*   mBreakableJoints[MAX_BREAKABLE_JOINTS_PER_SPRITE];
    int             mNumBreakableJoints;
    long            mCollideGroup;
    bool            mStatic;
  #endif

    virtual ~Sprite();

	// checks to see if the movingSprite collides with any features of the spriteLayer.  
	//	Determines the number of pixels that overlap based on the specified alphaThreshold
	//AB 10/2/10 - currently only returns 0 or 1
    //	virtual int    checkCollision(Sprite *movingSprite, uint8 alphaThreshold) const;

	// will apply the impulse based on each sprites' elasticity 
	void impartCollisionImpulse(Sprite* sprite, Vector& outNormal, Vector& outImpulse, float& outKineticEnergy);

	// return true if this sprite has collided with sprite paased in
	// saves the force and angle of the collision on the sprite in the 
	// sprite's collision info
	bool collidesWith(Sprite* sprite);
	
	bool collidesWith(const Point& p);
	
	void recalcOnscreenAndInBounds();
    
	
	// override the way we post events to go to the sprite layer first rather than directly to
	// the sprite manager
	virtual bool postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter = 0); // returns true if event handled
    
	// functions called from the layer
	virtual void	draw();
	virtual void	doAnimate(int msElapsed, bool layerDoCollisions);
    
 	int mNumFrames;

    typedef struct {
        ImageImpl*	image;
        ImageImpl*  collisionMask;
        Point	center;
        int		imageFrameNum;
        int		centerOffsetX;
        int		centerOffsetY;
    } FrameInfoT;


	// animation data
	FrameInfoT		mFrames[MAX_FRAMES_PER_SPRITE];
	float			mFps;
	float			mCurrFramePrecise;
	int				mCurrFrame;
	int				mFirstFrame;
	int				mLastFrame;
	bool			mLoopAnim;
	bool			mBackToFrontAnim;
	bool			mBidirectionalAnim;
	bool			mSpriteAnimating;
	bool			mSpriteAnimatingBackwardsNow;
	
	int				mDoCollisions;
	float			mCollisionRadius;
	float			mElasticity;
	int				mMouseDetectMode;

  #ifdef PDG_SCML_SUPPORT
	SCML_pdg::Entity*		mEntity;
	float					mEntityScaleX;
	float					mEntityScaleY;
  #endif // PDG_SCML_SUPPORT

  #ifndef PDG_NO_GUI
	// control which port this sprite draws into
    Port*   setPort(Port* newPort) { Port* port = mPort; mPort = newPort; return port; }
	
    Port* mPort;
  #endif

	SpriteLayer* mLayer;
	Sprite* mNextSprite;
	Sprite* mPrevSprite;
    
	float			mOpacity;
	int				mFadeCompleteAction;
	
  #ifndef PDG_NO_GUI
	ISpriteDrawHelper*	mDrawHelper;
	ISpriteDrawHelper*	mPostDrawHelper;
  #endif	

	ISpriteCollideHelper* mCollisionHelper;

	bool mOnscreen;
	bool mInBounds;
	bool mCompletelyInBounds;
	
	uint32 iid;

/// @endcond
};


inline int	
Sprite::getFrameCount() { 
    return mNumFrames; 
}

inline Sprite& 
Sprite::moveToFront() { 
    moveInFrontOf(0);
    return *this;
}

inline Sprite& 
Sprite::moveToBack() { 
    moveBehind(0); 
    return *this;
}

inline Sprite& 
Sprite::setWantsCollideWallEvents(bool wantsThem) {
	wantsWallCollide = wantsThem;
	if (wantsThem) recalcOnscreenAndInBounds();
	return *this;
}

inline bool	
Sprite::setWantsAnimLoopEvents(bool wantsThem) { 
	bool didWant = wantsAnimLoop; 
	wantsAnimLoop = wantsThem; 
	return didWant; 
}

inline bool	
Sprite::setWantsAnimEndEvents(bool wantsThem) {
	bool didWant = wantsAnimEnd; 
	wantsAnimEnd = wantsThem; 
	return didWant; 
}

#ifndef PDG_NO_GUI
inline bool	
Sprite::setWantsOffscreenEvents(bool wantsThem) {
	bool didWant = wantsOffscreen; 
	wantsOffscreen = wantsThem; 
	if (wantsThem) recalcOnscreenAndInBounds();
	return didWant; 
}
#endif // PDG_NO_GUI


} // end namespace pdg


#endif // PDG_SPRITE_H_INCLUDED
