// -----------------------------------------------
// spritelayer.cpp
// 
// sprite layer functionality
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

#include "pdg/sys/os.h"
#include "pdg/sys/sprite.h"
#include "pdg/sys/spritelayer.h"
#include "pdg/sys/iserializer.h"
#include "pdg/sys/ideserializer.h"

#include "spritemanager.h"
#include "internals.h"

#ifdef PDG_SCML_SUPPORT
#include "SCML_pdg.h"
#endif

#include <algorithm>
#include <fstream>

// define the following in your build environment, or uncomment it here to get
// debug output for the core events and timers
//#define PDG_DEBUG_SPRITELAYER
//#define PDG_DEBUG_SERIALIZATION

#ifndef PDG_DEBUG_SPRITELAYER
  #define SPRITELAYER_DEBUG_ONLY(_expression)
#else
  #define SPRITELAYER_DEBUG_ONLY(_expression) _expression
  #define SPRITELAYER_INTERNAL_DEBUG   // does some extra drawing to show layers
#endif
#ifndef PDG_DEBUG_SERIALIZATION
  #define SERIALIZATION_DEBUG_ONLY(_expression)
#else
  #define SERIALIZATION_DEBUG_ONLY(_expression) _expression
#endif

#ifndef PI
#define PI       3.141592        // the venerable pi
#endif
#ifndef M_PI
#define M_PI PI
#endif
#ifndef TWO_PI
#define TWO_PI    6.283184      // handy for dealing with circles
#endif

#ifdef PDG_DESERIALIZER_NO_THROW
	// we can't throw, so we report errors but don't exit
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)", #_except, _msg, __FUNCTION__, __FILE__, __LINE__); \
		DEBUG_PRINT(buf); }
#else
	// we throw exceptions on errors
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)", #_except, _msg, __FUNCTION__, __FILE__, __LINE__); \
		throw _except(buf); }
#endif

#define PDG_SPRITE_LAYER_MAGIC_NUMBER   0x31008971
#define PDG_SPRITE_LAYER_STREAM_V_1		0
// add new versions here
#define PDG_SPRITE_LAYER_STREAM_VERSION	PDG_SPRITE_LAYER_STREAM_V_1

#ifndef PDG_UNSAFE_SERIALIZATION
#define PDG_TAG_SERIALIZED_DATA
#endif

