// -----------------------------------------------
// coordinates.h
//
// Definitions for coordinate classes
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


#ifndef PDG_COORDINATES_H_INCLUDED
#define PDG_COORDINATES_H_INCLUDED

#include "pdg_project.h"

#ifndef PDG_BASE_COORD_TYPE
#define PDG_BASE_COORD_TYPE float
#endif

#ifdef RUBY_DOX
#define T float
#endif

#ifdef __cplusplus

#include <cmath>  // for sin() and cos()

namespace pdg {

//! \cond C++
inline int fround2i(float x) {
	return int(x > 0.0 ? x + 0.5 : x - 0.5);
}
	
template <typename T> class RotatedRectT;
template <typename T> class QuadT;
//! \endcond

// -----------------------------------------------------------------------------------
// PointT template
// -----------------------------------------------------------------------------------
/** Point for 2D coordinate system.
 * Point is a provides support for dealing with points as x, y floating point values
 * in 2 dimensional space. 
 * \if C++
 * PointT is not used directly in most cases. Use the Point class, which
 * is the long integer version of PointT, and has all the same methods.
 * PointT is a template class that provides support for dealing with points in
 * 2 dimensional space. A floating point version of PointT is predefined
 * as Point, but a long integer point version of PointT could easily be
 * declared using the PointT template.
 * \endif
 */
//! \cond C++
template <typename T>
//! \endcond
class OffsetT {
public:
    //! x coordinate of point on a grid or screen
    T    x;
    //! y coordinate of point on a grid or screen
    T    y;
    // operators
    //! return true if this point is equal to the other
    bool operator== (const OffsetT<T>& p2) const;
    //! return true if this point is not equal to the other
    bool operator!= (const OffsetT<T>& p2) const;
    //! set point equal to another
    OffsetT<T>& operator= (const OffsetT<T>& p2);
    //! add two points together by adding x & y coordinates
    OffsetT<T> operator+ (const OffsetT<T>& p2) const;
    //! subtract a point from another by subtracting x & y coordinates
    OffsetT<T> operator- (const OffsetT<T>& p2) const;
    //! add another point to this one by adding x & y coordinates
    OffsetT<T>& operator+= (const OffsetT<T>& p2);
    //! subtract another point from this one by subtracting x & y coordinates
    OffsetT<T>& operator-= (const OffsetT<T>& p2);
    //! scale the offset
    OffsetT<T>& operator*= (float n);
    //! scale the offset
    OffsetT<T>& operator/= (float n);
    //! scale an offset from by a factor
    OffsetT<T> operator* (float n) const;
    //! scale an offset from by a factor
    OffsetT<T> operator/ (float n) const;
	//! see if it's (0,0)
	bool zero() const;
    // constructors
	//! create a point and set its x & y values
    inline OffsetT(T ix, T iy) : x(ix), y(iy) {}
	//! create a point at the origin (0,0)
    inline OffsetT() : x(0), y(0) {}
};

//! \cond C++
template <typename T>
//! \endcond
class PointT : public OffsetT<T> {
public:
	// math
	//! get distance from another point
	float distance(const PointT<T>& p2) const;
	//! do a projection of a second point onto this point
	PointT<T> projection(const PointT<T> &pointB) const;
    // constructors
	//! create a point and set its x & y values
    inline PointT(const OffsetT<T>& o) : OffsetT<T>(o.x, o.y) {}
	//! create a point and set its x & y values
    inline PointT(T ix, T iy) : OffsetT<T>(ix, iy) {}
	//! create a point at the origin (0,0)
    inline PointT() : OffsetT<T>() {}
};

//! \cond C++
template <typename T>
//! \endcond
class VectorT : public OffsetT<T> {
public:
    // math
	//! get dot product for this vector with a 2nd vector
	float dotProduct(const VectorT<T> &vectorB) const;
	//! get length as a vector (distance from origin)
    float vectorLength() const;
    //! get angle (in radians) for this vector
    float vectorAngle() const;
    //! get the Normal (vector with length 1) for this vector
    VectorT<T> normal() const;
    // constructors
	//! create a vector and set its x & y values
    inline VectorT(const OffsetT<T>& o) : OffsetT<T>(o.x, o.y) {}
	//! create a point and set its x & y values
    inline VectorT(const PointT<T>& p) : OffsetT<T>(p.x, p.y) {}
    //! create a point and set its x & y values
    inline VectorT(T ix, T iy) : OffsetT<T>(ix, iy) {}
    //! create a point at the origin (0,0)
    inline VectorT() : OffsetT<T>() {}
};

//! \cond C++

template <typename T>
inline bool OffsetT<T>::operator== (const OffsetT<T>& p2) const {
	return ((x == p2.x) && (y == p2.y));
}

template <typename T>
inline bool OffsetT<T>::operator!= (const OffsetT<T>& p2) const {
	return ((x != p2.x) || (y != p2.y));
}

template <typename T>
inline OffsetT<T>& OffsetT<T>::operator= (const OffsetT<T>& p2) {
	x = p2.x;
	y = p2.y;
	return *this;
}

template <typename T>
inline OffsetT<T> OffsetT<T>::operator+ (const OffsetT<T>& p2) const {
	OffsetT<T> rp;
	rp.x = x + p2.x;
	rp.y = y + p2.y;
	return rp;
}

template <typename T>
inline OffsetT<T> OffsetT<T>::operator- (const OffsetT<T>& p2) const {
	OffsetT<T> rp;
	rp.x = x - p2.x;
	rp.y = y - p2.y;
	return rp;
}

template <typename T>
inline OffsetT<T>& OffsetT<T>::operator+= (const OffsetT<T>& p2) {
	x += p2.x;
	y += p2.y;
	return *this;
}

template <typename T>
inline OffsetT<T>& OffsetT<T>::operator-= (const OffsetT<T>& p2) {
	x -= p2.x;
	y -= p2.y;
	return *this;
}

template <typename T>
inline OffsetT<T>& OffsetT<T>::operator*= (float n) {
    x *= n;
    y *= n;
    return *this;
}

template <typename T>
inline OffsetT<T>& OffsetT<T>::operator/= (float n) {
    x /= n;
    y /= n;
    return *this;
}
template <typename T>
inline OffsetT<T> OffsetT<T>::operator* (float n) const {
    OffsetT<T> rp;
    rp.x = x * n;
    rp.y = y * n;
    return rp;
}

template <typename T>
inline OffsetT<T> OffsetT<T>::operator/ (float n) const {
    OffsetT<T> rp;
    rp.x = x / n;
    rp.y = y / n;
    return rp;
}

template <typename T>
inline bool OffsetT<T>::zero() const {
	return ((x == 0) && (y == 0));
}

template <typename T>
inline float PointT<T>::distance(const PointT<T>& p2) const {
	T dx = p2.x - OffsetT<T>::x;
	T dy = p2.y - OffsetT<T>::y;
	return sqrt( (dx * dx) + (dy * dy) );
}

template <typename T>
PointT<T> PointT<T>::projection(const PointT<T> &pointB) const {
	PointT<T> projectionPt;
	const float dp = dotProduct(pointB);
	const float lenBSq = pointB.x * pointB.x + pointB.y * pointB.y;
	projectionPt.x = (dp / lenBSq) * pointB.x;
	projectionPt.y = (dp/ lenBSq) * pointB.y;
	return projectionPt;
}

template <typename T>
inline float VectorT<T>::dotProduct(const VectorT<T> &vectorB) const {
	return (OffsetT<T>::x * vectorB.x) + (OffsetT<T>::y * vectorB.y);
	// the dot product describes the relative orientation of a and b; 
	// a negative dot product means that a and b point away from each other; 
	// a positive dot product means they point in the same direction. 
	// However, since the magnitude of the dot product is determined by the lengths of a and b, 
	//	only the sign (positive/negative) of the dot product is directly useful without further manipulation. 
}

template <typename T>
inline float VectorT<T>::vectorLength() const {
    return sqrt( (OffsetT<T>::x * OffsetT<T>::x) + (OffsetT<T>::y * OffsetT<T>::y) );
}

template <typename T>
inline float VectorT<T>::vectorAngle() const {
    return atan2(OffsetT<T>::y, OffsetT<T>::x);
}

template <typename T> 
inline VectorT<T> VectorT<T>::normal() const {
	T len = vectorLength();
	return VectorT<T>(OffsetT<T>::x/len, OffsetT<T>::y/len);
}
  
//! \endcond

typedef OffsetT<PDG_BASE_COORD_TYPE> Offset;
    
typedef PointT<PDG_BASE_COORD_TYPE> Point;

typedef VectorT<float> Vector;
    
// -----------------------------------------------------------------------------------
// RectT template
// -----------------------------------------------------------------------------------
/** Rectangle for 2D coordinate system.
 * Rect is a class that provides support for dealing with rectangles in
 * 2 dimensional space. 
 * \if C++
 * RectT is not used directly in most cases. Use the Rect class, which
 * is the long integer version of RectT, and has all the same methods.
 * RectT is a template class that provides support for dealing with rectangles in
 * 2 dimensional space. A long integer version of RectT is predefined
 * as Rect, but a floating point version of RectT could easily be
 * declared using the RectT template.
 * \endif
 */
//! \cond C++
template <typename T>
//! \endcond
class RectT {
public:
	//! leftmost coordinate of a rectangle on a grid or screen
    T    left;
	//! topmost coordinate of a rectangle on a grid or screen
    T    top;
	//! rightmost coordinate of a rectangle on a grid or screen
    T    right;
	//! bottom-most coordinate of a rectangle on a grid or screen
    T    bottom;
    // info
	//! return true if this rectangle is empty (no width or no height)
    bool    empty() const {return ((right <= left) || (bottom <= top));}
	//! return true if the point is inside this rectangle
    bool    contains(const PointT<T>& p) const { return ((p.x >= left) && (p.x <= right) && (p.y >= top) && (p.y <= bottom)); }
	//! return true if the rectangle passed in is entirely inside this rectangle
    bool    contains(const RectT<T>& r) const { return (contains(r.leftTop()) && contains(r.rightTop())
                                         && contains(r.leftBottom()) && contains(r.rightBottom())); }
	//! return true if this rectangle overlaps the other rectangle at all (sharing an edge is not overlapping)
    bool    overlaps(const RectT<T>& r) const { return !intersection(r).empty(); }
    // as Points
	//! get the top left corner point of the rectangle
    PointT<T>   leftTop() const {return PointT<T>(left, top);}
	//! get the top right corner point of the rectangle
    PointT<T>   rightTop() const {return PointT<T>(right, top);}
	//! get the bottom left corner point of the rectangle
    PointT<T>   leftBottom() const {return PointT<T>(left, bottom);}
	//! get the bottom right corner point of the rectangle
    PointT<T>   rightBottom() const {return PointT<T>(right, bottom);}
	//! get the center point of the rectangle, rounds down if fractional
    PointT<T>   centerPoint() const {return PointT<T>(width()/2 + left, height()/2 + top);}
    // as x1, y1, etc...
	//! get the 1st x coordinate (leftmost)
    T    x1() const {return left;}
	//! get the 1st y coordinate (topmost)
    T    y1() const {return top;}
	//! get the 2nd x coordinate (rightmost)
    T    x2() const {return right;}
	//! get the 2nd y coordinate (bottom-most)
    T    y2() const {return bottom;}
	//! get the width of the rectangle
    T    width() const {return right - left;}
	//! get the height of the rectangle
    T    height() const {return bottom - top; }
    // intersection and union
	//! get a new rectangle that is the overlapping area of the the rectangles
    RectT<T>    intersection(const RectT<T>& r) const { RectT<T> rd;
                                            rd.left = (r.left < left) ? left : r.left;
                                            rd.top = (r.top < top) ? top : r.top;
                                            rd.right = (r.right > right) ? right : r.right;
                                            rd.bottom = (r.bottom > bottom) ? bottom : r.bottom;
                                            return rd;
                                          }
	//! get the smallest possible new rectangle that contains both rectangles
    RectT<T>    unionWith(const RectT<T>& r) const { RectT<T> rd;
                                            rd.left = (r.left > left) ? left : r.left;
                                            rd.top = (r.top > top) ? top : r.top;
                                            rd.right = (r.right < right) ? right : r.right;
                                            rd.bottom = (r.bottom < bottom) ? bottom : r.bottom;
                                            return rd;
                                          }
    // move the rect
	//! move the rectangle to the left by some amount
    RectT<T>&        moveLeft(T delta)   { left -= delta; right -= delta; return *this; }
	//! move the rectangle to the right by some amount
    RectT<T>&        moveRight(T delta)  { left += delta; right += delta; return *this; }
	//! move the rectangle up by some amount
    RectT<T>&        moveUp(T delta)     { top -= delta; bottom -= delta; return *this; }
	//! move the rectangle down by some amount
    RectT<T>&        moveDown(T delta)   { top += delta; bottom += delta; return *this; }
	//! move the rectangle to a particular x location, leaving y unchanged
    RectT<T>&        moveXTo(T x)    { T w = width(); left = x; return setWidth(w); }
	//! move the rectangle to a particular y location, leaving x unchanged
    RectT<T>&        moveYTo(T y)    { T h = height(); top = y; return setHeight(h); }
	//! move the rectangle to a particular (x, y) location
    RectT<T>&        moveTo(T x, T y)    { T w = width(); T h = height(); left = x; top = y; return setWidth(w).setHeight(h); }
	//! move the rectangle to a particular point
    RectT<T>&        moveTo(PointT<T> p) { return moveTo(p.x, p.y); }
	//! move the rectangle to be centered over a particular point
    RectT<T>&        center(PointT<T> p) { return moveTo(p.x - width()/2, p.y - height()/2); }
	//! move the rectangle to be centered within/relative to another rectangle
    RectT<T>&        center(RectT<T> r)  { T x = (r.width() - width())/2 + r.left; T y = (r.height() - height())/2 + r.top; return moveTo(x, y); }
    // resize the rect
	//! set the size (width & height) of the rectangle
	RectT<T>&    	 setSize(T n) { return setWidth(n).setHeight(n); }
	//! set the width of the rectangle
    RectT<T>&        setWidth(T w) { right = left + w; return *this; }
	//! set the height of the rectangle
    RectT<T>&        setHeight(T h) { bottom = top + h; return *this; }
	//! reduce the width of the rectangle while leaving the center point unchanged
    RectT<T>&        horzShrink(T delta) { left += delta; right -= delta; return *this; }
	//! reduce the height of the rectangle while leaving the center point unchanged
    RectT<T>&        vertShrink(T delta) { top += delta; bottom -= delta; return *this; }
	//! increase the width of the rectangle while leaving the center point unchanged
    RectT<T>&        horzGrow(T delta)   { left -= delta; right += delta; return *this; }
	//! increase the height of the rectangle while leaving the center point unchanged
    RectT<T>&        vertGrow(T delta)   { top -= delta; bottom += delta; return *this; }
	//! reduce the height and width of the rectangle while leaving the center point unchanged
    RectT<T>&        shrink(T delta)     { return horzShrink(delta).vertShrink(delta); }
	//! increase the height and width of the rectangle while leaving the center point unchanged
    RectT<T>&        grow(T delta)       { return horzGrow(delta).vertGrow(delta); }
    // scaling rect
	//! change the x coordinates of the rectangle by a multiplier
    RectT<T>&        horzScale(float f)  { right = (T)((float)right * f); left = (T)((float)left * f); return *this; }
	//! change the y coordinates of the rectangle by a multiplier
    RectT<T>&        vertScale(float f)  { top = (T)((float)top * f); bottom = (T)((float)bottom * f); return *this; }
	//! change the coordinates of the rectangle by a multiplier
    RectT<T>&        scale(float f)      { return horzScale(f).vertScale(f); }
	//! round the coordinates to closest whole number
	RectT<T>&    	 round()			 { top = fround2i(top); bottom = fround2i(bottom); left = fround2i(left); right = fround2i(right); return *this; }
	// typecasts
	// typecast operators
	operator QuadT<T> () { return QuadT<T>(*this); }
	// operators
	//! return true if this rectangle is equal to the other
	bool operator== (const RectT<T>& r2);
	//! return true if this rectangle is not equal to the other
	bool operator!= (const RectT<T>& r2);
	//! set rectangle equal to another
	RectT<T>& operator= (const RectT<T>& r2);
	//! offset this rectangle's location by adding x & y coordinates of the point
	RectT<T>& operator+= (const PointT<T>& p1);
	//! offset this rectangle's location by subtracting x & y coordinates of the point
	RectT<T>& operator-= (const PointT<T>& p1);
	//! add another rectangle to this one by adding coordinates
	RectT<T>& operator+= (const RectT<T>& r2);
	//! subtract another rectangle from this one by subtracting coordinates
	RectT<T>& operator-= (const RectT<T>& r2);
	//! get a rectangle offset by adding the x & y coordinates of a point
	RectT<T> operator+ (const PointT<T>& p1);
	//! get a rectangle offset by subtracting the x & y coordinates of a point
	RectT<T> operator- (const PointT<T>& p1);
	//! add two rectangles together by adding coordinates
	RectT<T> operator+ (const RectT<T>& r2);
	//! subtract one rectangle from another by subtracting coordinates
	RectT<T> operator- (const RectT<T>& r2);
    // constructors
	//! create a rectangle with origin at (0,0) and set its height and width values
    RectT(T wid, T hgt) : left(0), top(0), right(wid), bottom(hgt) {}
	//! create a rectangle with left, top, right, and bottom coordinates (x1, y1, x2, y2) specified
    RectT(T lft, T tp, T rgt, T bot) : left(lft), top(tp), right(rgt), bottom(bot) {}
	//! create a rectangle given its top left and bottom right corner points
    RectT(const PointT<T>& lftTop, const PointT<T>& rgtBot) : left(lftTop.x), top(lftTop.y), right(rgtBot.x), bottom(rgtBot.y) {}
	//! create a rectangle given its top teft corner point and a height and width
    RectT(const PointT<T>& lftTop, T wid, T hgt) : left(lftTop.x), top(lftTop.y), right(lftTop.x + wid), bottom(lftTop.y + hgt) {}
	//! create an empty rectangle with origin at (0,0)
    RectT() : left(0), top(0), right(0), bottom(0) {}
};

//! \cond C++
template <typename T>
inline bool RectT<T>::operator== (const RectT<T>& r2) {
	return ((left == r2.left) && (top == r2.top) && (right == r2.right) && (bottom == r2.bottom));
}

template <typename T>
inline bool RectT<T>::operator!= (const RectT<T>& r2) {
	return ((left != r2.left) || (top != r2.top) || (right != r2.right) || (bottom != r2.bottom));
}

template <typename T>
inline RectT<T>& RectT<T>::operator= (const RectT<T>& r2) {
	left = r2.left;
	top = r2.top;
	right = r2.right;
	bottom = r2.bottom;
	return *this;
}

template <typename T>
inline RectT<T>& RectT<T>::operator+= (const PointT<T>& p1) {
	left += p1.x;
	top += p1.y;
	right += p1.x;
	bottom += p1.y;
	return *this;
}

template <typename T>
inline RectT<T>& RectT<T>::operator-= (const PointT<T>& p1) {
	left -= p1.x;
	top -= p1.y;
	right -= p1.x;
	bottom -= p1.y;
	return *this;
}

template <typename T>
inline RectT<T>& RectT<T>::operator+= (const RectT<T>& r2) {
	left += r2.left;
	top += r2.top;
	right += r2.right;
	bottom += r2.bottom;
	return *this;
}

template <typename T>
inline RectT<T>& RectT<T>::operator-= (const RectT<T>& r2) {
	left -= r2.left;
	top -= r2.top;
	right -= r2.right;
	bottom -= r2.bottom;
	return *this;
}

template <typename T>
inline RectT<T> RectT<T>::operator+ (const PointT<T>& p1) {
    RectT<T> r;
	r.left = left + p1.x;
	r.top = top + p1.y;
	r.right = right + p1.x;
	r.bottom = bottom + p1.y;
	return r;
}

template <typename T>
inline RectT<T> RectT<T>::operator- (const PointT<T>& p1) {
    RectT<T> r;
	r.left = left - p1.x;
	r.top = top - p1.y;
	r.right = right - p1.x;
	r.bottom = bottom - p1.y;
	return r;
}

template <typename T>
inline RectT<T> RectT<T>::operator+ (const RectT<T>& r2) {
    RectT<T> r;
	r.left = left + r2.left;
	r.top = top + r2.top;
	r.right = right + r2.right;
	r.bottom = bottom + r2.bottom;
	return r;
}

template <typename T>
inline RectT<T> RectT<T>::operator- (const RectT<T>& r2) {
    RectT<T> r;
	r.left = left - r2.left;
	r.top = top - r2.top;
	r.right = right - r2.right;
	r.bottom = bottom - r2.bottom;
	return r;
}

//! \endcond	

typedef RectT<PDG_BASE_COORD_TYPE> Rect;


enum {
	lftTop = 0,
	rgtTop = 1,
	rgtBot = 2,
	lftBot = 3
};

// -----------------------------------------------------------------------------------
// QuadT template
// -----------------------------------------------------------------------------------
/** a 4 point polygon in 2D system.
 * Quad is a class that provides support for dealing with 4 point
 * ploygons in 2 dimensional space
 * \if C++
 * QuadT is not used directly in most cases. Use the Quad class, which
 * is the long integer version of QuadT, and has all the same methods.
 * QuadT is a template class that provides support for dealing with 4 point
 * ploygons in 2 dimensional space. A floating point version of QuadT is
 * predefined as Quad, but a long integer version of QuadT could
 * easily be declared using the QuadT template.
 * \endif
 */
//! \cond C++
template <typename T>
//! \endcond
class QuadT {
public:
	PointT<T>  points[4];
	//! return a rectangle that bounds the quad
	RectT<T>   getBounds() const;
	//! return the calculated centerpoint of the quad
    PointT<T>  centerPoint() const;
    // constructors
    QuadT() {}
	//! create a quad from a rectangle
    QuadT(const RectT<T>& rect) {points[lftTop] = rect.leftTop(); points[rgtTop] = rect.rightTop(); points[lftBot] = rect.leftBottom(); points[rgtBot] = rect.rightBottom();}
    QuadT(const RotatedRectT<T>& rr) { QuadT<T> tq = rr.getQuad(); for (int i = 0; i<4; i++) { points[i] = tq.points[i]; } }
	//! create a quad given its corner points
    QuadT(const PointT<T>& _lftTop, const PointT<T>& _rgtTop, const PointT<T>& _rgtBot, const PointT<T>& _lftBot) {
		points[lftTop] = _lftTop; points[rgtTop] = _rgtTop; points[rgtBot] = _rgtBot; points[lftBot] = _lftBot; }
	// equality operator
	bool operator== (const QuadT<T>& q2) const;
	//! Determines if the point is contained within this quad
	bool contains(const PointT<T> &point) const;
    // move the quad
	//! move the rectangle to the left by some amount
    void    moveLeft(T delta)   { points[0].x -= delta; points[1].x -= delta; points[2].x -= delta; points[3].x -= delta; }
	//! move the rectangle to the right by some amount
    void    moveRight(T delta)  { points[0].x += delta; points[1].x += delta; points[2].x += delta; points[3].x += delta; }
	//! move the rectangle up by some amount
    void    moveUp(T delta)     { points[0].y -= delta; points[1].y -= delta; points[2].y -= delta; points[3].y -= delta; }
	//! move the rectangle down by some amount
    void    moveDown(T delta)   { points[0].y += delta; points[1].y += delta; points[2].y += delta; points[3].y += delta; }
	//! rotate the quad by a rotation in radians (around the calculated center point of the quad)
	void	rotate(float rotationRadians) { rotate(rotationRadians, PointT<T>((T)0,(T)0)); }
	//! rotate the quad by a rotation in radians around an offset center point
	void	rotate(float rotationRadians, const PointT<T>& centerPtOffset);
};

typedef QuadT<PDG_BASE_COORD_TYPE> Quad;

//! \cond C++
template <typename T>
bool QuadT<T>::operator== (const QuadT<T>& q2) const {
	return (points[lftTop] == q2.points[lftTop]) && (points[rgtTop] == q2.points[rgtTop]) &&
		   (points[rgtBot] == q2.points[rgtBot]) && (points[lftBot] == q2.points[lftBot]);
}

template <typename T>
RectT<T>   QuadT<T>::getBounds() const {
	RectT<T> r;
	r.left = points[0].x;
	r.top = points[0].y;
	r.right = points[0].x;
	r.bottom = points[0].y;
	for (int i = 1; i<4; i++) {
		if (points[i].x < r.left) r.left = points[i].x;
		if (points[i].x > r.right) r.right = points[i].x;
		if (points[i].y < r.top) r.top = points[i].y;
		if (points[i].y > r.bottom) r.bottom = points[i].y;
	}
	return r;
}

template <typename T>
PointT<T>  QuadT<T>::centerPoint() const {
//	    sa(pa2.x) - sb(pb2.x) + pb2.y - pa2.y
//	X = -------------------------------------
//	               (sa - sb)
	float sa, sb;
	PointT<float> pa1(points[lftTop].x, points[lftTop].y);
	PointT<float> pa2(points[rgtBot].x, points[rgtBot].y);
	PointT<float> pb1(points[lftBot].x, points[lftBot].y);
	PointT<float> pb2(points[rgtTop].x, points[rgtTop].y);
	if ( (pa2.y < pb2.y) && (pa1.x < pb2.x)) {
		// twisted around horizontal axis, swap for calculation
		PointT<float> tmp = pa2;
		pa2 = pb2;
		pb2 = tmp;
	}
	if ( (pa2.y > pb2.y) && (pa1.x > pb2.x)) {
		// twisted around vertical axis, swap for calculation
		PointT<float> tmp = pa1;
		pa1 = pb2;
		pb2 = tmp;
	}
	sa = (pa2.y - pa1.y)/(pa2.x - pa1.x);
	sb = (pb2.y - pb1.y)/(pb2.x - pb1.x);
	if (sa == sb) return getBounds().centerPoint();
	float x = ((sa * pa2.x) - (sb * pb2.x) + pb2.y - pa2.y) / (sa - sb);
	float y = (sa * (x - pa2.x)) + pa2.y;
	return PointT<T>(x, y);
}

template <typename T>
bool QuadT<T>::contains(const PointT<T> &point) const {
	// The test is just checking whether the point lies on the correct side of each rectangle edge.
	// if it does, then the point is inside the quad
	const float x = point.x;
	const float y = point.y;
	const float ex = points[rgtTop].x - points[lftTop].x; 
	const float ey = points[rgtTop].y - points[lftTop].y;
	const float fx = points[lftBot].x - points[lftTop].x;
	const float fy = points[lftBot].y - points[lftTop].y;

	if ( ((x - points[lftTop].x) * ex + (y - points[lftTop].y) * ey) < 0.0 ) return false;
	if ( ((x - points[rgtTop].x) * ex + (y - points[rgtTop].y) * ey) > 0.0 ) return false;
	if ( ((x - points[lftTop].x) * fx + (y - points[lftTop].y) * fy) < 0.0 ) return false;
	if ( ((x - points[lftBot].x) * fx + (y - points[lftBot].y) * fy) > 0.0 ) return false;

	return true;
}

template <typename T>
void	QuadT<T>::rotate(float rotationRadians, const PointT<T>& centerPtOffset) {
	PointT<float> cp = centerPoint();
	PointT<float> coff(centerPtOffset.x, centerPtOffset.y);
	cp += coff;
	// calc points with rotation
	VectorT<float> v;
	for (int i = 0; i < 4; i++) {
		v.x = points[i].x;
		v.y = points[i].y;
		v -= cp;
		float len = v.vectorLength();
		float rot = v.vectorAngle();
		points[i].x = (T) ((len * cos(rot + rotationRadians)) + cp.x);
		points[i].y = (T) ((len * sin(rot + rotationRadians)) + cp.y);
	}
}
//! \endcond

// -----------------------------------------------------------------------------------
// RotatedRectT template
// -----------------------------------------------------------------------------------
/** Rectangle with rotation for 2D coordinate system.
 * RotatedRect is a class that provides support for dealing with rotating
 * rectangles in 2 dimensional space.
 * \if C++
 * RotatedRectT is not used directly in most cases. Use the RotatedRect class, which
 * is the long integer version of RotatedRectT, and has all the same methods.
 * RotatedRectT is a template class that provides support for dealing with rotating
 * rectangles in 2 dimensional space. A floating point version of RotatedRectT is
 * predefined as RotatedRect, but a long integer version of RotatedRectT could
 * easily be declared using the RotatedRectT template.
 * \endif
 */
//! \cond C++
template <typename T>
//! \endcond
class RotatedRectT : public RectT<T> {
public:
	float      radians;
	OffsetT<T> centerOffset;
	// operations
	void	setCenterOffset(const PointT<T>& centerPtOffset) { centerOffset = centerPtOffset; }
	void	setRotation(float rotationRadians) { radians = rotationRadians; }
	void    setRotation(float rotationRadians, const PointT<T>& centerPtOffset) { radians = rotationRadians; centerOffset = centerPtOffset; }
	void	rotate(float rotateRadians) { radians += rotateRadians; }
	// conversions
	QuadT<T>  getQuad() const;
	RectT<T>   getBounds() const {return getQuad().getBounds(); }

