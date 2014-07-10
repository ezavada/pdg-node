// -----------------------------------------------
// sprite.cpp
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


#include "pdg_project.h"

#include "pdg/sys/sprite.h"
#include "pdg/sys/spritelayer.h"
#include "pdg/sys/os.h"
#include "pdg/sys/events.h"
#include "pdg/sys/image.h"
#include "pdg/sys/imagestrip.h"
#include "pdg/sys/iserializer.h"
#include "pdg/sys/ideserializer.h"

#include "image-impl.h"

#ifdef PDG_SCML_SUPPORT
#include "SCML_pdg.h"
#endif

#ifndef PDG_NO_GUI
  #include "pdg/sys/port.h"
#endif

#include "collisiondetection.h"
#include "spritemanager.h"

#include <cmath>
#include <limits>

#ifdef PDG_USE_CHIPMUNK_PHYSICS
  #define USE_CHIPMUNK (mLayer && mLayer->mUseChipmunkPhysics)
  #define CONSTRAINT_IS_CLASS(constraint, _klass)  constraint->CP_PRIVATE(klass) == cp##_klass##GetClass()
#else
  #define USE_CHIPMUNK false
#endif

// #ifdef DEBUG
//   #define SPRITE_INTERNAL_DEBUG 1
//   #define SPRITE_MOTION_DEBUG 1
//   #define SPRITE_DEBUG_COLLISIONS 1
//	#define SPRITEANIMATE_DEBUG_ONLY( ops ) ops
// #endif

#ifndef SPRITEANIMATE_DEBUG_ONLY
  #define SPRITEANIMATE_DEBUG_ONLY( ops )
#endif

#ifndef PI
#define PI       3.141592        /* the venerable pi */
#endif
#ifndef M_PI
#define M_PI PI
#endif
#ifndef TWO_PI
#define TWO_PI    6.283184      /* handy for dealing with circles */
#endif
#ifndef MIN_INT16
#define MIN_INT16 -32767
#endif
#ifndef MAX_INT16
#define MAX_INT16 32767
#endif

#define BREAK_COEFFICIENT  0.9f  // multiplier to get breaking force from max force on a joint

#define PDG_SPRITE_MAGIC_NUMBER    		0x45258805

#ifndef PDG_UNSAFE_SERIALIZATION
#define PDG_TAG_SERIALIZED_DATA
#endif


namespace pdg {

// list of variables in a sprite that can be animated
enum {
	var_None = 0,
	var_LocX, var_LocY,
	var_DeltaX, var_DeltaY,
	var_Width, var_Height,
	var_Facing,
	var_CenterOffsetX, var_CenterOffsetY,
	var_Opacity
};

static uint32 sUniqueSpriteId = 1;

#ifdef PDG_USE_CHIPMUNK_PHYSICS
uint8 getJointTypeId(cpConstraint* constraint);
cpConstraint* allocConstraintByJointTypeId(uint8 jointTypeId);
Point convertPoint(const cpVect& v);
cpVect convertPoint(const Point& p);

enum { 
	PinJointId = 1, SlideJointId = 2, PivotJointId = 3, GrooveJointId = 4, DampedSpringId = 5,
	DampedRotarySpringId = 6, RotaryLimitJointId = 7, RatchetJointId = 8, GearJointId = 9, 
	SimpleMotorId = 10
};
	
uint8 getJointTypeId(cpConstraint* constraint) {
	if (CONSTRAINT_IS_CLASS(constraint, PinJoint) ) {
		return PinJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, SlideJoint) ) {
		return SlideJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, PivotJoint) ) {
		return PivotJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, GrooveJoint) ) {
		return GrooveJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, DampedSpring) ) {
		return DampedSpringId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, DampedRotarySpring) ) {
		return DampedRotarySpringId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, RotaryLimitJoint) ) {
		return RotaryLimitJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, RatchetJoint) ) {
		return RatchetJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, GearJoint) ) {
		return GearJointId;
	}
	if (CONSTRAINT_IS_CLASS(constraint, SimpleMotor) ) {
		return SimpleMotorId;
	}
	return 0;
}
cpConstraint* allocConstraintByJointTypeId(uint8 jointTypeId) {
	if (jointTypeId == PinJointId) {
		return (cpConstraint*)cpPinJointAlloc();
	}
	if (jointTypeId == SlideJointId) {
		return (cpConstraint*)cpSlideJointAlloc();
	}
	if (jointTypeId == PivotJointId) {
		return (cpConstraint*)cpPivotJointAlloc();
	}
	if (jointTypeId == GrooveJointId) {
		return (cpConstraint*)cpGrooveJointAlloc();
	}
	if (jointTypeId == DampedSpringId) {
		return (cpConstraint*)cpDampedSpringAlloc();
	}
	if (jointTypeId == DampedRotarySpringId) {
		return (cpConstraint*)cpDampedRotarySpringAlloc();
	}
	if (jointTypeId == RotaryLimitJointId) {
		return (cpConstraint*)cpRotaryLimitJointAlloc();
	}
	if (jointTypeId == RatchetJointId) {
		return (cpConstraint*)cpRatchetJointAlloc();
	}
	if (jointTypeId == GearJointId) {
		return (cpConstraint*)cpGearJointAlloc();
	}
	if (jointTypeId == SimpleMotorId) {
		return (cpConstraint*)cpSimpleMotorAlloc();
	}
	return 0;
}

inline Point convertPoint(const cpVect& v) {
	return Point(v.x, v.y);
}

inline cpVect convertPoint(const Point& p) {
	return cpv(p.x, p.y);
}

#endif // PDG_USE_CHIPMUNK_PHYSICS