namespace pdg {

#ifdef PDG_SCML_SUPPORT
	SCML_pdg::FileSystem gSCMLFileSystem;
#endif

long gNextLayerId = 1;
static uint32 sUniqueLayerId = 1;


void SpriteAnimateInfo_ReleaseSprites(void* ptr);
void SpriteCollideInfo_ReleaseSprites(void* ptr);

void SpriteAnimateInfo_ReleaseSprites(void* ptr) {
	if (!ptr) return;
	SpriteAnimateInfo* sai = static_cast<SpriteAnimateInfo*>(ptr);
	if (sai->actingSprite) {
		sai->actingSprite->release();
		sai->actingSprite = 0;
	}
}

void SpriteCollideInfo_ReleaseSprites(void* ptr) {
	if (!ptr) return;
	SpriteAnimateInfo_ReleaseSprites(ptr);
	SpriteCollideInfo* sci = static_cast<SpriteCollideInfo*>(ptr);
	if (sci->targetSprite) {
		sci->targetSprite->release();
		sci->targetSprite = 0;
	}
}



// -----------------------------------------------------------------------------------
// Sprite Layer
// Used to create and track sets of sprites
// -----------------------------------------------------------------------------------

void SpriteLayer::setSerializationFlags(uint32 flags) {
	mSerFlags = flags;
}

uint32 SpriteLayer::getSerializedSize(ISerializer* serializer) const {
	uint32 totalSize = 0;
	Sprite* sprite = 0;
	uint32 count = 0;
	totalSize += 1;  // size of PDG_SPRITE_LAYER_STREAM_VERSION
	totalSize += serializer->serializedSize(mSerFlags); // size of serializable flags
	if ( (mSerFlags == ser_Micro) || (mSerFlags == ser_Positions) ) {
		// special case, smallest possible update, basically just the sprites
		sprite = mFirstSprite;
		while (sprite) {
			count++;
			if (mSerFlags & ser_ZOrder) {
				totalSize += serializer->serializedSize(sprite->iid);
			}
			// normally we would call serializer->serializedSize(sprite), but
			// that adds a bunch of overhead for object tags and such that we don't want
			// this only gives us the size of the data
			totalSize += sprite->getSerializedSize(serializer);
			sprite = sprite->mNextSprite;
		}
		totalSize += serializer->serializedSize(count);
	} else {
	  #ifdef PDG_TAG_SERIALIZED_DATA
		totalSize += 4;  // size of request magic number
	  #endif
		totalSize += 2; // size layerFlags
		if (mSerFlags & ser_InitialData) {
			totalSize += serializer->serializedSize(iid);
			totalSize += serializer->serializedSize((uint32)layerId);
		}
		SIZE_FLOAT_LIST_START(2, 15);
		if (mSerFlags & ser_Positions) {
			SIZE_NON_ZERO_F(mLocation.x, 0);
			SIZE_NON_ZERO_F(mLocation.y, 1);
			SIZE_NON_ZERO_F(mFacing, 2);
		}
		if (mSerFlags & ser_Sizes) {
			SIZE_NON_ZERO_F(mHeight, 3);
			SIZE_NON_ZERO_F(mWidth, 4);
		}
		if (mSerFlags & ser_LayerDraw) {
		  #ifndef PDG_NO_GUI
			SIZE_NON_ZERO_F(mOrigin.x, 5);
			SIZE_NON_ZERO_F(mOrigin.y, 6);
			SIZE_NON_ZERO_F(mZoom, 7);
		  #endif
		}
		if (mSerFlags & (ser_Animations | ser_Motion) ) {
			SIZE_NON_ZERO_F(mCenterOffset.x, 8);
			SIZE_NON_ZERO_F(mCenterOffset.y, 9);
		}
		if (mSerFlags & ser_Motion) {
			SIZE_NON_ZERO_F(mDeltaXPerMs, 10);
			SIZE_NON_ZERO_F(mDeltaYPerMs, 11);
			SIZE_NON_ZERO_F(mDeltaWidthPerMs, 12);
			SIZE_NON_ZERO_F(mDeltaHeightPerMs, 13);
			SIZE_NON_ZERO_F(mDeltaFacingPerMs, 14);
		}
		SIZE_FLOAT_LIST_END(totalSize);

		if (mSerFlags & ser_Animations) {
		}

		if (mSerFlags & ser_Forces) {
//			totalSize += 4;  // 1 float: mGravity
		}
		if (mSerFlags & ser_Physics) {
		}

		if (mSerFlags & ser_InitialData) {
	//		mNextLayer(0), mPrevLayer(0), mFirstSprite(0), mLastSprite(0),
	//		mControlledBy(0),
		}

		sprite = mFirstSprite;
		count = 0;
		while (sprite) {
			count++;
			if (mSerFlags & ser_ZOrder) {
				totalSize += serializer->serializedSize(sprite->iid);
			}
		  	if (mSerFlags & ser_InitialData) {
				totalSize += serializer->serializedSize(sprite);
			} else {
				totalSize += sprite->getSerializedSize(serializer);
			}
			sprite = sprite->mNextSprite;
		}
		totalSize += serializer->serializedSize(count); // for count

	}
	return totalSize;
}


void SpriteLayer::serialize(ISerializer* serializer) const {
	Sprite* sprite = 0;
	uint32 count = 0;
	serializer->serialize_1u(PDG_SPRITE_LAYER_STREAM_VERSION);
	serializer->serialize_uint(mSerFlags);
	SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer [%p] vers [%d] flags [%p]", this, PDG_SPRITE_LAYER_STREAM_VERSION, mSerFlags); )
	if ( (mSerFlags == ser_Micro) || (mSerFlags == ser_Positions) ) {
		SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer [%p] writing Micro update", this); )
		// special case, smallest possible update, basically just the sprites
		// get and write the count
		sprite = mFirstSprite;
		while (sprite) {
			count++;
			sprite = sprite->mNextSprite;
		}
		serializer->serialize_uint(count);
		SERIALIZATION_DEBUG_ONLY( int i = 0; DEBUG_PRINT("  sprite count: %d", count); )
		// now write the minimal position data for the sprites
		sprite = mFirstSprite;
		while (sprite) {
			SERIALIZATION_DEBUG_ONLY( i++; DEBUG_PRINT("  %d: ", i); )
			if (mSerFlags & ser_ZOrder) {
				serializer->serialize_uint(sprite->iid);
				SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    iid : %d ", sprite->iid); )
			}
			sprite->serialize(serializer);
			sprite = sprite->mNextSprite;
		}
	} else {
		uint16 layerFlags = 
			(mHidden ? 			1 << 0 : 0) |
			(mAnimating ? 		1 << 1 : 0) |
			(mDoCollisions ? 	1 << 2 : 0) |
			(mWantsMouseOver ?	1 << 3 : 0) |
			(mWantsClicks ? 	1 << 4 : 0) |
	  #ifndef PDG_NO_GUI
			(mAutoCenter ? 		1 << 5 : 0) |
			(mFixedMoveAxis ? 	1 << 6 : 0) |
	  #endif
	  #ifdef PDG_USE_CHIPMUNK_PHYSICS
			(mKeepGravityDownward ? 1 << 7 : 0) |
			(mUseChipmunkPhysics ? 	1 << 8 : 0) |
			(mIsStaticLayer ? 		1 << 9 : 0) |
	  #endif
			0;
	  #ifdef PDG_TAG_SERIALIZED_DATA
		serializer->serialize_4(PDG_SPRITE_LAYER_MAGIC_NUMBER);
	  #endif
		serializer->serialize_2u(layerFlags);
		if (mSerFlags & ser_InitialData) {
			serializer->serialize_uint(iid);
			serializer->serialize_uint(layerId);
		}
		SERIALIZE_FLOAT_LIST_START(2, 15);
		if (mSerFlags & ser_Positions) {
			SERIALIZE_NON_ZERO_F(mLocation.x, 0);
			SERIALIZE_NON_ZERO_F(mLocation.y, 1);
			SERIALIZE_NON_ZERO_F(mFacing, 2);
		}
		if (mSerFlags & ser_Sizes) {
			SERIALIZE_NON_ZERO_F(mHeight, 3);
			SERIALIZE_NON_ZERO_F(mWidth, 4);
		}
		if (mSerFlags & ser_LayerDraw) {
		  #ifndef PDG_NO_GUI
			SERIALIZE_NON_ZERO_F(mOrigin.x, 5);
			SERIALIZE_NON_ZERO_F(mOrigin.y, 6);
			SERIALIZE_NON_ZERO_F(mZoom, 7);
		  #else
			SERIALIZE_NON_ZERO_F(0.0f, 5);  // write zeroes so this stream will be readable by non-gui builds
			SERIALIZE_NON_ZERO_F(0.0f, 6);
			SERIALIZE_NON_ZERO_F(0.0f, 7);
		  #endif
		}
		if (mSerFlags & (ser_Animations | ser_Motion) ) {
			SERIALIZE_NON_ZERO_F(mCenterOffset.x, 8);
			SERIALIZE_NON_ZERO_F(mCenterOffset.y, 9);
		}
		if (mSerFlags & ser_Motion) {
			SERIALIZE_NON_ZERO_F(mDeltaXPerMs, 10);
			SERIALIZE_NON_ZERO_F(mDeltaYPerMs, 11);
			SERIALIZE_NON_ZERO_F(mDeltaWidthPerMs, 12);
			SERIALIZE_NON_ZERO_F(mDeltaHeightPerMs, 13);
			SERIALIZE_NON_ZERO_F(mDeltaFacingPerMs, 14);
		}
		SERIALIZE_FLOAT_LIST_END(2);
		
		if (mSerFlags & ser_Animations) {
		}

		if (mSerFlags & ser_Forces) {
		  #ifdef PDG_USE_CHIPMUNK_PHYSICS
// 			serializer->serialize_f(mGravity);
		  #else
// 			serializer->serialize_f(0.0f);  // write zero so this stream will be readable by process built with Chipmunk Physics
		  #endif
		}
		sprite = mFirstSprite;
		while (sprite) {
			count++;
			sprite = sprite->mNextSprite;
		}
		serializer->serialize_uint(count);
		SERIALIZATION_DEBUG_ONLY( int i = 0; DEBUG_PRINT("  sprite count: %d", count); )
		// now write the update data for the sprites
		sprite = mFirstSprite;
		while (sprite) {
			SERIALIZATION_DEBUG_ONLY( i++; DEBUG_PRINT("  %d: ", i); )
			if (mSerFlags & ser_ZOrder) {
				serializer->serialize_uint(sprite->iid);
				SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    iid : %d ", sprite->iid); )
			}
		  	if (mSerFlags & ser_InitialData) {
				serializer->serialize_obj(sprite);
			} else {
				sprite->serialize(serializer);
			}
			sprite = sprite->mNextSprite;
		}

	}
}


void SpriteLayer::deserialize(IDeserializer* deserializer) {
	int streamVers = deserializer->deserialize_1u();
	STREAM_SAFETY_CHECK(streamVers <= PDG_SPRITE_LAYER_STREAM_VERSION, "OUT OF SYNC: newer version of Sprite Layer Stream", sync_error);
	uint32 serFlags = deserializer->deserialize_uint();
	uint32 oldSerFlags = mSerFlags;
	mSerFlags = serFlags;
	SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer [%p] reading vers [%d] flags [%p]", this, streamVers, serFlags); )
	Sprite* sprite = 0;
	uint32 count = 0;
	if ( (mSerFlags == ser_Micro) || (mSerFlags == ser_Positions) ) {
		SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer [%p] reading Micro update", this); )
		// special case, smallest possible update, basically just the sprites
		// get and write the count
		count = deserializer->deserialize_uint();
		// now write the minimal position data for the sprites
		SERIALIZATION_DEBUG_ONLY( int i = 0; DEBUG_PRINT("  sprite count: %d", count); )
		sprite = mFirstSprite;
		while (sprite && count) {
			SERIALIZATION_DEBUG_ONLY( i++; DEBUG_PRINT("  %d: (rem: %d)", i, count); )
			if (mSerFlags & ser_ZOrder) {
				uint32 siid = deserializer->deserialize_uint(); // get the sprite's internal id
				SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    iid : %d ", siid); )
				if (sprite->iid != siid) {
					SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    ** mismatch ** - swapping z-order with sprite %d", sprite->iid); )
					// they don't match, find the correct sprite and swap Z order
					Sprite* targetSprite = findSpriteByInternalId(siid);
					STREAM_SAFETY_CHECK(targetSprite != 0, "OUT OF SYNC: missing target sprite when changing z-order", unknown_object);
					if (targetSprite) {
						quickSwapSprites(sprite, targetSprite);
						sprite = targetSprite;
					}
				}
			}
			sprite->deserialize(deserializer);
			sprite = sprite->mNextSprite;
			count--;
		}
		STREAM_SAFETY_CHECK(sprite == 0, "OUT OF SYNC: more sprites in layer than were targeted", sync_error);
		STREAM_SAFETY_CHECK(count == 0, "OUT OF SYNC: more sprites in stream than in targeted layer", sync_error);
	} else {
	  #ifdef PDG_TAG_SERIALIZED_DATA
		uint32 tag = deserializer->deserialize_4();
		STREAM_SAFETY_CHECK(tag == PDG_SPRITE_LAYER_MAGIC_NUMBER, "OUT OF SYNC: expected tag for Sprite Layer object", bad_tag);
	  #endif
		uint32 layerFlags = deserializer->deserialize_2u();
		mHidden = ((layerFlags & 1 << 0) != 0);
		mAnimating = ((layerFlags & 1 << 1) != 0);
		mDoCollisions = ((layerFlags & 1 << 2) != 0);
		mWantsMouseOver = ((layerFlags & 1 << 3) != 0);
		mWantsClicks = ((layerFlags & 1 << 4) != 0);
	  #ifndef PDG_NO_GUI
		mAutoCenter = ((layerFlags & 1 << 5) != 0);
		mFixedMoveAxis = ((layerFlags & 1 << 6) != 0);
	  #endif
	  #ifdef PDG_USE_CHIPMUNK_PHYSICS
		mKeepGravityDownward = ((layerFlags & 1 << 7) != 0);
		mUseChipmunkPhysics = ((layerFlags & 1 << 8) != 0);
		mIsStaticLayer = ((layerFlags & 1 << 9) != 0);
	  #endif
		DESERIALIZE_FLOAT_LIST_START(2, 15);
		if (mSerFlags & ser_Positions) {
			mLocation.x = DESERIALIZE_NON_ZERO_F(0);
			mLocation.y = DESERIALIZE_NON_ZERO_F(1);
			mFacing = DESERIALIZE_NON_ZERO_F(2);
		}
		if (mSerFlags & ser_Sizes) {
			mHeight = DESERIALIZE_NON_ZERO_F(3);
			mWidth = DESERIALIZE_NON_ZERO_F(4);
		}
		if (mSerFlags & ser_LayerDraw) {
		  #ifndef PDG_NO_GUI
			mOrigin.x = DESERIALIZE_NON_ZERO_F(5);
			mOrigin.y = DESERIALIZE_NON_ZERO_F(6);
			mZoom = DESERIALIZE_NON_ZERO_F(7);
		  #else
			DESERIALIZE_NON_ZERO_F(5);  // mOrigin.x,  read in the values but ignore them
			DESERIALIZE_NON_ZERO_F(6);  // mOrigin.y
			DESERIALIZE_NON_ZERO_F(7);  // mZoom
		  #endif
		}
		if (mSerFlags & (ser_Animations | ser_Motion) ) {
			mCenterOffset.x = DESERIALIZE_NON_ZERO_F(8);
			mCenterOffset.y = DESERIALIZE_NON_ZERO_F(9);
		}
		if (mSerFlags & ser_Motion) {
			mDeltaXPerMs = DESERIALIZE_NON_ZERO_F(10);
			mDeltaYPerMs = DESERIALIZE_NON_ZERO_F(11);
			mDeltaWidthPerMs = DESERIALIZE_NON_ZERO_F(12);
			mDeltaHeightPerMs = DESERIALIZE_NON_ZERO_F(13);
			mDeltaFacingPerMs = DESERIALIZE_NON_ZERO_F(14);
		}
		DESERIALIZE_FLOAT_LIST_END(2);

// 		if (mSerFlags & ser_Animations) {
// 		}
// 
		count = deserializer->deserialize_uint();

		// now read the data for the sprites
		SERIALIZATION_DEBUG_ONLY( int i = 0; DEBUG_PRINT("  sprite count: %d", count); )

		sprite = mFirstSprite;
		while (sprite && count) {
			SERIALIZATION_DEBUG_ONLY( i++; DEBUG_PRINT("  %d: (rem: %d)", i, count); )
			if (mSerFlags & ser_ZOrder) {
				uint32 siid = deserializer->deserialize_uint(); // get the sprite's internal id
				SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    iid : %d ", siid); )
				if (sprite->iid != siid) {
					SERIALIZATION_DEBUG_ONLY( DEBUG_PRINT("    ** mismatch ** - swapping z-order with sprite %d", sprite->iid); )
					// they don't match, find the correct sprite and swap Z order
					Sprite* targetSprite = findSpriteByInternalId(siid);
					STREAM_SAFETY_CHECK(targetSprite != 0, "OUT OF SYNC: missing target sprite when changing z-order", unknown_object);
					if (targetSprite) {
						quickSwapSprites(sprite, targetSprite);
						sprite = targetSprite;
					}
				}
			}
		  	if (mSerFlags & ser_InitialData) {
		  		// FIXME, this won't work
				sprite = dynamic_cast<Sprite*>(deserializer->deserialize_obj());
			} else {
				sprite->deserialize(deserializer);
			}
			sprite = sprite->mNextSprite;
			count--;
		}
		STREAM_SAFETY_CHECK(sprite == 0, "OUT OF SYNC: more sprites in layer than were targeted", sync_error);
		STREAM_SAFETY_CHECK(count == 0, "OUT OF SYNC: more sprites in stream than in targeted layer", sync_error);

	}
	mSerFlags = oldSerFlags;
}

// start and stop animating all sprites
void	SpriteLayer::startAnimations() {
	mAnimating = true;
}


void	SpriteLayer::stopAnimations() {
	mAnimating = false;
}

// hide and show (make the layer visible or invisible within the port)
// if the layer is hidden it will not get any mouseover or click events
void	SpriteLayer::hide() {
	mHidden = true;
}


void	SpriteLayer::show() {
	mHidden = false;
}

bool	SpriteLayer::isHidden() {
	return mHidden;
}

void	SpriteLayer::fadeIn(int32 msDuration, EasingFunc easing) {
	mDoneFadingInAt = OS::getMilliseconds() + msDuration;
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		sprite->fadeIn(msDuration, easing);
		sprite = sprite->mNextSprite;
	}
}

