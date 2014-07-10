// -----------------------------------------------
// graphics-opengl.cpp
//
// Open GL based implementation of
// common graphics functions
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


#include "pdg_project.h"

#ifndef PDG_NO_GUI

#include "pdg/msvcfix.h"

#include "pdg/sys/os.h"
#include "pdg/sys/graphics.h"
#include "pdg/sys/events.h"
#include "pdg/sys/initializer.h"
#include "internals.h"
#include "pdg-main.h"

#include <algorithm>
#include <string>
#include <cmath>

#include "graphics-opengl.h"
#include "include-opengl.h"

// Uncomment line below to get baselines and bounding boxes for text drawn automatically
//#define PDG_DEBUG_TEXT_DRAWING


#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
    #define GFX_CHECK_PTR(ptr, block, block_size) CHECK_PTR(ptr, block, block_size)
#else
    #define GFX_CHECK_PTR(ptr, block, block_size)
#endif

namespace pdg {

float gRotationAngle = 0.0f;
int gScreenPos = -1;
int gEffectiveScreenPos = -1;
bool gAllowVerticalRotation = true;
bool gAllowHorizontalRotation = true;
bool gPortDirty;
bool gModesSet = false;

extern GLuint gBoundTexture;

void graphics_startDrawing(Port* port) {
	pdg::PortImpl* thePort = dynamic_cast<pdg::PortImpl*>(port);
	platform_startDrawing(thePort->mPlatformWindowRef);
	GLsizei w = thePort->getDrawingArea().width();
	GLsizei h = thePort->getDrawingArea().height();
	if (   (gEffectiveScreenPos == pdg::screenPos_Rotated90Clockwise)
		|| (gEffectiveScreenPos == pdg::screenPos_Rotated90CounterClockwise)) {
		glViewport(0, 0, h, w);
	} else {
		glViewport(0, 0, w, h);
	}
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	gPortDirty = false;
    gModesSet = false;
    gBoundTexture = -1;
}

void graphics_finishDrawing(Port* port) {
	pdg::PortImpl* thePort = dynamic_cast<pdg::PortImpl*>(port);
	platform_finishDrawing(thePort->mPlatformWindowRef);
}

void graphics_getApplicationSupportedOrientations() {
	gAllowVerticalRotation = pdg::Initializer::allowVerticalOrientation();
	gAllowHorizontalRotation = pdg::Initializer::allowHorizontalOrientation();
}

int graphics_getEffectiveScreenPos() {
	return gEffectiveScreenPos;
}

void graphics_setScreenPos(int screenPos) {
	if (gScreenPos == -1) {
		// if screen position has never been determined, then set up some items
		// required for correct transition
		if (gAllowVerticalRotation) {
			gScreenPos = pdg::screenPos_Normal;
			gRotationAngle = 0.0f;
		} else {
			gScreenPos = pdg::screenPos_Rotated90Clockwise;
			gRotationAngle = -90.0f;
		}
		gEffectiveScreenPos = gScreenPos;
	}
	switch (screenPos) {
		case pdg::screenPos_Rotated180:
			if (gAllowVerticalRotation) {
				gRotationAngle = 180.0f;
				gEffectiveScreenPos = screenPos;
			}
			break;
		case pdg::screenPos_Rotated90Clockwise:
			if (gAllowHorizontalRotation) {
				gRotationAngle = -90.0f;
				gEffectiveScreenPos = screenPos;
			}
			break;
		case pdg::screenPos_Rotated90CounterClockwise:
			if (gAllowHorizontalRotation) {
				gRotationAngle = 90.0f;
				gEffectiveScreenPos = screenPos;
			}
			break;
		case pdg::screenPos_Normal:
			if (gAllowVerticalRotation) {
				gRotationAngle = 0.0f;
				gEffectiveScreenPos = screenPos;
			}
			break;
		default:
			break;
	}
	gScreenPos = screenPos;
}


bool graphics_allowHorizontalOrientation() {
	return gAllowHorizontalRotation;
}
	
bool graphics_allowVerticalOrientation() {
	return gAllowVerticalRotation;
}
	
