// -----------------------------------------------
//  Spline.cpp
//
// Written by Justin Holmes, 2010
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

#include "math.h"
#include "pdg/sys/spline.h"

namespace pdg {
	
	Point Spline::getFirstOrder(float u) {
		float X,Y;
		
		switch (mType) {
			case SPLINE_HERMITE:
				X = mPoints[0].x * (2*u*u*u - 3*u*u + 1);
				X += mPoints[1].x * (-2*u*u*u + 3*u*u);
				X += mPoints[2].x * (u*u*u - 2*u*u + u);
				X += mPoints[3].x * (u*u*u - u*u);
				
				Y = mPoints[0].y * (2*u*u*u - 3*u*u + 1);
				Y += mPoints[1].y * (-2*u*u*u + 3*u*u);
				Y += mPoints[2].y * (u*u*u - 2*u*u + u);
				Y += mPoints[3].y * (u*u*u - u*u);
				break;
				
// 			case SPLINE_CARDINAL:
// 				
// 				break;
// 				
// 			case SPLINE_UNIFORM_B:
// 				break;
		}
		return PointT<float>(X,Y);
	}
	
	
	Point Spline::getSecondOrder(float u) {
		float X,Y;
		
		switch (mType) {
			case SPLINE_HERMITE:
				X = mPoints[0].x * (6*u*u - 6*u);
				X += mPoints[1].x * (-6*u*u + 6*u);
				X += mPoints[2].x * (3*u*u - 4*u + 1);
				X += mPoints[3].x * (3*u*u - 2*u);
				
				Y = mPoints[0].y * (6*u*u - 6*u);
				Y += mPoints[1].y * (-6*u*u + 6*u);
				Y += mPoints[2].y * (3*u*u - 4*u + 1);
				Y += mPoints[3].y * (3*u*u - 2*u);
				break;
				
// 			case SPLINE_CARDINAL:
// 				
// 				break;
// 			
// 			case SPLINE_UNIFORM_B:
// 	
// 				break;
		}
		return PointT<float>(X,Y);
	}
	
	void Spline::initialize(Point a, Point b, Point c, Point d) {
		mPoints[0] = a;
		mPoints[1] = b;
		mPoints[2] = c;
		mPoints[3] = d;
		setIndex(0);
	}
	
	
	void Spline::pushBackPoint(Point p) {
		mPoints.push_back(p);
	}
	
	void Spline::setIndex(unsigned long i) {
		if (mPoints.size() < 4)
			return;
		
		int j = 0;
		if (mLooping)	{
			while (j < 4) {
				mIndex[j] = i + j;
				mIndex[j] = mIndex[j] % mPoints.size();
				j++;
			}
		}
		else {
			while (j < 4) {
				mIndex[j] = i + j;
				if (mIndex[j] == mPoints.size()) {
					setIndex(0);
					return;
				}
			}
			
		}

	}
	
	void Spline::incrementIndex() {
		if (mPoints.size() < 4)
			return;
		
		int i =0;
		if (mLooping) {
			while (i<4) {
				mIndex[i]++;
				mIndex[i] = mIndex[i] % mPoints.size();
				i++;
			}
		}
		else {
			while (i<4) {
				mIndex[i]++;
				if (mIndex[i] == mPoints.size()) {
					setIndex(0);
					return;
				}
			}
		}

	}
	
} // end namespace parthenon