void	SpriteLayer::fadeOut(int32 msDuration, EasingFunc easing) {
	mDoneFadingOutAt = OS::getMilliseconds() + msDuration;
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		sprite->fadeOut(msDuration, easing);
		sprite = sprite->mNextSprite;
	}
}

// arrange layers
void	SpriteLayer::moveBehind( SpriteLayer* inLayer) {
	SpriteLayer* layer = inLayer;
	SpriteManager::getSingletonInstance()->removeLayer(this);
	if (layer == 0) {
		// behind everything = in front of list
		layer = SpriteManager::getSingletonInstance()->mFirstLayer;
	}
	// update the layer before to point to us
	if (layer && layer->mPrevLayer) {
		layer->mPrevLayer->mNextLayer = this;
		this->mPrevLayer = layer->mPrevLayer;
	} else {
		SpriteManager::getSingletonInstance()->mFirstLayer = this;
		this->mPrevLayer = 0;
	}
	// update the layer after to point to us
	if (layer) {
		layer->mPrevLayer = this;
	} else {
		SpriteManager::getSingletonInstance()->mLastLayer = this;
	}
	mNextLayer = layer;
}


void	SpriteLayer::moveInFrontOf( SpriteLayer* inLayer) {
	SpriteLayer* layer = inLayer;
	SpriteManager::getSingletonInstance()->removeLayer(this);
	if (layer == 0) {
		// put after last layer
		layer = SpriteManager::getSingletonInstance()->mLastLayer;
	}
	// update the layer after to point to us
	if (layer && layer->mNextLayer) {
		layer->mNextLayer->mPrevLayer = this;
		this->mNextLayer = layer->mNextLayer;
	} else {
		SpriteManager::getSingletonInstance()->mLastLayer = this;
		this->mNextLayer = 0;
	}
	// update the layer before to point to us
	if (layer) {
		layer->mNextLayer = this;
	} else {
		SpriteManager::getSingletonInstance()->mFirstLayer = this;
	}
}