uint32 Sprite::getSerializedSize(ISerializer* serializer) const {
	uint32 totalSize = 0;
	uint32 serFlags = (mLayer) ? mLayer->mSerFlags : (uint32)ser_Full;
	if ( (serFlags == ser_Micro) || (serFlags == ser_Positions) ) {

		// special case, smallest possible update
		totalSize += 9; // pos as 2x 2 byte shorts, facing as 4 byte float, frame number as uint8

	} else {

	  #ifdef PDG_TAG_SERIALIZED_DATA
		totalSize += 4;  // size of sprite magic number
	  #endif
		totalSize += 2; // size of spriteFlags
		if (serFlags & ser_InitialData) {
			totalSize += serializer->serializedSize((uint32)iid);
			totalSize += serializer->serializedSize((uint32)spriteId);
			totalSize += serializer->serializedSize((uint32)mMouseDetectMode);
		}
		if (serFlags & (ser_InitialData | ser_Animations) ) {
			totalSize += 1;		
		}
		SIZE_FLOAT_LIST_START(2, 16);
		if (serFlags & ser_Positions) {
			SIZE_NON_ZERO_F(mLocation.x, 0);
			SIZE_NON_ZERO_F(mLocation.y, 1);
			SIZE_NON_ZERO_F(mFacing, 2);
		}
		if (serFlags & ser_Sizes) {
			SIZE_NON_ZERO_F(mHeight, 3);
			SIZE_NON_ZERO_F(mWidth, 4);
		}
		if (serFlags & (ser_Animations | ser_Motion) ) {
			SIZE_NON_ZERO_F(mCenterOffset.x, 5);
			SIZE_NON_ZERO_F(mCenterOffset.y, 6);
		}
		if (serFlags & ser_Motion) {
			SIZE_NON_ZERO_F(mDeltaXPerMs, 7);
			SIZE_NON_ZERO_F(mDeltaYPerMs, 8);
			SIZE_NON_ZERO_F(mDeltaWidthPerMs, 9);
			SIZE_NON_ZERO_F(mDeltaHeightPerMs, 10);
			SIZE_NON_ZERO_F(mDeltaFacingPerMs, 11);
		}
		if ( (serFlags & ser_InitialData) && (serFlags & (ser_Forces | ser_Physics)) ) {
			SIZE_NON_ZERO_F(mMass, 12);
			SIZE_NON_ZERO_F(mMoveFriction, 13);
			SIZE_NON_ZERO_F(mSpinFriction, 14);
			SIZE_NON_ZERO_F(mSizeFriction, 15);
		}
		SIZE_FLOAT_LIST_END(totalSize);
		if (serFlags & ser_Animations) {
			if (serFlags & ser_InitialData) {
				totalSize += 5;
				for (int i = 0; i < mNumFrames; i++) {
					if (serFlags & ser_ImageRefs) {
						// not sure yet how to handle images -- filename?
						//	totalSize += serializer->serializedSize(mFrames[i].image);
						//	totalSize += serializer->serializedSize(mFrames[i].collisionMask);
					} else if (serFlags & ser_ImageData) {
						// send all the data for the image, probably need to make ImageImpl serializable
					}
					totalSize += 8; // mFrames[i].center;
					totalSize += serializer->serializedSize((uint32)mFrames[i].imageFrameNum);
					totalSize += 4; // mFrames[i].centerOffsetX, mFrames[i].centerOffsetY);
				}
			}
			totalSize += 4; // 1 byte each for mCurrFrame, mFirstFrame, mLastFrame, mFadeCompleteAction
			uint32 numAnims = mAnimations.size();
			totalSize += serializer->serializedSize(numAnims);
			for (uint32 i = 0; i < numAnims; i++) {
				totalSize += serializer->serializedSize((uint32)mAnimations[i].delayMs);
				totalSize += 6; // easingId, variableId, anim.targetVal
				totalSize += serializer->serializedSize(mAnimations[i].durationMs);
				if (mAnimations[i].delayMs <= 0) {
					// these are only sent if animation is actually running
					totalSize += serializer->serializedSize(mAnimations[i].currMs);
					totalSize += 8; // anim.beginVal, anim.deltaVal
				}
			}
		}
//   #ifdef PDG_SCML_SUPPORT
// 	SCML_pdg::Entity*		mEntity;
// 	float					mEntityScaleX;
// 	float					mEntityScaleY;
//   #endif // PDG_SCML_SUPPORT
		if (serFlags & ser_Forces) {
			uint32 numForces = mForces.size();
			totalSize += serializer->serializedSize(numForces);
			for (uint32 i = 0; i < numForces; i++) {
				totalSize += serializer->serializedSize((uint32)mForces[i].delayRemaining);
				totalSize += serializer->serializedSize((uint32)mForces[i].milliRemaining);
				SIZE_FLOAT_LIST_START(1, 3);
				SIZE_NON_ZERO_F(mForces[i].xAccelerationPerMs2, 0);
				SIZE_NON_ZERO_F(mForces[i].yAccelerationPerMs2, 1);
				SIZE_NON_ZERO_F(mForces[i].radianAccelerationPerMs2, 2);
				SIZE_FLOAT_LIST_END(totalSize);
			}
		}
		if (serFlags & ser_Physics) {
			if (serFlags & ser_InitialData) {
				totalSize += serializer->serializedSize((uint32)mCollideGroup) + 4;  // mElasticity float
			}
			totalSize += serializer->serializedSize((uint32)mDoCollisions);
			if (mDoCollisions == collide_CollisionRadius) {
				totalSize += 4;  // mCollisionRadius
			};
          #ifndef PDG_USE_CHIPMUNK_PHYSICS
          	totalSize += serializer->serializedSize((uint32)0);
          #else
          	totalSize += serializer->serializedSize((uint32)mNumBreakableJoints);
			for (int i = 0; i < mNumBreakableJoints; i++) {
				cpConstraint* constraint = mBreakableJoints[i];
				uint8 jti = getJointTypeId(constraint);
				cpBody* b = cpConstraintGetB(constraint);
                Sprite* otherSprite = (Sprite*) cpBodyGetUserData(b);
                uint32 siid = otherSprite->iid;
				cpFloat maxForce = cpConstraintGetMaxForce(constraint);  // inf
				cpFloat errorBias = cpConstraintGetErrorBias(constraint);  // 0.1
				cpFloat maxBias = cpConstraintGetMaxBias(constraint); // inf
				bool hasMaxForce = (maxForce != (cpFloat)INFINITY);
				bool hasErrorBias = (errorBias != (cpFloat)0.1f);
				bool hasMaxBias = (maxBias != (cpFloat)INFINITY);		
				totalSize += 1 + serializer->serializedSize(siid);  // joint type id
				totalSize += serializer->serializedSize(hasMaxForce);
				totalSize += serializer->serializedSize(hasErrorBias);
				totalSize += serializer->serializedSize(hasMaxBias);
				if (hasMaxForce) {
					totalSize += 4; // maxForce)
				}
				if (hasErrorBias) {
					totalSize += 4; // errorBias)
				}
				if (hasMaxBias) {
					totalSize += 4; // maxBias
				}
				if ( jti == PinJointId ) {
					totalSize += serializer->serializedSize(convertPoint(cpPinJointGetAnchr1(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpPinJointGetAnchr2(constraint)));
					totalSize += 4; // Dist
				}
				if ( jti == SlideJointId ) {
					totalSize += serializer->serializedSize(convertPoint(cpSlideJointGetAnchr1(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpSlideJointGetAnchr2(constraint)));
					totalSize += 8; // Min, Max
				}
				if ( jti == PivotJointId ) {
					totalSize += serializer->serializedSize(convertPoint(cpPivotJointGetAnchr1(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpPivotJointGetAnchr2(constraint)));
				}
				if ( jti == GrooveJointId ) {
					totalSize += serializer->serializedSize(convertPoint(cpGrooveJointGetAnchr2(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpGrooveJointGetGrooveA(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpGrooveJointGetGrooveB(constraint)));
				}
				if ( jti == DampedSpringId ) {
					totalSize += serializer->serializedSize(convertPoint(cpDampedSpringGetAnchr1(constraint)));
					totalSize += serializer->serializedSize(convertPoint(cpDampedSpringGetAnchr2(constraint)));
					totalSize += 12; // RestLength, Stiffness, Damping
				}
				if ( jti == DampedRotarySpringId ) {
					totalSize += 12; // RestAngle, Stiffness, Damping
				}
				if ( jti == RotaryLimitJointId ) {
					totalSize += 8; // Min, Max
				}
				if ( jti == RatchetJointId ) {
					totalSize += 12; // Angle, Phase, Ratchet
				}
				if ( jti == GearJointId ) {
					totalSize += 8; // Phase, Ratio
				}
				if ( jti == SimpleMotorId ) {
					totalSize += 4; // Rate
				}
			}
          #endif // PDG_USE_CHIPMUNK_PHYSICS
		}

	}
	return totalSize;
}

void Sprite::serialize(ISerializer* serializer) const {
	uint32 serFlags = (mLayer) ? mLayer->mSerFlags : (uint32)ser_Full;
	if ( (serFlags == ser_Micro) || (serFlags == ser_Positions) ) {

		// special case, smallest possible update
		int16 x = (mLocation.x < MIN_INT16) ? MIN_INT16 : ((mLocation.x > MAX_INT16) ? MAX_INT16 : (int16)mLocation.x);
		int16 y = (mLocation.y < MIN_INT16) ? MIN_INT16 : ((mLocation.y > MAX_INT16) ? MAX_INT16 : (int16)mLocation.y);
		serializer->serialize_2(x);
		serializer->serialize_2(y);
		serializer->serialize_f(mFacing);
		serializer->serialize_1u(mCurrFrame);

	} else {

	  #ifdef PDG_TAG_SERIALIZED_DATA
		serializer->serialize_4(PDG_SPRITE_MAGIC_NUMBER);
	  #endif
	  	uint32 spriteFlags = 
			(wantsOffscreen ? 		1 << 0 : 0) |
			(mAnimating ? 			1 << 1 : 0) |   // from Animated
			(wantsWallCollide ? 	1 << 2 : 0) |
			(wantsMouseOver ?		1 << 3 : 0) |
			(wantsClicks ? 			1 << 4 : 0) |
			(wantsAnimLoop ? 		1 << 5 : 0) |
			(wantsAnimEnd ? 		1 << 6 : 0) |
			(mLoopAnim ? 			1 << 7 : 0) |
			(mBackToFrontAnim ? 	1 << 8 : 0) |
	  #ifdef PDG_USE_CHIPMUNK_PHYSICS
			(mStatic ? 				1 << 9 : 0) |
	  #endif
			(mBidirectionalAnim ? 	1 << 10 : 0) |
			(mSpriteAnimating ? 	1 << 11 : 0) |
			(mSpriteAnimatingBackwardsNow ? 1 << 12 : 0) |
			(mOnscreen ? 			1 << 13 : 0) |
			(mInBounds ? 			1 << 14 : 0) |
			(mCompletelyInBounds ? 	1 << 15 : 0) |
			0;
	  	serializer->serialize_2u(spriteFlags);
		if (serFlags & ser_InitialData) {
			serializer->serialize_uint(iid);
			serializer->serialize_uint(spriteId);
			serializer->serialize_uint(mMouseDetectMode);
		}
		if (serFlags & (ser_InitialData | ser_Animations) ) {
			uint8 opacity = (mOpacity * 255);
			serializer->serialize_1u(opacity);		
		}
		SERIALIZE_FLOAT_LIST_START(2, 16);
		if (serFlags & ser_Positions) {
			SERIALIZE_NON_ZERO_F(mLocation.x, 0);
			SERIALIZE_NON_ZERO_F(mLocation.y, 1);
			SERIALIZE_NON_ZERO_F(mFacing, 2);
		}
		if (serFlags & ser_Sizes) {
			SERIALIZE_NON_ZERO_F(mHeight, 3);
			SERIALIZE_NON_ZERO_F(mWidth, 4);
		}
		if (serFlags & (ser_Animations | ser_Motion) ) {
			SERIALIZE_NON_ZERO_F(mCenterOffset.x, 5);
			SERIALIZE_NON_ZERO_F(mCenterOffset.y, 6);
		}
		if (serFlags & ser_Motion) {
			SERIALIZE_NON_ZERO_F(mDeltaXPerMs, 7);
			SERIALIZE_NON_ZERO_F(mDeltaYPerMs, 8);
			SERIALIZE_NON_ZERO_F(mDeltaWidthPerMs, 9);
			SERIALIZE_NON_ZERO_F(mDeltaHeightPerMs, 10);
			SERIALIZE_NON_ZERO_F(mDeltaFacingPerMs, 11);
		}
		if ( (serFlags & ser_InitialData) && (serFlags & (ser_Forces | ser_Physics)) ) {
			SERIALIZE_NON_ZERO_F(mMass, 12);
			SERIALIZE_NON_ZERO_F(mMoveFriction, 13);
			SERIALIZE_NON_ZERO_F(mSpinFriction, 14);
			SERIALIZE_NON_ZERO_F(mSizeFriction, 15);
		}
		SERIALIZE_FLOAT_LIST_END(2);
		if (serFlags & ser_Animations) {
			if (serFlags & ser_InitialData) {
				serializer->serialize_f(mFps);
				serializer->serialize_1u(mNumFrames);
				for (int i = 0; i < mNumFrames; i++) {
					if (serFlags & ser_ImageRefs) {
						// not sure yet how to handle images -- filename?
						//	serializer->serialize_str(mFrames[i].image->filename);
						//	serializer->serialize_str(mFrames[i].collisionMask->filename);
					} else if (serFlags & ser_ImageData) {
						// send all the data for the image, probably need to make ImageImpl serializable
						//	serializer->serialize_obj(mFrames[i].image);
						//	serializer->serialize_obj(mFrames[i].collisionMask);
					}
					serializer->serialize_f(mFrames[i].center.x);
					serializer->serialize_f(mFrames[i].center.y);
					serializer->serialize_uint(mFrames[i].imageFrameNum);
					serializer->serialize_2(mFrames[i].centerOffsetX);
					serializer->serialize_2(mFrames[i].centerOffsetY);
				}
			}
			serializer->serialize_1u(mCurrFrame);
			serializer->serialize_1u(mFirstFrame);
			serializer->serialize_1u(mLastFrame);
			serializer->serialize_1(mFadeCompleteAction);
			// serializer->serialize_uint(mDelayMs);  don't actually send this, it is only used for building series of animations
			uint32 numAnims = mAnimations.size();
			serializer->serialize_uint(numAnims);
			for (uint32 i = 0; i < numAnims; i++) {
				Animation anim = mAnimations[i];
				uint8 easingId = easingFuncToId(anim.easing);
				uint8 variableId = 0; // figure out which variable base on what value* is pointing at and convert to an id
				if (anim.value == &mLocation.x) {
					variableId = var_LocX;
				} else if (anim.value == &mLocation.y) {
					variableId = var_LocY;
				} else if (anim.value == &mDeltaXPerMs) {
					variableId = var_DeltaX;
				} else if (anim.value == &mDeltaYPerMs) {
					variableId = var_DeltaY;
				} else if (anim.value == &mWidth) {
					variableId = var_Width;
				} else if (anim.value == &mHeight) {
					variableId = var_Height;
				} else if (anim.value == &mFacing) {
					variableId = var_Facing;
				} else if (anim.value == &mCenterOffset.x) {
					variableId = var_CenterOffsetX;
				} else if (anim.value == &mCenterOffset.y) {
					variableId = var_CenterOffsetY;
				} else if (anim.value == &mOpacity) {
					variableId = var_Opacity;
				}
				serializer->serialize_uint(anim.delayMs);
				serializer->serialize_1u(easingId);
				serializer->serialize_1u(variableId);
				serializer->serialize_f(anim.targetVal);
				serializer->serialize_uint(anim.durationMs);
				if (anim.delayMs <= 0) {
					// only send these if animation is actually running, they will be sent at start
					serializer->serialize_uint(anim.currMs);
					serializer->serialize_f(anim.beginVal);
					serializer->serialize_f(anim.deltaVal);
				}
			}

		}
//   #ifdef PDG_SCML_SUPPORT
// 	SCML_pdg::Entity*		mEntity;
// 	float					mEntityScaleX;
// 	float					mEntityScaleY;
//   #endif // PDG_SCML_SUPPORT
		if (serFlags & ser_Forces) {
			uint32 numForces = mForces.size();
			serializer->serialize_uint(numForces);
			for (uint32 i = 0; i < numForces; i++) {
				serializer->serialize_uint(mForces[i].delayRemaining);
				serializer->serialize_uint(mForces[i].milliRemaining);
				SERIALIZE_FLOAT_LIST_START(1, 3);
				SERIALIZE_NON_ZERO_F(mForces[i].xAccelerationPerMs2, 0);
				SERIALIZE_NON_ZERO_F(mForces[i].yAccelerationPerMs2, 1);
				SERIALIZE_NON_ZERO_F(mForces[i].radianAccelerationPerMs2, 2);
				SERIALIZE_FLOAT_LIST_END(1);
			}
		}
		if (serFlags & ser_Physics) {
			if (serFlags & ser_InitialData) {
				serializer->serialize_uint(mCollideGroup);
				serializer->serialize_f(mElasticity);
			}
			serializer->serialize_uint(mDoCollisions);
			if (mDoCollisions == collide_CollisionRadius) {
				serializer->serialize_f(mCollisionRadius);
			};
          #ifndef PDG_USE_CHIPMUNK_PHYSICS
          	serializer->serialize_uint(0);    // serialize a value indicating no breakable joints
          #else
          	serializer->serialize_uint(mNumBreakableJoints);
			for (int i = 0; i < mNumBreakableJoints; i++) {
				cpConstraint* constraint = mBreakableJoints[i];
				uint8 jti = getJointTypeId(constraint);
				cpBody* b = cpConstraintGetB(constraint);
                Sprite* otherSprite = (Sprite*) cpBodyGetUserData(b);
                uint32 siid = otherSprite->iid;
				cpFloat maxForce = cpConstraintGetMaxForce(constraint);  // inf
				cpFloat errorBias = cpConstraintGetErrorBias(constraint);  // 0.1
				cpFloat maxBias = cpConstraintGetMaxBias(constraint); // inf
				bool hasMaxForce = (maxForce != (cpFloat)INFINITY);
				bool hasErrorBias = (errorBias != (cpFloat)0.1f);
				bool hasMaxBias = (maxBias != (cpFloat)INFINITY);
				serializer->serialize_1u(jti);
				serializer->serialize_uint(siid);
				serializer->serialize_bool(hasMaxForce);
				serializer->serialize_bool(hasErrorBias);
				serializer->serialize_bool(hasMaxBias);
				if (hasMaxForce) {
					serializer->serialize_f(maxForce);
				}
				if (hasErrorBias) {
					serializer->serialize_f(errorBias);
				}
				if (hasMaxBias) {
					serializer->serialize_f(maxBias);
				}
				if ( jti == PinJointId ) {
					serializer->serialize_point(convertPoint(cpPinJointGetAnchr1(constraint)));
					serializer->serialize_point(convertPoint(cpPinJointGetAnchr2(constraint)));
					serializer->serialize_f(cpPinJointGetDist(constraint));
				}
				if ( jti == SlideJointId ) {
					serializer->serialize_point(convertPoint(cpSlideJointGetAnchr1(constraint)));
					serializer->serialize_point(convertPoint(cpSlideJointGetAnchr2(constraint)));
					serializer->serialize_f(cpSlideJointGetMin(constraint));
					serializer->serialize_f(cpSlideJointGetMax(constraint));
				}
				if ( jti == PivotJointId ) {
					serializer->serialize_point(convertPoint(cpPivotJointGetAnchr1(constraint)));
					serializer->serialize_point(convertPoint(cpPivotJointGetAnchr2(constraint)));
				}
				if ( jti == GrooveJointId ) {
					serializer->serialize_point(convertPoint(cpGrooveJointGetAnchr2(constraint)));
					serializer->serialize_point(convertPoint(cpGrooveJointGetGrooveA(constraint)));
					serializer->serialize_point(convertPoint(cpGrooveJointGetGrooveB(constraint)));
				}
				if ( jti == DampedSpringId ) {
					serializer->serialize_point(convertPoint(cpDampedSpringGetAnchr1(constraint)));
					serializer->serialize_point(convertPoint(cpDampedSpringGetAnchr2(constraint)));
					serializer->serialize_f(cpDampedSpringGetRestLength(constraint));
					serializer->serialize_f(cpDampedSpringGetStiffness(constraint));
					serializer->serialize_f(cpDampedSpringGetDamping(constraint));
				}
				if ( jti == DampedRotarySpringId ) {
					serializer->serialize_f(cpDampedRotarySpringGetRestAngle(constraint));
					serializer->serialize_f(cpDampedRotarySpringGetStiffness(constraint));
					serializer->serialize_f(cpDampedRotarySpringGetDamping(constraint));
				}
				if ( jti == RotaryLimitJointId ) {
					serializer->serialize_f(cpRotaryLimitJointGetMin(constraint));
					serializer->serialize_f(cpRotaryLimitJointGetMax(constraint));
				}
				if ( jti == RatchetJointId ) {
					serializer->serialize_f(cpRatchetJointGetAngle(constraint));
					serializer->serialize_f(cpRatchetJointGetPhase(constraint));
					serializer->serialize_f(cpRatchetJointGetRatchet(constraint));
				}
				if ( jti == GearJointId ) {
					serializer->serialize_f(cpGearJointGetPhase(constraint));
					serializer->serialize_f(cpGearJointGetRatio(constraint));
				}
				if ( jti == SimpleMotorId ) {
					serializer->serialize_f(cpSimpleMotorGetRate(constraint));
				}
//				cpConstraint* allocConstraintByJointTypeId(uint8 jointTypeId);
			}
          #endif // PDG_USE_CHIPMUNK_PHYSICS
		}

	}

		// stuff from animated
	

		// forces 
	

		// animation helpers
//	std::vector<IAnimationHelper*> mHelpers;
	
		// stuff from sprite
	

// animation
// 
// 
//   #ifndef PDG_NO_GUI
//     Port* mPort;
//   #endif
// 
// 	SpriteLayer* mLayer;
// 	Sprite* mNextSprite;
// 	Sprite* mPrevSprite;
//     
// 	
//   #ifndef PDG_NO_GUI
// 	ISpriteDrawHelper*	mDrawHelper;
// 	ISpriteDrawHelper*	mPostDrawHelper;
//   #endif	
// 
// 	ISpriteCollideHelper* mCollisionHelper;
// 

}


void Sprite::deserialize(IDeserializer* deserializer) {
	uint32 serFlags = (mLayer) ? mLayer->mSerFlags : (uint32)ser_Full;
	if ( (serFlags == ser_Micro) || (serFlags == ser_Positions) ) {

		// special case, smallest possible update
		mLocation.x = deserializer->deserialize_2();
		mLocation.y = deserializer->deserialize_2();
		mFacing = deserializer->deserialize_f();
		mCurrFrame = deserializer->deserialize_1u();
		mCurrFramePrecise = mCurrFrame;

	} else {

	  #ifdef PDG_TAG_SERIALIZED_DATA
		uint32 tag = deserializer->deserialize_4();
		DEBUG_ASSERT(tag == PDG_SPRITE_MAGIC_NUMBER, "OUT OF SYNC: expected tag for Sprite object");
	  #endif
		uint32 spriteFlags = deserializer->deserialize_2u();
		wantsOffscreen = 	((spriteFlags & 1 << 0) != 0);
		mAnimating = 		((spriteFlags & 1 << 1) != 0);
		wantsWallCollide = 	((spriteFlags & 1 << 2) != 0);
		wantsMouseOver = 	((spriteFlags & 1 << 3) != 0);
		wantsClicks = 		((spriteFlags & 1 << 4) != 0);
		wantsAnimLoop = 	((spriteFlags & 1 << 5) != 0);
		wantsAnimEnd = 		((spriteFlags & 1 << 6) != 0);
		mLoopAnim = 		((spriteFlags & 1 << 7) != 0);
		mBackToFrontAnim = 	((spriteFlags & 1 << 8) != 0);
	  #ifdef PDG_USE_CHIPMUNK_PHYSICS
		mStatic = 			((spriteFlags & 1 << 9) != 0);
	  #endif
		mBidirectionalAnim = 	((spriteFlags & 1 << 10) != 0);
		mSpriteAnimating = 		((spriteFlags & 1 << 11) != 0);
		mSpriteAnimatingBackwardsNow = 	((spriteFlags & 1 << 12) != 0);
		// better to recalculate these rather than read them from the flags so
		// they will generate the proper events
		if (serFlags & ser_InitialData) {
			mOnscreen = 			((spriteFlags & 1 << 13) != 0);
			mInBounds = 			((spriteFlags & 1 << 14) != 0);
			mCompletelyInBounds = 	((spriteFlags & 1 << 15) != 0);
		}
		if (serFlags & ser_InitialData) {
			iid = deserializer->deserialize_uint();
			spriteId = deserializer->deserialize_uint();
			mMouseDetectMode = deserializer->deserialize_uint();
		}
		if (serFlags & (ser_InitialData | ser_Animations) ) {
			uint8 opacity = deserializer->deserialize_1u();
			mOpacity = (float)opacity / 255.0f;
		}
		DESERIALIZE_FLOAT_LIST_START(2, 16);
		if (serFlags & ser_Positions) {
			mLocation.x = DESERIALIZE_NON_ZERO_F(0);
			mLocation.y = DESERIALIZE_NON_ZERO_F(1);
			mFacing = DESERIALIZE_NON_ZERO_F(2);
		}
		if (serFlags & ser_Sizes) {
			mHeight = DESERIALIZE_NON_ZERO_F(3);
			mWidth = DESERIALIZE_NON_ZERO_F(4);
		}
		if (serFlags & (ser_Animations | ser_Motion) ) {
			mCenterOffset.x = DESERIALIZE_NON_ZERO_F(5);
			mCenterOffset.y = DESERIALIZE_NON_ZERO_F(6);
		}
		if (serFlags & ser_Motion) {
			mDeltaXPerMs = DESERIALIZE_NON_ZERO_F(7);
			mDeltaYPerMs = DESERIALIZE_NON_ZERO_F(8);
			mDeltaWidthPerMs = DESERIALIZE_NON_ZERO_F(9);
			mDeltaHeightPerMs = DESERIALIZE_NON_ZERO_F(10);
			mDeltaFacingPerMs = DESERIALIZE_NON_ZERO_F(11);
		}
		if ( (serFlags & ser_InitialData) && (serFlags & (ser_Forces | ser_Physics)) ) {
			mMass = DESERIALIZE_NON_ZERO_F(12);
			mMoveFriction = DESERIALIZE_NON_ZERO_F(13);
			mSpinFriction = DESERIALIZE_NON_ZERO_F(14);
			mSizeFriction = DESERIALIZE_NON_ZERO_F(15);
		}
		DESERIALIZE_FLOAT_LIST_END(2);
		if (serFlags & ser_Animations) {
			if (serFlags & ser_InitialData) {
				mFps = deserializer->deserialize_f();
				mNumFrames = deserializer->deserialize_1u();
				for (int i = 0; i < mNumFrames; i++) {
					if (serFlags & ser_ImageRefs) {
						// not sure yet how to handle images -- filename?
						//	deserializer->deserialize_str(mFrames[i].image->filename);
						//	deserializer->deserialize_str(mFrames[i].collisionMask->filename);
					} else if (serFlags & ser_ImageData) {
						// send all the data for the image, probably need to make ImageImpl serializable
						//	mFrames[i].image = deserializer->deserialize_obj();
						//	mFrames[i].collisionMask = deserializer->deserialize_obj();
					}
					mFrames[i].center.x = deserializer->deserialize_f();
					mFrames[i].center.y = deserializer->deserialize_f();
					mFrames[i].imageFrameNum = deserializer->deserialize_uint();
					mFrames[i].centerOffsetX = deserializer->deserialize_2();
					mFrames[i].centerOffsetY = deserializer->deserialize_2();
				}
			}
			mCurrFrame = deserializer->deserialize_1u();
			mCurrFramePrecise = mCurrFrame;
			mFirstFrame = deserializer->deserialize_1u();
			mLastFrame = deserializer->deserialize_1u();
			mFadeCompleteAction = deserializer->deserialize_1();
			mDelayMs = 0;  // never gets sent, but this is correct value for it.			
			uint32 numAnims = deserializer->deserialize_uint();
			// mismatching mAnimations.size()
			if (numAnims != mAnimations.size()) {
				Animation a;
				mAnimations.assign(numAnims, a);
			}
			for (uint32 i = 0; i < numAnims; i++) {
				Animation anim;
				anim.delayMs = deserializer->deserialize_uint();
				uint8 easingId = deserializer->deserialize_1u();
				anim.easing = easingIdToFunc(easingId);
				uint8 variableId = deserializer->deserialize_1u();
				if (variableId == var_LocX){
					anim.value = &mLocation.x;
				} else if (variableId == var_LocY) {
					anim.value = &mLocation.y;
				} else if (variableId == var_DeltaX) {
					anim.value = &mDeltaXPerMs;
				} else if (variableId == var_DeltaY) {
					anim.value = &mDeltaYPerMs;
				} else if (variableId == var_Width) {
					anim.value = &mWidth;
				} else if (variableId == var_Height) {
					anim.value = &mHeight;
				} else if (variableId == var_Facing) {
					anim.value = &mFacing;
				} else if (variableId == var_CenterOffsetX) {
					anim.value = &mCenterOffset.x;
				} else if (variableId == var_CenterOffsetY) {
					anim.value = &mCenterOffset.y;
				} else if (variableId == var_Opacity) {
					anim.value = &mOpacity;
				} else {
					anim.value = 0;
				}
				anim.targetVal = deserializer->deserialize_f();
				anim.durationMs = deserializer->deserialize_uint();
				if (anim.delayMs <= 0) {
					// these were only sent if animation was actually running
					anim.currMs = deserializer->deserialize_uint();
					anim.beginVal = deserializer->deserialize_f();
					anim.deltaVal = deserializer->deserialize_f();
				}
				mAnimations[i] = anim;
			}

		}
//   #ifdef PDG_SCML_SUPPORT
// 	SCML_pdg::Entity*		mEntity;
// 	float					mEntityScaleX;
// 	float					mEntityScaleY;
//   #endif // PDG_SCML_SUPPORT
		if (serFlags & ser_Forces) {
			uint32 numForces = deserializer->deserialize_uint();
			Force force;
			if (numForces != mForces.size()) {
				mForces.assign(numForces, force);
			}
			for (uint32 i = 0; i < numForces; i++) {
				force.delayRemaining = deserializer->deserialize_uint();
				force.milliRemaining = deserializer->deserialize_uint();
				DESERIALIZE_FLOAT_LIST_START(1, 3);
				force.xAccelerationPerMs2 = DESERIALIZE_NON_ZERO_F(0);
				force.yAccelerationPerMs2 = DESERIALIZE_NON_ZERO_F(1);
				force.radianAccelerationPerMs2 = DESERIALIZE_NON_ZERO_F(2);
				DESERIALIZE_FLOAT_LIST_END(1);
				mForces[i] = force;
			}
		}
		if (serFlags & ser_Physics) {
			if (serFlags & ser_InitialData) {
				mCollideGroup = deserializer->deserialize_uint();
				mElasticity = deserializer->deserialize_f();
			}
			mDoCollisions = deserializer->deserialize_uint();
			if (mDoCollisions == collide_CollisionRadius) {
				mCollisionRadius = deserializer->deserialize_f();
			};
			// even if this isn't a Chipmunk build, read the data and just drop it
			// so we can still work with the Sprite
          	int numJoints = deserializer->deserialize_uint();
			for (int i = 0; i < numJoints; i++) {
				uint8 jti = deserializer->deserialize_1u();
				uint32 siid = deserializer->deserialize_uint();
				Sprite* otherSprite;
              #ifdef PDG_USE_CHIPMUNK_PHYSICS
				// get all the info about the existing constraint
				cpConstraint* constraint = 0;
				if (i < mNumBreakableJoints) {
					uint8 curr_jti = 0;
					uint32 curr_siid = 0;
					constraint = mBreakableJoints[i];
					curr_jti = getJointTypeId(constraint);
					cpBody* b = cpConstraintGetB(constraint);
                	otherSprite = (Sprite*) cpBodyGetUserData(b);
                	curr_siid = otherSprite->iid;
                	if ((siid != curr_siid) || (jti != curr_jti)) {
                		// existing constraint doesn't match with what's on the stream
                		// remove it and start over
                		removeJoint(constraint);
                	}
                	otherSprite = (mLayer) ? mLayer->findSpriteByInternalId(siid) : 0;
					if (serFlags & ser_InitialData) {
						// FIXME: if the other sprite comes after this one in the streaming order
						// and it doesn't exist yet, we are screwed -- we don't have any information
						// about it. This should only be an issue when we are streaming with
						// the ser_InitialData flag.
						// Possible solution would be to create the sprite immediately
						// and assign it the siid, then it would be updated with the new data when
						// it arrived.
                	} else {
						DEBUG_ASSERT(otherSprite != 0, "OUT OF SYNC: expected to find existing Sprite");
                	}
                	constraint = 0;
				}
              #endif  // PDG_USE_CHIPMUNK_PHYSICS
				bool hasMaxForce = deserializer->deserialize_bool();
				bool hasErrorBias = deserializer->deserialize_bool();
				bool hasMaxBias = deserializer->deserialize_bool();
				cpFloat maxForce, errorBias, maxBias;
				float f1, f2, f3;
				Point p1, p2, p3;
				if (hasMaxForce) {
					maxForce = deserializer->deserialize_f();
				}
				if (hasErrorBias) {
					errorBias = deserializer->deserialize_f();
				}
				if (hasMaxBias) {
					maxBias = deserializer->deserialize_f();
				}
				if ( jti == PinJointId ) {
					p1 = deserializer->deserialize_point(); // Anchr1
					p2 = deserializer->deserialize_point(); // Anchr2
					f1 = deserializer->deserialize_f();     // Dist
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = pinJoint(p1, otherSprite, p2);
					}
					cpPinJointSetAnchr1(constraint, convertPoint(p1));
					cpPinJointSetAnchr2(constraint, convertPoint(p2));
					cpPinJointSetDist(constraint, f1);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == SlideJointId ) {
					p1 = deserializer->deserialize_point(); // Anchr1
					p2 = deserializer->deserialize_point(); // Anchr2
					f1 = deserializer->deserialize_f();     // Min
					f2 = deserializer->deserialize_f();     // Max
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = slideJoint(p1, otherSprite, p2, f1, f2);
					}
					cpSlideJointSetAnchr1(constraint, convertPoint(p1));
					cpSlideJointSetAnchr2(constraint, convertPoint(p2));
					cpSlideJointSetMin(constraint, f1);
					cpSlideJointSetMax(constraint, f2);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == PivotJointId ) {
					p1 = deserializer->deserialize_point(); // Anchr1
					p2 = deserializer->deserialize_point(); // Anchr2
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = pivotJoint(otherSprite, p1);
					}
					cpPivotJointSetAnchr1(constraint, convertPoint(p1));
					cpPivotJointSetAnchr2(constraint, convertPoint(p2));
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == GrooveJointId ) {
					p1 = deserializer->deserialize_point(); // Anchr2
					p2 = deserializer->deserialize_point(); // GrooveA
					p3 = deserializer->deserialize_point(); // GrooveB
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = grooveJoint(p2, p3, otherSprite, p1);
				  	}
					cpGrooveJointSetAnchr2(constraint, convertPoint(p1));
					cpGrooveJointSetGrooveA(constraint, convertPoint(p2));
					cpGrooveJointSetGrooveB(constraint, convertPoint(p3));
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == DampedSpringId ) {
					p1 = deserializer->deserialize_point(); // Anchr1
					p2 = deserializer->deserialize_point(); // Anchr2
					f1 = deserializer->deserialize_f(); // RestLength
					f2 = deserializer->deserialize_f(); // Stiffness
					f3 = deserializer->deserialize_f(); // Damping
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = springJoint(p1, otherSprite, p2, f1, f2, f3);
					}
					cpDampedSpringSetAnchr1(constraint, convertPoint(p1));
					cpDampedSpringSetAnchr2(constraint, convertPoint(p2));
					cpDampedSpringSetRestLength(constraint, f1);
					cpDampedSpringSetStiffness(constraint, f2);
					cpDampedSpringSetDamping(constraint, f3);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == DampedRotarySpringId ) {
					f1 = deserializer->deserialize_f(); // RestAngle
					f2 = deserializer->deserialize_f(); // Stiffness
					f3 = deserializer->deserialize_f(); // Damping
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = rotarySpring(otherSprite, f1, f2, f3);
					}
					cpDampedRotarySpringSetRestAngle(constraint, f1);
					cpDampedRotarySpringSetStiffness(constraint, f2);
					cpDampedRotarySpringSetDamping(constraint, f3);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == RotaryLimitJointId ) {
					f1 = deserializer->deserialize_f();     // Min
					f2 = deserializer->deserialize_f();     // Max
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = rotaryLimit(otherSprite, f1, f2);
					}
					cpRotaryLimitJointSetMin(constraint, f1);
					cpRotaryLimitJointSetMax(constraint, f2);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == RatchetJointId ) {
					f1 = deserializer->deserialize_f(); // Angle
					f2 = deserializer->deserialize_f(); // Phase
					f3 = deserializer->deserialize_f(); // Ratchet
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = ratchet(otherSprite, f3);
					}
					cpRatchetJointSetAngle(constraint, f1);
					cpRatchetJointSetPhase(constraint, f2);
					cpRatchetJointSetRatchet(constraint, f3);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == GearJointId ) {
					f1 = deserializer->deserialize_f(); // Phase
					f2 = deserializer->deserialize_f(); // Ratio
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = gear(otherSprite, f2);
					}
					cpGearJointSetPhase(constraint, f1);
					cpGearJointSetRatio(constraint, f2);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
				if ( jti == SimpleMotorId ) {
					f1 = deserializer->deserialize_f(); // Rate
				  #ifdef PDG_USE_CHIPMUNK_PHYSICS
				  	if (!constraint) {
						constraint = motor(otherSprite, f1);
					}
					cpSimpleMotorSetRate(constraint, f1);
				  #endif  // PDG_USE_CHIPMUNK_PHYSICS
				}
              #ifdef PDG_USE_CHIPMUNK_PHYSICS
                if (constraint) {
					if (hasMaxForce) {
						cpConstraintSetMaxForce(constraint, maxForce);
					}
					if (hasErrorBias) {
						cpConstraintSetErrorBias(constraint, errorBias);
					}
					if (hasMaxBias) {
						cpConstraintSetMaxBias(constraint, maxBias);
					}
				}
              #endif  // PDG_USE_CHIPMUNK_PHYSICS
			}
          #ifdef PDG_USE_CHIPMUNK_PHYSICS
			for (int i = numJoints; i < mNumBreakableJoints; i++) {
				// these are extra, remove them
				cpConstraint* constraint = mBreakableJoints[i];
				removeJoint(constraint);
			}
         	mNumBreakableJoints = numJoints; 
          #endif
		}

	}
}

