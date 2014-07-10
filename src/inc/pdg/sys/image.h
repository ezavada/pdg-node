// -----------------------------------------------
// image.h
//
// image handling functionality
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


#ifndef PDG_IMAGE_H_INCLUDED
#define PDG_IMAGE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/color.h"
#include "pdg/sys/serializable.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

//! \defgroup Graphics
//! Collection of classes, types and constants that are used for drawing

#ifndef PDG_NO_GUI
class Port;
#endif

// -----------------------------------------------------------------------------------
//! Image
//! A bit image that can be blitted onto the screen
//! You cannot create images directly, they must be loaded
//! by the Graphics Manager
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

class Image : public ISerializable {
// classes that can call protected members
#ifndef PDG_NO_GUI
friend class Port;
friend class ImageOpenGL;
#endif
friend class Sprite;
public:
	//! convenient shortcuts to get the bounds of the image as a rectangle, offset by a point if desired
	//! if the image has multiple frames, it returns the bounds as the frame width, not the total width
	virtual Rect getImageBounds();
	virtual Rect getImageBounds( Point& at );

    enum FitType {
		fit_None,
        fit_Height,             // match heights, don't change proportions
        fit_Width,              // match widths, don't change proportions
        fit_Inside,             // match height or width, but keep image entirely inside rect
        fit_Fill,               // match both height and width, change proportions if needed
        fit_FillKeepProportions // match height or width so that entire rect is filled with image, image size may be larger than rect
    };

	virtual long 	getHeight();
	virtual long	getWidth();

    virtual void    setTransparentColor(Color rgb) = 0;
    virtual Color   getTransparentColor() = 0;

    virtual void    setOpacity(uint8 opacity) = 0;  // 0xff means solid bit blit
	virtual uint8	getOpacity() const = 0;

	virtual void	setEdgeClamping(bool inUseEdgeClamp) = 0;  // hack to approximate OpenGL edge clamping when needed

	//! By default, data is released as soon as the image has been prepared for rasterization
	//! Calling retainData() will retain the data for use even after that has happened
	virtual void	retainData() = 0;
	
	//! Retain the alpha channel even after the image has been prepared for rasterization
	virtual void	retainAlpha() = 0;
	
	//! does whatever is necessary to put the image into the final state for rasterization
	//! On OpenGL this converts it into an OpenGL texture
	virtual void	prepareToRasterize() = 0;
	
	virtual void*   getData() = 0;

	//! If the image has alpha information it returns the alpha value (0 = transparent, 255 = opaque)
	//! If the image has no alpha information, then it returns getOpacity();
	virtual uint8   getAlphaValue(int32 x, int32 y) const = 0;

	//! return the full color info, including alpha channel, for the pixel at the given location
	virtual Color   getPixel(int32 x, int32 y) const = 0;

    enum FilterType {
        filter_Box,         // very fast, excellent for 0.5 scale. Pixelates > 1.0
        filter_Normal,
        filter_Triangle,
        filter_Bell,
        filter_BSpline,
        filter_Mitchell,    // slow, but excellent for all sizes
        filter_Lanczos3,     // slower than Mitchell, not as sharp as Mitchell for < 1.0
        filter_Best = filter_Lanczos3
    };

    //! create a new image from a rectangular subsection of the current one
	virtual Image*  getSubsection(Rect& r) = 0;
    //! create a new image from a rectangular subsection of the current one
	Image*          getSubsection(RotatedRect& rr);
    //! create a new image from an Quad subsection of the current one
	virtual Image*  getSubsection(Quad& quad) = 0;

    //! create a new image by scaling the current one by a specific scaling factor
    virtual Image*  createImageScaled(float xscale, float yscale, FilterType filterType = filter_Best) = 0;
    //! create a new image by scaling the current one to fit a new bounding rectangle
    virtual Image*  createImageScaledToFit(Rect r, FitType fitType = fit_Fill, FilterType filterType = filter_Best) = 0;

#ifndef PDG_NO_GUI
    //! control which port this image draws into
    virtual Port*   setPort(Port* newPort);
#endif

    static Image* createImageFromData(const char* imageName, char* imageData, long imageDataLen);
    static Image* createImageFromFile(const char* imageFileName);

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mImageScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
    Image();
    virtual ~Image();
    //! create a new image from a single frame of the current one
	virtual Image*  createImageFromFrame(int frame) = 0;

    long    height;		// read only
    long    width;		// read only

  #ifndef PDG_NO_GUI

    Port*   mPort;