	// ==================================================================
// PORT
// implementation of publicly declared methods (ie, exposed outside
// system framework by framework/inc/graphics.h)
// these implementations are not OS dependent
// ==================================================================

void
Port::fillRectEx(const Quad& quad, uint32 pattern, uint8 patternShift, Color rgba) {
  #ifdef DEBUG
    if ( pattern != Graphics::solidPat ) {
        OS::_DOUT("fillRectEx: pattern NOT IMPLEMENTED");
    }
  #endif
    fillRect(quad, rgba);
}

void
Port::frameRectEx(const Quad& quad, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba) {
  #ifdef DEBUG
    if ( pattern != Graphics::solidPat ) {
        OS::_DOUT("frameRectEx: pattern NOT IMPLEMENTED");
    }
  #endif
	Point p1 = quad.points[3];
	for (int i = 0; i<4; i++) {
		Point p2 = quad.points[i];
		drawLineEx(p1, p2, thickness, pattern, patternShift, rgba);
		p1 = p2;
    }
}

void
Port::drawLineEx(const Point& from, const Point& to, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba) {
  #ifdef DEBUG
    if ( pattern != Graphics::solidPat ) {
        OS::_DOUT("drawLineEx: pattern NOT IMPLEMENTED");
    }
  #endif
    Point p1 = from;
    Point p2 = to;
    while (thickness > 0) {
        drawLine(p1, p2, rgba);
        long dx = to.x - from.x;    // get deltas
        long dy = to.y - from.y;
        long lx = std::abs(dx);     // get absolute deltas (lengths)
        long ly = std::abs(dy);
        if (lx > ly) {
            // horizontal
            ++p1.y;
            ++p2.y;
        } else {
            ++p1.x;
            ++p2.x;
        }
        --thickness;
    }
}

void
Port::fillRect(const Quad& quad, Color rgba) {
	PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (quad.getBounds().intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
	port.setOpenGLModesForDrawing((rgba.alpha != 1.0f));  // sets up clip rect too

	glColor4f( rgba.red, rgba.green, rgba.blue, rgba.alpha );

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f( quad.points[lftBot].x, quad.points[lftBot].y );
	glVertex2f( quad.points[lftTop].x, quad.points[lftTop].y );
	glVertex2f( quad.points[rgtBot].x, quad.points[rgtBot].y );
	glVertex2f( quad.points[rgtTop].x, quad.points[rgtTop].y );
	glEnd();
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::fillRectWithGradient(const Quad& quad, Color startColor, Color endColor) {
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (quad.getBounds().intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
	port.setOpenGLModesForDrawing( (startColor.alpha < 1.0f) || (endColor.alpha < 1.0f) );  // sets up clip rect too

	glBegin(GL_TRIANGLE_STRIP);
	glVertexColor4f ( endColor.red, endColor.green, endColor.blue, endColor.alpha);
	glVertex2f( quad.points[lftBot].x, quad.points[lftBot].y );
	glVertexColor4f ( startColor.red, startColor.green, startColor.blue, startColor.alpha);
	glVertex2f( quad.points[lftTop].x, quad.points[lftTop].y );
	glVertexColor4f ( endColor.red, endColor.green, endColor.blue, endColor.alpha);
	glVertex2f( quad.points[rgtBot].x, quad.points[rgtBot].y );
	glVertexColor4f ( startColor.red, startColor.green, startColor.blue, startColor.alpha);
	glVertex2f( quad.points[rgtTop].x, quad.points[rgtTop].y );
	glEnd();
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::frameRect(const Quad& quad, Color rgba) {
	PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (quad.getBounds().intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
	port.setOpenGLModesForDrawing((rgba.alpha < 1.0f));  // sets up clip rect too

	glColor4f ( (float) rgba.red, rgba.green, rgba.blue, rgba.alpha);

	glBegin(GL_LINE_LOOP);
	glVertex2f( quad.points[lftBot].x, quad.points[lftBot].y );
	glVertex2f( quad.points[rgtBot].x, quad.points[rgtBot].y );
	glVertex2f( quad.points[rgtTop].x, quad.points[rgtTop].y );
	glVertex2f( quad.points[lftTop].x, quad.points[lftTop].y );
	glEnd();

/*	for some reason this mode generates a lot of flicker... possibly because it is drawing directly to the screen and not to the back buffer?
	GLshort thePoints[] = {
			rect.left, rect.top, // origin of the line
			rect.right, rect.top, // next line segment
			rect.right, rect.bottom, // next line segment
			rect.left, rect.bottom  // next line segment and back to first
	};
	glVertexPointer(2, GL_SHORT, 0, thePoints);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
 */
	port.mNeedRedraw = true;
	gPortDirty = true;
}

Rect
Port::getDrawingArea() {
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
    return port.mDrawingRect;
}

Rect
Port::getClipRect() {
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
    return port.mClipRect;
}

void
Port::setClipRect(const Rect& rect) {
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
    // make sure clip rect is never outside drawing area
    port.mClipRect.top = std::max<long>(rect.top, port.mDrawingRect.top);
    port.mClipRect.left = std::max<long>(rect.left, port.mDrawingRect.left);
    port.mClipRect.bottom = std::min<long>(rect.bottom, port.mDrawingRect.bottom);
    port.mClipRect.right = std::min<long>(rect.right, port.mDrawingRect.right);
    mClipChanged = true;
}

void
Port::drawLine(const Point& from, const Point& to, Color rgba) {

	PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	Rect bounds(from, to);
	if (bounds.left > bounds.right) { bounds.right = bounds.left; bounds.left = to.x; }
	if (bounds.top > bounds.bottom) { bounds.bottom = bounds.top; bounds.top = to.y; }
	if (bounds.height() == 0) bounds.setHeight(1);
	if (bounds.width() == 0) bounds.setWidth(1);
	if (bounds.intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
	port.setOpenGLModesForDrawing((rgba.alpha < 1.0f));  // sets up clip rect too

	glColor4f ( (float) rgba.red, rgba.green, rgba.blue, rgba.alpha);

	glBegin(GL_LINES);
	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);
	glEnd();


/*	GLshort thePoints[] = {
		from.x, from.y, // origin of the line
		to.x, to.y // end of line
	};
	glVertexPointer(2, GL_SHORT, 0, thePoints);
	glDrawArrays(GL_LINES, 0, 2); */
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::frameOval(const Point& centerPt, float xRadius, float yRadius, Color rgba)
{
}

void
Port::fillOval(const Point& centerPt, float xRadius, float yRadius, Color rgba)
{
}

void
Port::frameCircle(const Point& centerPt, float radius, Color rgba)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data

    // calc the actual, clipped rect that will be altered
    Rect r;
    r.top = std::max<long>(centerPt.y - (long)radius, port.mClipRect.top);
    r.left = std::max<long>(centerPt.x - (long)radius, port.mClipRect.left);
    r.bottom = std::min<long>(centerPt.y + (long)radius, port.mClipRect.bottom-1);
    r.right = std::min<long>(centerPt.x + (long)radius, port.mClipRect.right-1);

	if (r.intersection(port.drawableRect()).empty()) return; // exit early if completely clipped

 	port.setOpenGLModesForDrawing((rgba.alpha < 1.0f));

	glColor4f ( (float) rgba.red, rgba.green, rgba.blue, rgba.alpha);

	// now offset the rect by the centerPt
	r.moveLeft(centerPt.x);
	r.moveUp(centerPt.y);
	// we would use these for clipping, but we are not going to do clipping right now

	float r2 = radius * radius; // precalc radius squared, we will need it a lot
	float x;
	float y;
	float centerY = centerPt.y;
	float centerX = centerPt.x;

	float lastX = 0;
	float lastY = radius;
	// iterating y by 1/16 of the radius to give us values for x
	// this divides the circle into 64 segments (or less if the circle is < 16 pixels in diameter)
	// for a consistent drawing time
	glBegin(GL_LINES);
	float radDiv16 = radius / 16.0f;
	int inc = std::ceil(radDiv16);
	for (long d = inc; d <= radius; d += inc) {
		x = (float)d;
		y = std::sqrt(r2 - (x*x));
		glVertex2f(centerX - lastX, centerY - lastY); // origin of the line
		glVertex2f(centerX - x, centerY - y); // ending point of the line

		glVertex2f(centerX + lastX, centerY - lastY); // origin of the line
		glVertex2f(centerX + x, centerY - y); // ending point of the line

		glVertex2f(centerX - lastX, centerY + lastY); // origin of the line
		glVertex2f(centerX - x, centerY + y); // ending point of the line

		glVertex2f(centerX + lastX, centerY + lastY); // origin of the line
		glVertex2f(centerX + x, centerY + y); // ending point of the line
		lastX = x;
		lastY = y;
	} // end radius for loop
	glVertex2f(centerX - lastX, centerY - lastY); // origin of the line
	glVertex2f(centerX - radius, centerY); // ending point of the line

	glVertex2f(centerX + lastX, centerY - lastY); // origin of the line
	glVertex2f(centerX + radius, centerY); // ending point of the line

	glVertex2f(centerX - lastX, centerY + lastY); // origin of the line
	glVertex2f(centerX - radius, centerY); // ending point of the line

	glVertex2f(centerX + lastX, centerY + lastY); // origin of the line
	glVertex2f(centerX + radius, centerY); // ending point of the line
	glEnd( );
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::fillCircle(const Point& centerPt, float radius, Color rgba)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data

    // calc the actual, clipped rect that will be altered
    Rect r;
    r.top = std::max<long>(centerPt.y - (long)radius, port.mClipRect.top);
    r.left = std::max<long>(centerPt.x - (long)radius, port.mClipRect.left);
    r.bottom = std::min<long>(centerPt.y + (long)radius, port.mClipRect.bottom-1);
    r.right = std::min<long>(centerPt.x + (long)radius, port.mClipRect.right-1);

	if (r.intersection(port.drawableRect()).empty()) return; // exit early if completely clipped

	port.setOpenGLModesForDrawing((rgba.alpha < 1.0f));

	glColor4f ( (float) rgba.red, rgba.green, rgba.blue, rgba.alpha);

	// now offset the rect by the centerPt
	r.moveLeft(centerPt.x);
	r.moveUp(centerPt.y);
	// we would use these for clipping, but we are not going to do clipping right now

	float r2 = radius * radius; // precalc radius squared, we will need it a lot
	float x;
	float y;
	float centerY = centerPt.y;
	float centerX = centerPt.x;
	float lastX = 0;
	float lastY = radius;

	// iterating y by 1/16 of the radius to give us values for x
	// this divides the circle into 64 segments (or less if the circle is < 16 pixels in diameter)
	// for a consistent drawing time
	float radDiv16 = radius / 16.0f;
	int inc = std::ceil(radDiv16);
	for (long d = inc; d <= radius; d += inc) {
		x = (float)d;
		y = std::sqrt(r2 - (x*x));
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(centerX - x, centerY - y); // ending point of the line
		glVertex2f(centerX - lastX, centerY - lastY); // origin of the line
		glVertex2f(centerX + x, centerY - y); // ending point of the line
		glVertex2f(centerX + lastX, centerY - lastY); // origin of the line
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(centerX - x, centerY + y); // ending point of the line
		glVertex2f(centerX - lastX, centerY + lastY); // origin of the line
		glVertex2f(centerX + x, centerY + y); // ending point of the line
		glVertex2f(centerX + lastX, centerY + lastY); // origin of the line
		glEnd();
		lastX = x;
		lastY = y;
	} // end radius for loop
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(centerX - lastX, centerY - lastY); // origin of the line
	glVertex2f(centerX - radius, centerY); // ending point of the line
	glVertex2f(centerX + lastX, centerY - lastY); // origin of the line
	glVertex2f(centerX + radius, centerY); // ending point of the line
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(centerX - lastX, centerY + lastY); // origin of the line
	glVertex2f(centerX - radius, centerY); // ending point of the line
	glVertex2f(centerX + lastX, centerY + lastY); // origin of the line
	glVertex2f(centerX + radius, centerY); // ending point of the line
	glEnd( );
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::frameRoundRect(const Rect& rect, float radius, Color rgb)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (rect.intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
	port.setOpenGLModesForDrawing((rgb.alpha < 1.0f));

    // calc the actual, clipped rect that will be altered
    Rect r;
    r.top = std::max<long>(rect.top, port.mClipRect.top);
    r.left = std::max<long>(rect.left, port.mClipRect.left);
    r.bottom = std::min<long>(rect.bottom, port.mClipRect.bottom-1);
    r.right = std::min<long>(rect.right, port.mClipRect.right-1);

    // make sure there is actually something to draw
    if ((r.width() > 0) && (r.height() > 0)) {
		 // limit radius
		if ((radius * 2) > rect.width()) {
			radius = (float)rect.width()/2.0f;
		}
		if ((radius * 2) > rect.height()) {
			radius = (float)rect.height()/2.0f;
		}

/*		float r2 = radius * radius; // precalc radius squared, we will need it a lot
		float last = radius;
		float x;
		float y;
		Point centerPt = rect.centerPoint();
		long extraX = (long)(((float)rect.width() / 2.0f) - radius);
		long extraY = (long)(((float)rect.height() / 2.0f) - radius);
		bool swapped = false;
*/
		long lradius = (long)radius;
		drawLine(Point(rect.left + lradius, rect.top), Point(rect.right - lradius, rect.top), rgb);
		drawLine(Point(rect.left + lradius, rect.bottom), Point(rect.right - lradius, rect.bottom), rgb);
		drawLine(Point(rect.left, rect.top + lradius), Point(rect.left, rect.bottom - lradius), rgb);
		drawLine(Point(rect.right, rect.top + lradius), Point(rect.right, rect.bottom - lradius), rgb);

    }
	port.mNeedRedraw = true;
	gPortDirty = true;
}

void
Port::fillRoundRect(const Rect& rect, float radius, Color rgb)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data

    // calc the actual, clipped rect that will be altered
    Rect r;
    r.top = std::max<long>(rect.top, port.mClipRect.top);
    r.left = std::max<long>(rect.left, port.mClipRect.left);
    r.bottom = std::min<long>(rect.bottom, port.mClipRect.bottom-1);
    r.right = std::min<long>(rect.right, port.mClipRect.right-1);

	if (r.intersection(port.drawableRect()).empty()) return; // exit early if completely clipped

    // make sure there is actually something to draw
    if ((r.width() > 0) && (r.height() > 0)) {
		// limit radius
		if ((radius * 2) > rect.width()) {
			radius = (float)rect.width()/2.0f;
		}
		if ((radius * 2) > rect.height()) {
			radius = (float)rect.height()/2.0f;
		}

/*
		float r2 = radius * radius; // precalc radius squared, we will need it a lot
		float x;
		float y;
		Point centerPt = rect.centerPoint();
		long extraX = (long)(((float)rect.width() / 2.0f) - radius);
		long extraY = (long)(((float)rect.height() / 2.0f) - radius);
		long rowtop = r.top;      // lowest row drawn in top section
		long rowbot = r.bottom;   // highest row drawn in bottom section
		bool swapped = false;
 */
	}
	port.mNeedRedraw = true;
	gPortDirty = true;
}

// returns the font currently in use for the port
Font*     
Port::getCurrentFont(uint32 style)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	style &= TEXT_STYLES_MASK;
	return port.mFontForStyle[style];
}

// set the name of the font used for this port
// this undoes any setFontNameForStyle calls you may have already made on this port
void     
Port::setFont(Font* font)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (!font) {
		font = port.mGraphicsMgr->createFont("Arial");
	}
	if (!font) return;
	for (int i = 0; i < NUM_TEXT_STYLES; i++) {
		if (port.mFontForStyle[i]) {
			port.mFontForStyle[i]->release();
		}
		port.mFontForStyle[i] = font;
		font->addRef();
	}
}

// set the name of the font used for a particular style of text in this port
// does not affect what font is used for any other styles
// NOTE: only works for textStyle_Bold at the moment
void     
Port::setFontForStyle(Font* font, uint32 style)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (!font) {
		font = port.mGraphicsMgr->createFont("Arial");
	}
	if (!font) return;
	style &= TEXT_STYLES_MASK;
	if (port.mFontForStyle[style]) {
		port.mFontForStyle[style]->release();
	}
	port.mFontForStyle[style] = font;
	font->addRef();
}

// set a factor by which all font sizes are enlarged or reduced
// > 1.0 is enlarge, < 1.0 is reduce, 1.0 is no scaling
void 
Port::setFontScalingFactor(float scaleBy)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	port.mFontScalingFactor = scaleBy;
}

void     
Port::drawImage(Image* img, const Point& loc) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->draw(loc);
}

void     
Port::drawImage(Image* img, const Quad& quad) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->draw(quad);
}

