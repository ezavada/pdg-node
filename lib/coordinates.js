// -----------------------------------------------
// coordinates.js
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

// this is one of the few javascript files that are built-in
// to the pdg standalone app but also part of the pure 
// javascript implementation

if ((typeof process != "undefined") && ((typeof process.pdg != "undefined")
	|| (typeof process.versions['jsc'] != "undefined"))) {
	// node in standalone, classify becomes a built-in
	require('classify');
} else {
	require('./classify');
	require('./dump');
}

// -----------------------------------------------------------------------------------
// Offset
// -----------------------------------------------------------------------------------
// An x and y offset in 2 dimensional space

classify('Offset', function() {
	//! new Offset() create a point at the origin (0,0)
	//! new Offset(x,y): create a point and set its x & y values
	//! new Offset([x,y]): create a point and set its x & y values
	//! new Offset({x:x,y:y}): create a point and set its x & y values
    def('initialize', function(ix, iy) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create and set x & y values", arguments, "undefined", 2, "({ () | (number x, number y) | (number[] xy) | (object xy) })");
			}
			if (arguments.length == 1) {
				if (ix instanceof Offset || typeof ix.x != "undefined") {
					// initializing from another Offset object
					this.x = ix.x; this.y = ix.y;
				} else if (ix instanceof Array) {
					this.x = ix[0]; this.y = ix[1];
				} else {
					this.x = 0; this.y = 0;
				}
			} else {
				this.x = (ix) ? ix : 0; this.y = (iy) ? iy : 0;
			}
		});
	// operators
	//! return true if this point is equal to the other
	def('equals', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this point is equal to the other", arguments, "boolean", 1, "([object Offset] offset)");
			}
			return ((this.x == offset.x) && (this.y == offset.y));
		});
	def('notEquals', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this point is not equal to the other", arguments, "boolean", 1, "([object Offset] offset)");
			}
			return ((this.x != offset.x) || (this.y != offset.y));
		});
	def('assign', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set this offset equal to the given offset", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			this.x = offset.x; this.y = offset.y; return this;
		});
	// unary operators:   myOffset.add(otherOffset)
	def('add', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("add an offset to this one", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			this.x += offset.x; this.y += offset.y; return this;
		});
	def('sub', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("subtract an offset from this one", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			this.x -= offset.x; this.y -= offset.y; return this;
		});
	def('mul', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("multiply this offset by the given one", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			this.x *= offset.x; this.y *= offset.y; return this;
		});
	def('div', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("divide this offset by the given one", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			this.x /= offset.x; this.y /= offset.y; return this;
		});
	// binary operators:   newOffset = myOffset.plus(otherOffset).plus(thirdOffset);
	def('plus', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return new offset that is this offset plus given offset", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			var o = new Offset(this.x, this.y); return o.add(offset);
		});
	def('minus', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return new offset that is this offset minus given offset", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			var o = new Offset(this.x, this.y); return o.sub(offset);
		});
	def('times', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return new offset that is this offset multiplied by given offset", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			var o = new Offset(this.x, this.y); return o.mul(offset);
		});
	def('dividedby', function(offset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return new offset that is this offset divided by given offset", arguments, "[object Offset]", 1, "([object Offset] offset)");
			}
			var o = new Offset(this.x, this.y); return o.div(offset);
		});
	def('toString', function() {
			return "Offset("+this.x+","+this.y+")";
		});

});

// -----------------------------------------------------------------------------------
// Point
// -----------------------------------------------------------------------------------
// A location in a 2 dimensional cartesian coordinate system

classify(Offset, 'Point', function() {
	//! get distance from another point
    def('distance', function(point) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get distance from another point", arguments, "number", 1, "([object Point] point)");
			}
			var dx = point.x - this.x;
			var dy = point.y - this.y;
			return Math.sqrt( (dx * dx) + (dy * dy) );
		});
    // does this belong in Vector rather than point?
	//! do a projection of a second point onto this point
    def('projection', function(point) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("do a projection of a second point onto this point", arguments, "[object Point]", 1, "([object Point] point)");
			}
			var dp = this.dotProduct(point);
			var lenBSq = point.x * point.x + point.y * point.y;
			var projectionPt = new Point();
			projectionPt.x = (dp / lenBSq) * pointB.x;
			projectionPt.y = (dp/ lenBSq) * pointB.y;
			return projectionPt;
		});
	def('toString', function() {
			return "Point("+this.x+","+this.y+")";
		});
});

