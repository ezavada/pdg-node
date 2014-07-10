// -----------------------------------------------
// spritelayer.h
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


#ifndef PDG_SPRITELAYER_H_INCLUDED
#define PDG_SPRITELAYER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/animated.h"
#include "pdg/sys/eventemitter.h"
#include "pdg/sys/serializable.h"
#include "pdg/sys/os.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

#ifdef PDG_USE_CHIPMUNK_PHYSICS
#include "chipmunk.h"
#endif

#ifdef PDG_SCML_SUPPORT
#include "SCML_pdg.h"
#endif

#include <vector>

namespace pdg {

#ifndef PDG_NO_GUI
class Port;
#endif
class Sprite;
class SpriteLayer;
class SpriteManager;
class TimerManager;

/// @cond INTERNAL
struct LinkedLayerInfo {
	float	moveRatio;
	float	zoomRatio;
	SpriteLayer* linkedLayer;
};
/// @endcond

// flags for SpriteLayer::setSerializationFlags()
enum {
	ser_Positions = 	1 << 0,  // just position, rotation, frame number
	ser_ZOrder =		1 << 1,  // Z-Order of sprites within layer
	ser_Sizes =			1 << 2,  // height & width of sprites
	ser_Animations = 	1 << 3,  // animation settings in effect
	ser_Motion = 		1 << 4,  // motion settings in effect
	ser_Forces = 		1 << 5,  // physics forces in effect
	ser_Physics = 		1 << 6,  // physics settings (mass, friction, etc)
	ser_LayerDraw =		1 << 7,  // layer position, zoom and rotation info
	ser_ImageRefs =		1 << 8,  // references to recreate images
	ser_SCMLRefs =		1 << 9,  // references to SCML data
	ser_HelperRefs = 	1 << 10, // references to helper object
	ser_ImageData = 	1 << 11, // send the images
	ser_SCMLData =		1 << 12, // send the SCML data
	ser_HelperObjs = 	1 << 13, // send the helper objects (which must be serializable)
	ser_InitialData =   1 << 14, // send whatever is needed to initialize everything
	ser_Micro =	ser_Positions | ser_ZOrder,
	ser_Update = ser_Micro | ser_Sizes | ser_Animations | ser_Motion | ser_Forces | ser_Physics,
	ser_Full = ser_Update | ser_ImageRefs | ser_SCMLRefs | ser_HelperRefs | ser_InitialData
};

// -----------------------------------------------------------------------------------
// Sprite Layer
// Used to create and track collections of sprites
// -----------------------------------------------------------------------------------

class SpriteLayer : public EventEmitter, public Animated, public Serializable<SpriteLayer> {
friend class Sprite;
friend class SpriteManager;
public:
    
	SERIALIZABLE_TAG( CLASSTAG_SPRITE_LAYER )
	SERIALIZABLE_METHODS()
	void setSerializationFlags(uint32 flags);

	enum {
		addSprites = true,		// add sprites for all entities when initializing from SCML
		dontAddSprites = false,	// don't add sprites when initializing from SCML, just load entity definitions

		// actions for SprintLayerInfo
		action_ErasePort = 40,			// before any layers are drawn, before port is erased, return handled if you erase it yourself
		action_PreDrawLayer = 41,		// before each layer is drawn
		action_PostDrawLayer = 42,		// after each layer is drawn
		action_DrawPortComplete = 43,	// after all layers are drawn, but before any animations are considered

		action_AnimationStart = 44,		// before any animation is run for a particular animation cycle
		action_PreAnimateLayer = 45,	// before animation is run on any of the objects in a particular layer
		action_PostAnimateLayer = 46,	// after animation is run on any of the object in a particular layer
		action_AnimationComplete = 47,	// after animation is run on all the objects in a layer for a particular animation cycle
		
		action_ZoomComplete = 48,		// a zoom done with animateZoomTo has finished 
		action_FadeInComplete = 49,		// a fade done with fadeIn has finished
		action_FadeOutComplete = 50		// a fade done with fadeOut has finished
	};

	const float noZoom;
	
	// this is the port that the sprites in this layer will render into
    // multiple layers can render into same port creating, drawn in order of creation
  #ifndef PDG_NO_GUI
    Port*           getSpritePort() const;
    virtual void    setSpritePort(Port* port);
  #endif

