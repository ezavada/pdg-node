// -----------------------------------------------
//  Spline.h
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


#ifndef PDG_SPLINE_H_INCLUDED
#define PDG_SPLINE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/coordinates.h"
#include "pdg/sys/global_types.h"
#include <vector>

#define SPLINE_HERMITE		1
#define SPLINE_CARDINAL		2
#define SPLINE_UNIFORM_B	3	


namespace pdg {
	
	typedef PointT<float> Point;
	
	class Spline
	{
	public:	
		Spline(int type) : mType(type) {};
		~Spline() {};
		
		Point getFirstOrder(float u);
		Point getSecondOrder(float u);
		
		void initialize(Point a, Point b, Point c, Point d);
		
		void pushBackPoint(Point p);
		void setIndex(unsigned long i);
		void incrementIndex();
		
	private:
		int mType;
		std::vector< Point > mPoints;
		bool mLooping;
		unsigned long mIndex[4];
	};	
	
	
}

#endif // SPLINE_H_INCLUDED