	operator QuadT<T> () { return getQuad(); }

    // constructors
	//! create a rotated rect from an unrotated rect, along with a rotation in radians and an optional center point offset
    RotatedRectT() : RectT<T>(), radians(0.0f), centerOffset()  {}
    RotatedRectT(const RectT<T>& r, float rotationRadians = 0.0f, const OffsetT<T>& centerPtOffset = OffsetT<T>(0,0) ) 
    			: RectT<T>(r), radians(rotationRadians), centerOffset(centerPtOffset) { }
};

typedef RotatedRectT<PDG_BASE_COORD_TYPE> RotatedRect;
	
//! \cond C++	
template <typename T>
QuadT<T>  RotatedRectT<T>::getQuad() const {
	RectT<float> fr(this->left, this->top, this->right, this->bottom);
	QuadT<T> quad(fr);
	if (radians == 0.0f) return quad; // special common case
	PointT<float> cp = fr.centerPoint();
	OffsetT<float> coff(centerOffset.x, centerOffset.y);
	cp += coff;
	// calc points with rotation
	VectorT<float> v;
	for (int i = 0; i < 4; i++) {
		v.x = quad.points[i].x;
		v.y = quad.points[i].y;
		v -= cp;
		float len = v.vectorLength();
		float rot = v.vectorAngle();
		quad.points[i].x = (T) ((len * cos(rot + radians)) + cp.x);
		quad.points[i].y = (T) ((len * sin(rot + radians)) + cp.y);
	}
	return quad;
}

//! \endcond

} // end namespace pdg