void Sprite::setUserData(UserData* inUserData) {
	userData = inUserData;
}

void Sprite::freeUserData() {
//	SPRITE_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] freeUserData: userData [%p]", this, id, userData); )
	if (userData) {
		userData->release();
	}
	userData = 0;
}


// sets current frame of Sprite to a given frame number
Sprite&    Sprite::setFrame(int frame) {
	mSpriteAnimatingBackwardsNow = false;
	DEBUG_ASSERT(frame >= 0 && frame < MAX_FRAMES_PER_SPRITE, "invalid frame number passed to sprite setFrame");
	if (frame < 0) {
		frame = 0;
	}
	if (frame >= MAX_FRAMES_PER_SPRITE) {
		frame = MAX_FRAMES_PER_SPRITE - 1;
	}
	if ((frame == start_FromLastFrame) || (frame >= mNumFrames)) {
		mCurrFrame = mNumFrames - 1;
		if (mBidirectionalAnim) {
			mSpriteAnimatingBackwardsNow = true;
		}
	} else if (frame < 0) {
		mCurrFrame = 0;
	} else {
		mCurrFrame = frame;
	}
	mCurrFramePrecise = mCurrFrame;
	return *this;
}


int		Sprite::getCurrentFrame() {
	return mCurrFrame;
}