// -----------------------------------------------------------------------------------
// Vector
// -----------------------------------------------------------------------------------
// 2 dimensional vector has magnitude and direction, useful for physics stuff

classify(Offset, 'Vector', function() {
	//! get dot product for this vector with a 2nd vector
    def('dotProduct', function(vector) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get dot product for this vector with a 2nd vector", arguments, "number", 1, "([object Vector] vector)");
			}
			return (this.x * vector.x) + (this.y * vector.y);
		});
	//! get length as a vector (distance from origin)
    def('vectorLength', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get length as a vector (distance from origin)", arguments, "number", 0, "()");
			}
			return Math.sqrt( (this.x * this.x) + (this.y * this.y) );
		});
    //! get angle (in radians) for this vector
	def('vectorAngle', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get angle (in radians) for this vector", arguments, "number", 0, "()");
			}
			return Math.atan2(this.y, this.x);
		});
	def('toString', function() {
			return "Vector("+this.x+","+this.y+")";
		});
});

// -----------------------------------------------------------------------------------
// Rect
// -----------------------------------------------------------------------------------
// 2 dimensional box where left and right sides are parallel to the y axis and 
// top and bottom sides are parallel to the x axis

if (typeof Rect != 'undefined') {
	// erase any prior definitions
	Rect = undefined;
}