int SpriteLayer::getZOrder() {
	int z = 0;
	SpriteLayer* layer = SpriteManager::getSingletonInstance()->mFirstLayer;
	while (layer) {
		if (layer == this) {
			return z;
		} else {
			z++;
			layer = layer->mNextLayer;
		}
	}
	return -1; // this shouldn't ever happen, all layers should be in the SpriteManager
}

void	SpriteLayer::moveWith(SpriteLayer* layer, float moveRatio, float zoomRatio) {
	if (mControlledBy || (layer == 0) ) {
		return;	// already controlled
	}
	mControlledBy = layer;
	LinkedLayerInfo info;
	info.moveRatio = moveRatio;
	info.zoomRatio = zoomRatio;
	info.linkedLayer = this;
	layer->mLinkedLayers.push_back(info);
}


// add and remove sprites from the manager

// find a sprite in the layer by id. Be sure to addRef() the sprite if you hang onto the reference
Sprite*	SpriteLayer::findSprite(long id) {
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		if (sprite->spriteId == id) {
			return sprite;
		}
		sprite = sprite->mNextSprite;
	}
	return 0;
}

// find a sprite in the layer by internal id (PROTECTED)
Sprite*	SpriteLayer::findSpriteByInternalId(uint32 iid) const {
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		if (sprite->iid == iid) {
			return sprite;
		}
		sprite = sprite->mNextSprite;
	}
	return 0;
}

// get the nth sprite in the layer by z-order, 0 is the one furthest back
Sprite* SpriteLayer::getNthSprite(int index) {
	if (index < 0) return 0;
	Sprite* sprite = mFirstSprite;
	while (sprite && index) {
		sprite = sprite->mNextSprite;
		index--;
	}
	return sprite;
}

// z-order 0 is furthest back, increment from there
// return -1 if not in this layer
int SpriteLayer::getSpriteZOrder(Sprite* sprite) {
	int z = 0;
	Sprite* s = mFirstSprite;
	while (s) {
		if (s == sprite) {
			return z;
		} else {
			z++;
			s = s->mNextSprite;
		}
	}
	return -1;
}

// does otherSprite have higher z-order than sprite?
bool SpriteLayer::isSpriteBehind(Sprite* sprite, Sprite* otherSprite) {
	while (sprite) {
		if (sprite->mNextSprite == otherSprite) {
			return true;
		} else {
			sprite = sprite->mNextSprite;
		}
	}
	return false;
}

// see if a sprite is in this layer
bool SpriteLayer::hasSprite(Sprite* inSprite) {
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		if (inSprite == sprite) {
			return true;
		}
		sprite = sprite->mNextSprite;
	}
	return false;
}
	
// add a sprite to the end of the doubly-linked list, which makes it draw last
// in front of everything else
void	SpriteLayer::addSprite(Sprite* newSprite) {
	Sprite* sprite = newSprite;
	if (!sprite) return;
	if (sprite->mLayer) return; // don't add the sprite if it already belongs to a layer
	sprite->mNextSprite = 0;  // we always add to the end
	if (mFirstSprite == 0) {
		mFirstSprite = sprite;
		sprite->mPrevSprite = 0;
	} else {
		sprite->mPrevSprite = mLastSprite;
		mLastSprite->mNextSprite = sprite;
	}
	mLastSprite = sprite;
	sprite->mLayer = this;  // sprite now belongs to us
	sprite->addRef();
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (mUseChipmunkPhysics) {
		sprite->initCpBody();
	}
  #endif
  #ifndef PDG_NO_GUI
	sprite->setPort(mPort); // make sure it is drawing into our port
  #endif // ! PDG_NO_GUI
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Added Sprite [%p] to layer [%p]", sprite, this); )
}

// PROTECTED: swap z order of 2 sprites that are known to be in the same layer
void SpriteLayer::quickSwapSprites(Sprite* s1, Sprite* s2) {
	Sprite* tmp = s1->mNextSprite;
	s1->mNextSprite = s2->mNextSprite;
	s2->mNextSprite = tmp;
	tmp = s1->mPrevSprite;
	s1->mPrevSprite = s2->mPrevSprite;
	s2->mPrevSprite = tmp;
	if (s1->mPrevSprite == 0) {
		mFirstSprite = s1;
	} else {
		s1->mPrevSprite->mNextSprite = s1;
	}
	if (s2->mPrevSprite == 0) {
		mFirstSprite = s2;
	} else {
		s2->mPrevSprite->mNextSprite = s2;
	}
	if (s1->mNextSprite == 0) {
		mLastSprite = s1;
	} else {
		s1->mNextSprite->mPrevSprite = s1;
	}
	if (s2->mNextSprite == 0) {
		mLastSprite = s2;
	} else {
		s2->mNextSprite->mPrevSprite = s2;
	}
}

// add a sprite right after targetSprite in the doubly-linked list, which makes it draw
// just after that sprite and thus apparently in front of it.
void	SpriteLayer::addSpriteInFrontOf(Sprite* newSprite, Sprite* targetSprite) {
	Sprite* sprite = newSprite;
	if (!sprite) return;
	if (sprite->mLayer) return; // don't add the sprite if it already belongs to a layer
	if (!targetSprite) {
		// adding to the beginning of the list, in front of nothing
		// set new sprite
		sprite->mNextSprite = mFirstSprite;
		sprite->mPrevSprite = 0;
		// set first sprite
		if (mFirstSprite) {
			mFirstSprite->mPrevSprite = sprite;
		}
		mFirstSprite = sprite;
	} else {
		if (targetSprite->mLayer != this) return; // target must be in this layer
		// set new sprite
		sprite->mNextSprite = targetSprite->mNextSprite;
		sprite->mPrevSprite = targetSprite;
		if (targetSprite->mNextSprite) {
			// set sprite after target sprite
			targetSprite->mNextSprite->mPrevSprite = sprite;
		} else {
			// set last sprite
			mLastSprite = sprite;
		}
		// set target sprite
		targetSprite->mNextSprite = sprite;
	}
	sprite->mLayer = this;  // sprite now belongs to us
	sprite->addRef();
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (mUseChipmunkPhysics) {
		sprite->initCpBody();
	}
  #endif
  #ifndef PDG_NO_GUI
	sprite->setPort(mPort); // make sure it is drawing into our port
  #endif // ! PDG_NO_GUI
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Added Sprite [%p] to layer [%p]", sprite, this); )
}