// set how fast we animate through the frames
// startingFrame and number of frames can be used to only animate through a subset of the
// Sprite's frames.
void	Sprite::startFrameAnimation(float fps, int startingFrame, int numFrames, int animateFlags) {
	mFps = fps;
	if (startingFrame == start_FromFirstFrame) {
		startingFrame = 0;
	} else if (startingFrame == start_FromLastFrame) {
		startingFrame = mLastFrame;
	}
	mFirstFrame = startingFrame;
	mLastFrame = mFirstFrame + numFrames;
	if (mFirstFrame < 0) {
		mFirstFrame = 0;
	}
	if (mLastFrame < 0) {
		mLastFrame = 0;
	}
	if (mFirstFrame >= mNumFrames) {
		mFirstFrame = mNumFrames - 1;
	}
	if (mLastFrame >= mNumFrames) {
		mLastFrame = mNumFrames - 1;
	}
	mLoopAnim = (animateFlags & animate_Looping);
	mBackToFrontAnim = (animateFlags & animate_EndToStart);
	mBidirectionalAnim = (animateFlags & animate_Bidirectional);
	if (mFirstFrame < mLastFrame) {
		mSpriteAnimating = true;
	}
	if (mBackToFrontAnim) {
		mCurrFrame = mLastFrame;
		mSpriteAnimatingBackwardsNow = true;
	} else {
		mCurrFrame = mFirstFrame;
		mSpriteAnimatingBackwardsNow = false;
	}
	mCurrFramePrecise = mCurrFrame;
}



// stops all frame animation, the current frame becomes the sprite's image
// does not send action_AnimationEnd sprite events
void	Sprite::stopFrameAnimation() {
	mSpriteAnimating = false;
}

// adds an image that is used for one or more frames
// since an image itself can have multiple frames, all frames of the image are added
// to the Sprite, unless startingFrame and/or numFrames is passed in.
// the frames are added to the end of the frame list 
void	Sprite::addFramesImage(Image* image, int startingFrame, int numFrames) {
	if (image == 0) return;
	if (startingFrame == start_FromFirstFrame) {
		startingFrame = 0;
	}
	do {
		if (mNumFrames >= MAX_FRAMES_PER_SPRITE) break;  // stop if we run out of space
		ImageImpl* img = dynamic_cast<ImageImpl*>(image);
		numFrames--; // goes negative and loop continues if numFrames was all_frames (0)
		mFrames[mNumFrames].image = img;
		img->addRef();
		mFrames[mNumFrames].collisionMask = 0;
		mFrames[mNumFrames].imageFrameNum = startingFrame;
		mFrames[mNumFrames].centerOffsetX = 0;
		mFrames[mNumFrames].centerOffsetY = 0;
		mFrames[mNumFrames].center = Rect( (img->frames) ? img->frameWidth : img->width, img->height).centerPoint();
		mNumFrames++;
		startingFrame++;
		if (startingFrame >= img->frames) break; // stop if we run out of frames in the image
	} while (numFrames != 0);
}


#ifdef PDG_SCML_SUPPORT
bool	Sprite::hasAnimation(const char* animationName) {
	return mEntity ? mEntity->getAnimation(animationName) != 0 : false;
}

bool	Sprite::hasAnimation(int animationId) {
	return mEntity ? mEntity->getAnimation(animationId) != 0 : false;
}

void	Sprite::startAnimation(const char* animationName) {
	if (mEntity) {
		mEntity->startAnimation(animationName);
	}
}