classify('Rect', function() {
	//! new Rect(): create an empty rectangle with origin at (0,0)
	//! new Rect(w,h): create a rectangle with origin at (0,0) and set its height and width values
	//! new Rect([Point] tl,[Point] br): create a rectangle given its top left and bottom right corner points
	//! new Rect([Point] tl,w,h): create a rectangle given its top teft corner point and a height and width
	//! new Rect(l,t,r,b): create a rectangle with left, top, right, and bottom coordinates (x1, y1, x2, y2) specified
    def('initialize', function(i1, i2, i3, i4) {
// 			if ((arguments.length == 1) && (arguments[0] == null)) { 
// 				return methodSignature("create and set values", arguments, "undefined", 4, 
// 				"({ () | (number w, number h) | ([object Point] topLeft, number w, number h) | ([object Point] leftTop, [object Point] rightBottom) | (number left, number top, number right, number bottom) })");
// 			}
    		if (arguments.length == 1) {
    			if (i1 instanceof Array) {
    				this.left = i1[0]; this.top = i1[1]; this.right = i1[2]; this.bottom = i1[3];
    			} else if (i1 instanceof Rect || typeof i1.top != "undefined") {
    				this.assign(i1);
    			} else {
    				this.left = 0; this.top = 0; this.right = 0; this.bottom = 0;
    			}
    		} else if (arguments.length == 2) {
				if (i1 instanceof Point || typeof i1.x != "undefined") {
					// initializing from Point Objects
					this.left = i1.x; this.top = i1.y;
					if (i2 instanceof Point || typeof i2.x != "undefined") {
						this.right = i2.x; // 2nd param is rightBottom point
						this.bottom = i2.y;
					} else { // 2nd and 3rd params are width and height
						this.right = this.left + i2;
						this.bottom = this.top + i3;
					}
				} else { // 2 params: width and height
					this.left = 0; this.top = 0; this.right = i1; this.bottom = i2;
				}
    		} else if (arguments.length == 3) {
				// 3 params: top-left Point, width and height
				this.left = i1.x; this.top = i1.y;
				this.right = this.left + i2;
				this.bottom = this.top + i3;
			} else {
				this.left = (i1) ? i1 : 0; 
				this.top = (i2) ? i2 : 0;
				this.right = (i3) ? i3 : 0; 
				this.bottom = (i4) ? i4 : 0;
			}
		});
	//! return true if this rectangle is empty (no width or no height)
    def('empty', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this rectangle is empty (no width or no height)", arguments, "boolean", 0, "()");
			}
			return ((this.right <= this.left) || (this.bottom <= this.top));
		});
	//! contains([Point] p) return true if the point is inside this rectangle
	//! contains([Rect] r) return true if the rectangle passed in is entirely inside this rectangle
    def('contains', function(o) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("contains([Point] p) return true if the point is inside this rectangle; contains([Rect] r) return true if the rectangle passed in is entirely inside this rectangle", arguments, "boolean", 1, "({ ([object Rect] r) | ([object Point] p) })");
			}
			if (o instanceof Point || typeof o.x != "undefined") {
				return ((o.x >= this.left) && (o.x <= this.right) && (o.y >= this.top) && (o.y <= this.bottom));
			} else if (o instanceof Rect || typeof o.top != "undefined") {
				return (this.contains(o.leftTop()) && this.contains(o.rightTop())
						&& this.contains(o.leftBottom()) && this.contains(o.rightBottom()));
			}
		});
	//! return true if this rectangle overlaps the other rectangle at all (sharing an edge is not overlapping)
    def('overlaps', function(r) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this rectangle overlaps the other rectangle at all (sharing an edge is not overlapping)", arguments, "boolean", 1, "([object Rect] r)");
			}
			return !this.intersection(r).empty();
		});
	def('leftTop', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get top left corner point of this rectangle", arguments, "[object Point]", 0, "()");
			}
			return new Point(this.left, this.top);
		});
	def('rightTop', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get top right corner point of this rectangle", arguments, "[object Point]", 0, "()");
			}
			return new Point(this.right, this.top);
		});
	def('leftBottom', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get bottom left corner point of this rectangle", arguments, "[object Point]", 0, "()");
			}
			return new Point(this.left, this.bottom);
		});
	def('rightBottom', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get bottom right corner point of this rectangle", arguments, "[object Point]", 0, "()");
			}
			return new Point(this.right, this.bottom);
		});
	def('centerPoint', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get center point of this rectangle", arguments, "[object Point]", 0, "()");
			}
			return new Point((this.right - this.left)/2 + this.left, (this.bottom - this.top)/2 + this.top);
		});
	def('x1', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("alias for Rect.left", arguments, "number", 0, "()");
			}
			return this.left;
		});
	def('y1', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("alias for Rect.top", arguments, "number", 0, "()");
			}
			return this.top;
		});
	def('x2', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("alias for Rect.right", arguments, "number", 0, "()");
			}
			return this.right;
		});
	def('y2', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("alias for Rect.bottom", arguments, "number", 0, "()");
			}
			return this.bottom;
		});
	def('width', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get the width of this rectangle", arguments, "number", 0, "()");
			}
			return this.right - this.left;
		});
	def('height', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get the height of this rectangle", arguments, "number", 0, "()");
			}
			return this.bottom - this.top;
		});
	//! get a new rectangle that is the overlapping area of the the rectangles
	def('intersection', function(r) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get a new rectangle that is the overlapping area of the the rectangles", arguments, "[object Rect]", 1, "([object Rect] r)");
			}
			var rd = new Rect();
			rd.left = (r.left < this.left) ? this.left : r.left;
			rd.top = (r.top < this.top) ? this.top : r.top;
			rd.right = (r.right > this.right) ? this.right : r.right;
			rd.bottom = (r.bottom > this.bottom) ? this.bottom : r.bottom;
			return rd;
		});
	//! get the smallest possible new rectangle that contains both rectangles
	def('unionWith', function(r) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("get the smallest possible new rectangle that contains both rectangles", arguments, "[object Rect]", 1, "([object Rect] r)");
			}
			var rd = new Rect();
			rd.left = (r.left > this.left) ? this.left : r.left;
			rd.top = (r.top > this.top) ? this.top : r.top;
			rd.right = (r.right < this.right) ? this.right : r.right;
			rd.bottom = (r.bottom < this.bottom) ? this.bottom : r.bottom;
			return rd;
		});
	def('moveLeft', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle to the left by some amount", arguments, "undefined", 1, "(number delta)");
			}
			this.left -= delta; this.right -= delta; 
			return this;
		});
	def('moveRight', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle to the right by some amount", arguments, "undefined", 1, "(number delta)");
			}
			this.left += delta; this.right += delta; 
			return this;
		});
	def('moveUp', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle up by some amount", arguments, "undefined", 1, "(number delta)");
			}
			this.top -= delta; this.bottom -= delta; 
			return this;
		});
	def('moveDown', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle down by some amount", arguments, "undefined", 1, "(number delta)");
			}
			this.top += delta; this.bottom += delta; 
			return this;
		});
	//! move the rectangle to a particular x location, leaving y unchanged
	def('moveXTo', function(x) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle to a particular x location, leaving y unchanged", arguments, "undefined", 1, "(number x)");
			}
			var w = this.width(); this.left = x; this.setWidth(w); 
			return this;
		});
	//! move the rectangle to a particular y location, leaving x unchanged
	def('moveYTo', function(y) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the rectangle to a particular y location, leaving x unchanged", arguments, "undefined", 1, "(number y)");
			}
			var h = this.height(); this.top = y; this.setHeight(h); 
			return this;
		});
	//! moveTo(x,y): the rectangle to a particular (x, y) location
	//! moveTo([Point] p): move the rectangle to a particular point
	def('moveTo', function(x, y) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("moveTo(x,y): the rectangle to a particular (x, y) location; moveTo([Point] p): move the rectangle to a particular point", arguments, "undefined", 2, "({ (number x, number y) | ([object Point] p) })");
			}
			if (x instanceof Point) {
				this.moveTo(p.x, p.y); 
			} else {
				var w = this.width(); this.left = x; this.setWidth(w);
				var h = this.height(); this.top = y; this.setHeight(h);
			}
			return this;
		});
	//! center([Point] p): move the rectangle to be centered over a particular point
	//! center([Rect] r): move the rectangle to be centered within/relative to another rectangle
	def('center', function(r) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("center([Point] p): move the rectangle to be centered over a particular point; center([Rect] r): move the rectangle to be centered within/relative to another rectangle", arguments, "undefined", 1, "({ ([object Rect] r) | ([object Point] p) })");
			}
			if (r instanceof Point) {
				this.moveTo(r.x - this.width()/2, r.y - this.height()/2);
			} else {
				var x = (r.width() - this.width())/2 + r.left; 
				var y = (r.height() - this.height())/2 + r.top;
				this.moveTo(x, y); 
			}
			return this;
		});
	//! set the size (width & height) of the rectangle
	def('setSize', function(n) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set the size (width & height) of the rectangle", arguments, "undefined", 1, "(number n)");
			}
			this.setWidth(n); this.setHeight(n); 
			return this;
		});
	def('setWidth', function(w) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "undefined", 1, "(number w)");
			}
			this.right = this.left + w; 
			return this;
		});
	def('setHeight', function(h) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "undefined", 1, "(number h)");
			}
			this.bottom = this.top + h; 
			return this;
		});
	//! reduce the width of the rectangle while leaving the center point unchanged
	def('horzShrink', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("reduce the width of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.left += delta; this.right -= delta; 
			return this;
		});
	//! reduce the height of the rectangle while leaving the center point unchanged
	def('vertShrink', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("reduce the height of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.top += delta; this.bottom -= delta; 
			return this;
		});
	//! increase the width of the rectangle while leaving the center point unchanged
	def('horzGrow', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("increase the width of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.left -= delta; this.right += delta; 
			return this;
		});
	//! increase the height of the rectangle while leaving the center point unchanged
	def('vertGrow', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("increase the height of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.top -= delta; this.bottom += delta; 
			return this;
		});
	//! reduce the height and width of the rectangle while leaving the center point unchanged
	def('shrink', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("reduce the height and width of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.horzShrink(delta); this.vertShrink(delta); 
			return this;
		});
	//! increase the height and width of the rectangle while leaving the center point unchanged
	def('grow', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("increase the height and width of the rectangle while leaving the center point unchanged", arguments, "undefined", 1, "(number delta)");
			}
			this.horzGrow(delta); this.vertGrow(delta); 
			return this;
		});
	//! change the x coordinates of the rectangle by a multiplier
	def('horzScale', function(f) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("change the x coordinates of the rectangle by a multiplier", arguments, "undefined", 1, "(number f)");
			}
			this.right *= f; this.left *= f; 
			return this;
		});
	//! change the y coordinates of the rectangle by a multiplier
	def('vertScale', function(f) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("change the y coordinates of the rectangle by a multiplier", arguments, "undefined", 1, "(number f)");
			}
			this.top *= f; this.bottom *= f; 
			return this;
		});
	//! change the coordinates of the rectangle by a multiplier
	def('scale', function(f) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("change the coordinates of the rectangle by a multiplier", arguments, "undefined", 1, "(number f)");
			}
			this.horzScale(f); this.vertScale(f); 
			return this;
		});
	//! round the coordinates to closest whole number
	def('round', function() { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("round the coordinates to closest whole number", arguments, "undefined", 0, "()");
			}
			this.left = Math.round(left); 
			this.top = Math.round(top); 
			this.right = Math.round(right); 
			this.bottom = Math.round(bottom); 
			return this;
		});
		
	def('toQuad', function() { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("convert rectangle to a Quad", arguments, "[object Quad]", 0, "()");
			}
			return new Quad(this); 
		});
		
	def('equals', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this rectangle is equal to the given one", arguments, "boolean", 1, "([object Rect] r2)");
			}
			return ((this.left == r2.left) && (this.top == r2.top) && (this.right == r2.right) && (this.bottom == r2.bottom));
		});
	def('notEquals', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this rectangle is not equal to the given one", arguments, "boolean", 1, "([object Rect] r2)");
			}
			return ((this.left != r2.left) || (this.top != r2.top) || (this.right != r2.right) || (this.bottom != r2.bottom));
		});
	def('assign', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set this rectangle equal to the given one", arguments, "[object Rect]", 1, "([object Rect] r2)");
			}
			this.left = r2.left; this.top = r2.top; this.right = r2.right; this.bottom = r2.bottom;
			return this;
		});
		// unary operators:   myRect.add(otherRect)
		//! add([Point] p): offset this rectangle's location by adding x & y coordinates of the point
		//! add([Rect] r): add another rectangle to this one by adding corresponding coordinates
	def('add', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("add([Point] p): offset this rectangle's location by adding x & y coordinates of the point; add([Rect] r): add another rectangle to this one by adding corresponding coordinates", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			if (r2 instanceof Point) {
				this.left += r2.x; this.top += r2.y; this.right += r2.x; this.bottom += r2.y;
			} else {
				this.left += r2.left; this.top += r2.top; this.right += r2.right; this.bottom += r2.bottom;
			}
			return this;
		});
		//! sub([Point] p): offset this rectangle's location by subtracting x & y coordinates of the point
		//! sub([Rect] r): add another rectangle to this one by subtracting corresponding coordinates
	def('sub', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("sub([Point] p): offset this rectangle's location by subtracting x & y coordinates of the point; sub([Rect] r): add another rectangle to this one by subtracting corresponding coordinates", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			if (r2 instanceof Point) {
				this.left -= r2.x; this.top -= r2.y; this.right -= r2.x; this.bottom -= r2.y;
			} else {
				this.left -= r2.left; this.top -= r2.top; this.right -= r2.right; this.bottom -= r2.bottom;
			}
			return this;
		});
		//! mul([Point] p): change this rectangle's location by multiplying by x & y coordinates of the point
		//! mul([Rect] r): change this rect by multiplying by corresponding coordinates of another rectangle
	def('mul', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("mul([Point] p): change this rectangle's location by multiplying by x & y coordinates of the point; mul([Rect] r): change this rect by multiplying by corresponding coordinates of another rectangle", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			if (r2 instanceof Point) {
				this.left *= r2.x; this.top *= r2.y; this.right *= r2.x; this.bottom *= r2.y;
			} else {
				this.left *= r2.left; this.top *= r2.top; this.right *= r2.right; this.bottom *= r2.bottom;
			}
			return this;
		});
		//! div([Point] p): change this rectangle's location by dividing by x & y coordinates of the point
		//! div([Rect] r): change this rect by dividing by corresponding coordinates of another rectangle
	def('div', function(r2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("div([Point] p): change this rectangle's location by dividing by x & y coordinates of the point; div([Rect] r): change this rect by dividing by corresponding coordinates of another rectangle", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			if (r2 instanceof Point) {
				this.left /= r2.x; this.top /= r2.y; this.right /= r2.x; this.bottom /= r2.y;
			} else {
				this.left /= r2.left; this.top /= r2.top; this.right /= r2.right; this.bottom /= r2.bottom;
			}
			return this;
		});
		// binary operators:   newRect = myRect.plus(otherRect).plus(thirdRect);
	def('plus', function(o) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			var r = new Rect(this); return r.add(o);
		});
	def('minus', function(o) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			var r = new Rect(this); return r.sub(o);
		});
	def('times', function(o) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			var r = new Rect(this); return r.mul(o);
		});
	def('dividedby', function(o) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("", arguments, "[object Rect]", 1, "({ ([object Rect] r2) | ([object Point] p) })");
			}
			var r = new Rect(this); return r.div(o);
		});
	def('toString', function() {
			return "Rect {left:"+this.left+",top:"+this.top+",right:"+this.right+",bottom:"+this.bottom+"}";
		});
});