void     
Port::drawImage(Image* img, const Rect& r, Image::FitType fitType, bool clipOverflow) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->draw(r, fitType, clipOverflow);
}

void     
Port::drawImage(ImageStrip* img, int frame, const Point& loc) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawFrame(loc, frame);
}

void     
Port::drawImage(ImageStrip* img, int frame, const Quad& quad) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawFrame(quad, frame);
}

void     
Port::drawImage(ImageStrip* img, int frame, const Rect& r, Image::FitType fitType, bool clipOverflow) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawFrame(r, frame, fitType, clipOverflow);
}

void 
Port::drawTexture(Image* img, const Rect& r) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawTexture(r);
}

void 
Port::drawTexture(ImageStrip* img, int frame, const Rect& r) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawTextureFrame(r, frame);
}

void 
Port::drawTexturedSphere(Image* img, const Point& loc, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawTexturedSphere(loc, radius, rotation, polarOffsetRadians, lightOffsetRadians);
}

void 
Port::drawTexturedSphere(ImageStrip* img, int frame, const Point& loc, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians) {
	if (img->mPort != this) {
		img->setPort(this);
	}
	img->drawTexturedSphereFrame(loc, frame, radius, rotation, polarOffsetRadians, lightOffsetRadians);
}


void
Port::drawText(const char* text, const Point& loc, int size, uint32 style, Color rgba) {
	if (text == 0) return;
	int len = std::strlen(text);
	if (len == 0) return;
	// look for text entirely outside clip rect
	PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	Rect drawable = port.drawableRect();
	// cheapest checks, off to right and left
	if (((style & (Graphics::textStyle_Centered | Graphics::textStyle_RightJustified)) == 0) && (loc.x >= drawable.right)) return; // exit early if completely clipped
	if (((style & Graphics::textStyle_RightJustified) == Graphics::textStyle_RightJustified) && (loc.x <= drawable.left)) return; // exit early if completely clipped
	// further checks require font info
	FontImpl* font = dynamic_cast<FontImpl*> ( getCurrentFont(style) );
	if (!font) return;
	Rect textRect;
	textRect.bottom = loc.y + std::ceil( font->getFontDescent(size, style) );
	if (textRect.bottom < drawable.top) return; // exit early if completely clipped
	textRect.top = loc.y - std::ceil( font->getFontAscent(size, style) );
	if (textRect.top > drawable.bottom) return; // exit early if completely clipped
	textRect.left = loc.x;

	// adjust for text justification
	int textwidth = getTextWidth(text, size, style, len);
	if (style & Graphics::textStyle_Centered) {
		textRect.left -= (textwidth/2);     // centered means the point given is the centerpoint for the text
	} else if (style & Graphics::textStyle_RightJustified) {
		textRect.left -= textwidth;         // otherwise the point given is the right-side end of the text
	}
	textRect.setWidth(textwidth);
	if (drawable.intersection(textRect).empty()) return; // exit early if completely clipped
#ifdef PDG_DEBUG_TEXT_DRAWING
	port.frameRect(textRect, PDG_MAGENTA_COLOR); // draw text bounding box
	port.drawLine(Point(textRect.left, loc.y), Point(textRect.right, loc.y), PDG_RED_COLOR); // draw baseline
#endif
	graphics_drawText(port, text, len, (Quad)textRect, size, style, rgba);
	port.mNeedRedraw = true;
	gPortDirty = true;
}


