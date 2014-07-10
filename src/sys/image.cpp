// -----------------------------------------------
// image.cpp
// 
// image as data, no rendering
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

#include "pdg/msvcfix.h"
#include "image-impl.h"
#include "pdg/sys/os.h"
#include "internals.h"

#include <fstream>
#include <cstdlib>

#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
    #define GFX_CHECK_PTR(ptr, block, block_size) CHECK_PTR(ptr, block, block_size)
#else
    #define GFX_CHECK_PTR(ptr, block, block_size)
#endif

typedef char* addr;


#ifndef PDG_NO_GUI
  #include "pdg/sys/graphicsmanager.h"
  #include "image-opengl.h"
  #define NEW_IMAGE(port) new ImageOpenGL(port)
#else
  #define NEW_IMAGE(port) new ImageImpl()
#endif


namespace pdg {


uint32 ImageImpl::getSerializedSize(pdg::ISerializer* serializer) const { 
#ifndef COMPILER_MSVC
	#warning implement ImageImpl::getSerializedSize()
#endif
	return 0; 
}


void ImageImpl::serialize(pdg::ISerializer* serializer) const {
#ifndef COMPILER_MSVC
	#warning implement ImageImpl::serialize()
#endif
}


void ImageImpl::deserialize(pdg::IDeserializer* deserializer) {
#ifndef COMPILER_MSVC
	#warning implement ImageImpl::deserialize()
#endif
}


Image*  ImageImpl::getFrame(int frame) {
	ImageImpl* img = NEW_IMAGE(mPort);
	img->mSuperImage = this;
	img->mFrameNum = frame;
	addRef();
  #ifdef DEBUG
	std::strncpy(img->mFilename, mFilename, 255);
	std::strncat(img->mFilename, " (sub-frame)", 255);
  #endif
	return img;
}

Image*  ImageImpl::getSubsection(Rect& r) {
	ImageImpl* img = NEW_IMAGE(mPort);
	img->mSuperImage = this;
	addRef();
	img->mFrameNum = -1;
	img->mIsQuadSection = false;
	img->mSectionRect = r;
	img->height = r.height();
	img->width = r.width();
  #ifdef DEBUG
	std::strncpy(img->mFilename, mFilename, 255);
	std::strncat(img->mFilename, " (sub-rect)", 255);
  #endif
	return img;
}

Image*  ImageImpl::getSubsection(Quad& quad) {
	ImageImpl* img = NEW_IMAGE(mPort);
	img->mSuperImage = this;
	addRef();
	img->mFrameNum = -1;
	img->mIsQuadSection = true;
	img->mSectionQuad = quad;
	Rect r = quad.getBounds();
	img->height = r.height();
	img->width = r.width();
  #ifdef DEBUG
	std::strncpy(img->mFilename, mFilename, 255);
	std::strncat(img->mFilename, " (sub-quad)", 255);
  #endif
	return img;
}

void	
ImageImpl::prepareToRasterize() {
}

void    
ImageImpl::setTransparentColor(Color rgb) {
	if (mSuperImage) return;  // don't do for subimage
	if (!data) return;  // no data assigned, can't do this yet
	transparentColor = rgb;
	// FIXME: this probably needs to be different for GL_RGBA
	transparentPixel = (uint32)rgb.red << 16 | (uint32)rgb.green << 8 | (uint32)rgb.blue;
	if (mTextureFormat == GL_RGB) {
		// convert RGB --> RGBA so we have an alpha channel
		long tempPitch = ((mBufferWidth*16) + 3) / 4; // closest 4 byte modulus width that has 4 bytes per pixel
		uint8* tempData = (uint8*) std::malloc( mBufferHeight * tempPitch );
		if (!tempData) return;
		std::memset(tempData, 0, mBufferHeight * tempPitch);
		uint8* dst = tempData;
		uint8* src = (uint8*) data;
		for (int row = 0; row < height; row++) {
			uint8* p = (uint8*)dst;
			uint8* sp = (uint8*)src;
			for (int i = 0; i < width; i++) {
				// NOTE: we could do antialiasing here if we grabbed the pixels around and generated an alpha level based on how many
				// of them are transparent
				p[0] = sp[0];
				p[1] = sp[1];
				p[2] = sp[2];
				p += 4;
				sp += 3;
			}
			dst += tempPitch;
			src += pitch;
		}
		std::free(data);
		data = tempData;
		pitch = tempPitch;
		mTextureFormat = GL_RGBA;
	}
	// assign values in the Alpha Channel, 1 wherever transparent pixel appears, 0 where it doesn't
	uint8 tcc[3];
	tcc[0] = rgb.red * 255;
	tcc[1] = rgb.green * 255;
	tcc[2] = rgb.blue * 255;		
	char* src = (char*) data;
	for (int row = 0; row < height; row++) {
		uint8* p = (uint8*)src;
		for (int i = 0; i < width; i++) {
			uint8 alpha = ( (p[0] == tcc[0]) && (p[1] == tcc[1]) && (p[2] == tcc[2]) ) ? 0 : 255;
			p[3] = alpha;
			p += 4;
		}
		src += pitch;
	}
}

void
ImageImpl::setOpacity(uint8 inOpacity) {
    opacity = inOpacity;
}

uint8
ImageImpl::getOpacity() const {
	return opacity;
}

uint8 ImageImpl::getAlphaValue(int32 x, int32 y) const {
	DEBUG_ONLY(
	   if (data == 0) {
		   DEBUG_PRINT("ERROR: Image [%s]", mFilename);
		   DEBUG_BREAK("Attempted to getAlphaValue when no data available. Do you need to call retainAlpha()?");
	   }
	);
	if ( (x < 0) || (x >= width) || (y < 0) || (y >= height) ) {
		return 0; // there is no image outside the image
	}
    long bytespixel = (bpp >> 3);
	if (bytespixel != 4 ) {
		return getOpacity();
	}
	long rowOffset = y * pitch;
	long colOffset = x * bytespixel;
	uint8* src = (uint8*)data + rowOffset + colOffset;
	return src[3];
}
	
Color ImageImpl::getPixel(int32 x, int32 y) const {
	DEBUG_ONLY(
		if (data == 0) {
			DEBUG_PRINT("ERROR: Image [%s]", mFilename);
			DEBUG_BREAK("Attempted to getPixel when no pixel data available. Do you need to call retainData()?");
		}
	);
	if ( (x < 0) || (x >= width) || (y < 0) || (y >= height) ) {
		return Color(0.0f,0.0f,0.0f,0.0f); // there is no image outside the image
	}
    long bytespixel = (bpp >> 3);
	long rowOffset = y * pitch;
	long colOffset = x * bytespixel;
	uint8* src = (uint8*)data + rowOffset + colOffset;
	uint8 alpha = (bytespixel == 4) ? src[3] : 255;
	return Color(src[0], src[1], src[2], alpha);
}

void 
ImageImpl::initFromData(char* imageData, long imageDataLen, const char* filename) {
	if (mSuperImage) return;  // don't do for subimage
  #ifdef DEBUG
	std::strncpy(mFilename, filename, 255);
  #endif
	// pass this on to our platform code
	platform_initImageData((unsigned char*)imageData, imageDataLen, (unsigned char**)&data, &width, 
			&height, &mBufferWidth, &mBufferHeight, &pitch, &mTextureFormat);
	if (mTextureFormat == GL_RGBA) {
		bpp = 32;
//			RedMask   = 0xff000000;
//			GreenMask = 0x00ff0000;
//			BlueMask  = 0x0000ff00;
//			RedMaskOff   = 24;
//			GreenMaskOff = 16;
//			BlueMaskOff  =  8;
	} else {
		bpp = 24;
//			RedMask   = 0xff0000;
//			GreenMask = 0x00ff00;
//			BlueMask  = 0x0000ff;
//			RedMaskOff   = 16;
//			GreenMaskOff =  8;
//			BlueMaskOff  =  0;
	}
}
	
void
ImageImpl::initEmpty(int w, int h, uint8 inBitsPerPixel) {
    // set the output params
    width = w;
    height = h;
    bpp = inBitsPerPixel;

    long bytespixel = (bpp >> 3);
	long row_size = w * bytespixel;    // match the drawing environment's bytes per pixel

	pitch = row_size;
    
    long bufsize =  row_size * h;
	data = (uint8*) std::malloc(bufsize);
    dataSize = bufsize;
}


ImageImpl::ImageImpl() 
	 : ImageStrip(), data(0), pitch(0), transparentPixel(0), opacity(255), 
	   dataSize(0), mRetainData(false), mRetainAlpha(false),
	   mBufferWidth(0), 
	   mBufferHeight(0), mUseEdgeClamp(true), mSuperImage(0), mFrameNum(-1), 
	   mIsQuadSection(false) 
{
  #ifdef DEBUG
	mFilename[0] = 0;
  #endif
}


ImageImpl::~ImageImpl() {
	if (!data) {
		std::free(data);
		data = 0;
	}
}

Image* 
Image::createImageFromData(const char* imageName, char* imageData, long imageDataLen) {
	// create a new image and initialize it from the data
  #ifndef PDG_NO_GUI
	Port* port = GraphicsManager::getSingletonInstance()->getMainPort();
  #endif // ! PDG_NO_GUI
	ImageImpl *img = NEW_IMAGE(port);
	img->initFromData(imageData, imageDataLen, imageName);
	img->addRef();
	return img;
}

Image* 
Image::createImageFromFile(const char* imageFileName) {
	Image *img = 0;
	std::ifstream file;
	file.open(imageFileName, std::ios::binary);
	file.seekg(0, std::ios::end);
	int len = file.tellg();
	if (len != -1) {
		char* imageData = (char*) std::malloc(len);
		if (imageData != NULL) {
			file.seekg(0, std::ios::beg);
			file.read(imageData, len);
			img = Image::createImageFromData(imageFileName, imageData, len);
			std::free(imageData);  // always free the data, the image will have copied it if necessary
		}
	}
	DEBUG_ONLY( else {
		OS::_DOUT("Couldn't read image file: [%s]", imageFileName);
	} )
	file.close();
	return img;
}

ImageStrip* 
ImageStrip::createImageStripFromData(const char* imageName, char* imageData, long imageDataLen) {
	return dynamic_cast<ImageStrip*>(Image::createImageFromData(imageName, imageData, imageDataLen));
}

ImageStrip* 
ImageStrip::createImageStripFromFile(const char* imageFileName) {
	return dynamic_cast<ImageStrip*>(Image::createImageFromFile(imageFileName));
}

struct uintColor {
	uint8  red;
	uint8  green;
	uint8  blue;
};



/*void ImageImpl::remapToColorFormat(TrueColorFormat& dtcf) {  
    if (dtcf == tcf) return;
    // remap entire image to new format
    long bytespixel = (tcf.BPP >> 3);
    long nbytespixel = (dtcf.BPP >> 3);
	long npitch = width * nbytespixel;
	long ndatasize = dataSize;
	uint8* ndata;

	// alloc new buffer if we changed bit depths
	if (npitch != pitch) {
	    long bufsize =  npitch * height;
		ndata = (uint8*) std::malloc(bufsize);
    	ndatasize = bufsize;
    	if (ndata == NULL) {
			DEBUG_ONLY( OS::_DOUT("ImageImpl::remapToColorFormat: Failed to allocate [%d] bytes", bufsize); )
            return; // nothing we can do
        }
	} else {
	    ndata = (uint8*)data;   // we are just going to change in place
	}
    
    long y = height;
    while (y-- > 0) {
        addr srcrow = ((addr)data +  (y * pitch));
        addr dstrow = ((addr)ndata + (y * npitch));
        register long w = width;
        while (w-- > 0) {
            // get the source pixel
            uintColor c;
            if (bytespixel == 4) {  // 32 bpp
				
                tcf.UnpackColor(*((uint32*)srcrow), &c.red, &c.green, &c.blue);
            } else {    // must be 16 bpp
                tcf.UnpackColor(*((uint16*)srcrow), &c.red, &c.green, &c.blue);
            }
            srcrow += bytespixel;
            // save the dest pixel
            if (nbytespixel == 4) {
                *((uint32*)dstrow) = dtcf.PackColor(c.red, c.green, c.blue);
            } else {
                *((uint16*)dstrow) = dtcf.PackColor(c.red, c.green, c.blue);
            }
            dstrow += nbytespixel;
        }
    }
    if (ndata != data) {  // if we created a new buffer, save it
		std::free(data);
        data = ndata;
        dataSize = ndatasize;
    }
    // save the new color format
    tcf = dtcf;
    // recalc the transparent pixel value
    transparentPixel = tcf.PackColor(transparentColor.red, transparentColor.green, transparentColor.blue);
}
*/
    
#define	box_support		    (0.5)
#define	filter_support		(1.0)
#define	triangle_support	(1.0)
#define	bell_support		(1.5)
#define	B_spline_support	(2.0)
#define	Mitchell_support	(2.0)
#define	Lanczos3_support	(3.0)

double filter(double t);
double box_filter(double t);
double triangle_filter(double t);
double bell_filter(double t);
double B_spline_filter(double t);
double Lanczos3_filter(double t);
double Mitchell_filter(double t);

void zoom(ImageImpl *dst, ImageImpl *src, double (*filterf)(double), double fwidth);

// create a new image by scaling the current one
Image*
ImageImpl::createImageScaled(float xscale, float yscale, FilterType filterType) {
    if ((xscale == 1.0) && (yscale == 1.0)) {
        addRef();   // we are returning this as a copy
        return this; // exact match, no scaling required
    }
    if (data == 0) {
        DEBUG_ONLY( OS::_DOUT("ImageImpl::createImageScaled, src image data is null"); )
        return 0;
    }
    ImageImpl* newImage = NEW_IMAGE(mPort);

    // in multiframe images, make sure we use a width that divides evenly by number of frames
    int newWidth = (int)((float)width * xscale);
    if (frames > 1) {
        long frameExtra = newWidth % frames;
        if (frameExtra) {
            newWidth += (frames - frameExtra);
        }
    }

    newImage->initEmpty(newWidth, (int)((float)height * yscale), bpp);
    // copy other characteristics of this image to the new copy
    newImage->transparentColor = transparentColor;
    newImage->transparentPixel = transparentPixel;
    newImage->opacity = opacity;
    newImage->frames = frames;
	if(frames)
	{
		newImage->frameWidth = newWidth / frames;
	}
    
    switch (filterType) {
        case filter_Box:
            zoom(newImage, this, box_filter, box_support);
            break;
            
        case filter_Normal:
            zoom(newImage, this, filter, filter_support);
            break;

        case filter_Triangle:
            zoom(newImage, this, triangle_filter, triangle_support);
            break;

        case filter_Bell:
            zoom(newImage, this, bell_filter, bell_support);
            break;

        case filter_BSpline:
            zoom(newImage, this, B_spline_filter, B_spline_support);
            break;

        case filter_Mitchell:
            zoom(newImage, this, Mitchell_filter, Mitchell_support);
            break;

        case filter_Lanczos3:
            zoom(newImage, this, Lanczos3_filter, Lanczos3_support);
            break;
    }
    newImage->addRef();  // make sure new image can be released
    return newImage;
}

Image*  
ImageImpl::createImageScaledToFit(Rect r, FitType fitType, FilterType filterType) {
    if ((width == r.width()) && (height == r.height())) {
        addRef();   // we are returning this as a copy
        return this; // exact match, no scaling required
    }
    r.moveTo(0,0);
    float xscale = (float)r.width()/(float)width;
    float yscale = (float)r.height()/(float)height;
    if (fitType == fit_Height) {
        xscale = yscale;
        if (height == r.height()) {
            addRef();   // no scaling required, we are returning this as a copy
            return this;
        }
    } else if (fitType == fit_Width) {
        yscale = xscale;
        if (width == r.width()) {
            addRef();   // no scaling required, we are returning this as a copy
            return this;
        }
    } else if (fitType == fit_Inside) {
        if (xscale < yscale) {
            yscale = xscale;
            if (width == r.width()) {
                addRef();   // no scaling required, we are returning this as a copy
                return this;
            }
        } else {
            xscale = yscale;
            if (height == r.height()) {
                addRef();   // no scaling required, we are returning this as a copy
                return this;
            }
        }
    } else if (fitType == fit_FillKeepProportions) {
        if (xscale > yscale) {
            yscale = xscale;
            if (width == r.width()) {
                addRef();   // no scaling required, we are returning this as a copy
                return this;
            }
        } else {
            xscale = yscale;
            if (height == r.height()) {
                addRef();   // no scaling required, we are returning this as a copy
                return this;
            }
        }
    }
    return createImageScaled(xscale, yscale, filterType);
}

/*
 *	filter function definitions
 */

double
filter(double t) {
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

double
box_filter(double t) {
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

double
triangle_filter(double t) {
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#ifndef PI
#define PI       3.141592        /* the venerable pi */
#endif
#ifndef M_PI
#define M_PI PI
#endif

double
bell_filter(double t) {
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

double
B_spline_filter(double t) {
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

double sinc(double x);
    
double
sinc(double x) {
	x *= M_PI;
	if(x != 0) return(std::sin(x) / x);
	return(1.0);
}

double
Lanczos3_filter(double t) {
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double
Mitchell_filter(double t) {
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */


typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
	uint32 csize;   /* size of allocated pointer */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

/* clamp the input to the specified range */
#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)
#define xsize getWidth()
#define ysize getHeight()
//typedef	unsigned char	Pixel;

#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
void get_row(addr row, ImageImpl *image, long y, addr dstblock, uint32 dstsize);
void get_row(addr row, ImageImpl *image, long y, addr dstblock, uint32 dstsize) 
#else
void get_row(addr row, ImageImpl *image, long y);
void get_row(addr row, ImageImpl *image, long y) 
#endif // PDG_GFX_POINTER_SAFETY_CHECKS
{
	if((y < 0) || (y >= image->getHeight())) {
		return;
	}
    GFX_CHECK_PTR( (addr)image->data + (y * image->pitch), image->data, image->dataSize);
    GFX_CHECK_PTR( (addr)image->data + ((y+1) * image->pitch) - 1, image->data, image->dataSize);
    GFX_CHECK_PTR( row, dstblock, dstsize);
    GFX_CHECK_PTR( row + image->pitch - 1, dstblock, dstsize);
	std::memcpy(row, (addr)image->data + (y * image->pitch), image->pitch);
}

#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
void get_column(addr column, ImageImpl *image, long x, addr dstblock, uint32 dstsize);
void get_column(addr column, ImageImpl *image, long x, addr dstblock, uint32 dstsize)
#else
void get_column(addr column, ImageImpl *image, long x);
void get_column(addr column, ImageImpl *image, long x)
#endif // PDG_GFX_POINTER_SAFETY_CHECKS
{
	long i, d;
	addr p;

	if((x < 0) || (x >= image->getWidth())) {
		return;
	}
	d = image->pitch;
	if (image->bpp == 32) {
    	for(i = image->getHeight(), p = (addr)image->data + (x<<2); i-- > 0; p += d) { // bc assign ptr out of range ok -- p += d will put p 1 past end of buf on last line, but p is not used then
            GFX_CHECK_PTR( p, image->data, image->dataSize);
            GFX_CHECK_PTR( p+3, image->data, image->dataSize);
            GFX_CHECK_PTR( column, dstblock, dstsize);
            GFX_CHECK_PTR( column+3, dstblock, dstsize);
    		*((uint32*)column) = *((uint32*)p);
			column += 4;
    	}
	} else {    // assume 16 bpp
		for(i = image->getHeight(), p = (addr)image->data + (x<<1); i-- > 0; p += d) { // bc assign ptr out of range ok -- p += d will put p 1 past end of buf on last line, but p is not used then
            GFX_CHECK_PTR( p, image->data, image->dataSize);
            GFX_CHECK_PTR( p+1, image->data, image->dataSize);
            GFX_CHECK_PTR( column, dstblock, dstsize);
            GFX_CHECK_PTR( column+1, dstblock, dstsize);
    		*((uint16*)column) = *((uint16*)p);
			column += 2;
    	}
    }
}

void put_pixel(ImageImpl* image, long x, long y, uintColor c, int bytespixel);
void put_pixel(ImageImpl* image, long x, long y, uintColor c, int bytespixel) {
	static ImageImpl *im = NULL;
	static long yy = -1;
	static addr p = NULL;

	if((x < 0) || (x >= image->getWidth()) || (y < 0) || (y >= image->getHeight())) {
		return;
	}
	if((im != image) || (yy != y)) {  // bc dangling ptr ok, only compared to current image ptr to see if pixel row addr cache valid
		im = image;
		yy = y;
		p = (addr)image->data + (y * image->pitch);
	}
    if (bytespixel == 4) {
        GFX_CHECK_PTR( &p[x], image->data, image->dataSize);
        GFX_CHECK_PTR( ((addr)&p[x])+3, image->data, image->dataSize);
        // RGBA
        ((uint32*)p)[x] = c.red << 24 | c.green << 16 | c.blue << 8;
    } else {    // assume 16 bpp
        GFX_CHECK_PTR( &p[x], image->data, image->dataSize);
        GFX_CHECK_PTR( ((addr)&p[x])+1, image->data, image->dataSize);
        // RGB 16
        ((uint16*)p)[x] = c.red << 10 | c.green << 5 | c.blue;
    }
}

// 0.75 good for Mitchell filter

#define TRANSPARENCY_MIN_WEIGHT 0.57

void
zoom(ImageImpl *dst, ImageImpl *src, double (*filterf)(double), double fwidth)
//Image *dst;				/* destination image structure */
//Image *src;				/* source image structure */
//double (*filterf)();			/* filter function */
//double fwidth;				/* filter width (support) */
{
	ImageImpl *tmp;			/* intermediate image */
	double xscale, yscale;		/* zoom scale factors */
	int i, j, k;			/* loop variables */
	int n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	addr raster;			/* a row or column of pixels */
    int bytespixel = (src->bpp >> 3);  // bytes per pixel
	uintColor src_tc;
	src_tc.red = src->transparentColor.red * 255;
	src_tc.green = src->transparentColor.green * 255;
	src_tc.blue = src->transparentColor.blue * 255;
	uintColor dst_tc;
	dst_tc.red = dst->transparentColor.red * 255;
	dst_tc.green = dst->transparentColor.green * 255;
	dst_tc.blue = dst->transparentColor.blue * 255;
	
  #ifndef PDG_NO_GUI
	/* create intermediate image to hold horizontal zoom */
	Port* port = src->setPort(0);   // trick to get the current port
	src->setPort(port);             // restore port
  #endif // ! PDG_NO_GUI

	tmp = NEW_IMAGE(port);
	tmp->initEmpty(dst->xsize, src->ysize, src->bpp);
	
	xscale = (double) dst->xsize / (double) src->xsize;
	yscale = (double) dst->ysize / (double) src->ysize;

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)std::calloc(dst->xsize, sizeof(CLIST));
	uint32 contribsize = dst->xsize * sizeof(CLIST);
	if(xscale < 1.0) {
		width = fwidth / xscale;
		fscale = 1.0 / xscale;
		for(i = 0; i < dst->xsize; ++i) {
            GFX_CHECK_PTR( &contrib[i], contrib, contribsize);
            GFX_CHECK_PTR( &contrib[i].csize, contrib, contribsize);
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)std::calloc((int) (width * 2 + 1), sizeof(CONTRIB));
			contrib[i].csize = (int)(width * 2 + 1) * sizeof(CONTRIB);
			center = (double) i / xscale;
			left = std::ceil(center - width);
			right = std::floor(center + width);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
                GFX_CHECK_PTR( &contrib[i].p[k].pixel, contrib[i].p, contrib[i].csize);
                GFX_CHECK_PTR( ((addr)&contrib[i].p[k].weight) + sizeof(double) - 1, contrib[i].p, contrib[i].csize);
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->xsize; ++i) {
            GFX_CHECK_PTR( &contrib[i], contrib, contribsize);
            GFX_CHECK_PTR( &contrib[i].csize, contrib, contribsize);
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)std::calloc((int) (fwidth * 2 + 1), sizeof(CONTRIB));
			contrib[i].csize = (int)(fwidth * 2 + 1) * sizeof(CONTRIB);
			center = (double) i / xscale;
			left = std::ceil(center - fwidth);
			right = std::floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
                GFX_CHECK_PTR( &contrib[i].p[k].pixel, contrib[i].p, contrib[i].csize);
                GFX_CHECK_PTR( ((addr)&contrib[i].p[k].weight) + sizeof(double) - 1, contrib[i].p, contrib[i].csize);
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

    uintColor c;
	/* apply filter to zoom horizontally from src to tmp */
	raster = (addr)std::malloc(src->pitch);
    long tmpWidth = tmp->getWidth();
	for(k = 0; k < tmp->ysize; ++k) {
#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
		get_row(raster, src, k, raster, src->pitch);
#else
		get_row(raster, src, k);
#endif
		for(i = 0; i < tmpWidth; ++i) {
		    double redWeight = 0.0;
		    double greenWeight = 0.0;
		    double blueWeight = 0.0;
		    double transparentWeight = 0.0;
    		bool transparentDst = false;
    		for(j = 0; j < contrib[i].n; ++j) {
          		bool transparent = false;
                if (bytespixel == 4) {  // 32 bpp
                    uint32 pix = ((uint32*)raster)[contrib[i].p[j].pixel];
                    if ((src->transparentPixel == pix) && (pix != 0)) {
                        transparent = true;
                    } else {
                        // RGBA
                        c.red = (pix >> 24) & 0xff;
                        c.green = (pix >> 16) & 0xff;
                        c.blue = (pix >> 8) & 0xff;
                    }
                } else {    // must be 16 bpp
                    uint16 pix = ((uint16*)raster)[contrib[i].p[j].pixel];
                    if (((uint16)src->transparentPixel == pix) && (pix != 0)) {
                        transparent = true;
                    } else {
                        // RGB 16
                        c.red = (pix >> 10) & 0x1f;
                        c.green = (pix >> 5) & 0x1f;
                        c.blue = (pix & 0x1f);
                    }
                }
                // handle transparency
                weight = contrib[i].p[j].weight;
                if (transparent) {
                    transparentWeight += weight;
                    transparentDst = true;
                } else {
                    redWeight   += weight * c.red;
                    greenWeight += weight * c.green;
                    blueWeight  += weight * c.blue;
                }
            }
            if (transparentWeight > TRANSPARENCY_MIN_WEIGHT) {
                put_pixel(tmp, i, k, src_tc, bytespixel);
            } else {
                if (transparentDst) {
                    // mix in gray for transparent part of color
                    redWeight   += transparentWeight * 128; //redWeight;
                    greenWeight += transparentWeight * 128; //greenWeight;
                    blueWeight  += transparentWeight * 128; //blueWeight;
                }
                c.red   = (uint8)CLAMP(redWeight,   BLACK_PIXEL, WHITE_PIXEL);
                c.green = (uint8)CLAMP(greenWeight, BLACK_PIXEL, WHITE_PIXEL);
                c.blue  = (uint8)CLAMP(blueWeight,  BLACK_PIXEL, WHITE_PIXEL);
        	    put_pixel(tmp, i, k, c, bytespixel);
        	}
    	}
	}
	std::free(raster);

	/* free the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp->xsize; ++i) {
		std::free(contrib[i].p);
	}
	std::free(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)std::calloc(dst->ysize, sizeof(CLIST));
	contribsize = dst->ysize * sizeof(CLIST);
	if(yscale < 1.0) {
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < dst->ysize; ++i) {
            GFX_CHECK_PTR( &contrib[i], contrib, contribsize);
            GFX_CHECK_PTR( &contrib[i].csize, contrib, contribsize);
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)std::calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			contrib[i].csize = (int)(width * 2 + 1) * sizeof(CONTRIB);
			center = (double) i / yscale;
			left = std::ceil(center - width);
			right = std::floor(center + width);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
                GFX_CHECK_PTR( &contrib[i].p[k].pixel, contrib[i].p, contrib[i].csize);
                GFX_CHECK_PTR( ((addr)&contrib[i].p[k].weight) + sizeof(double) - 1, contrib[i].p, contrib[i].csize);
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
            GFX_CHECK_PTR( &contrib[i], contrib, contribsize);
            GFX_CHECK_PTR( &contrib[i].csize, contrib, contribsize);
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)std::calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			contrib[i].csize = (int)(fwidth * 2 + 1) * sizeof(CONTRIB);
			center = (double) i / yscale;
			left = std::ceil(center - fwidth);
			right = std::floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
                GFX_CHECK_PTR( &contrib[i].p[k].pixel, contrib[i].p, contrib[i].csize);
                GFX_CHECK_PTR( ((addr)&contrib[i].p[k].weight) + sizeof(double) - 1, contrib[i].p, contrib[i].csize);
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	uint32 bufsize = tmp->ysize * (tmp->bpp >> 3);
	raster = (addr) std::malloc(bufsize);
    long dstHeight = dst->getHeight();
	for(k = 0; k < dst->xsize; ++k) {
#ifdef PDG_GFX_POINTER_SAFETY_CHECKS
		get_column(raster, tmp, k, raster, bufsize);
#else
		get_column(raster, tmp, k);
#endif
		for(i = 0; i < dstHeight; ++i) {
		    double redWeight = 0.0;
		    double greenWeight = 0.0;
		    double blueWeight = 0.0;
		    double transparentWeight = 0.0;
    		bool transparentDst = false;
    		for(j = 0; j < contrib[i].n; ++j) {
        		bool transparent = false;
                if (bytespixel == 4) {  // 32 bpp
                    uint32 pix = ((uint32*)raster)[contrib[i].p[j].pixel];
                    if ((src->transparentPixel == pix) && (pix != 0)) {
                        transparent = true;
                    } else {
                        // RGBA
                        c.red = (pix >> 24) & 0xff;
                        c.green = (pix >> 16) & 0xff;
                        c.blue = (pix >> 8) & 0xff;
                    }
                } else {    // must be 16 bpp
                    uint16 pix = ((uint16*)raster)[contrib[i].p[j].pixel];
                    if (((uint16)src->transparentPixel == pix) && (pix != 0)) {
                        transparent = true;
                    } else {
                        // RGB 16
                        c.red = (pix >> 10) & 0x1f;
                        c.green = (pix >> 5) & 0x1f;
                        c.blue = (pix & 0x1f);
                    }
                }
                // handle transparency
                weight = contrib[i].p[j].weight;
                if (transparent) {
                    transparentWeight += weight;
                    transparentDst = true;
                } else {
                    redWeight   += c.red   * weight;
                    greenWeight += c.green * weight;
                    blueWeight  += c.blue  * weight;
                }
            }
            if (transparentWeight > TRANSPARENCY_MIN_WEIGHT) {
                put_pixel(dst, k, i, dst_tc, bytespixel);
            } else {
                if (transparentDst) {
                    redWeight   += (redWeight   * transparentWeight);
                    greenWeight += (greenWeight * transparentWeight);
                    blueWeight  += (blueWeight  * transparentWeight);
                }
                c.red   = (uint8)CLAMP(redWeight,   BLACK_PIXEL, WHITE_PIXEL);
                c.green = (uint8)CLAMP(greenWeight, BLACK_PIXEL, WHITE_PIXEL);
                c.blue  = (uint8)CLAMP(blueWeight,  BLACK_PIXEL, WHITE_PIXEL);
        	    put_pixel(dst, k, i, c, bytespixel);
            }
    	}
	}
	std::free(raster);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i) {
		std::free(contrib[i].p);
	}
	std::free(contrib);

	delete tmp;
}
} // end namespace pdg