void	SpriteLayer::removeSprite(Sprite* oldSprite) {
	Sprite* sprite = oldSprite;
	if (!sprite) return;
	if (sprite->mLayer != this) return; // don't remove the sprite unless it belongs to this layer
	// update our first and last layers
	if (sprite == mFirstSprite) {
		mFirstSprite = sprite->mNextSprite;
	}
	if (sprite == mLastSprite) {
		mLastSprite = sprite->mPrevSprite;
	}
	// now update the prev and next sprites to point to one another
	if (sprite->mPrevSprite) {
		sprite->mPrevSprite->mNextSprite = sprite->mNextSprite;
	}
	if (sprite->mNextSprite) {
		sprite->mNextSprite->mPrevSprite = sprite->mPrevSprite;
	}
	// finally, clear out prev and next sprites
	sprite->mNextSprite = 0;
	sprite->mPrevSprite = 0;
	sprite->mLayer = 0; // we are no longer in a layer
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Removed Sprite [%p] from layer [%p]", sprite, this); )
	sprite->release();
}

void SpriteLayer::removeAllSprites() {
	while (mFirstSprite) {
		removeSprite(mFirstSprite);
	}
}

#ifndef PDG_NO_GUI

void SpriteLayer::zoomChanged(float deltaZoom) {
    // move any layers we are controlling
    if (mLinkedLayers.size() > 0) {
		for (std::vector<LinkedLayerInfo>::iterator itr = mLinkedLayers.begin(); itr != mLinkedLayers.end(); itr++) {
			float targetZoom = (itr->linkedLayer->mZoom + deltaZoom) * itr->zoomRatio;
			itr->linkedLayer->setZoom(targetZoom);
		}
    }
}

// this is the port that the sprites in this manager will render into
// multiple managers can render into same port creating layers, drawn in order of creation
void    SpriteLayer::setSpritePort(Port* port) {
	mPort = port;
}

// zooming
void
SpriteLayer::setZoom(float zoomLevel) {
    float deltaZoom = zoomLevel - mZoom;
	mZoom = zoomLevel;
    zoomChanged(deltaZoom);
}

void
SpriteLayer::zoomTo(float zoomLevel, int32 msDuration, EasingFunc easing, 
					Rect keepInRect, const Point* centerOn) 
{
    int32 saveDelay = mDelayMs;
    if (centerOn != 0) {
        moveTo(*centerOn, msDuration, (zoomLevel < mZoom) ? easeOutExpo : easeInOutQuad);
    }
    mDelayMs = saveDelay;
	Animation a(&mZoom, zoomLevel, easing, mDelayMs, msDuration);
	mAnimations.push_back(a);
    mDelayMs = 0;
}


// coordinate conversions, adjusting for offset, zoom and rotation of layer
Point
SpriteLayer::layerToPort(const Point& p) const {
    Point a = layerToPort(Offset(p - mCenterOffset));  // rotate and zoom point to match layer
    // add in offset for layer's location
    a += (mLocation + mCenterOffset)*mZoom + mOrigin;
    return a;
}

Offset
SpriteLayer::layerToPort(const Offset& o) const {
    // rotate about layer center (0,0)
    Offset a(o.x*mFacingCos - o.y*mFacingSin, o.x*mFacingSin + o.y*mFacingCos);
    a *= mZoom;
    return a;
}

RotatedRect
SpriteLayer::layerToPort(const Rect& r) const {
    RotatedRect rr(r);
    Point cp = layerToPort(r.centerPoint());
    rr.center(Point(0,0));
    rr.top *= mZoom;
    rr.left *= mZoom;
    rr.right *= mZoom;
    rr.bottom *= mZoom;
    rr.center(cp);
    rr.radians = mFacing;
    return rr;
}

RotatedRect
SpriteLayer::layerToPort(const RotatedRect& r) const {
    RotatedRect rr = layerToPort(static_cast<const Rect&>(r));
    rr.centerOffset = layerToPort(r.centerOffset);
    rr.radians = r.radians + mFacing;
    return rr;
}

Quad
SpriteLayer::layerToPort(const Quad& q) const {
    return q;
    Quad nq;
    for (int i = 0; i<4; i++) {
        nq.points[i] = layerToPort(q.points[i]);
    }
    return nq;
}


Point
SpriteLayer::portToLayer(const Point& p) const {
    Point a = p - mOrigin;
    a -= (mLocation + mCenterOffset)/mZoom;
    Point b = portToLayer(Offset(a)) + mCenterOffset;
    return b;
}

Offset
SpriteLayer::portToLayer(const Offset& o) const {
    Offset a = o / mZoom;
    Point b(a.x*cos(-mFacing) - a.y*sin(-mFacing), a.x*sin(-mFacing) + a.y*cos(-mFacing));
    return b;
}
    
RotatedRect
SpriteLayer::portToLayer(const Rect& r) const {
    RotatedRect rr(r);
    Point cp = portToLayer(r.centerPoint());
    rr.center(cp);
    rr.top /= mZoom;
    rr.left /= mZoom;
    rr.right /= mZoom;
    rr.bottom /= mZoom;
    rr.radians = -mFacing;
    return rr;
}

RotatedRect
SpriteLayer::portToLayer(const RotatedRect& r) const {
    RotatedRect rr = portToLayer(static_cast<const Rect&>(r));
    rr.centerOffset = portToLayer(r.centerOffset);
    rr.radians = r.radians - mFacing;
    return rr;
}

Quad
SpriteLayer::portToLayer(const Quad& q) const {
    Quad nq;
    for (int i = 0; i<4; i++) {
        nq.points[i] = portToLayer(q.points[i]);
    }
    return nq;
}

#endif // ! PDG_NO_GUI



	
// collisions
void	SpriteLayer::enableCollisions() {
	mDoCollisions = true;
}


void	SpriteLayer::disableCollisions() {
	mDoCollisions = false;
}


void    SpriteLayer::collide(long msElapsed, SpriteLayer* withLayer, bool deferEvents)  {
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
  if (!mUseChipmunkPhysics) { // if we are compiled with chipmunk support, make sure we want it for this layer
  #endif
    // if we are using chipmunk physics it handles detecting collisions and we just
    // get callbacks to SpriteManager
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		Sprite* next = sprite->mNextSprite;
	    sprite->addRef(); // make sure this won't be deleted by being removed from the layer while we are working with it
		if (sprite->mDoCollisions) {
			Sprite* otherSprite = withLayer->mFirstSprite;
			while (otherSprite) {
				Sprite* nextOther = otherSprite->mNextSprite;
				if (otherSprite->mDoCollisions) {
					if (sprite->collidesWith(otherSprite)) {
                        Vector normal;
                        Vector impulse;
                        float kineticEnergy;
                        sprite->impartCollisionImpulse(otherSprite, normal, impulse, kineticEnergy);
                        float dt = ((float)msElapsed) / 1000.0f;
                        float force = impulse.vectorLength() / dt;
						notifyCollisionAction(Sprite::action_CollideSprite, sprite, normal, impulse, force, kineticEnergy, 
									  #ifdef PDG_USE_CHIPMUNK_PHYSICS
										0, // need something for the cpArbiter param
									  #endif // PDG_USE_CHIPMUNK_PHYSICS							
										otherSprite, !deferEvents);
					}
				}
				otherSprite = nextOther;
			}
		}
		sprite->release();
		sprite = next;
	}
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
  }
  #endif
}
	
