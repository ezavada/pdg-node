// -----------------------------------------------
// collisiondetection.h
// 
// Implementation of collision detection algorithms.
//
// Written by Aaron Buchanan, 2010
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


#ifndef COLLISION_DETECTION_H_INCLUDED
#define COLLISION_DETECTION_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/sprite.h"
#include "pdg/sys/tilelayer.h"

// uncomment line below to debug per-pixel collisions
//#define DEBUG_PIXEL_COLLISIONS


namespace pdg {

	class Image;
	
	namespace CollisionDetection {

		bool detectRadiusCollision(const Point &locA, float radiusA, const Point &locB, float radiusB);
			// returns true if there is an overlap between the two circles, otherwise false

		bool detectBoundingBoxCollision(const RotatedRect &rectA, const RotatedRect &rectB);
			// returns true if there is an overlap between the two rectangles, otherwise false

		bool detectPixelCollision(const RotatedRect &rectA, const RotatedRect &rectB, 
								  const Image* imageA, const Image* imageB, 
								  const Rect &imageBoundsA, const Rect &imageBoundsB, 
								  TileLayer::TFacing facingB = TileLayer::facing_Ignore, uint8 alphaThreshold = 128, 
								  uint32 *outOverlapCount = 0, float *outThresholdExcess = 0);
			// returns true if any pixels in the overlapping region of rectA and rectB
			//	exceed the specified alphaThreshold
			// this starts with a boundingBoxCollision, so there is no need to check
			//	the bounding box before calling this.
			// imageB can be null if you want to assume collision against a solid object that is 100% opaque
	}
}

#endif // COLLISION_DETECTION_H_INCLUDED
