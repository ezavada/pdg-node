// -----------------------------------------------
// imagestrip.h
//
// multi-frame image handling functionality
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


#ifndef PDG_IMAGESTRIP_H_INCLUDED
#define PDG_IMAGESTRIP_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/coordinates.h"
#include "pdg/sys/image.h"

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
//! ImageStrip
//! A bit image that can be blitted onto the screen
//! You cannot create images directly, they must be loaded
//! by the Graphics Manager
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

class ImageStrip : public Image {
    friend class Sprite;
public:

	virtual long	getWidth();

	long    getFrameWidth();
	int     getNumFrames();

    // two ways to set the frame information
    void    setFrameWidth(long wid);
    void    setNumFrames(int num);

	//! convenient shortcuts to get the bounds of the image as a rectangle, offset by a point if desired
	//! if the image has multiple frames, it returns the bounds as the frame width, not the total width
	virtual Rect getImageBounds();
	virtual Rect getImageBounds( Point& at );

	//! return a frame of the image an image which can be passed to port draw calls
	virtual Image*  getFrame(int frame) = 0;

    static ImageStrip* createImageStripFromData(const char* imageName, char* imageData, long imageDataLen);
    static ImageStrip* createImageStripFromFile(const char* imageFileName);

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mImageStripScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
    ImageStrip();
    virtual ~ImageStrip();

    int     frames;
    long    frameWidth;

/// @endcond

};

/// @cond INTERNAL
inline ImageStrip::ImageStrip()
	: Image(), frames(0), frameWidth(0) 
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mImageStripScriptObj);
#endif
}

inline ImageStrip::~ImageStrip() {
//                DEBUG_ONLY( OS::_DOUT("dt Image %p", this); )
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupImageStripScriptObject(mImageStripScriptObj);
#endif
}
/// @endcond


inline long 
ImageStrip::getWidth() {
    return (frameWidth) ? frameWidth : width;
}

inline long 
ImageStrip::getFrameWidth() {
	return frameWidth;
}

inline int 
ImageStrip::getNumFrames() {
	return frames;
}

inline Rect 
ImageStrip::getImageBounds() { 
    return Rect((PDG_BASE_COORD_TYPE)((frameWidth) ? frameWidth : width), (PDG_BASE_COORD_TYPE)height);
}

inline Rect 
ImageStrip::getImageBounds( Point& at ) { 
    return Rect(at, (PDG_BASE_COORD_TYPE)((frameWidth) ? frameWidth : width), (PDG_BASE_COORD_TYPE)height);
}

inline void 
ImageStrip::setFrameWidth(long wid) { 
	frameWidth = wid; 
	frames = width/frameWidth; 
}

inline void 
ImageStrip::setNumFrames(int num) { 
	frameWidth = width/num; 
	frames = num; 
}

} // end namespace pdg

#endif // PDG_IMAGESTRIP_H_INCLUDED
