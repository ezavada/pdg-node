// -----------------------------------------------
// port.h
//
// graphics drawing port functionality
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


#ifndef PDG_PORT_H_INCLUDED
#define PDG_PORT_H_INCLUDED

#ifndef PDG_NO_GUI

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/color.h"
#include "pdg/sys/image.h"
#include "pdg/sys/imagestrip.h"
#include "pdg/sys/font.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

//! \defgroup Graphics
//! Collection of classes, types and constants that are used for drawing

class Image;

// -----------------------------------------------------------------------------------
//! Graphics
//! A collection of static enumerations that are used when drawing
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

namespace Graphics {

	//extended drawing, with bit patterns, line thickness, and opacity
	//thickness: width of line (0 - 255), 0 means no line
	//pattern: a bit mask for which pixels are drawn and which are skipped. 0xff is solid, 0xf0 is dashed, 0x11 is dotted, etc..
	//patternShift: how much to shift the pattern for each successive line. pattern = 0x66 and shift = 1 gives 50% grey pattern

	enum {
		solidPat = 0xffffffff,
		greyPat  = 0x66666666,
		emptyPat = 0x00000000,
		dashPat  = 0xff00ff00,
		dotPat   = 0x10101010
	};

};



// -----------------------------------------------------------------------------------
//! Port
//! An area in which drawing is done, a viewport. This could be a full screen drawing
//! context or a window.
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

class Port {
public:

    Rect     getDrawingArea();
    Rect     getClipRect();
    void     setClipRect(const Rect& rect);

    // normal drawing
	//! you can also pass in a RotatedRect instead of a Quad, since the conversion is automatic
	void     fillRect(const Rect& r, Color rgba = PDG_BLACK_COLOR);
	void     fillRect(const RotatedRect& rr, Color rgba = PDG_BLACK_COLOR);
    void     fillRect(const Quad& quad, Color rgba = PDG_BLACK_COLOR);
	void     frameRect(const Rect& r, Color rgba = PDG_BLACK_COLOR);
	void     frameRect(const RotatedRect& rr, Color rgba = PDG_BLACK_COLOR);
    void     frameRect(const Quad& quad, Color rgba = PDG_BLACK_COLOR);

    void     drawLine(const Point& from, const Point& to, Color rgba = PDG_BLACK_COLOR);

    void     frameOval(const Point& centerPt, float xRadius, float yRadius, Color rgba = PDG_BLACK_COLOR);
    void     fillOval(const Point& centerPt, float xRadius, float yRadius, Color rgba = PDG_BLACK_COLOR);

    void     frameCircle(const Point& centerPt, float radius, Color rgba = PDG_BLACK_COLOR);
    void     fillCircle(const Point& centerPt, float radius, Color rgba = PDG_BLACK_COLOR);

    void     frameRoundRect(const Rect& rect, float radius, Color rgba = PDG_BLACK_COLOR);
    void     fillRoundRect(const Rect& rect, float radius, Color rgba = PDG_BLACK_COLOR);

    // extended drawing, with bit patterns, line thickness
    // thickness: width of line (0 - 255), 0 means no line
    // pattern: a bit mask for which pixels are drawn and which are skipped.
    //    0xff is solid, 0xf0 is dashed, 0x11 is dotted, etc..
    // patternShift: how much to shift the pattern for each successive line.
    //    pattern = 0x66 and shift = 1 gives 50% grey pattern

	//! you can also pass in a Rect or a RotatedRect instead of a Quad, since the conversion is automatic
    void     fillRectEx(const Rect& r, uint32 pattern, uint8 patternShift, Color rgba);
    void     fillRectEx(const RotatedRect& rr, uint32 pattern, uint8 patternShift, Color rgba);
    void     fillRectEx(const Quad& quad, uint32 pattern, uint8 patternShift, Color rgba);

    void     frameRectEx(const Rect& r, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba);
    void     frameRectEx(const RotatedRect& rr, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba);
    void     frameRectEx(const Quad& quad, uint8 thickness, uint32 pattern,
								uint8 patternShift, Color rgba);