#else  // end C++, begin ! C++

#ifdef PDG_DECORATE_GLOBAL_TYPES
  #define OffsetT pdg_OffsetT
  #define Offset pdg_Offset
  #define PointT pdg_PointT
  #define Point pdg_Point
  #define VectorT pdg_VectorT
  #define Vector pdg_Vector
  #define RectT pdg_RectT
  #define Rect pdg_Rect
  #define RotatedRect pdg_RotatedRect
#endif

typedef struct OffsetT {
	//! x coordinate of point on a grid or screen
    PDG_BASE_COORD_TYPE    x;
	//! y coordinate of point on a grid or screen
    PDG_BASE_COORD_TYPE    y;
} Offset;

typedef struct OffsetT PointT;
typedef struct OffsetT VectorT;
typedef Offset Point;
typedef Offset Vector;

typedef struct RectT {
	//! leftmost coordinate of a rectangle on a grid or screen
    PDG_BASE_COORD_TYPE    left;
	//! topmost coordinate of a rectangle on a grid or screen
    PDG_BASE_COORD_TYPE    top;
	//! rightmost coordinate of a rectangle on a grid or screen
    PDG_BASE_COORD_TYPE    right;
	//! bottom-most coordinate of a rectangle on a grid or screen
    PDG_BASE_COORD_TYPE    bottom;
} Rect;

typedef struct RotatedRectT {
	//! the rectangle prior to rotation
	struct RectT   rect;
	//! the offset from the true centerpoint of the rectangle to get the centerpoint for the rotation
	struct OffsetT centerOffset;
	//! the angle of the rotation
	float  radians;
} RotatedRect;

enum {
	Quad_lftTop = 0,
	Quad_rgtTop = 1,
	Quad_rgtBot = 2,
	Quad_lftBot = 3
};

typedef struct QuadT {
	Point  points[4];
} Quad;

#endif // end ! C++

#endif // PDG_COORDINATES_H_INCLUDED