void	Sprite::startAnimation(int animationId) {
	if (mEntity) {
		mEntity->startAnimation(animationId);
	}
}

Sprite&	Sprite::setEntityScale(int xScale, int yScale) {
	mEntityScaleX = xScale;
	mEntityScaleY = yScale;
	return *this;
}

#endif // PDG_SCML_SUPPORT


#ifndef PDG_NO_GUI
// set a drawing helper that will be called before the sprite is drawn
// it can do whatever drawing it likes, then return true if it wants to let the
// sprite draw itself, or false if the sprite should skip it's normal drawing
// if a post draw helper is set, it will always be called, even if false is returned
void    Sprite::setDrawHelper(ISpriteDrawHelper* helper) {
	if (mDrawHelper && mDrawHelper->ownedBySprite()) {
		delete mDrawHelper;
		mDrawHelper = 0;
	}
	mDrawHelper = helper;

}

// set a drawing helper that will be called after a sprite is drawn
// it can do whatever drawing it likes. The return value is ignored since
// the sprite has already drawn. If set, it is called even if the primary draw 
// helper tells the Sprite to skip normal drawing.
void    Sprite::setPostDrawHelper(ISpriteDrawHelper* helper) {
	if (mPostDrawHelper && mPostDrawHelper->ownedBySprite()) {
		delete mPostDrawHelper;
		mPostDrawHelper = 0;
	}
	mPostDrawHelper = helper;
}	
#endif

// replace an image with another one for all the frames that reference that image
void	Sprite::changeFramesImage(Image* oldImage, Image* newImage) {
	ImageImpl* img = dynamic_cast<ImageImpl*>(newImage);
	for (int i = 0; i< mNumFrames; i++) {
		if (mFrames[i].image == oldImage) {
			mFrames[i].image = img;
			oldImage->release();
			newImage->addRef();
		}
	}
}

RotatedRect
Sprite::getFrameRotatedBounds(int frame) {
    Rect r = mFrames[frame].image->getImageBounds();
    r.center(mLocation);
    Offset coff(mFrames[frame].centerOffsetX, mFrames[frame].centerOffsetY);
    RotatedRect rr(r, mFacing, coff);
    return rr;
}


// set offset of centerpoint of sprite (rotation and location are all relative to centerpoint)
// this can be set for the whole sprite, if image, or for an individual frame or group of
// frames by specifying the image or by specifying the range of frames
// if image is 0, the startingFrame and numFrames parameters are frame numbers for the Sprite
// if image is specified, the startingFrame and numFrames paramaters are Image frame numbers
// this offset is relative to the absolute center of the image or frame
void	Sprite::offsetFrameCenters(int offsetX, int offsetY, Image* image, int startingFrame, int numFrames) {
	if (startingFrame < 0) startingFrame = 0;
	if (numFrames >= 0) {
		if (image == 0) {
			for (int i = startingFrame; i< mNumFrames; i++) {
				mFrames[i].centerOffsetX = offsetX;
				mFrames[i].centerOffsetY = offsetY;
				numFrames--;
				if (numFrames == 0) break;
			}
		} else {
			bool found = false;
			for (int i = 0; i< mNumFrames; i++) {
				if (mFrames[i].image == image) {
					if (!found) {
						if (startingFrame == 0) {
							found = true;
						} else {
							startingFrame--;
						}
					}
					if (found) {
						mFrames[i].centerOffsetX = offsetX;
						mFrames[i].centerOffsetY = offsetY;
						numFrames--;
						if (numFrames == 0) break;
					}
				} else if (found) {
					break;
				}
			}
		}
	}
}


// fetch the offsets set above, but only for a single frame
void	Sprite::getFrameCenterOffset(int &offsetX, int &offsetY, Image* image, int frameNum) {
	offsetX = 0;
	offsetY = 0;
	if (frameNum >= 0) {
		if (image == 0) {
			if (frameNum < mNumFrames) {
				offsetX = mFrames[frameNum].centerOffsetX;
				offsetY = mFrames[frameNum].centerOffsetY;
			}
		} else {
			for (int i = 0; i< mNumFrames; i++) {
				if (mFrames[i].image == image) {
					int fn = frameNum + i;
					if ( (fn < mNumFrames) && (mFrames[fn].image == image)) {
						offsetX = mFrames[fn].centerOffsetX;
						offsetY = mFrames[fn].centerOffsetY;
					}
					break;
				}
			}
		}
	}
}



// arrange sprites within layer
Sprite&	Sprite::moveBehind( Sprite* inSprite) {
	Sprite* sprite = inSprite->mPrevSprite;
	SpriteLayer* layer = mLayer;
	// make sure we are assigned to a layer
	if (layer == 0) return *this;
	if (sprite && (layer != sprite->mLayer)) return *this;
	addRef(); // make sure we don't get deleted by removal
	layer->removeSprite(this);
	layer->addSpriteInFrontOf(this, sprite);
	release(); // don't keep the extra reference
	return *this;
}


Sprite&	Sprite::moveInFrontOf( Sprite* inSprite) {
	Sprite* sprite = inSprite;
	SpriteLayer* layer = mLayer;
	// make sure we are assigned to a layer
	if (layer == 0) return *this;
	if (layer != sprite->mLayer) return *this;
	addRef(); // make sure we don't get deleted by removal
	layer->removeSprite(this);
	layer->addSpriteInFrontOf(this, sprite);
	release(); // don't keep the extra reference
	return *this;
}

int Sprite::getZOrder() {
	if (mLayer == 0) return -1;
	return mLayer->getSpriteZOrder(this);
}

bool Sprite::isBehind(Sprite* sprite) {
	if (mLayer == 0) return false;
	return mLayer->isSpriteBehind(this, sprite);
}


// fading, with 1.0 being complete opaque and 0.0 being completely transparent
Sprite& Sprite::setOpacity(float opacity) {
	mOpacity = opacity;
	if (mOpacity > 1.0) {
		mOpacity = 1.0;
	} else if (mOpacity < 0.0) {
		mOpacity = 0.0;
	}
	return *this;
}

float Sprite::getOpacity() {
	return mOpacity;
}

void Sprite::fadeTo(float targetOpacity, int32 msDuration, EasingFunc easing) {
	if (targetOpacity > 1.0) {
		targetOpacity = 1.0;
	} else if (targetOpacity < 0.0) {
		targetOpacity = 0.0;
	}
	Animation a(&mOpacity, targetOpacity, easing, mDelayMs, msDuration);
	mAnimations.push_back(a);
	mFadeCompleteAction = action_FadeComplete;
    mDelayMs = 0;
}

void Sprite::fadeIn(int32 msDuration, EasingFunc easing)  {
	if (mOpacity == 1.0) {  // if we are at our default full opacity, then set to transparent
		mOpacity = 0.0;     // this handles common case where we create a new sprite and want it
	}                       // to fade in, but don't want to manually set opacity to 0 first
	fadeTo(1.0, msDuration, easing);
	mFadeCompleteAction = action_FadeInComplete;
}

void Sprite::fadeOut(int32 msDuration, EasingFunc easing) {
	fadeTo(0.0, msDuration, easing);
	mFadeCompleteAction = action_FadeOutComplete;
}


// collisions
Sprite&	Sprite::enableCollisions(int collisionType) {
	if (collisionType < 0) {
		disableCollisions();
	} else if (collisionType > collide_Last) {
		mDoCollisions = collide_Last;
	} else {
		mDoCollisions = collisionType;
	}
#ifdef PDG_USE_CHIPMUNK_PHYSICS
	if (!USE_CHIPMUNK) return *this;
    switch (mDoCollisions) {
        case collide_Point:
            setCollisionRadius(1);
            break;
        case collide_BoundingBox:
        {
            // calculate the size of the bounding box shape from the height and width of the sprite
            // and offset it by the center offset
            if (mCollideShape) {
                cpSpaceRemoveShape(getSpace(), mCollideShape);
                cpShapeFree(mCollideShape);
            }
            cpFloat hw = mWidth/2.0f;
            cpFloat hh = mHeight/2.0f;
            cpBB box = cpBBNew(-hw, -hh, hw, hh);
            cpVect verts[] = {
                cpv(box.l, box.b),
                cpv(box.l, box.t),
                cpv(box.r, box.t),
                cpv(box.r, box.b),
            };
            mCollideShape = cpPolyShapeNew(mBody, 4, verts, cpv(-mCenterOffset.x, -mCenterOffset.y));
            cpSpaceAddShape(getSpace(), mCollideShape);
            cpShapeSetCollisionType(mCollideShape, CP_COLLIDE_TYPE_SPRITE);
            cpShapeSetElasticity(mCollideShape, mElasticity);
            cpShapeSetFriction(mCollideShape, mMoveFriction);
        }
            break;
        case collide_AlphaChannel:
            // TODO: not sure how to handle this one yet
            // for now, we just do radius collision
            // break // deliberately fall through
        case collide_CollisionRadius:
            if (mCollisionRadius <= 0.0f) {
                // best guess at a collision radius
                mCollisionRadius = (mWidth + mHeight) / 4;
            }
            setCollisionRadius(mCollisionRadius);
            break;
    }

#endif
	return *this;
}


Sprite&	Sprite::disableCollisions() {
	mDoCollisions = collide_None;
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (USE_CHIPMUNK && mCollideShape) {
        cpSpaceRemoveShape(getSpace(), mCollideShape);
        cpShapeFree(mCollideShape);
        mCollideShape = 0;
    }
  #endif
  	return *this;
}

Sprite&	Sprite::setCollisionRadius(float pixelRadius) {
	mCollisionRadius = pixelRadius;
	if (mCollisionRadius > 0.0f) {
        if (mDoCollisions < collide_CollisionRadius) {
            // if our collision type was alpha collisions, we still want to use
            // the collision radius as a first pass cheap collision, but we
            // want to leave the collision type set to alpha collision so we
            // know to do more
            mDoCollisions = collide_CollisionRadius;
        }
      #ifdef PDG_USE_CHIPMUNK_PHYSICS
        if (!USE_CHIPMUNK) return *this;
        if (mCollideShape) {
            cpSpaceRemoveShape(getSpace(), mCollideShape);
            cpShapeFree(mCollideShape);
        }
        // Create a collision shape for the sprite
        mCollideShape = cpCircleShapeNew(mBody, mCollisionRadius, cpv(-mCenterOffset.x, -mCenterOffset.y));
        cpSpaceAddShape(getSpace(), mCollideShape);
        cpShapeSetCollisionType(mCollideShape, CP_COLLIDE_TYPE_SPRITE);
        cpShapeSetElasticity(mCollideShape, mElasticity);
        cpShapeSetFriction(mCollideShape, mMoveFriction);
      #endif
	} else if (mCollisionRadius <= 0.0f) {
		disableCollisions();
	}
	return *this;
}

float	Sprite::getCollisionRadius() {
	if (mDoCollisions < collide_CollisionRadius) {
		return 0.0f;
	} else {
		return 	mCollisionRadius;
	}
}

void	Sprite::useCollisionMask(Image* frameImage, Image* maskImage) {
	ImageImpl* img = dynamic_cast<ImageImpl*>(maskImage);
	for (int i = 0; i< mNumFrames; i++) {
		if (mFrames[i].image == frameImage) {
			Image* oldMask = mFrames[i].collisionMask;
			if (oldMask) {
				oldMask->release();
			}
			mFrames[i].collisionMask = img;
			if (maskImage) {
				maskImage->addRef();
			}
		}
	}	
}

void    Sprite::setCollisionHelper(ISpriteCollideHelper* helper) {
	mCollisionHelper = helper;
}


Sprite&	Sprite::setElasticity(float elasticity) {
	mElasticity = elasticity;
#ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (USE_CHIPMUNK && mCollideShape) {
        cpShapeSetElasticity(mCollideShape, mElasticity);
    }
#endif
	return *this;
}

float	Sprite::getElasticity() {
	return mElasticity;
}
	
#ifndef PDG_NO_GUI
bool Sprite::setWantsMouseOverEvents(bool wantsThem) { 
	bool didWant = wantsMouseOver; 
	wantsMouseOver = wantsThem; 
	if (wantsThem) {
		mLayer->wantMouseOverEvents();
	} else {
		mLayer->checkIfMouseOverEventsStillWanted();
	}
	return didWant; 
}
	
bool Sprite::setWantsClickEvents(bool wantsThem) { 
	bool didWant = wantsClicks; 
	wantsClicks = wantsThem;
	if (wantsThem) {
		mLayer->wantClickEvents();
	} else {
		mLayer->checkIfClickEventsStillWanted();
	}
	return didWant; 
}

