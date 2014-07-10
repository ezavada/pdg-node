// -----------------------------------------------
// image-impl.h
//
// implementation as data, no rendering
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


#ifndef PDG_IMAGE_IMPL_H_INCLUDED
#define PDG_IMAGE_IMPL_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/image.h"
#include "pdg/sys/imagestrip.h"
#include "pdg/sys/color.h"

#ifdef PDG_NO_GUI
  // not really using GL textures, just need to keep track 
  // of what format the image data is stored in
  #define GL_RGB  1
  #define GL_RGBA 2
#endif

namespace pdg {

	class ImageImpl : public ImageStrip {  // all images implement ImageStrip, the distinction is for the caller only
	public:

		void*   data;
		long    pitch;    // bytes per row in the image data
		Color   transparentColor;
		uint32  transparentPixel;   // the value for the transparent pixel
		uint8   opacity;            // how solid is the overlay (255 = completely solid)
        uint8   bpp;                // bits per pixel (32, 24, 16, 8)
		uint32  dataSize;           // total size as allocated of the data ptr in memory
		bool	mRetainData;
		bool	mRetainAlpha;
	  #ifdef DEBUG
		char	mFilename[256];
	  #endif

		SERIALIZABLE_TAG( CLASSTAG_IMAGE );
		SERIALIZABLE_METHODS();

	  #ifdef PDG_NO_GUI
		static pdg::ISerializable* CreateInstance() { return new ImageImpl(); }
	  #endif

		virtual Image*  createImageFromFrame(int frame);
		
		virtual Image*  getFrame(int frame);
		virtual Image*  getSubsection(Rect& r);
		virtual Image*  getSubsection(Quad& quad);
		
//		virtual void    remapToColorFormat(TrueColorFormat& dtcf);
		virtual Image*  createImageScaled(float xscale, float yscale, FilterType filterType);
		virtual Image*  createImageScaledToFit(Rect r, FitType fitType, FilterType filterType);

		virtual void    setTransparentColor(Color rgb);

		virtual Color   getTransparentColor();
		virtual uint8   getAlphaValue(int32 x, int32 y) const;
		virtual Color   getPixel(int32 x, int32 y) const;
		virtual void    setOpacity(uint8 opacity);
		virtual uint8   getOpacity() const;
		virtual void	retainData();
		virtual void	retainAlpha();
		virtual void*   getData();

		virtual void	prepareToRasterize();
		
		virtual void	setEdgeClamping(bool inUseEdgeClamp);

		virtual void	initFromData(char* imageData, long imageDataLen, const char* filename);
    	virtual void    initEmpty(int width, int height, uint8 inBitsPerPixel = 32);

		ImageImpl();
		virtual ~ImageImpl();

		long	 mBufferWidth;
		long	 mBufferHeight;
		bool     mUseEdgeClamp;
		int		 mTextureFormat;

		Image*  mSuperImage;	// when set, indicates that this is a sub image
		int     mFrameNum;		// only valid when this is a sub image
		bool	mIsQuadSection;	// only valid when this is a sub image
			// mFrameNum < 0, mIsQuadSection = true -- this is a Quad Subsection Image
			// mFrameNum < 0, mIsQuadSection = false -- this is a Rect Subsection Image
			// mFrameNum >= 0 -- this is a Frame Sub-Image
		Rect    mSectionRect;	// only valid when (mFrameNum < 0 && !mIsQuadSection)
		Quad	mSectionQuad;	// only valid when (mFrameNum < 0 && mIsQuadSection)

	};


inline Image*  
ImageImpl::createImageFromFrame(int frame) {
	return getFrame(frame);
}

inline Color  
ImageImpl::getTransparentColor() { 
	return transparentColor; 
}

inline void	 
ImageImpl::retainData() { 
	mRetainData = true; 
}

inline void	 
ImageImpl::retainAlpha() { 
	mRetainAlpha = true; 
}

inline void*    
ImageImpl::getData() { 
	return data; 
}

inline void 
ImageImpl::setEdgeClamping(bool inUseEdgeClamp) {
	mUseEdgeClamp = inUseEdgeClamp; 
}

} // end namespace pdg


#endif // PDG_IMAGE_IMPL_H_INCLUDED