    void     drawLineEx(const Point& from, const Point& to, uint8 thickness,
                                uint32 pattern, uint8 patternShift, Color rgba);

	// Fill rectangle with gradient of start to end colors from top to bottom
	//! you can also pass in a Rect or a RotatedRect instead of a Quad, since the conversion is automatic
	void	 fillRectWithGradient(const Rect& r, Color startColor, Color endColor);
	void	 fillRectWithGradient(const RotatedRect& rr, Color startColor, Color endColor);
	void	 fillRectWithGradient(const Quad& quad, Color startColor, Color endColor);

	// image drawing
	
    //! draws image at given location
	void     drawImage(Image* img, const Point& loc);
    //! draws image into a given rectangle, scaling as needed
	void     drawImage(Image* img, const Rect& r);
    //! draws image into a given rectangle, scaling as specified to fit, with optional clipping
	void     drawImage(Image* img, const Rect& r, Image::FitType fitType, bool clipOverflow = false);
    //! draws image at given location, scaling if necessary to fit
	void     drawImage(Image* img, const RotatedRect& rr);
    //! draws image into a given quad-polygon, mapping each of the corners to the corresponding points
	void     drawImage(Image* img, const Quad& quad);	

    //! draws single frame of multiframe image at given location
	void     drawImage(ImageStrip* img, int frame, const Point& loc);
    //! draws single frame of multiframe image into a given rectangle, scaling as needed
	void     drawImage(ImageStrip* img, int frame, const Rect& r);
    //! draws single frame of multiframe image into a given rectangle, scaling as specified to fit, with optional clipping
	void     drawImage(ImageStrip* img, int frame, const Rect& r, Image::FitType fitType, bool clipOverflow = false);
    //! draws single frame of multiframe image at given location, scaling if necessary to fit
	void     drawImage(ImageStrip* img, int frame, const RotatedRect& rr);
    //! draws single frame of multiframe image into a given quad-polygon, mapping each of the corners to the corresponding points
	void     drawImage(ImageStrip* img, int frame, const Quad& quad);	

	// texture drawing

    //! draws image as texture filling given area, replicating as needed to fill entire area
    void    drawTexture(Image* img, const Rect& r);
    //! draws single frame of multiframe image as texture filling given area, replicating as needed to fill entire area
    void    drawTexture(ImageStrip* img, int frame, const Rect& r);
    //! draws image as texture wrapped around a sphere
    void    drawTexturedSphere(Image* img, const Point& loc, float radius, float rotation = 0.0f, const Offset& polarOffsetRadians = Offset(0,0), const Offset& lightOffsetRadians = Offset(0,0));
    //! draws single frame of multiframe image as texture wrapped around a sphere
    void    drawTexturedSphere(ImageStrip* img, int frame, const Point& loc, float radius, float rotation = 0.0f, const Offset& polarOffsetRadians = Offset(0,0), const Offset& lightOffsetRadians = Offset(0,0));

    // text drawing and measurement

    void     drawText(const char* text, const Point& loc, int size,
                            uint32 style = Graphics::textStyle_Plain,
                            Color rgba = PDG_BLACK_COLOR);

	void	 drawText(const char* text, const Rect& r, int size,
							uint32 style = Graphics::textStyle_Plain,
							Color rgba = PDG_BLACK_COLOR);
	void	 drawText(const char* text, const RotatedRect& rr, int size,
                            uint32 style = Graphics::textStyle_Plain,
                            Color rgba = PDG_BLACK_COLOR);
	void	 drawText(const char* text, const Quad& quad, int size,
							uint32 style = Graphics::textStyle_Plain,
							Color rgba = PDG_BLACK_COLOR);

    int      getTextWidth(const char* text, int size,
                            uint32 style = Graphics::textStyle_Plain,
                            int len = -1);

	// returns the current font used to draw
	Font*    getCurrentFont(uint32 style = Graphics::textStyle_Plain);