void
Port::drawText(const char* text, const Quad& quad, int size, uint32 style, Color rgba) {
	if (text == 0) return;
	int len = std::strlen(text);
	if (len == 0) return;
	PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if (quad.getBounds().intersection(port.drawableRect()).empty()) return; // exit early if completely clipped
#ifdef PDG_DEBUG_TEXT_DRAWING
	port.frameRect(quad, PDG_MAGENTA_COLOR); // draw text bounding box
#endif
	graphics_drawText(port, text, len, quad, size, style, rgba);
	port.mNeedRedraw = true;
	gPortDirty = true;
}


// Note: The image you set will be released when the cursor is set again or reset so
// make sure you allocate Image memory for this function to release. -ADD
void
Port::setCursor(Image* cursorImage, const Point& hotSpot)
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data

    cursorImage->addRef();

	// If there is already a cursor set....
	if(port.mCurrentCursor)
	{
		// Delete the old cursor
		port.mCurrentCursor->release();
		port.mCurrentCursor = 0;
	}
	else
	{
		// otherwise just turn off the OS cursor
//		setHardwareCursorVisible(false);
	}

	// remove the cursor background storage if there is any
	if (port.mCurrentCursorBackground) {
	    std::free(port.mCurrentCursorBackground);
	    port.mCurrentCursorBackground = 0;
	    port.mCurrentCursorBackgroundSize = 0;
	}

	// Set the new cursor
	port.mCurrentCursor = cursorImage;
	port.mHotSpot = hotSpot;
}