int Sprite::setMouseDetectMode(int collisionType) {
	int t = mMouseDetectMode;
	mMouseDetectMode = collisionType;
	return t;
}
#endif // ! PDG_NO_GUI

bool Sprite::collidesWith(Sprite* sprite) {
	if (!mDoCollisions) return false;
	if (!sprite) return false;
	if (!sprite->mDoCollisions) return false;

	if (mDoCollisions == collide_CollisionRadius) {
		return CollisionDetection::detectRadiusCollision(mLocation, mCollisionRadius, sprite->getLocation(), sprite->getCollisionRadius());
	}

	else if (mDoCollisions == collide_BoundingBox) {	
		// local sprite bounding rect
        RotatedRect rectA = getFrameRotatedBounds(mCurrFrame);

		// argument sprite bounding rect		
        RotatedRect rectB = sprite->getFrameRotatedBounds(sprite->mCurrFrame);

		return CollisionDetection::detectBoundingBoxCollision(rectA, rectB);
	}

	// per pixel collision based on alpha channel of sprite or collision mask
	else if (mDoCollisions == collide_AlphaChannel) {
		// local sprite bounding rect
        RotatedRect rectA = getFrameRotatedBounds(mCurrFrame);
		// argument sprite bounding rect
        RotatedRect rectB = sprite->getFrameRotatedBounds(sprite->mCurrFrame);
		
		ImageImpl* imageA = mFrames[mCurrFrame].collisionMask;
		if (imageA == 0) {
			imageA = mFrames[mCurrFrame].image;
		}
		Rect imageBoundsA = imageA->getImageBounds();
		imageBoundsA.moveRight(mCurrFrame * imageA->frameWidth);
		ImageImpl* imageB = sprite->mFrames[sprite->mCurrFrame].image;
		if (imageB == 0) {
			imageB = sprite->mFrames[sprite->mCurrFrame].collisionMask;
		}
		Rect imageBoundsB = imageB->getImageBounds();
		imageBoundsB.moveRight(sprite->mCurrFrame * imageB->frameWidth);
		return CollisionDetection::detectPixelCollision(rectA, rectB, imageA, imageB, imageBoundsA, imageBoundsB);
	}

	
	return false;
}


bool Sprite::collidesWith(const Point& p) {
	if (!mDoCollisions) return false;
	
	if (mDoCollisions == collide_CollisionRadius) {
		return CollisionDetection::detectRadiusCollision(mLocation, mCollisionRadius, p, 1);
	}
	
	else if (mDoCollisions == collide_BoundingBox) {	
		// local sprite bounding rect
        RotatedRect rectA = getFrameRotatedBounds(mCurrFrame);
		
		// unit rect to represent the point
		RotatedRect rectB;
		rectB.setSize(1);
		rectB.moveTo(p);
		
		return CollisionDetection::detectBoundingBoxCollision(rectA, rectB);
	}
	
	// per pixel collision based on alpha channel of sprite or collision mask
	else if (mDoCollisions == collide_AlphaChannel) {
		// local sprite bounding rect
        RotatedRect rectA = getFrameRotatedBounds(mCurrFrame);
		// argument sprite bounding rect

		RotatedRect rectB;
		rectB.setSize(1);
		rectB.moveTo(p);
		
		ImageImpl* imageA = mFrames[mCurrFrame].collisionMask;
		if (imageA == 0) {
			imageA = mFrames[mCurrFrame].image;
		}
		Rect imageBoundsA = imageA->getImageBounds();
		imageBoundsA.moveRight(mCurrFrame * imageA->frameWidth);
		Rect imageBoundsB(p, 1, 1);
		return CollisionDetection::detectPixelCollision(rectA, rectB, imageA, 0, imageBoundsA, imageBoundsB);
	}
	
	
	return false;
}

	
void Sprite::impartCollisionImpulse(Sprite* sprite, Vector& outNormal, Vector& outImpulse, float& outKineticEnergy) {

	// find line of action and normalize it
	Vector normalVector = mLocation - sprite->getLocation();
	float length = normalVector.vectorLength();
	normalVector.x /= length;
	normalVector.y /= length;
    outNormal = normalVector;

	// calculate relative normal velocity
	Vector velocityA, velocityB; 
	velocityA = getVelocity();
	velocityB = sprite->getVelocity();
	Point relativeVelocity = velocityA - velocityB;
	float relativeNormalVelocity = (relativeVelocity.x * normalVector.x) + (relativeVelocity.y * normalVector.y);
	
	// apply conservation of momentum in the normal direction
	float m1 = getMass();
	float m2 = sprite->getMass();
	float base = -relativeNormalVelocity / ( ((normalVector.x*normalVector.x)+(normalVector.y*normalVector.y)) * (1/m1+1/m2) );
	
    outImpulse.x = base * normalVector.x;
    outImpulse.y = base * normalVector.y;

    // calculate kinetic energy of collision
    // we will do this by taking the difference in KineticEnergy of both objects before and after
    float v1 = length;
    float v2 = sprite->getSpeed();
    float KE1t1 = 0.5 * m1 * v1 * v1;
    float KE2t1 = 0.5 * m2 * v2 * v2;

	// y direction is inverted
	float i = (1+getElasticity()) * base;
	velocityA.x += i*normalVector.x / m1;
	velocityA.y += i*normalVector.y / m1;
	setVelocity(velocityA);
	v1 = getSpeed();

	float j = (1+sprite->getElasticity()) * base;
	velocityB.x -= j*normalVector.x / m2;
	velocityB.y -= j*normalVector.y / m2;
	sprite->setVelocity(velocityB);
	v2 = sprite->getSpeed();

    float KE1t2 = 0.5 * m1 * v1 * v1;
    float KE2t2 = 0.5 * m2 * v2 * v2;

    outKineticEnergy = KE1t1 + KE2t1 - KE1t2 - KE2t2; 
}

	
void	Sprite::draw() {
#ifndef PDG_NO_GUI
	bool shouldDraw = true;
	if (mDrawHelper) {
		shouldDraw = mDrawHelper->draw(this, mPort);
	}

  #ifdef PDG_SCML_SUPPORT
  	if (shouldDraw && mEntity) {
  		// since we don't have a preset size, we work with the scaling rect
        RotatedRect r = RotatedRect( Rect(mLocation, mEntityScaleX, mEntityScaleY), mFacing);
        RotatedRect pr = mLayer->layerToPort(r);
  		mEntity->draw(pr.centerPoint().x, pr.centerPoint().y, (180.0f/M_PI)*pr.radians, 
  			pr.width(), pr.height() ); // width and height is entity scale * layer zoom
  	}
  #endif

	if (shouldDraw && mNumFrames > 0) {
        ImageImpl* image = mFrames[mCurrFrame].image;
        if (image == 0) return;
        int opacitySave = image->getOpacity();
        if (mOpacity < 1.0) { 
            image->setOpacity((uint8)((float)255 * mOpacity));
        }
        Port* oldPort = image->setPort(mPort);

        Point p = mLocation - mFrames[mCurrFrame].center;
        RotatedRect r = RotatedRect(image->getImageBounds(p), mFacing);
        r.setCenterOffset(Point(mFrames[mCurrFrame].centerOffsetX, mFrames[mCurrFrame].centerOffsetY));
        RotatedRect pr = mLayer->layerToPort(r);
        
        if (image->frames > 1) {
            image->drawFrame(pr, mFrames[mCurrFrame].imageFrameNum );
        } else {
            mPort->drawImage(image, pr);
        }
        image->setOpacity(opacitySave);
        image->setPort(oldPort);
      #ifdef SPRITE_INTERNAL_DEBUG
        mPort->frameRect(pr, Color(1.0f, 0.0f, 1.0f, 0.5f));
      #endif
    }
    if (mPostDrawHelper) {
    	mPostDrawHelper->draw(this, mPort);
    }

  #ifdef SPRITE_DEBUG_COLLISIONS
    if (mDoCollisions >= collide_CollisionRadius) {
        Point cp = mLocation - mCenterOffset;
        cp = mLayer->layerToPort(cp);
        mPort->frameCircle(cp, mCollisionRadius, PDG_RED_COLOR);
    } else if (mDoCollisions == collide_BoundingBox) {
        RotatedRect crr = getRotatedBounds();
        crr = mLayer->layerToPort(crr);
        mPort->frameRect(crr, PDG_RED_COLOR);
    }
  #endif
  #ifdef SPRITE_MOTION_DEBUG
	Point a = mLocation - mCenterOffset;
    Vector delta(mDeltaXPerMs * 1000.0f, mDeltaYPerMs * 1000.0f);
    Point b = a + mLayer->layerToPort(delta);
	mPort->drawLine(a, b, Color(1.0f,0.0f,0.0f,0.5f));
  #endif
  #endif //!PDG_NO_GUI
}


void    
Sprite::easingCompleted(const Animation& a) {
    if (a.value == &mOpacity) {
        // this is a fade operation, so send the proper notification
        if (mFadeCompleteAction != 0) {
            mLayer->notifyAnimationAction(mFadeCompleteAction, this);
            mFadeCompleteAction = 0;
        }
    }
}