    // set the font used for this port
	// with no params it sets the font to the default font for the port, which is Arial
    void     setFont(Font* font = 0);

    // set the name of the font used for a particular style of text in this port
    // does not affect what font is used for any other styles
	// passing nil for font resets the font for that style to the default font, which is Arial
    void     setFontForStyle(Font* font, uint32 style);

    // set a factor by which all font sizes are enlarged or reduced
    // > 1.0 is enlarge, < 1.0 is reduce, 1.0 is no scaling
    void     setFontScalingFactor(float scaleBy);

	// start tracking a particular area for mouse enter/leave events
	// when the mouse enters that area, a mouse enter event will be generated
	// when the mouse leaves that area, a mouse leave event will be generated
	// userData is passed back in the enter and leave events
	// returns a reference that can be used to stop the tracking
	int      startTrackingMouse(const Rect& rect, void* userData = 0);

	// stop tracking the mouse for a particular tracking rectangle
	void     stopTrackingMouse(int trackingRef);

	void        setCursor(Image* cursorImage, const Point& hotSpot);
	Image*      getCursor();
	void        resetCursor();

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mPortScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
    Port(); // protected constructor, Ports can't be created directly
    virtual ~Port(); // protected destructor, Ports can't be deleted directly
    bool mClipChanged;
/// @endcond
};

inline void 
Port::fillRect(const Rect& r, Color rgba) {
	fillRect((const Quad)r, rgba);
}

inline void 
Port::fillRect(const RotatedRect& rr, Color rgba) {
	fillRect((const Quad)rr, rgba);
}

inline void 
Port::frameRect(const Rect& r, Color rgba) {
	frameRect((const Quad)r, rgba);
}

inline void 
Port::frameRect(const RotatedRect& rr, Color rgba) {
	frameRect((const Quad)rr, rgba);
}

inline void 
Port::fillRectEx(const Rect& r, uint32 pattern, uint8 patternShift, Color rgba) {
	fillRectEx((const Quad)r, pattern, patternShift, rgba);
}

inline void 
Port::fillRectEx(const RotatedRect& rr, uint32 pattern, uint8 patternShift, Color rgba) {
	fillRectEx((const Quad)rr, pattern, patternShift, rgba);
}

inline void 
Port::frameRectEx(const Rect& r, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba) {
	frameRectEx((const Quad)r, thickness, pattern, patternShift, rgba);
}

inline void 
Port::frameRectEx(const RotatedRect& rr, uint8 thickness, uint32 pattern, uint8 patternShift, Color rgba) {
	frameRectEx((const Quad)rr, thickness, pattern, patternShift, rgba);
}

inline void 
Port::fillRectWithGradient(const Rect& r, Color startColor, Color endColor) {
	fillRectWithGradient((const Quad)r, startColor, endColor);
}

inline void 
Port::fillRectWithGradient(const RotatedRect& rr, Color startColor, Color endColor) {
	fillRectWithGradient((const Quad)rr, startColor, endColor);
}

inline void
Port::drawImage(Image* img, const Rect& r) {
	drawImage(img, (const Quad)r);
}

inline void 
Port::drawImage(Image* img, const RotatedRect& rr) {
	drawImage(img, (const Quad)rr);
}

inline void
Port::drawImage(ImageStrip* img, int frame, const Rect& r) {
	drawImage(img, frame, (const Quad)r);
}

inline void 
Port::drawImage(ImageStrip* img, int frame, const RotatedRect& rr) {
	drawImage(img, frame, (const Quad)rr);
}

inline void 
Port::drawText(const char* text, const Rect& r, int size, uint32 style, Color rgba) {
	drawText(text, (const Quad)r, size, style, rgba);
}

inline void 
Port::drawText(const char* text, const RotatedRect& rr, int size, uint32 style, Color rgba) {
	drawText(text, (const Quad)rr, size, style, rgba);
}


} // end namespace pdg

#endif // PDG_NO_GUI

#endif // PDG_PORT_H_INCLUDED