Image*
Port::getCursor()
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	return port.mCurrentCursor;
}

void
Port::resetCursor()
{
    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	if(port.mCurrentCursor)
	{
		port.mCurrentCursor->release();
		port.mCurrentCursor = 0;
		platform_setHardwareCursorVisible(true);
	}

	// remove the cursor background storage if there is any
	if (port.mCurrentCursorBackground) {
	    std::free(port.mCurrentCursorBackground);
	    port.mCurrentCursorBackground = 0;
	    port.mCurrentCursorBackgroundSize = 0;
	}
}

int
Port::startTrackingMouse(const Rect& rect, void* userData)
{
//    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	// TODO: call private methods in PortImp to add a tracking rect
	return 0;
}

void
Port::stopTrackingMouse(int trackingRef)
{
//    PortImpl& port = static_cast<PortImpl&>(*this); // get us access to our private data
	// TODO: call private methods in PortImp to remove a tracking rect
}

Port::Port() : mClipChanged(false)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mPortScriptObj);
#endif
}

Port::~Port()
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupPortScriptObject(mPortScriptObj);
#endif
}


// ==================================================================
// PORTIMPL
// implementation of hidden implementation methods defined in PortImpl
// these implementations are not OS dependent
// ==================================================================

void
PortImpl::internalSaveCursorBackground()
{
	if(!mCurrentCursor)
	{
		return;
	}

	// only allocate the cursor background when the cursor changes
	// this saves lots of allocations and deallocations every time
	// the cursor is drawn
/*	if(!mCurrentCursorBackground) {
	    mCurrentCursorBackgroundSize = mBufInfo.bytespixel * mCurrentCursor->width * mCurrentCursor->height;
        mCurrentCursorBackground = (addr) std::malloc(mCurrentCursorBackgroundSize);
	}

	Point mousePt = OS::getMouse();
	mousePt = mousePt - mHotSpot;
	Point cursorExtent = mousePt;
	cursorExtent.x += mCurrentCursor->width;
	cursorExtent.y += mCurrentCursor->height;
	// Don't save the background if is mouse is off the drawing surface.
	if(mousePt.x < 0 || cursorExtent.x >= mBufInfo.width ||
	   mousePt.y < 0 || cursorExtent.y >= mBufInfo.height)
	{
		return;
	}

    addr deviceDataPtr = (addr) mBufInfo.data;
    int  devicePitch = mBufInfo.pitch;
    int  deviceBytesPerPixel = mBufInfo.bytespixel;
    long deviceYByteOffset = (mousePt.y * devicePitch);
    long deviceXByteOffset;
	if (deviceBytesPerPixel == 4) {     // 32 bit color
	    deviceXByteOffset = mousePt.x << 2;
	} else if (deviceBytesPerPixel == 2) {  // 16 bit color
	    deviceXByteOffset = mousePt.x << 1;
	} else {
        deviceXByteOffset = mousePt.x;  // 256 color
	}

    register long i = mCurrentCursor->height;    // number of pixel rows to grab
	register addr dst = mCurrentCursorBackground;
	register addr src = deviceDataPtr + deviceYByteOffset + deviceXByteOffset;
	register long bytes = deviceBytesPerPixel * mCurrentCursor->width;
    while (i-- > 0) {
        CHECK_PTR(src, mBufInfo.data, mBufInfo.dataSize);
        CHECK_PTR(src+bytes-1, mBufInfo.data, mBufInfo.dataSize);
        CHECK_PTR(dst, mCurrentCursorBackground, mCurrentCursorBackgroundSize);
        CHECK_PTR(dst+bytes-1, mCurrentCursorBackground, mCurrentCursorBackgroundSize);
        std::memcpy(dst, src, bytes);
        dst += bytes;
        src += devicePitch;
    } */

}