void
Sprite::doAnimate(int msElapsed, bool layerDoCollisions) {
    
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (USE_CHIPMUNK && !cpBodyIsSleeping(mBody)) {
        cpVect v = cpBodyGetPos(mBody);
        cpFloat angle = cpBodyGetAngle(mBody);
        mLocation.x = v.x - mCenterOffset.x;
        mLocation.y = v.y - mCenterOffset.y;
        mFacing = angle;
    }
    Point saveLoc = mLocation;
    Offset saveOffset = mCenterOffset;
  #endif

    // do all the primary animation
  	SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] animate @ %ld", this, msElapsed); )
    Animated::animate(msElapsed);

	// select the appropriate frame of the animation
	float elapsed = (float)msElapsed / 1000.0;


  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (USE_CHIPMUNK && (mLocation != saveLoc || mCenterOffset != saveOffset)) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] fixing chipmunk pos", this); )
        // animate() call changed the location or center offset, so update the physics engine
        cpVect v;
        v.x = mLocation.y + mCenterOffset.x; v.y = mLocation.y + mCenterOffset.y;
        cpBodySetPos(mBody, v);
        cpSpaceReindexShapesForBody(getSpace(), mBody);
        // TODO: do something other than cpSpaceReindexShapesForBody for static sprites;
        if (mCenterOffset != saveOffset) {
            setCollisionRadius(mCollisionRadius);
        }
    }
  #endif

  #ifdef PDG_SCML_SUPPORT
  	if (mEntity) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] doing SCML animation", this); )
  		mEntity->update(msElapsed);
  	}
  #endif

	if (mSpriteAnimating) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] doing frame animation", this); )
		if (!mSpriteAnimatingBackwardsNow) {
			// animating foward
			mCurrFramePrecise += (elapsed * mFps);
			mCurrFrame = mCurrFramePrecise;
			if ( mCurrFrame >= mLastFrame ) {	 
				if (!mLoopAnim && (!mBidirectionalAnim || mBackToFrontAnim)) {
					mCurrFramePrecise = mLastFrame;
					if (wantsAnimEnd) {
						mLayer->notifyAnimationAction(action_AnimationEnd, this);
					}
				} else {
					float excess = mCurrFramePrecise - (float)mLastFrame;
					if (mBidirectionalAnim) {
						mCurrFramePrecise = (float)mLastFrame - excess;
						mSpriteAnimatingBackwardsNow = true; // reverse direction
					} else {
						mCurrFramePrecise = (float)mFirstFrame + excess;
					}
					if (wantsAnimLoop) {
						mLayer->notifyAnimationAction(action_AnimationLoop, this);
					}
				}
				mCurrFrame = mCurrFramePrecise;
			}
		} else {
			// animating backwards
			mCurrFramePrecise -= (elapsed * mFps);
			mCurrFrame = mCurrFramePrecise;
			if ( mCurrFrame < mFirstFrame ) {
				if (!mLoopAnim && (!mBidirectionalAnim || !mBackToFrontAnim)) {
					mCurrFramePrecise = mFirstFrame;
					if (wantsAnimEnd) {
						mLayer->notifyAnimationAction(action_AnimationEnd, this);
					}
				} else {
					float excess = (float)mFirstFrame - mCurrFramePrecise;
					if (mBidirectionalAnim) {
						mCurrFramePrecise = (float)mFirstFrame + excess;
						mSpriteAnimatingBackwardsNow = false; // reverse direction
					} else {
						mCurrFramePrecise = (float)mLastFrame - excess;
					}
					if (wantsAnimLoop) {
						mLayer->notifyAnimationAction(action_AnimationLoop, this);
					}
				}
				mCurrFrame = mCurrFramePrecise;
			}
		}
		// final anim bounds check
		if (mCurrFrame < mFirstFrame) {
			mCurrFrame = mFirstFrame;
			mCurrFramePrecise = mFirstFrame;
		} else if (mCurrFrame > mLastFrame) {
			mCurrFrame = mLastFrame;
			mCurrFramePrecise = mLastFrame;
		}
		DEBUG_ASSERT((mCurrFrame >= 0) && (mCurrFrame < MAX_FRAMES_PER_SPRITE), "invalid frame number in Sprite::animate()");
		if (mCurrFrame < 0) {
			mCurrFrame = 0;
			mCurrFramePrecise = mFirstFrame;
		}
		if (mCurrFrame >= MAX_FRAMES_PER_SPRITE) {
			mCurrFrame = MAX_FRAMES_PER_SPRITE - 1;
			mCurrFramePrecise = mLastFrame;
		}
	}

	bool dead = false;
	if (!USE_CHIPMUNK && layerDoCollisions && mDoCollisions) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] checking collisions", this); )
		
		addRef(); // make sure this won't be deleted by being removed from the layer while we are working with it
		Sprite* otherSprite = mLayer->mFirstSprite;
		while (otherSprite) {
			Sprite* nextOther = otherSprite->mNextSprite;
			if (otherSprite != this && otherSprite->mDoCollisions && otherSprite) {
				if (collidesWith(otherSprite)) {
                    Vector normal;
                    Vector impulse;
                    float kineticEnergy;
                    impartCollisionImpulse(otherSprite, normal, impulse, kineticEnergy);
                    float force = impulse.vectorLength() / elapsed;
					mLayer->notifyCollisionAction(Sprite::action_CollideSprite, this, normal, impulse, force, kineticEnergy, 
                            #ifdef PDG_USE_CHIPMUNK_PHYSICS
                                0,  // need to pass in something for cpArbiter param
                            #endif
								otherSprite);
				}
			}
			otherSprite = nextOther;
		}
		if (refs == 1) dead = true;
		release();
		if (dead) return;
	}
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (mNumBreakableJoints > 0) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] checking joints", this); )
        addRef();
        for (int i = 0; i < mNumBreakableJoints; i++) {
            cpFloat impulse = cpConstraintGetImpulse(mBreakableJoints[i]);
            cpFloat force = impulse/elapsed;
            cpFloat maxForce = cpConstraintGetMaxForce(mBreakableJoints[i]);
            cpFloat breakingForce = BREAK_COEFFICIENT * maxForce;
            // If the force is almost as big as the joint's max force, break it.
            if (force > breakingForce) {
                cpBody* b = cpConstraintGetB(mBreakableJoints[i]);
                Sprite* otherSprite = (Sprite*) cpBodyGetUserData(b);
                SpriteJointBreakInfo si;
                si.action = action_JointBreak;
                si.actingSprite = this;
                si.targetSprite = otherSprite;
                si.impulse = impulse;
                si.force = force;
                si.breakForce = breakingForce;
                si.joint = mBreakableJoints[i];   // the joint that broke
                si.inLayer = this->mLayer;
                if (postEvent(eventType_SpriteBreak, &si) == false) {
                    // "unhandled" means go ahead and break it
                    disconnect(otherSprite);
                }
            }
        }
		if (refs == 1) dead = true;
		release();
		if (dead) return;
    }
  #endif  // PDG_USE_CHIPMUNK_PHYSICS
  	if (wantsWallCollide || wantsOffscreen) {
  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] checking layer bounds", this); )
  	  #ifndef PDG_NO_GUI
  		bool oldOnscreen = mOnscreen;
	  #endif // !PDG_NO_GUI
  		bool oldInBounds = mInBounds;
  		bool oldCompletelyInBounds = mCompletelyInBounds;
  		recalcOnscreenAndInBounds();
  	  #ifndef PDG_NO_GUI
		if (wantsOffscreen && mPort) {
			uint16 action = 0;
			if (oldOnscreen && !mOnscreen) {
				action = action_Offscreen;
			} else if (!oldOnscreen && mOnscreen) {
				action = action_Onscreen;
			}
			if (action) {
				addRef();
				mLayer->notifyAnimationAction(action, this);
				if (refs == 1) dead = true;
				release();
				if (dead) return;
			} 
		}
	  #endif // !PDG_NO_GUI
		if (wantsWallCollide) {
//  			SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] wall collide %d %d %d %d", this, oldInBounds, mInBounds, oldCompletelyInBounds, mCompletelyInBounds); )
			if (oldInBounds && !mInBounds) {
				addRef();
				mLayer->notifyAnimationAction(Sprite::action_ExitLayer, this);
				if (refs == 1) dead = true;
				release();
			} else if (oldCompletelyInBounds && !mCompletelyInBounds) {
				addRef();
				Vector normal = getVelocity().normal();
				// TODO: calculate real values for these things, but don't apply them
				Vector impulse;
				float kineticEnergy = 0;
				float force = impulse.vectorLength() / elapsed;  // TODO: not sure why we divide by elapsed, collision force is instantaneous
				mLayer->notifyCollisionAction(Sprite::action_CollideWall, this, normal, impulse, force, kineticEnergy, 
						#ifdef PDG_USE_CHIPMUNK_PHYSICS
							0,  // need to pass in something for cpArbiter param
						#endif
							0); // no other sprite
				if (refs == 1) dead = true;
				release();
			}
			if (dead) return;
		}
    }

}

bool 
Sprite::postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter) {
	// simplier version just tries to emit the event locally and cleanup afterwards if not
	if (fromEmitter == 0) {
		fromEmitter = this;
	}
	bool wasHandled = emitEvent(fromEmitter, inEventType, inEventData);
	cleanupRemovedHandlers();
	// special case for collision events because we want to give both objects in the
	// collision a chance to respond
	if (inEventType == eventType_SpriteCollide) {
		SpriteCollideInfo* si = static_cast<SpriteCollideInfo*>(inEventData);
		if (this == si->targetSprite) {
			// nothing more to do if we are the target sprite
			// we don't want to pass this message on to the layer
			// since the acting sprite will do that
			return wasHandled;
		} else if (si->targetSprite) {
			// not the target sprite, give the target a chance do something
			wasHandled |= si->targetSprite->postEvent(inEventType, inEventData);
		}
	}
	if (!wasHandled) {
		// not handled locally, try the sprite layer, which will pass it on to
		// the event manager if needed
		if (mLayer) {
			wasHandled = mLayer->postEvent(inEventType, inEventData, fromEmitter);
		}
	}
	return wasHandled;
}