    //! draws image at given location
    virtual void    draw(const Point& loc) = 0;
    //! draws image at given location, scaling if necessary to fit
    virtual void    draw(const Rect& r, FitType fitType = fit_Fill, bool clipOverflow = false) = 0;
    //! draws one frame of a multiframe image at given location, scaling to fit if necessary
    void            draw(const RotatedRect& rr);
    //! draws image into a given quad-polygon, mapping each of the corners to the corresponding points
    virtual void    draw(const Quad& quad) = 0;
	

    //! draws one frame of a multiframe image at given location
    virtual void    drawFrame(const Point& loc, int frame) = 0;
    //! draws one frame of a multiframe image at given location, scaling to fit if necessary
    virtual void    drawFrame(const Rect& r, int frame, FitType fitType = fit_Fill, bool clipOverflow = false) = 0;
    //! draws one frame of a multiframe image at given location, scaling to fit if necessary
    void            drawFrame(const RotatedRect& rr, int frame);
    //! draws one frame of a multiframe image into a given quad-polygon, mapping each of the corners to the corresponding points
    virtual void    drawFrame(const Quad& quad, int frame) = 0;

    //! draws image as texture filling given area, replicating as needed to fill entire area
    virtual void    drawTexture(const Rect& r) = 0;
	//! draws one frame of a multiframe image as texture filling given area, replicating as needed to fill entire area
 	virtual void    drawTextureFrame(const Rect& r, int frame) = 0;

    //! draws image as texture wrapped around a sphere
    virtual void    drawTexturedSphere(const Point& loc, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians) = 0;
    //! draws single frame of multiframe image as texture wrapped around a sphere
    virtual void    drawTexturedSphereFrame(const Point& loc, int frame, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians)= 0;

	//! draws a subsection of the image by scaling to fit the destination rectangle
	virtual void	drawSection(const Rect& r, const Rect& section) = 0;
	//! draws a subsection of the image by scaling to fit the destination quad-polygon
	void			drawSection(const RotatedRect& rr, const Rect& section);
	//! draws a subsection of the image by scaling to fit the destination quad-polygon
	virtual void	drawSection(const Quad& r, const Rect& section) = 0;
	//! draws a subsection (specified as a quad-polygon) of the image by scaling to fit the destination rectangle
	void			drawSection(const Rect& r, const RotatedRect& section);
	//! draws a subsection of the image by scaling to fit the destination quad-polygon
	virtual void	drawSection(const Rect& r, const Quad& section) = 0;
	//! draws a subsection (specified as a quad-polygon) of the image by scaling to fit the destination quad-polygon
	void			drawSection(const RotatedRect& rr, const RotatedRect& section);
	//! draws a subsection (specified as a quad-polygon) of the image by scaling to fit the destination quad-polygon
	virtual void	drawSection(const Quad& r, const Quad& section) = 0;
  #endif
/// @endcond
};

/// @cond INTERNAL

inline 
Image::Image()
	: height(0), width(0)
  #ifndef PDG_NO_GUI
	, mPort(0) 
  #endif
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mImageScriptObj);
#endif
}

inline  
Image::~Image() {
//                DEBUG_ONLY( OS::_DOUT("dt Image %p", this); )
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupImageScriptObject(mImageScriptObj);
#endif
}

inline Rect 
Image::getImageBounds() { 
	return Rect((PDG_BASE_COORD_TYPE)width, (PDG_BASE_COORD_TYPE)height);
}

inline Rect 
Image::getImageBounds( Point& at ) { 
	return Rect(at, (PDG_BASE_COORD_TYPE)width, (PDG_BASE_COORD_TYPE)height); 
}

inline long 
Image::getHeight() {
	return height;
}

inline long 
Image::getWidth() {
	return width;
}

inline Image* 
Image::getSubsection(RotatedRect& rr) { 
	return getSubsection( (Quad&) rr); 
}

#ifndef PDG_NO_GUI

inline Port* 
Image::setPort(Port* newPort) { 
	Port* port = mPort; 
	mPort = newPort; 
	return port; 
}

inline void 
Image::draw(const RotatedRect& rr) { 
	draw( (const Quad) rr); 
}

inline void 
Image::drawFrame(const RotatedRect& rr, int frame) { 
	drawFrame( (const Quad) rr, frame);
}

inline void 
Image::drawSection(const RotatedRect& rr, const Rect& section) { 
	drawSection((const Quad)rr, section); 
}

inline void 
Image::drawSection(const Rect& r, const RotatedRect& section) { 
	drawSection(r, (const Quad)section); 
}

inline void 
Image::drawSection(const RotatedRect& rr, const RotatedRect& section)  { 
	drawSection((const Quad)rr, (const Quad)section); 
}

/// @endcond

#endif // !PDG_NO_GUI

} // end namespace pdg

#endif // PDG_IMAGE_H_INCLUDED
