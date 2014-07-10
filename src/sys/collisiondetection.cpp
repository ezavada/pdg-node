// -----------------------------------------------
// collisiondetection.cpp
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

#include "collisiondetection.h"

#include <vector>

#ifdef DEBUG_PIXEL_COLLISIONS
namespace parthenon {
	class WorldPart;
	extern WorldPart* gWorldPart;
}
#include "view/WorldPart.h"
#endif // DEBUG_PIXEL_COLLISIONS



namespace pdg {

bool CollisionDetection::detectRadiusCollision(const Point &locA, float radiusA, const Point &locB, float radiusB) {
	// don't need to compare actual distance, just compare squared distance
	float distanceSquared = (locA.x-locB.x)*(locA.x-locB.x) + (locA.y-locB.y)*(locA.y-locB.y);
	if ( distanceSquared <= (radiusA*radiusA) + (radiusB*radiusB) ) {
		return true;
	}
	return false;
}

bool CollisionDetection::detectBoundingBoxCollision(const RotatedRect &rectA, const RotatedRect &rectB) {
	/********************************* separating axis theorem ********************************
	http://www.gamedev.net/reference/programming/features/2dRotatedRectCollision/default.asp */
		
	// each rect has 2 unique axes, so we must check projection against 4 total
	std::vector<pdg::Vector> axes;
	Quad quadA = rectA.getQuad();
	Quad quadB = rectB.getQuad();
	axes.push_back( pdg::Vector(quadA.points[rgtBot] - quadA.points[lftBot]) );
	axes.push_back( pdg::Vector(quadA.points[rgtBot] - quadA.points[rgtTop]) );
	axes.push_back( pdg::Vector(quadB.points[lftBot] - quadB.points[lftTop]) );
	axes.push_back( pdg::Vector(quadB.points[lftBot] - quadB.points[rgtBot]) );

	float minA, minB, maxA, maxB, d, scalar;
	for (int i=0; i<4; i++){
		// reset min and max
		minA = minB = 100000000;
		maxA = maxB = -100000000;
		
		const float xAxesSq = axes[i].x * axes[i].x;
		const float yAxesSq = axes[i].y * axes[i].y;
		const float lenAxisSq = xAxesSq + yAxesSq;
		for (int j=0; j<4; j++) {
			// project the jth point of rectA against the ith axis
			d = pdg::Vector(quadA.points[j]).dotProduct(axes[i]);
			d = d / lenAxisSq;
			// find our scalar along the ith axis
			scalar = (d * xAxesSq) + (d * yAxesSq);
			// check min and max for recA
			if (scalar < minA) minA = scalar;
			if (scalar > maxA) maxA = scalar;
				
			// project the jth point of rectB against the ith axis
			d = pdg::Vector(quadB.points[j]).dotProduct(axes[i]);
			d = d / lenAxisSq;
			// find the scalar along the ith axis
			scalar = (d * xAxesSq) + (d * yAxesSq);
			// check min and max for recB		
			if (scalar < minB) minB = scalar;
			if (scalar > maxB) maxB = scalar;
		}
		// test condition, must pass for all 4 axes
		if (minB > maxA || maxB < minA) return false;
	}
	// if all 4 axes pass the condition, we have a collision
	return true;
}

		
bool CollisionDetection::detectPixelCollision(const RotatedRect &rectA, const RotatedRect &rectB, const Image* imageA, const Image* imageB, 
											   const Rect &imageBoundsA, const Rect &imageBoundsB, TileLayer::TFacing facingB, uint8 alphaThreshold,
											   uint32 *outOverlapCount, float *outThresholdExcess) {
	if (outOverlapCount) {
		*outOverlapCount = 0;
	}
	if (outThresholdExcess) {
		*outThresholdExcess = 0.0f;
	}
	bool collided = false;
	if ( true == detectBoundingBoxCollision(rectA, rectB) ) {
		// rotate B to align with axes, and move A along with it, maintaining A's position relative to B
		Quad quadA = rectA.getQuad();
		Quad quadB = rectB.getQuad();
		Point centerSeparation = quadB.centerPoint() - quadA.centerPoint();
		float rotA = rectA.radians;
		float rotB = rectB.radians;
		Point coB = rectB.centerOffset;
		quadB.rotate(-rotB, coB);
		coB += centerSeparation;
		quadA.rotate(-rotB, coB);
		rotA -= rotB;
		
		Rect boundsA = quadA.getBounds();
		Rect boundsB = quadB.getBounds();
		
		// find the intersection of the two rectangles
		Rect r = boundsA.intersection(boundsB);
		// find the the rotated rect A unrotated rect B
		// then make A, B and r relative to r, so r defines our coordinate system
		Rect B = boundsB;
		Quad A = quadA;
#ifdef DEBUG_PIXEL_COLLISIONS
		Rect debugIntersect = r; // save onscreen r for debug purposes
#endif
		B.moveLeft(r.left);
		B.moveUp(r.top);
		A.moveLeft(r.left);
		A.moveUp(r.top);
		r.moveTo(0,0);
		
#ifdef DEBUG_PIXEL_COLLISIONS
		Point debugPt;
		Point imgPointB;
		Point imgPointA;
		if (parthenon::gWorldPart && (alphaThreshold == 25)) {
			debugPt = parthenon::gWorldPart->getOrigin();
			Quad tq = rectA.getQuad();
			tq.moveUp(debugPt.y);
			tq.moveLeft(debugPt.x);
			parthenon::gWorldPart->addCollisionDebugRect(tq, Color(0.0f, 0.0f, 1.0f, 0.5f));
			tq = rectB.getQuad();
			tq.moveUp(debugPt.y);
			tq.moveLeft(debugPt.x);
			parthenon::gWorldPart->addCollisionDebugRect(tq, Color(0.0f, 1.0f, 0.0f, 0.5f));
			debugPt.x -= 200;
			tq = quadA;
			tq.moveUp(debugPt.y);
			tq.moveLeft(debugPt.x);
			parthenon::gWorldPart->addCollisionDebugRect(tq, Color(0.0f, 0.0f, 1.0f, 0.5f));
			tq = quadB;
			tq.moveUp(debugPt.y);
			tq.moveLeft(debugPt.x);
			imgPointB = tq.points[lftTop];
			parthenon::gWorldPart->addCollisionDebugRect(tq, Color(0.0f, 1.0f, 0.0f, 0.5f));
			debugIntersect.moveUp(debugPt.y);
			debugIntersect.moveLeft(debugPt.x);
			parthenon::gWorldPart->addCollisionDebugRect(debugIntersect, Color(1.0f, 0.0f, 0.0f, 0.50f));
		}
#endif
		
		// let A' = A, R' = r
		// then rotate A to align with axes, and move r along with it, maintaining r's position relative to A
		Quad quadR_prime = r;
		Quad quadA_prime = A;
		centerSeparation = quadA_prime.centerPoint() - quadR_prime.centerPoint();
		quadA_prime.rotate(-rotA);
		quadR_prime.rotate(-rotA, centerSeparation);
		// offset both to be relative to A
		Rect A_prime = quadA_prime.getBounds();
		quadR_prime.moveUp(A_prime.top);
		quadR_prime.moveLeft(A_prime.left);
		A_prime.moveTo(0,0);
		
#ifdef DEBUG_PIXEL_COLLISIONS
		if (parthenon::gWorldPart && (alphaThreshold == 25)) {
			Rect debugArea = A_prime.unionWith(quadR_prime.getBounds());
			Rect ta = A_prime;
			ta.moveUp(debugArea.top);
			ta.moveLeft(debugArea.left);
			imgPointA = ta.leftTop();
			parthenon::gWorldPart->addCollisionDebugRect(ta, Color(0.0f, 0.0f, 1.0f, 0.5f));
			Quad tq = quadR_prime;
			tq.moveUp(debugArea.top);
			tq.moveLeft(debugArea.left);
			parthenon::gWorldPart->addCollisionDebugRect(tq, Color(1.0f, 0.0f, 0.0f, 0.5f));
		}
#endif
		
		float imgA_v_ratio = imageBoundsA.height()/rectA.height();
		float imgA_h_ratio = imageBoundsA.width()/rectA.width();

		float mR = (quadR_prime.points[rgtTop].x == quadR_prime.points[lftTop].x) ?
			0.0f : ((quadR_prime.points[rgtTop].y - quadR_prime.points[lftTop].y) / (quadR_prime.points[rgtTop].x - quadR_prime.points[lftTop].x));

		float imgB_v_ratio = imageBoundsB.height()/rectB.height();
		float imgB_h_ratio = imageBoundsB.width()/rectB.width();
		
		float imgA_width = imageBoundsA.width()/imgA_h_ratio;
		float imgA_height = imageBoundsA.height()/imgA_v_ratio;
		
		for (int y = 0; y < r.bottom; y++) {
			float fy = y;
			float b_x = -B.left;
			float b_y = fy - B.top;
			float fy_mR = fy * -mR;
			uint8 b_a = 255;
			for (int x = r.left; x < r.right; x++) {
				if (imageB) {
					long lx, ly;
					if ((facingB == TileLayer::facing_North) || (facingB == TileLayer::facing_Ignore)) {
						lx = b_x * imgB_h_ratio + imageBoundsB.left;
						ly = b_y * imgB_v_ratio + imageBoundsB.top;
					} else if (facingB == TileLayer::facing_South) {
						lx = -b_x * imgB_h_ratio + imageBoundsB.right;
						ly = -b_y * imgB_v_ratio + imageBoundsB.bottom;					
					} else if (facingB == TileLayer::facing_West) {
						lx = -b_y * imgB_h_ratio + imageBoundsB.right;
						ly = b_x * imgB_v_ratio + imageBoundsB.top;					
					} else { // if (facingB == TileLayer::facing_East) 
						lx = b_y * imgB_h_ratio + imageBoundsB.left;
						ly = -b_x * imgB_v_ratio + imageBoundsB.bottom;					
					}
	//#ifdef DEBUG_PIXEL_COLLISIONS
	//				Color bp = imageB->getPixel(lx, ly);
	//				uint8 b_a = bp.alpha * 255;
	//#else
					b_a = imageB->getAlphaValue(lx, ly);
				}
//#endif
				b_x += 1.0f;
				if (b_a > alphaThreshold) {
#ifdef DEBUG_PIXEL_COLLISIONS
					if (parthenon::gWorldPart && (alphaThreshold == 25)) {
						if ((x % 2 == 0) && (y % 2 == 0)) {
							parthenon::gWorldPart->addCollisionDebugMaskPoint(b_x + imgPointB.x, 
																			  b_y + imgPointB.y, 
																			  Color(0.0f,1.0f,0.0f,1.0f)); //bp);
						}
					}
#endif
					float fx = x;
					float a_x = fx + fy_mR + quadR_prime.points[lftTop].x;
					float a_y = fy + (fx * mR) + quadR_prime.points[lftTop].y;
					if ((a_x >= 0) && (a_y >= 0) && (a_x < imgA_width) && (a_y < imgA_height)) {
//#ifdef DEBUG_PIXEL_COLLISIONS
//						Color ap = imageA->getPixel(a_x * imgA_h_ratio + imageBoundsA.left, a_y * imgA_v_ratio + imageBoundsA.top);
//						uint8 a_a = ap.alpha * 255;
//#else
						uint8 a_a = imageA->getAlphaValue(a_x * imgA_h_ratio + imageBoundsA.left, a_y * imgA_v_ratio + imageBoundsA.top);
//#endif
						if (a_a > alphaThreshold) {
							collided = true;
#ifdef DEBUG_PIXEL_COLLISIONS
							if (parthenon::gWorldPart && (alphaThreshold == 25)) {
								if (true) { ///*(x % 2 == 0) && */ (y % 2 == 0)) {
									parthenon::gWorldPart->addCollisionDebugMaskPoint(a_x + imgPointA.x, 
																					  a_y + imgPointA.y, 
																					  Color(0.0f,1.0f,0.0f,1.0f)); //ap);
								}
							}
#else
							if (!outThresholdExcess && !outOverlapCount) {
								return true;
							}
#endif
							if (outOverlapCount) {
								*outOverlapCount += 1;
							}
							if (outThresholdExcess) {
								*outThresholdExcess += (float)((a_a - alphaThreshold) + (b_a - alphaThreshold))/510.0f;
							}
						}
					}
				}
			}
		}
		
	}
	return collided;
}

	
} // end namespace