bool
PortImpl::lockDrawingSurface()
{
	return true;
}

void
PortImpl::unlockDrawingSurface()
{
}


void
PortImpl::internalDrawCursor()
{
/*	if(mCurrentCursor)
	{
		if (!lockDrawingSurface())
		{
			return;
		}
		Point mousePt = OS::getMouse();
		mousePt = mousePt - mHotSpot;
		Point cursorExtent = mousePt;
		cursorExtent.x += mCurrentCursor->width;
		cursorExtent.y += mCurrentCursor->height;
		// Don't draw the cursor if the mouse is off the drawing surface.
		if(mousePt.x < 0 || cursorExtent.x >= mBufInfo.width ||
			mousePt.y < 0 || cursorExtent.y >= mBufInfo.height)
		{
			return;
		}
		mCurrentCursor->draw(mousePt);
	} */
}

void
PortImpl::internalRestoreCursorBackground()
{
/*	if(!mCurrentCursorBackground)
	{
		return;
	}
	if(!mCurrentCursor)
	{
		return;
	}
    if (!lockDrawingSurface())
	{
		return;
	}

	Point mousePt = OS::getMouse();
	mousePt = mousePt - mHotSpot;
	Point cursorExtent = mousePt;
	cursorExtent.x += mCurrentCursor->width;
	cursorExtent.y += mCurrentCursor->height;
	// Don't save the background if is mouse is off the drawing surface.
	if(mousePt.x < 0 || cursorExtent.x >= mBufInfo.width ||
	   mousePt.y < 0 || cursorExtent.y >= mBufInfo.height)
	{
		return;
	}

    addr deviceDataPtr = (addr) mBufInfo.data;
    int  devicePitch = mBufInfo.pitch;
    int  deviceBytesPerPixel = mBufInfo.bytespixel;
    long deviceYByteOffset = (mousePt.y * devicePitch);
    long deviceXByteOffset;
	if (deviceBytesPerPixel == 4) {     // 32 bit color
	    deviceXByteOffset = mousePt.x << 2;
	} else if (deviceBytesPerPixel == 2) {  // 16 bit color
	    deviceXByteOffset = mousePt.x << 1;
	} else {
        deviceXByteOffset = mousePt.x;  // 256 color
	}

    register long i = mCurrentCursor->height;    // number of pixel rows to grab
	register addr src = mCurrentCursorBackground;
	register addr dst = deviceDataPtr + deviceYByteOffset + deviceXByteOffset;
	register long bytes = deviceBytesPerPixel * mCurrentCursor->width;
    while (i-- > 0) {
        CHECK_PTR(dst, mBufInfo.data, mBufInfo.dataSize);
        CHECK_PTR(dst+bytes-1, mBufInfo.data, mBufInfo.dataSize);
        CHECK_PTR(src, mCurrentCursorBackground, mCurrentCursorBackgroundSize);
        CHECK_PTR(src+bytes-1, mCurrentCursorBackground, mCurrentCursorBackgroundSize);
        std::memcpy(dst, src, bytes);
        src += bytes;
        dst += devicePitch;
    } */
}