var lftTop = 0;
var rgtTop = 1;
var rgtBot = 2;
var lftBot = 3;

// -----------------------------------------------------------------------------------
// Quad
// -----------------------------------------------------------------------------------
// Quad is a class that provides support for dealing with 4 point
// ploygons in 2 dimensional space

classify('Quad', function() {

	//! new Quad(): create an empty quad at 0,0
	//! new Quad([Quad] q): copy from another quad
	//! new Quad([Rect] r): create a quad from a rectangle
	//! new Quad([RotatedRect r]): create a quad from a rotated rectangle
	//! new Quad([Point] p1, [Point] p2, [Point] p3, [Point] p4): create a quad given its corner points
	//! new Quad([Point] p[4]): create a quad given an array of 4 points
    def('initialize', function(i1, i2, i3, i4) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("new Quad(): create an empty quad at 0,0; new Quad([Quad] q): copy from another quad; new Quad([Rect] r): create a quad from a rectangle; new Quad([RotatedRect r]): create a quad from a rotated rectangle; new Quad([Point] p1, [Point] p2, [Point] p3, [Point] p4): create a quad given its corner points; new Quad([Point] p[4]): create a quad given an array of 4 points", arguments, "[object Quad]", 4, "(args...)");
			}
    		this.points = new Array();
			if (arguments.length == 1) {
				if (i1 instanceof Rect || typeof i1.top != "undefined") {
					// initializing from a Rect or RotatedRect
					this.points[lftTop] = new Point(i1.left, i1.top);
					this.points[rgtTop] = new Point(i1.right, i1.top);
					this.points[lftBot] = new Point(i1.left, i1.bottom);
					this.points[rgtBot] = new Point(i1.right, i1.bottom);
				} else if (ix instanceof Quad) {
					for (var i = 0; i < 4; i++) {
						this.points[i] = new Point(i1.points[i]);
					}
				} else if (ix instanceof Array) {
					for (var i = 0; i < 4; i++) {
						this.points[i] = new Point(i1[i]);
					}
				} else {
					for (var i = 0; i < 4; i++) {
						this.points[i] = new Point();
					}
				}
			} else if (arguments.length == 4) {
				// initializing from a Rect or RotatedRect
				this.points[lftTop] = new Point(i1);
				this.points[rgtTop] = new Point(i2);
				this.points[lftBot] = new Point(i3);
				this.points[rgtBot] = new Point(i4);
			} else {
				for (var i = 0; i < 4; i++) {
					this.points[i] = new Point();
				}
			}
		});

	//! return a rectangle that bounds the quad
	def('getBounds', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return a rectangle that bounds the quad", arguments, "[object Rect]", 0, "()");
			}
			var r = new Rect(this.points[0].x, this.points[0].y, this.points[0].x, this.points[0].y);
			for (var i = 1; i<4; i++) {
				if (this.points[i].y < r.top) r.top = this.points[i].y;
				if (this.points[i].x < r.left) r.left = this.points[i].x;
				if (this.points[i].x > r.right) r.right = this.points[i].x;
				if (this.points[i].y > r.bottom) r.bottom = this.points[i].y;
			}
			return r;
		});
	//! return the calculated centerpoint of the quad
    def('centerPoint', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return the calculated centerpoint of the quad", arguments, "[object Point]", 0, "()");
			}
			//	    sa(pa2.x) - sb(pb2.x) + pb2.y - pa2.y
			//	X = -------------------------------------
			//	               (sa - sb)
			var sa, sb;
			var pa1 = new Point(this.points[lftTop].x, this.points[lftTop].y);
			var pa2 = new Point(this.points[rgtBot].x, this.points[rgtBot].y);
			var pb1 = new Point(this.points[lftBot].x, this.points[lftBot].y);
			var pb2 = new Point(this.points[rgtTop].x, this.points[rgtTop].y);
			var tmp;
			if ( (pa2.y < pb2.y) && (pa1.x < pb2.x)) {
				// twisted around horizontal axis, swap for calculation
				tmp = new Point(pa2); pa2.assign(pb2); pb2.assign(tmp);
			}
			if ( (pa2.y > pb2.y) && (pa1.x > pb2.x)) {
				// twisted around vertical axis, swap for calculation
				tmp = new Point(pa1); pa1.assign(pb2); pb2.assign(tmp);
			}
			sa = (pa2.y - pa1.y)/(pa2.x - pa1.x);
			sb = (pb2.y - pb1.y)/(pb2.x - pb1.x);
			if (sa == sb) return this.getBounds().centerPoint();
			var x = ((sa * pa2.x) - (sb * pb2.x) + pb2.y - pa2.y) / (sa - sb);
			var y = (sa * (x - pa2.x)) + pa2.y;
			return new Point(x, y);
		});
	def('equals', function(q2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this quad is equal to the given one", arguments, "boolean", 1, "([object Quad] q2)");
			}
			return (this.points[lftTop] == q2.points[lftTop]) && (this.points[rgtTop] == q2.points[rgtTop]) &&
				   (this.points[rgtBot] == q2.points[rgtBot]) && (this.points[lftBot] == q2.points[lftBot]);
		});
	def('notEquals', function(q2) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this quad is not equal to the given one", arguments, "boolean", 1, "([object Quad] q2)");
			}
			return (this.points[lftTop] != q2.points[lftTop]) || (this.points[rgtTop] != q2.points[rgtTop]) ||
				   (this.points[rgtBot] != q2.points[rgtBot]) || (this.points[lftBot] != q2.points[lftBot]);
		});
	//! Determines if the point is contained within this quad
	def('contains', function(point) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("returns true if the point is contained within this quad", arguments, "boolean", 1, "([object Point] p)");
			}
			// The test is just checking whether the point lies on the correct side of each rectangle edge.
			// if it does, then the point is inside the quad
			var x = point.x;
			var y = point.y;
			var ex = this.points[rgtTop].x - this.points[lftTop].x; 
			var ey = this.points[rgtTop].y - this.points[lftTop].y;
			var fx = this.points[lftBot].x - this.points[lftTop].x;
			var fy = this.points[lftBot].y - this.points[lftTop].y;
			if ( ((x - this.points[lftTop].x) * ex + (y - this.points[lftTop].y) * ey) < 0.0 ) return false;
			if ( ((x - this.points[rgtTop].x) * ex + (y - this.points[rgtTop].y) * ey) > 0.0 ) return false;
			if ( ((x - this.points[lftTop].x) * fx + (y - this.points[lftTop].y) * fy) < 0.0 ) return false;
			if ( ((x - this.points[lftBot].x) * fx + (y - this.points[lftBot].y) * fy) > 0.0 ) return false;
			return true;
		});
    // move the quad
	//! move the quad to the left by some amount
    def('moveLeft', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the quad to the left by some amount", arguments, "undefined", 1, "(number delta)");
			}
    		this.points[0].x -= delta; this.points[1].x -= delta; this.points[2].x -= delta; this.points[3].x -= delta; 
			return this;
		});
	//! move the quad to the right by some amount
    def('moveRight', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the quad to the right by some amount", arguments, "undefined", 1, "(number delta)");
			}
    		this.points[0].x += delta; this.points[1].x += delta; this.points[2].x += delta; this.points[3].x += delta; 
			return this;
		});
	//! move the quad up by some amount
    def('moveUp', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the quad up by some amount", arguments, "undefined", 1, "(number delta)");
			}
    		this.points[0].y -= delta; this.points[1].y -= delta; this.points[2].y -= delta; this.points[3].y -= delta;
			return this;
		});
	//! move the quad down by some amount
    def('moveDown', function(delta) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("move the quad down by some amount", arguments, "undefined", 1, "(number delta)");
			}
    		this.points[0].y += delta; this.points[1].y += delta; this.points[2].y += delta; this.points[3].y += delta; 
			return this;
		});
	//! rotate(n): rotate the quad by a rotation in radians (around the calculated center point of the quad)
	//! rotate(n, [Offset] o): rotate the quad by a rotation in radians around an offset center point
	def('rotate', function(rotationRadians, centerPtOffset) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("rotate(n): rotate the quad by a rotation in radians (around the calculated center point of the quad); rotate(n, [Offset] o): rotate the quad by a rotation in radians around an offset center point", arguments, "undefined", 2, "(number rotationRadians, [object Offset] centerPtOffset = Point(0,0))");
			}
			var center = this.centerPoint();
			if (typeof centerPtOffset.x != "undefined") {
				center.add(centerPtOffset);
			}
			// calc this.points with rotation
			var v = new Vector();
			for (var i = 0; i < 4; i++) {
				v.x = this.points[i].x - center.x;
				v.y = this.points[i].y - center.y;
				var len = v.vectorLength();
				var rot = v.vectorAngle();
				this.points[i].x = (len * Math.cos(rot + rotationRadians)) + center.x;
				this.points[i].y = (len * Math.sin(rot + rotationRadians)) + center.y;
			}
			return this;
		});
});