#ifndef PDG_NO_GUI
void	SpriteLayer::drawLayer() {
	if (!mPort) return;	// can't draw without a port

	Sprite* sprite = mFirstSprite;
	while (sprite && !mHidden) {
		sprite->draw();
		sprite = sprite->mNextSprite;
	}

#ifdef SPRITELAYER_INTERNAL_DEBUG
    if (!mHidden) {
        Rect r(30, 30);
        r.center(Point(0,0));
        RotatedRect rr = layerToPort(r);
        rr.radians += PI/4;
        Quad q = rr.getQuad();
        mPort->drawLine(q.points[0], q.points[2], PDG_BLUE_COLOR);
        mPort->drawLine(q.points[1], q.points[3], PDG_BLUE_COLOR);
        r.setSize(20);
        r.center(mCenterOffset);
        rr = layerToPort(r);
        q = rr.getQuad();
        mPort->drawLine(q.points[0], q.points[2], PDG_RED_COLOR);
        mPort->drawLine(q.points[1], q.points[3], PDG_RED_COLOR);
        rr.radians += PI/4;
        q = rr.getQuad();
        mPort->drawLine(q.points[0], q.points[2], PDG_RED_COLOR);
        mPort->drawLine(q.points[1], q.points[3], PDG_RED_COLOR);
    }
#endif // SPRITELAYER_INTERNAL_DEBUG
}
#endif // ! PDG_NO_GUI

void
SpriteLayer::animateLayer(long msElapsed) {
	Animated::animate(msElapsed);

	mFacingCos = cos(mFacing); // cache these frequently used values
	mFacingSin = sin(mFacing);
	
	SpriteLayerInfo evntInfo;
	uint32 currMs = OS::getMilliseconds();
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Animating layer [%p] @ %ld", this, msElapsed); )

	if (mDoneFadingInAt && (currMs > mDoneFadingInAt)) {
		mDoneFadingInAt = 0;
		evntInfo.actingLayer = this;
		evntInfo.action = SpriteLayer::action_FadeInComplete;
		evntInfo.millisec = currMs;
		postEvent(eventType_SpriteLayer, &evntInfo);		
	}
	if (mDoneFadingOutAt && (currMs > mDoneFadingOutAt)) {
		mDoneFadingOutAt = 0;
		evntInfo.actingLayer = this;
		evntInfo.action = SpriteLayer::action_FadeOutComplete;
		evntInfo.millisec = currMs;
		postEvent(eventType_SpriteLayer, &evntInfo);		
	}
	Sprite* sprite = mFirstSprite;
	while (sprite && mAnimating) {
		sprite->doAnimate(msElapsed, mDoCollisions);
		sprite = sprite->mNextSprite;
	}
    // layer to layer collisions
    for (std::vector<SpriteLayer*>::iterator itr = mCollideLayers.begin(); itr != mCollideLayers.end(); itr++) {
        collide(msElapsed, *itr, true);
    }
}

void
SpriteLayer::locationChanged(const Offset& delta) {
  #ifndef PDG_NO_GUI
	if (!mControlledBy) {
		if (mAutoCenter) {
			if (mFixedMoveAxis) {
				Offset a(delta.x*cos(-mFacing) - delta.y*sin(-mFacing), delta.x*sin(-mFacing) + delta.y*cos(-mFacing));
				mLocation -= delta;
				mLocation += a;
				mCenterOffset -= a;
			} else {
				mCenterOffset -= delta;
			}
		} else if (mFixedMoveAxis) {
			// TODO This mode is broken, fix it
	        Offset a(delta.x*mFacingCos - delta.y*mFacingSin, delta.x*mFacingSin + delta.y*mFacingCos);
	        mLocation -= delta;
	        mLocation += a;
		}
    }
  #endif // !PDG_NO_GUI
    // move any layers we are controlling
    if (mLinkedLayers.size() > 0) {
		for (std::vector<LinkedLayerInfo>::iterator itr = mLinkedLayers.begin(); itr != mLinkedLayers.end(); itr++) {
			Offset targetDelta = delta * itr->moveRatio;
			itr->linkedLayer->move(targetDelta);
		}
    }
}

    
#ifndef PDG_NO_GUI
void    
SpriteLayer::easingCompleted(const Animation& a) {
	SpriteLayerInfo evntInfo;
    if (a.value == &mZoom) {
        // this is a zoom operation, so send the proper notification
        evntInfo.actingLayer = this;
        evntInfo.action = SpriteLayer::action_ZoomComplete;
        evntInfo.millisec = OS::getMilliseconds();
        postEvent(eventType_SpriteLayer, &evntInfo);
    }
}

void SpriteLayer::wantMouseOverEvents() {
	mWantsMouseOver = true;
}

void SpriteLayer::checkIfMouseOverEventsStillWanted() {
	Sprite* sprite = mFirstSprite;
	mWantsMouseOver = false;
	while (sprite) {
		if (sprite->wantsMouseOver) {
			mWantsMouseOver = true;
			break;
		}
		sprite = sprite->mNextSprite;
	}
}

void SpriteLayer::wantClickEvents() {
	mWantsClicks = true;
}

void SpriteLayer::checkIfClickEventsStillWanted() {
	Sprite* sprite = mFirstSprite;
	mWantsClicks = false;
	while (sprite) {
		if (sprite->wantsClicks) {
			mWantsClicks = true;
			break;
		}
		sprite = sprite->mNextSprite;
	}
}
#endif // ! PDG_NO_GUI

void SpriteLayer::notifyAnimationAction(int action, Sprite* actingSprite, bool sendImmediately) {
	SpriteAnimateInfo si;
	si.action = action;
	si.actingSprite = actingSprite;
	si.inLayer = this;
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Sprite [%p] -> anim event %d", actingSprite, action); )
  #ifndef PDG_NO_EVENT_QUEUE
	if (sendImmediately) {
  #endif
  		actingSprite->postEvent(eventType_SpriteAnimate, &si);
  #ifndef PDG_NO_EVENT_QUEUE
	} else {
		// if the events are to be deferred, then we will be retaining copies of the Sprite pointers in the SpriteAnimateInfo within the queue. 
		// We need to increment the refcount and make the User Data object call our custom SpriteAnimateInfo_ReleaseSprites function when
		// it goes to free the SpriteAnimateInfo object
		if (actingSprite) {
			actingSprite->addRef();
		}
		EventManager::getSingletonInstance()->enqueueEvent(eventType_SpriteAnimate, 
			UserData::makeUserDataViaCopy(&si, sizeof(SpriteAnimateInfo), &SpriteAnimateInfo_ReleaseSprites),
			actingSprite);
	}
  #endif
}