void
PortImpl::resizePort(long width, long height) {
    Rect r(width, height);
    mDrawingRect = r;
    mClipRect = r;
    mNeedRedraw = true;
}

//	bool avoidRecursion = false;

void
PortImpl::setOpenGLModesForDrawing(bool useAlpha) {
//	if (avoidRecursion) return;
//	avoidRecursion = true;
    if (!gModesSet) {
        long width = mDrawingRect.width();
        long height = mDrawingRect.height();
        long swidth = width;
        long sheight = height;
        if (   (gEffectiveScreenPos == pdg::screenPos_Rotated90Clockwise)
            || (gEffectiveScreenPos == pdg::screenPos_Rotated90CounterClockwise)) {
            sheight = width;
            swidth = height;
        }

        // set orthograhic 1:1  pixel transform in local view coords
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(2.0f / swidth, -2.0f /  sheight, 1.0f);
        glRotatef(gRotationAngle, 0, 0, 1);
        glTranslatef(-width / 2.0f, -height / 2.0f, 0.0f);
        glDisable(GL_DEPTH_TEST); // ensure stuff we are about to draw is not removed by depth test
        gModesSet = true; // don't do this again till next frame
    }

	if (useAlpha) {
		glEnable(GL_BLEND); // for line fading
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ditto
	} else {
		glDisable(GL_BLEND);
	}

    if ( mClipChanged) {
        if ( mClipRect.empty() ) {
            glDisable(GL_SCISSOR_TEST);
        } else {
            // scissor is in OpenGL window coordinates, so 0,0 is bottom left with flipped Y axis
            if (gEffectiveScreenPos == screenPos_Normal) {
                glScissor(mClipRect.left, mDrawingRect.bottom - mClipRect.bottom, mClipRect.width(), mClipRect.height() );
            } else if (gEffectiveScreenPos == screenPos_Rotated180) {
                glScissor(mDrawingRect.right - mClipRect.right, mClipRect.top, mClipRect.width(), mClipRect.height() );
            } else if (gEffectiveScreenPos == screenPos_Rotated90Clockwise) {
                glScissor(mClipRect.top, mClipRect.left, mClipRect.height(), mClipRect.width() );
            } else if (gEffectiveScreenPos == screenPos_Rotated90CounterClockwise) {
                Rect sr(mClipRect.height(), mClipRect.width() );
                sr.moveTo(mDrawingRect.bottom - mClipRect.bottom, mDrawingRect.right - mClipRect.right);
                glScissor(sr.left, sr.top, sr.width(), sr.height() );
            }
            glEnable(GL_SCISSOR_TEST);  // make sure we are clipping
        }
        mClipChanged = false;
    }
//	glEnableClientState(GL_VERTEX_ARRAY);
//	avoidRecursion = false;
}

PortImpl::PortImpl(GraphicsManager* graphicsMgr)
: Port(),
  mGraphicsMgr(graphicsMgr),
  mNeedRedraw(false),
  mDrawingRect(),
  mClipRect(),
  mCurrentCursor(0),
  mHotSpot(),
  mCurrentCursorBackground(0),
  mFontScalingFactor(0.0)
{
	for (int i = 0; i<NUM_TEXT_STYLES; i++) {
		mFontForStyle[i] = 0;
	}
	setFont();  // set the default font
}

PortImpl::~PortImpl()
{
    // do nothing
	if(mCurrentCursor)
	{
		mCurrentCursor->release();
		mCurrentCursor = 0;
		platform_setHardwareCursorVisible(true);
	}
	if (mCurrentCursorBackground) {
	    std::free(mCurrentCursorBackground);
	    mCurrentCursorBackground = 0;
	    mCurrentCursorBackgroundSize = 0;
	}
}


} // end namespace pdg

#endif // PDG_NO_GUI

