// -----------------------------------------------
// spritemanager.cpp
// 
// sprite manager functionality
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

#include "pdg/sys/sprite.h"
#include "pdg/sys/spritelayer.h"
#include "pdg/sys/tilelayer.h"
#include "pdg/sys/os.h"

#ifndef PDG_NO_GUI
#include "pdg/sys/port.h"
#endif

#include "spritemanager.h"


#define SPRITE_LAYER_TIMER_ID -1234031
#define SPRITE_TIMER_INTERVAL_MS 10  // 100 fps for sprite animation

//#define SPRITE_IGNORE_ANIMATION_TIMER_DRIFT

namespace pdg {
	

#ifdef PDG_USE_CHIPMUNK_PHYSICS

cpBool 
SpriteManager::ChipmunkSpriteCollisionBeginFunc(cpArbiter *arb, struct cpSpace *space, void *data) {
    // TODO: deal with Sensors, which will never generate the PostSolve callback
    //     if (cpShapeGetSensor())
    
    // Get pointers to the two bodies in the collision pair and define local variables for them.
    // Their order matches the order of the collision types passed
    // to the collision handler this function was defined for
    CP_ARBITER_GET_BODIES(arb, sprite1, sprite2);
    Sprite* s1 = static_cast<Sprite*>(cpBodyGetUserData(sprite1));
    if (!s1->mDoCollisions) return false;
    Sprite* s2 = static_cast<Sprite*>(cpBodyGetUserData(sprite2));
    if (!s2->mDoCollisions) return false;
    // same layer, okay to collide
	bool canCollide =  (s1->mLayer == s2->mLayer);
	if (!canCollide) {
		// layer to layer collisions, only between layers specifically marked    
		for (std::vector<SpriteLayer*>::iterator itr = s1->mLayer->mCollideLayers.begin(); itr != s1->mLayer->mCollideLayers.end(); itr++) {
			if (s2->mLayer == *itr) {
				canCollide = true;
				break;
			}
		}
    }
	if (!canCollide) {
    	for (std::vector<SpriteLayer*>::iterator itr = s2->mLayer->mCollideLayers.begin(); itr != s2->mLayer->mCollideLayers.end(); itr++) {
       	 	if (s1->mLayer == *itr) {
				canCollide = true;
				break;
			}
		}
    }
    // if we have a collision helper for either of the sprites, check it
    if (canCollide && s1->mCollisionHelper) {
    	canCollide = s1->mCollisionHelper->allowCollision(s1, s2);
    }
    if (canCollide && s2->mCollisionHelper) {
    	canCollide = s2->mCollisionHelper->allowCollision(s1, s2);
    }
    return canCollide;
}

void 
SpriteManager::ChipmunkSpriteCollisionPostSolveFunc(cpArbiter *arb, cpSpace *space, void *data) {
    // this is only called if we have approved the collision
    CP_ARBITER_GET_BODIES(arb, sprite1, sprite2);
    Sprite* s1 = static_cast<Sprite*>(cpBodyGetUserData(sprite1));
    Sprite* s2 = static_cast<Sprite*>(cpBodyGetUserData(sprite2));
    cpVect norm = cpArbiterGetNormal(arb, 0);
    cpVect imp = cpArbiterTotalImpulse(arb);
    Vector normal(norm.x, norm.y);
    Vector impulse(imp.x, imp.y);
    cpFloat kineticEnergy = cpArbiterTotalKE(arb);
    cpFloat dt = ((float)SPRITE_TIMER_INTERVAL_MS)/1000.0f;
    cpFloat force = impulse.vectorLength() / dt;
    s1->mLayer->notifyCollisionAction(Sprite::action_CollideSprite, s1, normal, impulse, force, kineticEnergy, arb, s2);
}

cpBool 
SpriteManager::ChipmunkWallCollisionBeginFunc(cpArbiter *arb, struct cpSpace *space, void *data) {
    // Get pointers to the two bodies in the collision pair and define local variables for them.
    // Their order matches the order of the collision types passed
    // to the collision handler this function was defined for
    CP_ARBITER_GET_BODIES(arb, sprite, wall);
    
    return true; // if we want this collision to happen
}

void 
SpriteManager::ChipmunkWallCollisionPostSolveFunc(cpArbiter *arb, cpSpace *space, void *data) {
}

#endif  // PDG_USE_CHIPMUNK_PHYSICS
    
#ifndef PDG_NO_GUI
SpriteLayer* 
SpriteManager::createSpriteLayer(Port* port) {
    return new SpriteLayer(port);
}

TileLayer* 
SpriteManager::createTileLayer(Port* port) {
    return new TileLayer(port);
}
#endif // ! PDG_NO_GUI

SpriteLayer* 
SpriteManager::createSpriteLayer() {
    return new SpriteLayer();
}

TileLayer* 
SpriteManager::createTileLayer() {
    return new TileLayer();
}

void
SpriteManager::cleanupLayer(SpriteLayer* layer) {
    delete layer;
}

// add a layer to the end
void SpriteManager::addLayer(SpriteLayer* layer) {
	if (!layer) return;
	layer->mNextLayer = 0;
	if (mFirstLayer == 0) {
		mFirstLayer = layer;
		layer->mPrevLayer = 0;
	} else {
		layer->mPrevLayer = mLastLayer;
		mLastLayer->mNextLayer = layer;
	}
	mLastLayer = layer;  // we are always the new last layer
}

void SpriteManager::removeLayer(SpriteLayer* layer) {
	// update our first and last layers
	if (layer == mFirstLayer) {
		mFirstLayer = layer->mNextLayer;
	}
	if (layer == mLastLayer) {
		mLastLayer = layer->mPrevLayer;
	}
	// now update the previous and next layers to point to one another
	if (layer->mPrevLayer) {
		layer->mPrevLayer->mNextLayer = layer->mNextLayer;
	}
	if (layer->mNextLayer) {
		layer->mNextLayer->mPrevLayer = layer->mPrevLayer;
	}
	// finally clear our prev and next layers
	layer->mNextLayer = 0;
	layer->mPrevLayer = 0;
}

SpriteManager::SpriteManager(EventManager* eventMgr, TimerManager* timerMgr): 
	mEventMgr(eventMgr), 
	mTimerMgr(timerMgr),
	mFirstLayer(0),
	mLastLayer(0)
{	
	// for now we need these
	DEBUG_ASSERT(mEventMgr, "must have a pdg::EventManager");
	DEBUG_ASSERT(mTimerMgr, "must have a pdg::TimerManager");
	
	eventMgr->addHandler(this, eventType_Timer);
	eventMgr->addHandler(this, eventType_PortDraw);
	eventMgr->addHandler(this, eventType_MouseDown);
	eventMgr->addHandler(this, eventType_MouseUp);
	eventMgr->addHandler(this, eventType_MouseMove);
	
	timerMgr->startTimer(SPRITE_LAYER_TIMER_ID, SPRITE_TIMER_INTERVAL_MS,
						 timer_Repeating, UserData::makeUserDataFromPointer(this, data_DoNothing) );
#ifdef PDG_USE_CHIPMUNK_PHYSICS
    mSpace = cpSpaceNew();
    cpSpaceAddCollisionHandler(mSpace, CP_COLLIDE_TYPE_SPRITE, CP_COLLIDE_TYPE_SPRITE,
                               ChipmunkSpriteCollisionBeginFunc, // begin
                               NULL, // preSolve
                               ChipmunkSpriteCollisionPostSolveFunc, // postSolve,
                               NULL, // separate,
                               NULL); // *data
    cpSpaceAddCollisionHandler(mSpace, CP_COLLIDE_TYPE_SPRITE, CP_COLLIDE_TYPE_WALL,
                               ChipmunkWallCollisionBeginFunc, // begin
                               NULL, // preSolve
                               ChipmunkWallCollisionPostSolveFunc, // postSolve,
                               NULL, // separate,
                               NULL); // *data
#endif
    
}

SpriteManager::~SpriteManager() {
#ifdef PDG_USE_CHIPMUNK_PHYSICS
    cpSpaceFree(mSpace);
    mSpace = 0;
#endif
}

// return true if completely handled
bool SpriteManager::handleEvent(EventEmitter* inEmitter, long inEventType, void* inEventData) throw() {
	if (mFirstLayer == 0) return false;  // we can't do anything if we don't have any sprite layers
	if (inEventType == eventType_Timer) {
		pdg::TimerInfo* infoP = static_cast<TimerInfo*>(inEventData);
		if ((infoP->id == SPRITE_LAYER_TIMER_ID) && (infoP->userData == (void*) this)) {
			
			// time passed since we last animated
          #ifdef SPRITE_IGNORE_ANIMATION_TIMER_DRIFT
			uint32 elapsedMs = SPRITE_TIMER_INTERVAL_MS; // this is how long we told it to take
          #else
			uint32 elapsedMs = infoP->msElapsed;  // this is the actual time it took
			if (elapsedMs > 100) { 
				// 1/10 of a second is more than timer drift. We were probably
				// paused or in the debugger or something. So use the normal timer interval
				elapsedMs = SPRITE_TIMER_INTERVAL_MS;
			}
          #endif
  
			SpriteLayerInfo evntInfo;
			// Animate all the layers to their position at the next draw loop
			SpriteLayer* layer = mFirstLayer;
			evntInfo.actingLayer = mFirstLayer;
			evntInfo.action = SpriteLayer::action_AnimationStart;
			evntInfo.millisec = infoP->millisec;
			mFirstLayer->postEvent(eventType_SpriteLayer, &evntInfo);

          #ifdef PDG_USE_CHIPMUNK_PHYSICS
            // Chipmunk docs say it is highly recommended we use a regular step amount, 
            // but when we force the amount to a regular step that doesn't correspond 
            // to the time it actually took, we get erratic movement. Instead we
            // try to do a lot of simulations with a small step decoupled from the
            // drawing loop and that seems to work well
            cpFloat dt = (float) elapsedMs / 1000.0f;
            cpSpaceStep(mSpace, dt);
          #endif

			while (layer) {
				evntInfo.actingLayer = layer;
				evntInfo.action = SpriteLayer::action_PreAnimateLayer;
				layer->postEvent(eventType_SpriteLayer, &evntInfo);
				layer->animateLayer(elapsedMs);
				evntInfo.actingLayer = layer;
				evntInfo.action = SpriteLayer::action_PostAnimateLayer;
				layer->postEvent(eventType_SpriteLayer, &evntInfo);
				layer = layer->mNextLayer;
			}
			evntInfo.actingLayer = mLastLayer;
			evntInfo.action = SpriteLayer::action_AnimationComplete;
			mLastLayer->postEvent(eventType_SpriteLayer, &evntInfo);
			
			return true;
		}
	}

#ifndef PDG_NO_GUI
	if (inEventType == eventType_PortDraw) {
		pdg::PortDrawInfo* infoP = static_cast<PortDrawInfo*>(inEventData);
		if (infoP->port == mFirstLayer->mPort) {

			SpriteLayerInfo evntInfo;
			SpriteLayer* layer = mFirstLayer;
			// Draw all the layers and their appropriate ports, and time how long it took
			evntInfo.actingLayer = mFirstLayer;
			evntInfo.action = SpriteLayer::action_ErasePort;
			evntInfo.millisec = OS::getMilliseconds();
			if (mFirstLayer->mPort) {
				if (!mFirstLayer->postEvent(eventType_SpriteLayer, &evntInfo) ) {
				// erase the port to black
//					mFirstLayer->mPort->fillRect(mFirstLayer->mPort->getDrawingArea(), PDG_BLACK_COLOR );
				}
			}

			layer = mFirstLayer;
			while (layer) {
				if (layer->mPort) {
					evntInfo.actingLayer = layer;
					evntInfo.action = SpriteLayer::action_PreDrawLayer;
					layer->postEvent(eventType_SpriteLayer, &evntInfo);
					layer->drawLayer();
					evntInfo.actingLayer = layer;
					evntInfo.action = SpriteLayer::action_PostDrawLayer;
					layer->postEvent(eventType_SpriteLayer, &evntInfo);
				}
				layer = layer->mNextLayer;
			}
			evntInfo.actingLayer = mLastLayer;
			evntInfo.action = SpriteLayer::action_DrawPortComplete;
			if (mLastLayer->mPort) {
				mLastLayer->postEvent(eventType_SpriteLayer, &evntInfo);
			}		
			return false; // we didn't completely handle this, others may want to draw
		}
	}
#endif // !PDG_NO_GUI

	if ( (inEventType == eventType_MouseDown) || (inEventType == eventType_MouseUp)) {
		Sprite* hitSprite = 0;
		MouseInfo* mi = static_cast<MouseInfo*>(inEventData);
 		SpriteLayer* layer = mLastLayer;
		while (layer) {
			if (!layer->mHidden && layer->mWantsClicks) {
				Sprite* sprite = layer->mLastSprite;
			  #ifndef PDG_NO_GUI
                Point clickPt = layer->portToLayer(mi->mousePos);
			  #else 
                Point clickPt = mi->mousePos;
			  #endif
				while (sprite) {
					if (sprite->wantsClicks) {
						int collideMode = sprite->mDoCollisions;
						sprite->mDoCollisions = sprite->mMouseDetectMode;
						bool didCollide = sprite->collidesWith(clickPt);
						sprite->mDoCollisions = collideMode;
						if (didCollide) {
							hitSprite = sprite;
							break;
						}
					}
					sprite = sprite->mPrevSprite;
				}
				if (hitSprite) {
					break;
				}
			}
			layer = layer->mPrevLayer;
		}
		if (hitSprite) {
			static Sprite* sLastHitSprite = 0;
			SpriteTouchInfo sti;
			std::memcpy(&sti, mi, sizeof(MouseInfo));
			if (inEventType == eventType_MouseDown) {
				sti.touchType = Sprite::touch_MouseDown;
				sLastHitSprite = hitSprite;
			} else {
				sti.touchType = Sprite::touch_MouseUp;
			}
			sti.touchedSprite = hitSprite;
			sti.inLayer = layer;
			hitSprite->postEvent(eventType_SpriteTouch, &sti);
			if ((inEventType == eventType_MouseUp) && (hitSprite == sLastHitSprite)) {
				sti.touchType = Sprite::touch_MouseClick;
				hitSprite->postEvent(eventType_SpriteTouch, &sti);
			}
			return true;
		}
	}
	return false;
}
	
	

SpriteManager* SpriteManager::createSingletonInstance() {
	EventManager* evtMgr = EventManager::getSingletonInstance();
	TimerManager* tmrMgr = TimerManager::getSingletonInstance();
	return new SpriteManager(evtMgr, tmrMgr);
}

	
} // end namespace pdg