    // start and stop animating all sprites. 
	// If animations are stopped you won't get any simulation, action or collison events for the entire layer
    virtual void	startAnimations();
    virtual void	stopAnimations();

    // hide and show (make the layer visible or invisible within the port)
	// if the layer is hidden it will not get any mouseover or click events
    virtual void	hide();
    virtual void	show();
	virtual bool	isHidden();
	virtual void	fadeIn(int32 msDuration, EasingFunc easing = linearTween);  // fadeInComplete notification when done
	virtual void	fadeOut(int32 msDuration, EasingFunc easing = linearTween);  // fadeOutComplete notification when done
	
	// arrange layers
	virtual void	moveBehind(SpriteLayer* layer);
	virtual void	moveInFrontOf(SpriteLayer* layer);
	void 			moveToFront();
	void 			moveToBack();
	int				getZOrder();

	// link layers so they move, rotate and zoom together
	// rotation is always at 1:1 ratio, but movement and zoom ratios can be optionally specified
	virtual	void	moveWith(SpriteLayer* layer, float moveRatio = 1.0f, float zoomRatio = 1.0f);

  #ifndef PDG_NO_GUI
	// origin is the position of the sprite world's center point in the Port when the location is 0,0
    // origin isn't affect by zoom or rotation, it happens after all of those are applied
    void    		setOrigin(const Point& origin);
    Point			getOrigin() const;

    // auto-adjust center offset and location to keep both zoom and rotation center at the origin 
    // (same point in Port). So for example if you want to have the world rotate and zoom around your 
    // character then set the origin to your character's screen location, setAutoCenter(true), and
    // setFixedMoveAxis(true) then all movement and rotation will be relative to your character's
    // fixed screen location
    void            setAutoCenter(bool autoCenter = true);
    // if true, movement is along port axis. If false, movement is on the layer axis
    void            setFixedMoveAxis(bool fixedAxis = true);

	// zooming
	virtual void	setZoom(float zoomLevel);
	float           getZoom() const;

	 // keeps centered, taking into account layer center offset
	virtual void	zoomTo(float zoomLevel, int32 msDuration, EasingFunc easing = easeInOutQuad, 
							Rect keepInRect = Rect(0,0), const Point* centerOn = 0);
	void			zoom(float deltaZoomLevel, int32 msDuration, EasingFunc easing = easeInOutQuad, 
							Rect keepInRect = Rect(0,0), const Point* centerOn = 0);
  #endif // ! PDG_NO_GUI

	// find, add and remove sprites from the layer
	virtual Sprite*	findSprite(long id); 	// find a sprite in the layer by id. Be sure to addRef() the sprite if you hang onto the reference
	virtual Sprite* getNthSprite(int index); 	// get the nth sprite in the layer by z-order, 0 is the one furthest back
	virtual int		getSpriteZOrder(Sprite* sprite); // z-order 0 is furthest back, increment from there
	virtual bool	isSpriteBehind(Sprite* sprite, Sprite* otherSprite); // does otherSprite have higher z-order than sprite?
	virtual bool	hasSprite(Sprite* sprite);  // see if a sprite is in this layer
	virtual void	addSprite(Sprite* newSprite);
	virtual void	addSpriteInFrontOf(Sprite* newSprite, Sprite* targetSprite);
	virtual void	removeSprite(Sprite* oldSprite); // will delete if no longer referenced
	virtual void	removeAllSprites(); // completely empty out the layer

	// turn on or off collisions of objects within this layer
    // individual sprites in the layer still won't collide if their collisions are turned off
	virtual void	enableCollisions();
	virtual void	disableCollisions();

	// collide objects in this layer against items in another layer
	// pass true for deferEvents to have the collision events added to the event queue rather than processed immediately
    virtual void    enableCollisionsWithLayer(SpriteLayer* otherLayer);
    virtual void    disableCollisionsWithLayer(SpriteLayer* otherLayer);

	// create sprites in the layer
	virtual Sprite* createSprite();
	virtual Sprite* cloneSprite(const Sprite* originalSprite);
	
  #ifdef PDG_SCML_SUPPORT
	// Spriter file (SCML) support
	