void SpriteLayer::notifyCollisionAction(int action, Sprite* actingSprite, Vector normal, Vector impulse, float force, float kineticEnergy, 
                                    #ifdef PDG_USE_CHIPMUNK_PHYSICS
                                        cpArbiter* arbiter,
                                    #endif
                                        Sprite* targetSprite, bool sendImmediately) {
	SpriteCollideInfo si;
	si.action = action;
	si.actingSprite = actingSprite;
	si.targetSprite = targetSprite;
    si.normal = normal;
    si.impulse = impulse;
    si.force = force;
    si.kineticEnergy = kineticEnergy;
	si.inLayer = this;
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
	si.arbiter = arbiter;
  #endif
  #ifndef PDG_NO_EVENT_QUEUE
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("Sprite [%p] -> collide event %d", actingSprite, action); )
	if (sendImmediately) {
  #endif
  		actingSprite->postEvent(eventType_SpriteCollide, &si);
  #ifndef PDG_NO_EVENT_QUEUE
	} else {
		// if the events are to be deferred, then we will be retaining copies of the Sprite pointers in the SpriteCollideInfo within the queue. 
		// We need to increment the refcount and make the User Data object call our custom SpriteCollideInfo_ReleaseSprites function when
		// it goes to free the SpriteCollideInfo object
		if (actingSprite) {
			actingSprite->addRef();
		}
		if (targetSprite) {
			targetSprite->addRef();
		}
		EventManager::getSingletonInstance()->enqueueEvent(eventType_SpriteCollide, 
			UserData::makeUserDataViaCopy(&si, sizeof(SpriteCollideInfo), &SpriteCollideInfo_ReleaseSprites),
			actingSprite);
	}
  #endif
}

void    
SpriteLayer::enableCollisionsWithLayer(SpriteLayer* otherLayer) {
    mCollideLayers.push_back(otherLayer);
}

void    
SpriteLayer::disableCollisionsWithLayer(SpriteLayer* otherLayer) {
    std::vector<SpriteLayer*>::iterator position = std::find(mCollideLayers.begin(), mCollideLayers.end(), otherLayer);
    if (position != mCollideLayers.end()) {
        mCollideLayers.erase(position);
    }
}

// create sprites
Sprite* SpriteLayer::createSprite() {
	Sprite* sprite = new Sprite();
	addSprite(sprite);
	return sprite;
}


Sprite* SpriteLayer::cloneSprite(const Sprite* originalSprite) {
//	Sprite* sprite =  new Sprite(originalSprite);
//	addSprite(sprite);
//	return sprite;
    return 0;
}

#ifdef PDG_SCML_SUPPORT

// protected method, loads an SCML file and creates a sprite for the named entity, or first entity if no name given
Sprite* SpriteLayer::createSCMLSprite(SCML::Data* scmlData, bool needLoad, const char* inEntityName, bool addAll) {
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer::createSCMLSprite [%s]", scmlData->name.c_str()); )
#ifdef PLATFORM_WIN32
	size_t len = scmlData->name.rfind('\\');
#else
	size_t len = scmlData->name.rfind('/');
#endif
	std::string dirPath(scmlData->name, 0, len);
	if (needLoad) {
		int resFileRef = ResourceManager::instance().openResourceFile(dirPath.c_str());
		gSCMLFileSystem.load(scmlData);
//		scmlData->log(10);  // this will do a complete dump of what was loaded
		ResourceManager::instance().closeResourceFile(resFileRef);
	}
	Sprite* result = 0;
	bool first = true;
    for (std::map<int, SCML::Data::Entity*>::iterator e = scmlData->entities.begin(); e != scmlData->entities.end(); e++) {
        if (addAll || (!inEntityName && first) || (inEntityName && std::strcmp(e->second->name.c_str(), inEntityName) == 0)) {
			SCML_pdg::Entity* entity = new SCML_pdg::Entity(scmlData, e->first);
			entity->setFileSystem(&gSCMLFileSystem);
		  #ifndef PDG_NO_GUI
			entity->setPort(mPort);
		  #endif
        	result = new Sprite();
		  	result->mEntity = entity;
			addSprite(result);
		  	if (!addAll) break;
        }
        first = false;
    }
	return result;
}

// protected method, loads an SCML file and creates sprites for all the entities
void SpriteLayer::createSCMLSprites(SCML::Data* scmlData) {
	createSCMLSprite(scmlData, true, 0, true);
}

Sprite* SpriteLayer::createSpriteFromSCML(const char* inSCML, const char* inEntityName) {
	// create a sprite from Spriter SCML data, optionally specifying which entity if there are several
	SCML::Data data;
	data.fromTextData(inSCML);
	// we are just being given the data, so any caching must be handled elsewhere
	return createSCMLSprite(&data, true, inEntityName);
}

// create a sprite from a Spriter SCML file, optionally specifying which entity if there are several
Sprite* SpriteLayer::createSpriteFromSCMLFile(const char* inFileName, const char* inEntityName) {
	std::string fullPath;
	if (os_isAbsolutePath(inFileName)) {
		fullPath = inFileName;
	} else {
		fullPath = OS::getApplicationResourceDirectory();
		fullPath += inFileName;
        fullPath = OS::makeCanonicalPath(fullPath.c_str());
      #ifndef PLATFORM_WIN32
		DEBUG_ASSERT(fullPath[0] == '/', "Full path doesn't start with '/'!");
	  #endif
	}
	SCML::Data* scmlData = 0;
	bool needLoad = false;
	// check if we already have this SCML data cached
	for (std::list<SCML::Data*>::iterator itr = mSCMLData.begin(); itr != mSCMLData.end(); itr++) {
		SCML::Data* dat = *itr;
		if (std::strlen(dat->name.c_str()) > 0 && dat->name == fullPath) {
			scmlData = dat;
			break;
		}
	}
	if (!scmlData) {
		// not cached, load it
		SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("SpriteLayer::createSpriteFromSCMLFile [%s] [%s]", inFileName, fullPath.c_str()); )
		scmlData = new SCML::Data(fullPath);
//		gSCMLFileSystem.load(scmlData);
		mSCMLData.push_back(scmlData);
		needLoad = true;
	}
	return createSCMLSprite(scmlData, needLoad, inEntityName);
}

// create a sprite from previously loaded data
// createSpriteLayerFromSCMLFile() and createSpriteFromSCMLFile() both
// cache their data for later use by this call
Sprite* SpriteLayer::createSpriteFromSCMLEntity(const char* inEntityName) {
	Sprite* result = 0;
	for (std::list<SCML::Data*>::iterator itr = mSCMLData.begin(); itr != mSCMLData.end(); itr++) {
		SCML::Data* scmlData = *itr;
		result = createSCMLSprite(scmlData, false, inEntityName);
		if (result) {
			return result;
		}
	}
	return result;
}

#endif // PDG_SCML_SUPPORT

    
#ifdef PDG_USE_CHIPMUNK_PHYSICS
cpSpace*
SpriteLayer::getSpace() {
	if (!mUseChipmunkPhysics) return 0;
    return SpriteManager::getSingletonInstance()->mSpace;
}

void
SpriteLayer::setGravity(float gravity, bool keepItDownward) {
	if (!mUseChipmunkPhysics) return;
    mGravity = gravity;
    mKeepGravityDownward = keepItDownward;
    if (mKeepGravityDownward) {
        cpSpaceSetGravity(getSpace(), cpv(mGravity * mFacingSin, mGravity * mFacingCos));
    } else {
        cpSpaceSetGravity(getSpace(), cpv(0, mGravity));
    }
}

void
SpriteLayer::setKeepGravityDownward(bool keepItDownward) {
	if (!mUseChipmunkPhysics) return;
    setGravity(mGravity, keepItDownward);
}