#ifdef PDG_USE_CHIPMUNK_PHYSICS
    Animated&
    Sprite::setMass(float mass) {
    	if (!USE_CHIPMUNK) {
    		Animated::setMass(mass);
    		return *this;
    	}
        DEBUG_ASSERT(mStatic == false, "Setting Mass for static sprite. Static sprites always have infinite mass.");
        Animated::setMass(mass);
        cpBodySetMass(mBody, mass);
        cpFloat moment = cpMomentForBox(mass, mWidth, mHeight);
        cpBodySetMoment(mBody, moment);
        return *this;
    }

    Animated&
    Sprite::setFriction(float friction) {
    	if (!USE_CHIPMUNK) {
    		Animated::setFriction(friction);
    		return *this;
    	}
        mMoveFriction = friction;
        if (mCollideShape) {
            cpShapeSetFriction(mCollideShape, mMoveFriction);
        }
        return *this;
    }
    
    float
    Sprite::getFriction() {
        return mMoveFriction;
    }

    void	
    Sprite::locationChanged(const Offset& delta) {
        if (USE_CHIPMUNK && !mAnimating) {
            // only do when not changed by animate() call because we
            // don't want to recalc this multiple times if center changes too
            cpBodySetPos(mBody, cpv(mLocation.x + mCenterOffset.x, mLocation.y + mCenterOffset.y));
            if (mStatic) {
                cpSpaceReindexStatic(getSpace());
            } else {
                cpSpaceReindexShapesForBody(getSpace(), mBody);
            }
        }
    }

    void	
    Sprite::sizeChanged(float deltaW, float deltaH) {
        if (USE_CHIPMUNK && !mAnimating && !mStatic) {
            cpFloat moment = cpMomentForBox(mMass, mWidth, mHeight);
            if (moment > 0) {
                cpBodySetMoment(mBody, moment);
            }
        }
    }

    void	
    Sprite::rotationChanged(float deltaRadians) {
    	if (!USE_CHIPMUNK) return;
        // do this anytime we change rotation
        cpBodySetAngle(mBody, mFacing);
        if (mStatic) {
            cpSpaceReindexStatic(getSpace());
        } else {
            cpSpaceReindexShapesForBody(getSpace(), mBody);
        }
    }

    void	
    Sprite::centerChanged(const Offset& delta) {
        if (USE_CHIPMUNK && !mAnimating) {
            // only do when not changed by animate() call because we
            // don't want to recalc this multiple times if location changes too
            cpVect v;
            v.x = mLocation.y + mCenterOffset.x; v.y = mLocation.y + mCenterOffset.y;
            cpBodySetPos(mBody, v);
            if (mStatic) {
                cpSpaceReindexStatic(getSpace());
            } else {
                cpSpaceReindexShapesForBody(getSpace(), mBody);
            }
        }
    }
    
    void		
    Sprite::applyForce(const Vector& force, int32 msDuration) {
    	if (!USE_CHIPMUNK) {
    		Animated::applyForce(force, msDuration);
    		return;
    	}
        cpVect v;
        v.x = force.x; v.y = force.y;
        if (msDuration == duration_Instantaneous) {
            cpBodyApplyImpulse(mBody, v, cpvzero);
        } else {
            cpBodyApplyForce(mBody, v, cpvzero);
        }
    }

    void		
    Sprite::applyTorque(float forceSpin, int32 msDuration) {
    	if (!USE_CHIPMUNK) {
    		Animated::applyTorque(forceSpin, msDuration);
    		return;
    	}
        if (msDuration == duration_Instantaneous) {
            cpFloat torque = cpBodyGetTorque(mBody);
            torque += forceSpin;
            cpBodySetTorque(mBody, torque);
        } else {
            // TODO: apply two forces in opposite directions each offset from the center? 
        }
    }

    void		
    Sprite::stopAllForces() {
    	if (!USE_CHIPMUNK) {
    		Animated::stopAllForces();
    		return;
    	}
        cpBodyResetForces(mBody);
    }

    Animated&
    Sprite::setVelocity(const Vector& delta) {
    	if (!USE_CHIPMUNK) {
    		Animated::setVelocity(delta);
    		return *this;
    	}
        cpVect v;
        v.x = delta.x;
        v.y = delta.y;
        cpBodySetVel(mBody, v);
        return *this;
    }
    
    Vector
    Sprite::getVelocity() {
    	if (!USE_CHIPMUNK) {
    		return Animated::getVelocity();
    	}
        cpVect v = cpBodyGetVel(mBody);
        return Vector(v.x, v.y);
    }

    void
    Sprite::setupCollideGroup(Sprite* otherSprite) {
    	if (!USE_CHIPMUNK) return;
        // try to get a valid collideGroup
        if (!mCollideGroup) mCollideGroup = otherSprite->mCollideGroup; // maybe the other sprite has it set?
        if (!mCollideGroup) mCollideGroup = spriteId;               // grab a possibly usable value
        if (!mCollideGroup) mCollideGroup = otherSprite->spriteId;
        DEBUG_ONLY(
            if (mCollideShape && otherSprite->mCollideShape) {
                DEBUG_ASSERT(mCollideGroup != 0, "Must have a collideGroup (or spriteId) assigned to use joints with collisions enabled!!");
            }
        )
        if (mCollideGroup) {
            otherSprite->mCollideGroup = mCollideGroup; // make sure both sprites agree on the collide group
            if (mCollideShape) {
                cpShapeSetGroup(mCollideShape, mCollideGroup);
            }
            if (otherSprite->mCollideShape) {
                cpShapeSetGroup(otherSprite->mCollideShape, mCollideGroup); // use our spriteId
            }
        }
    }

    void
    Sprite::makeJointBreakable(cpConstraint* joint, float breakingForce, Sound* breakSound) {
    	if (!USE_CHIPMUNK) return;
        DEBUG_ASSERT(breakingForce > 0, "Breaking Force must be > 0");
        DEBUG_ASSERT(mNumBreakableJoints < MAX_BREAKABLE_JOINTS_PER_SPRITE, "Too many breakable joints!!");
        if ((breakingForce > 0) && (mNumBreakableJoints < MAX_BREAKABLE_JOINTS_PER_SPRITE)) {
            cpConstraintSetMaxForce(joint, breakingForce / BREAK_COEFFICIENT);
            mBreakableJoints[mNumBreakableJoints] = joint;
            mNumBreakableJoints++;
        }
    }

    void
    Sprite::makeJointUnbreakable(cpConstraint* joint) {
    	if (!USE_CHIPMUNK) return;
        bool found = false;
        for (int i = 0; i < mNumBreakableJoints; i++) {
            // remove from the breakable joints array
            if (mBreakableJoints[i] == joint) {
                found = true;
                mNumBreakableJoints--;
            }
            if (found && (i < mNumBreakableJoints-1) ) {
                mBreakableJoints[i] = mBreakableJoints[i+1];
            }
        }
    }

    // pin sprites together, at a particular anchor point (offset from center) on each
    cpConstraint*            
    Sprite::pinJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpPinJointNew(mBody, otherSprite->mBody, 
                                          cpv(anchor.x, anchor.y), cpv(otherAnchor.x, otherAnchor.y));
        cpPinJointSetDist(con, 0);  // no distance between anchor points
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // join sprites together via a slider between anchor points that has a min/max distance
    cpConstraint*            
    Sprite::slideJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float minDist, float maxDist, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpSlideJointNew(mBody, otherSprite->mBody, 
                                            cpv(anchor.x, anchor.y), cpv(otherAnchor.x, otherAnchor.y), 
                                            minDist, maxDist);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // join sprites together at a particular location in layer coordinates
    cpConstraint*            
    Sprite::pivotJoint(Sprite* otherSprite, Point pivot, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpPivotJointNew(mBody, otherSprite->mBody, cpv(pivot.x, pivot.y));
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
//        cpFloat errorBias = pow(1.0 - 0.25, 60.0);  // 25% correction every 1/60th of a second
//        cpConstraintSetErrorBias(con, errorBias);
        return con;
    }

    // join sprites together via a groove on this sprite to an anchor point on another
    cpConstraint*            
    Sprite::grooveJoint(Offset grooveStart, Offset grooveEnd, Sprite* otherSprite, Offset otherAnchor, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpGrooveJointNew(mBody, otherSprite->mBody, 
                                             cpv(grooveStart.x, grooveStart.y), cpv(grooveEnd.x, grooveEnd.y), 
                                             cpv(otherAnchor.x, otherAnchor.y));    
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // join sprites together via a spring between anchor points
    cpConstraint*            
    Sprite::springJoint(Offset anchor, Sprite* otherSprite, Offset otherAnchor, float restLength, float stiffness, float damping, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpDampedSpringNew(mBody, otherSprite->mBody, 
                                              cpv(anchor.x, anchor.y), cpv(otherAnchor.x, otherAnchor.y), 
                                              restLength, stiffness, damping);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // keep sprites at a particular angle relative to one another via a rotary spring
    cpConstraint*            
    Sprite::rotarySpring(Sprite* otherSprite, float restAngle, float stiffness, float damping, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpDampedRotarySpringNew(mBody, otherSprite->mBody, restAngle, stiffness, damping);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // limit the angle another sprite can have relative to this one
    cpConstraint*            
    Sprite::rotaryLimit(Sprite* otherSprite, float minAngle, float maxAngle, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpRotaryLimitJointNew(mBody, otherSprite->mBody, minAngle, maxAngle);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // like a socket wrench. ratchetInterval is the distance between “clicks”, phase is the initial 
    // angular offset to use when deciding where the ratchet angles are.
    cpConstraint*            
    Sprite::ratchet(Sprite* otherSprite, float rachetInterval, float phase, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpRatchetJointNew(mBody, otherSprite->mBody, phase, rachetInterval);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // keep another sprite's rotation relative to this one at a particular gear ratio
    cpConstraint*            
    Sprite::gear(Sprite* otherSprite, float gearRatio, float initialAngle, float breakingForce) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpGearJointNew(mBody, otherSprite->mBody, initialAngle, gearRatio);
        cpSpaceAddConstraint(getSpace(), con);
        if (breakingForce > 0) {
            makeJointBreakable(con, breakingForce);
        }
        return con;
    }

    // keep spin of another sprite at a constant rate compared to this one
    cpConstraint*            
    Sprite::motor(Sprite* otherSprite, float spin, float maxTorque) {
    	if (!USE_CHIPMUNK) return 0;
        setupCollideGroup(otherSprite);
        cpConstraint* con = cpSimpleMotorNew(mBody, otherSprite->mBody, spin);
        if (maxTorque != std::numeric_limits<float>::infinity()) {
            cpConstraintSetMaxForce(con, maxTorque);
        }
        cpSpaceAddConstraint(getSpace(), con);
        return con;
    }
    
    static void RemoveConstraint(cpBody* body, cpConstraint* constraint, void* data);
    static void RemoveConstraint(cpBody* body, cpConstraint* constraint, void* data) {
        cpBody* a = cpConstraintGetA(constraint);
        cpBody* b = cpConstraintGetB(constraint);
        if (data && (b != ((Sprite*)data)->mBody)) {
            return;
        }
        Sprite* sprite = (Sprite*)cpBodyGetUserData(a);
        sprite->removeJoint(constraint);
    }
    
    void 
    Sprite::removeJoint(cpConstraint* joint) {
    	if (!USE_CHIPMUNK) return;
        makeJointUnbreakable(joint);
        cpSpaceRemoveConstraint(cpConstraintGetSpace(joint), joint);
        cpConstraintFree(joint);
    }
    
    // remove all connections, or if otherSprite != 0 remove all connections to other sprites
    void            
    Sprite::disconnect(Sprite* otherSprite) {
    	if (!USE_CHIPMUNK) return;
        if (!otherSprite) {
            mNumBreakableJoints = 0; // this saves us having to do a lot of manipulation of the mBreakableJoints array
        }
        cpBodyEachConstraint(mBody, RemoveConstraint, otherSprite);
    }

    Sprite&
    Sprite::makeStatic() {
    	if (!USE_CHIPMUNK) return *this;
        DEBUG_ASSERT(cpBodyIsStatic(mBody) == false, "Sprite::makeStatic called twice on same sprite!");
        DEBUG_ASSERT(mCollideShape == 0, "You must call Sprite::makeStatic before setting up collisions");
        disconnect();
        cpSpace* space = getSpace();
        cpSpaceRemoveBody(space, mBody);
        cpBodyInitStatic(mBody); // re-initialize as a static
        cpBodySetUserData(mBody, this);
        mStatic = true;
        return *this;
    }

    cpSpace*
    Sprite::getSpace() {
    	if (!USE_CHIPMUNK) return 0;
        if (cpBodyIsStatic(mBody) || cpBodyIsRogue(mBody)) {
            return mLayer->getSpace();
        } else {
            return cpBodyGetSpace(mBody);
        }
    }

    void
    Sprite::initCpBody() {
    	if (!mBody) {
    		if (!mLayer->mIsStaticLayer) {
				mBody = cpBodyNew(1, 1);
				cpSpaceAddBody(getSpace(), mBody);
			} else {
				mBody = cpBodyNewStatic(); // initialize as a static
				mStatic = true;		
			}
			cpBodySetUserData(mBody, this);
		}
    }
    
    void
    Sprite::freeCpBody() {
		if (mCollideShape) {
			cpSpaceRemoveShape(getSpace(), mCollideShape);
			cpShapeFree(mCollideShape);
		}
		if (mBody) {
			cpSpaceRemoveBody(getSpace(), mBody);
			cpBodyFree(mBody);
    	}
		mCollideShape = 0;
		mBody = 0;
    }


#endif // PDG_USE_CHIPMUNK_PHYSICS

void
Sprite::recalcOnscreenAndInBounds() {
	RotatedRect srb = getRotatedBounds();
//  	SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("Sprite [%p] recalc inbounds [%0.2f,%0.2f,%0.2f,%0.2f r %0.2f]", this, srb.left, srb.top, srb.right, srb.bottom, srb.radians); )
  #ifndef PDG_NO_GUI
	if (wantsOffscreen && mPort) {
		Rect portBounds = mPort->getDrawingArea();
		Rect portSb = mLayer->layerToPort(srb).getBounds();
		mOnscreen = portSb.overlaps(portBounds);
	}
  #endif
	if (wantsWallCollide) {
		Rect lb = mLayer->getRotatedBounds();
		Rect sb = srb.getBounds();
//  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("  lb [%0.2f,%0.2f,%0.2f,%0.2f] sb [%0.2f,%0.2f,%0.2f,%0.2f]", lb.left, lb.top, lb.right, lb.bottom, sb.left, sb.top, sb.right, sb.bottom ); )
		mInBounds = lb.overlaps(sb);
		mCompletelyInBounds = lb.contains(sb);
//  		SPRITEANIMATE_DEBUG_ONLY( OS::_DOUT("  ib: %d  cib: %d", mInBounds, mCompletelyInBounds ); )
	}
}


Sprite::Sprite() : 
    spriteId(0), 
    wantsMouseOver(false), 
    wantsClicks(false), 
    wantsAnimLoop(false), 
    wantsAnimEnd(false), 
	wantsOffscreen(false),
	wantsWallCollide(false),
    userData(0),	
    mNumFrames(0), 
	mFps(1.0),
	mCurrFramePrecise(0),
	mCurrFrame(0),
	mFirstFrame(0),
	mLastFrame(0),
	mLoopAnim(false),
	mBackToFrontAnim(false),
	mBidirectionalAnim(false),
	mSpriteAnimating(false),
	mSpriteAnimatingBackwardsNow(false),
	mDoCollisions(false),
	mMouseDetectMode(collide_BoundingBox),
  #ifdef PDG_SCML_SUPPORT
	mEntity(0),
	mEntityScaleX(1.0f),
	mEntityScaleY(1.0f),
  #endif
  #ifndef PDG_NO_GUI
	mPort(0),
  #endif
	mLayer(0), 
	mNextSprite(0),
	mPrevSprite(0),
	mOpacity(1.0),
	mFadeCompleteAction(0),
  #ifndef PDG_NO_GUI
	mDrawHelper(0),
	mPostDrawHelper(0),
  #endif
  	mCollisionHelper(0),
  	mOnscreen(true),
  	mInBounds(true),
  	mCompletelyInBounds(true),
	iid(sUniqueSpriteId++)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mSpriteScriptObj);
#endif
	mLocation = PointT<float>(0.0, 0.0);
#ifdef PDG_USE_CHIPMUNK_PHYSICS
	mBody = 0;
    mHeight = 1;    // make sure we've got some height and width
    mWidth = 1;
    mCollideShape = 0; // only create this when we set collisions
    mNumBreakableJoints = 0;
    mStatic = false;
#endif
//	mBounds = Rect(20, 20);
//	DEBUG_PRINT("Constructed Sprite [%p]", this);
}

//Sprite::Sprite(const Sprite* sprite) : mLayer(0) {
//	TODO("Copy over state from sprite");
//	DEBUG_PRINT("ERROR! Copy constructed Sprite [%p]", this);
//	DEBUG_BREAK("Unimplemented call!");
//}
//
Sprite::~Sprite() {
    //                DEBUG_ONLY( OS::_DOUT("dt Sprite %p", this) = 0; )
  #ifndef PDG_NO_GUI
	if (mDrawHelper && mDrawHelper->ownedBySprite()) {
		delete mDrawHelper;
		mDrawHelper = 0;
	}
	if (mPostDrawHelper && mPostDrawHelper->ownedBySprite()) {
		delete mPostDrawHelper;
		mPostDrawHelper = 0;
	}
  #endif
    if (userData) {
        userData->release();
        userData = 0;
    }
  #ifdef PDG_USE_CHIPMUNK_PHYSICS
    if (mLayer && USE_CHIPMUNK) {
		disconnect();   // remove all constraints
		freeCpBody();
    }
  #endif
  #ifdef PDG_SCML_SUPPORT
	if (mEntity) {
		delete mEntity;
		mEntity = 0;
	}
  #endif
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupSpriteScriptObject(mSpriteScriptObj);
  #endif
//	DEBUG_PRINT("Destroyed Sprite [%p]", this);
}


	
} // end namespace pdg