	// create a sprite from Spriter SCML data, optionally specifying which entity if there are several
	virtual Sprite* createSpriteFromSCML(const char* inSCML, const char* inEntityName = 0);
	// create a sprite from a Spriter SCML file, optionally specifying which entity if there are several
	virtual Sprite* createSpriteFromSCMLFile(const char* inFileName, const char* inEntityName = 0);
	// create a sprite from an entity that was previously loaded
	// this only works for layers created via createSpriteLayerFromSCMLFile
	virtual Sprite* createSpriteFromSCMLEntity(const char* inEntityName);
  #endif // PDG_SCML_SUPPORT

  #ifndef PDG_NO_GUI
    // coordinate conversions, adjusting for offset, zoom and rotation of layer
    virtual Point           layerToPort(const Point& p) const;
    virtual Offset          layerToPort(const Offset& p) const;
    virtual Vector          layerToPort(const Vector& p) const;
    virtual RotatedRect     layerToPort(const Rect& r) const;
    virtual RotatedRect     layerToPort(const RotatedRect& r) const;
    virtual Quad            layerToPort(const Quad& q) const;

    virtual Point           portToLayer(const Point& p) const;
    virtual Offset          portToLayer(const Offset& p) const;
    virtual Vector          portToLayer(const Vector& p) const;
    virtual RotatedRect     portToLayer(const Rect& r) const;
    virtual RotatedRect     portToLayer(const RotatedRect& r) const;
    virtual Quad            portToLayer(const Quad& q) const;
  #endif // ! PDG_NO_GUI

    // fetch a sprite layer by id
    static SpriteLayer* getLayer(long id);
    
	long layerId;

  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mSpriteLayerScriptObj;
  #endif

  #ifdef PDG_USE_CHIPMUNK_PHYSICS
	void		setUseChipmunkPhysics(bool useIt = true);	// do we actually use chipmunk for the sprites in this layer?
	void		setStaticLayer(bool isStatic = true);		// if true, create all sprites for this layer as static (see Chipmunk docs)
    cpSpace*    getSpace();

    // continual force
    void        setGravity(float gravity, bool keepItDownward = true); // applies to all layers, pulls everything downward
    void        setKeepGravityDownward(bool keepItDownward = true);

    // continual damping effect of movement. Identical to calling
    // setMoveFriction() and setSpinFriction() on every sprite in every layer
    void        setDamping(float damping); // applies to all layers
  #endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond C++

	Sprite*	findSpriteByInternalId(uint32 iid) const; 	// find a sprite in the layer by internal id.
	void quickSwapSprites(Sprite* s1, Sprite* s2);  // swap z-order of two sprites already in layer

  #ifndef PDG_NO_GUI
	SpriteLayer(Port* port);
  #endif
    SpriteLayer();
    virtual ~SpriteLayer();
	virtual void	locationChanged(const Offset& delta);
    virtual void    rotationChanged(float deltaRadians);

  #ifndef PDG_NO_GUI
  	// zoom is a visual effect only, so we don't worry about it on a non-gui build
    virtual void    easingCompleted(const Animation& a);  // override for zoom-based easing
	virtual void	zoomChanged(float deltaZoom);
	// layer drawing
	virtual void drawLayer();
  #endif // ! PDG_NO_GUI

	virtual void animateLayer(long msElapsed);

    // do collision between layers
	virtual void    collide(long msElapsed, SpriteLayer* withLayer, bool deferEvents = false);

	// sprite action notifications
	// normally these notifications will be enqueued to be handled at the end of the event loop,
	// but you can pass true to sendImmediately to have the notifications directly posted to the event handlers
	void notifyAnimationAction(int action, Sprite* actingSprite, bool sendImmediately = false);
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
	void notifyCollisionAction(int action, Sprite* actingSprite, Vector normal, Vector impulse, float force, float kineticEnergy, cpArbiter* arbiter, Sprite* targetSprite = 0, bool sendImmediately = false);
  #else
	void notifyCollisionAction(int action, Sprite* actingSprite, Vector normal, Vector impulse, float force, float kineticEnergy, Sprite* targetSprite = 0, bool sendImmediately = false);
  #endif
  #ifndef PDG_NO_GUI
	void wantMouseOverEvents();
	void checkIfMouseOverEventsStillWanted();
	void wantClickEvents();
	void checkIfClickEventsStillWanted();
  #endif // ! PDG_NO_GUI

  #ifndef PDG_NO_GUI
    Port* mPort;
    Point mOrigin;
  #endif

	bool mHidden;
	bool mAnimating;
	bool mDoCollisions;
	bool mWantsMouseOver;
	bool mWantsClicks;
	uint32 mDoneFadingInAt;
	uint32 mDoneFadingOutAt;