void
SpriteLayer::setDamping(float damping) {
	if (!mUseChipmunkPhysics) return;
    cpSpaceSetDamping(getSpace(), damping);
}
#endif // PDG_USE_CHIPMUNK_PHYSICS

void	
SpriteLayer::rotationChanged(float deltaRadians) {
    // rotate any layers we are controlling
    if (mLinkedLayers.size() > 0) {
		for (std::vector<LinkedLayerInfo>::iterator itr = mLinkedLayers.begin(); itr != mLinkedLayers.end(); itr++) {
			itr->linkedLayer->rotate(deltaRadians);
		}
    }
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    // do this anytime we change rotation
    if (mUseChipmunkPhysics && mKeepGravityDownward) {
        cpSpaceSetGravity(getSpace(), cpv(mGravity * mFacingSin, mGravity * mFacingCos));
    }
  #endif //PDG_USE_CHIPMUNK_PHYSICS
}

#ifndef PDG_NO_GUI
SpriteLayer::SpriteLayer(Port* port): 
    noZoom(1.0f),
	mPort(port), mOrigin(0,0), 
	mHidden(false), mAnimating(true), mDoCollisions(false), 
	mWantsMouseOver(false), mWantsClicks(false),
	mZoom(1.0), // mTargetZoom(1.0), mDeltaZoomPerMs(0.0),
    mAutoCenter(false), mFixedMoveAxis(true), 
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    mGravity(0.0), mKeepGravityDownward(false), mUseChipmunkPhysics(false), mIsStaticLayer(false),
  #endif
	mNextLayer(0), mPrevLayer(0), mFirstSprite(0), mLastSprite(0),
	mControlledBy(0),
	mFacingCos(1.0), mFacingSin(0.0),
	mSerFlags(ser_Full),
	iid(sUniqueLayerId++)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mSpriteLayerScriptObj);
#endif
    layerId = gNextLayerId++;
}
#endif // ! PDG_NO_GUI

SpriteLayer::SpriteLayer(): 
    noZoom(1.0f),
  #ifndef PDG_NO_GUI
	mPort(0), mOrigin(0,0), 
  #endif // ! PDG_NO_GUI
	mHidden(false), mAnimating(true), mDoCollisions(false), 
  #ifndef PDG_NO_GUI
	mWantsMouseOver(false), mWantsClicks(false),
	mZoom(1.0), // mTargetZoom(1.0), mDeltaZoomPerMs(0.0),
    mAutoCenter(false), mFixedMoveAxis(true), 
  #endif // ! PDG_NO_GUI
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    mGravity(0.0), mKeepGravityDownward(false), mUseChipmunkPhysics(false), mIsStaticLayer(false),
  #endif
	mNextLayer(0), mPrevLayer(0), mFirstSprite(0), mLastSprite(0),
	mControlledBy(0),
	mFacingCos(1.0), mFacingSin(0.0),
	mSerFlags(ser_Full),
	iid(sUniqueLayerId++)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mSpriteLayerScriptObj);
#endif
    layerId = gNextLayerId++;
}

SpriteLayer::~SpriteLayer() {
	SPRITELAYER_DEBUG_ONLY( OS::_DOUT("dt SpriteLayer %p", this); )
	Sprite* sprite = mFirstSprite;
	while (sprite) {
		Sprite* next = sprite->mNextSprite;
		sprite->release();
		sprite = next;
	}
  #ifdef PDG_SCML_SUPPORT
	// iterate through and delete all the cached SCML Data
	SPRITELAYER_DEBUG_ONLY( DEBUG_PRINT("about to delete %d SCML::Data pointer(s) from list", mSCMLData.size()); )
	for (std::list<SCML::Data*>::iterator itr = mSCMLData.begin(); itr != mSCMLData.end(); itr++) {
		SCML::Data* scmlData = *itr;
		delete scmlData;
	}
  #endif // PDG_SCML_SUPPORT
	SpriteManager::getSingletonInstance()->removeLayer(this);
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupSpriteLayerScriptObject(mSpriteLayerScriptObj);
  #endif
}

#ifndef PDG_NO_GUI
SpriteLayer* createSpriteLayer(Port* port) {
	// create sprite manager singleton instance if necessary
	SpriteLayer* layer = SpriteManager::createSpriteLayer(port);
	SpriteManager::getSingletonInstance()->addLayer(layer);
	return layer;
}
#else
SpriteLayer* createSpriteLayer() {
	// create sprite manager singleton instance if necessary
	SpriteLayer* layer = SpriteManager::createSpriteLayer();
	SpriteManager::getSingletonInstance()->addLayer(layer);
	return layer;
}
#endif // ! PDG_NO_GUI

#ifndef PDG_NO_GUI
SpriteLayer* createSpriteLayerFromSCMLFile(const char* layerSCMLFile, bool addSprites, Port* port) {
	// create sprite manager singleton instance if necessary
	SpriteLayer* layer = SpriteManager::createSpriteLayer(port);
	SpriteManager::getSingletonInstance()->addLayer(layer);
	std::string fullPath;
	if (layerSCMLFile[0] == '/') {
		fullPath = layerSCMLFile;
	} else {
		fullPath = OS::getApplicationResourceDirectory();
		fullPath += layerSCMLFile;
        fullPath = OS::makeCanonicalPath(fullPath.c_str());

		DEBUG_ASSERT(fullPath[0] == '/', "Full path doesn't start with '/'!");
	}
	// this can't possibly be cached already, we just created the sprite layer
	SCML::Data* scmlData = new SCML::Data(fullPath);
//	gSCMLFileSystem.load(scmlData);
	layer->mSCMLData.push_back(scmlData);
	layer->createSCMLSprites(scmlData);
	return layer;
}
#else
SpriteLayer* createSpriteLayerFromSCMLFile(const char* layerSCMLFile, bool addSprites) {
	// create sprite manager singleton instance if necessary
	SpriteLayer* layer = SpriteManager::createSpriteLayer();
	SpriteManager::getSingletonInstance()->addLayer(layer);
	std::string fullPath;
	if (layerSCMLFile[0] == '/') {
		fullPath = layerSCMLFile;
	} else {
		fullPath = OS::getApplicationResourceDirectory();
		fullPath += layerSCMLFile;
        fullPath = OS::makeCanonicalPath(fullPath.c_str());

		DEBUG_ASSERT(fullPath[0] == '/', "Full path doesn't start with '/'!");
	}
	// this can't possibly be cached already, we just created the sprite layer
	SCML::Data* scmlData = new SCML::Data(fullPath);
//	gSCMLFileSystem.load(scmlData);
	layer->mSCMLData.push_back(scmlData);
	layer->createSCMLSprites(scmlData);
	return layer;
}
#endif // ! PDG_NO_GUI

void cleanupSpriteLayer(SpriteLayer* layer) {
	SPRITELAYER_DEBUG_ONLY( OS::_DOUT("cleanupSpriteLayer %p", layer); )
	if (layer) {
		SpriteManager::getSingletonInstance()->removeLayer(layer);
		SpriteManager::cleanupLayer(layer);
	}
}

SpriteLayer* SpriteLayer::getLayer(long id) {
	SpriteLayer* layer = SpriteManager::getSingletonInstance()->mFirstLayer;
	while (layer && (id != layer->layerId)) {
		layer = layer->mNextLayer;
	}
	return layer;
}

	
} // end namespace pdg