// -----------------------------------------------------------------------------------
// RotatedRectT template
// -----------------------------------------------------------------------------------
// RotatedRect is a class that provides support for dealing with rotating
// rectangles in 2 dimensional space.

classify(Rect, 'RotatedRect', function() {
	//! set an offset for point around which rotation is applied
	def('setCenterOffset', function(cpOffset) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set an offset for point around which rotation is applied", arguments, "undefined", 1, "([object Offset] cpOffset)");
			}
			this.centerOffset.assign(cpOffset); 
			return this;
		});
	//! set the rotation of this rectangle to a particular amount in radians
	def('setRotation', function(rotationRadians, cpOffset) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set the rotation of this rectangle to a particular amount in radians", arguments, "undefined", 2, "(number rotationRadians, [object Offset] cpOffset = null)");
			}
			radians = rotationRadians; 
			if (typeof cpOffset != "undefined" && typeof cpOffset.x != "undefined") {
				this.centerOffset.assign(cpOffset);
			}
			return this;
		});
	//! change the rotation of this rectangle by some number of radians
	def('rotate', function(rotateRadians) { 
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("change the rotation of this rectangle by some number of radians", arguments, "undefined", 1, "(number rotateRadians)");
			}
			this.radians += rotateRadians; 
		});
    def('initialize', function(rect, rotationRadians, cpOffset) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create a new RotatedRect", arguments, "[object RotatedRect]", 1, "([object Rect] rect = Rect(0,0), number rotationRadians = 0.0, [object Offset] cpOffset = null)");
			}
    		if (arguments.length == 0) {
    			this.callSuper();
    		} else if (arguments.length >= 1) {
    			this.callSuper(rect);
    		}
    		if (arguments.length >= 2) {
    			this.radians = rotationRadians;
    		}
    		this.centerOffset = new Point(0, 0);
    		if (typeof cpOffset != "undefined" && typeof cpOffset.x != "undefined") {
    			this.centerOffset.assign(cpOffset);
    		}
		});
	//! create a Quad by applying the rectangle's rotation around the center point with offset
    def('getQuad', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create a Quad by applying the rectangle's rotation around the center point with offset", arguments, "[object Quad]", 1, "()");
			}
			var fr = new Rect(this.left, this.top, this.right, this.bottom);
			var quad = new Quad(fr);
			if (radians == 0.0) return quad;
			var center = fr.centerPoint();
			var coff = new Offset(centerOffset.x, centerOffset.y);
			center.add(coff);
			// calc points with rotation
			var v = new Vector;
			for (var i = 0; i < 4; i++) {
				v.x = quad.points[i].x - center.x;
				v.y = quad.points[i].y - center.y;
				var len = v.vectorLength();
				var rot = v.vectorAngle();
				quad.points[i].x = (len * Math.cos(rot + radians)) + center.x;
				quad.points[i].y = (len * Math.sin(rot + radians)) + center.y;
			}
			return quad;
		});
});


if(!(typeof exports === 'undefined')) {
    exports.Offset = Offset;
    exports.Point = Point;
    exports.Vector = Vector;
    exports.Rect = Rect;
    exports.Quad = Quad;
    exports.RotatedRect = RotatedRect;
    exports.lftTop = lftTop;
    exports.rgtTop = rgtTop;
    exports.rgtBot = rgtBot;
    exports.lftBot = lftBot;
}