  #ifndef PDG_NO_GUI
	float mZoom;

	bool mAutoCenter;
    bool mFixedMoveAxis;
  #endif

  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    float mGravity;
	bool mKeepGravityDownward;
	bool mUseChipmunkPhysics;
	bool mIsStaticLayer;
  #endif

  #ifdef PDG_SCML_SUPPORT
	Sprite* createSCMLSprite(SCML::Data* scmlData, bool needLoad, const char* inEntityName, bool addAll = false);
	void createSCMLSprites(SCML::Data* scmlData);
  	std::list<SCML::Data*>	mSCMLData;
  #endif

	SpriteLayer* mNextLayer;
	SpriteLayer* mPrevLayer;
	Sprite* mFirstSprite;
	Sprite* mLastSprite;

    std::vector<SpriteLayer*> mCollideLayers;
    
    std::vector<LinkedLayerInfo> mLinkedLayers;
	SpriteLayer* mControlledBy;

	// caching of frequently used expensive calculations
	
	float mFacingCos;
	float mFacingSin;
	
	uint32 mSerFlags;
	
	uint32 iid;

/// @endcond
};

#ifndef PDG_NO_GUI
inline void
SpriteLayer::setAutoCenter(bool autoCenter) {
    mAutoCenter = autoCenter;
}

inline void
SpriteLayer::setFixedMoveAxis(bool fixedMove) {
    mFixedMoveAxis = fixedMove;
}
#endif

inline void 
SpriteLayer::moveToFront() { 
    moveInFrontOf(0); 
}

inline void 
SpriteLayer::moveToBack() { 
    moveBehind(0); 
}

#ifndef PDG_NO_GUI
inline Vector
SpriteLayer::layerToPort(const Vector& v) const {
    return layerToPort(Offset(v));
}

inline Vector
SpriteLayer::portToLayer(const Vector& v) const {
    return portToLayer(Offset(v));
}

inline void  
SpriteLayer::setOrigin(const Point& origin) {
    mOrigin = origin;
}

inline Point 
SpriteLayer::getOrigin() const {
	return mOrigin;
}

inline float
SpriteLayer::getZoom() const {
    return mZoom;
}

inline void	
SpriteLayer::zoom(float deltaZoom, int32 msDuration, EasingFunc easing, 
				Rect keepInRect, const Point* centerOn) {
	zoomTo(mZoom * deltaZoom, msDuration, easing, keepInRect, centerOn);
}

inline Port*   
SpriteLayer::getSpritePort() const {
    return mPort;
}    
#endif

#ifdef PDG_USE_CHIPMUNK_PHYSICS
inline void
SpriteLayer::setStaticLayer(bool isStatic) {
	DEBUG_ASSERT(mFirstSprite == 0, "SpriteLayer::setStaticLayer() must be called before any sprites are added to this layer");
    mIsStaticLayer = isStatic;
}

inline void
SpriteLayer::setUseChipmunkPhysics(bool useIt) {
	DEBUG_ASSERT(mFirstSprite == 0, "SpriteLayer::setUseChipmunkPhysics() must be called before any sprites are added to this layer");
    mUseChipmunkPhysics = useIt;
}
#endif // PDG_USE_CHIPMUNK_PHYSICS

// create a Sprite Layer object
// will use main port if none given
#ifndef PDG_NO_GUI

SpriteLayer* createSpriteLayer(Port* port = 0);
#ifdef PDG_SCML_SUPPORT
// if addSprites is true, every entity in the SCML file will have a sprite created for it
// and added to the layer.
SpriteLayer* createSpriteLayerFromSCMLFile(const char* layerSCMLFile, bool addSprites = true, Port* port = 0);
#endif

#else

SpriteLayer* createSpriteLayer();
#ifdef PDG_SCML_SUPPORT
// if addSprites is true, every entity in the SCML file will have a sprite created for it
// and added to the layer.
SpriteLayer* createSpriteLayerFromSCMLFile(const char* layerSCMLFile, bool addSprites = true);
#endif

#endif // PDG_NO_GUI

// delete a sprite layer, release all sprites in it, and remove it from the sprite manager
void cleanupSpriteLayer(SpriteLayer* layer);
	

} // end namespace pdg

#endif // PDG_SPRITELAYER_H_INCLUDED
