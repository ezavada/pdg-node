// -----------------------------------------------
// graphics-macosx.mm
// 
// Mac OS X implementation of graphics functionality
//
// Written by Ed Zavada, 2009-2012
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

#import <Foundation/NSData.h>
#import <Foundation/NSException.h>
#import <AppKit/NSBitmapImageRep.h>


#ifndef PDG_NO_GUI
  #import <OpenGL/gl.h>  // for real GL_RGB & GL_RGBA definitions
#else
  #include "image-impl.h"  // for fake GL_RGB & GL_RGBA definitions
#endif
#import "internals-macosx.h"

int pow2(int n) {
	int x = 1;
	while(x < n) {
		x <<= 1;
	}
	return x;
}

namespace pdg {


void platform_initImageData(unsigned char* imageData, long imageDataLen, unsigned char** outDataPtr, 
	long* outWidth, long* outHeight, long* outBufferWidth, long* outBufferHeight, long* outBufferPitch, 
	int* outFormat) {

	NSBitmapImageRep *bitmap = 0;
	*outWidth = 0;
	*outHeight = 0;
	*outBufferWidth = 0;
	*outBufferHeight = 0;
	*outBufferPitch = 0; 
	*outDataPtr = 0;
	*outFormat = GL_RGB;
	
	@try {
		
		NSData *data = [NSData dataWithBytesNoCopy:imageData length:imageDataLen];

		bitmap = [NSBitmapImageRep imageRepWithData:data];

		int width   = [bitmap pixelsWide];
		int height  = [bitmap pixelsHigh];
		int pitch   = [bitmap bytesPerRow];
		int bpp     = [bitmap bitsPerPixel];
		
		unsigned long glBufferWidth = pow2(width);
		unsigned long glBufferHeight = pow2(height);
		unsigned long glBufferPitch = ((glBufferWidth * bpp/2) + 3) / 4;
		
		*outDataPtr = (unsigned char*) malloc(glBufferPitch * glBufferHeight);
		// copy the image bitmap data line by line into the new malloc'd buffer
		unsigned char* dst = *outDataPtr;
		unsigned char* src = (unsigned char*) [bitmap bitmapData];
		for (int i = 0; i<height; i++) {
			memcpy(dst, src, pitch);
			memset(dst + pitch, 0, glBufferPitch - pitch); // clear the remaining part of the buffer
			dst += glBufferPitch;
			src += pitch;
		}

		*outWidth = width;
		*outHeight = height;
		*outBufferWidth = glBufferWidth;
		*outBufferHeight = glBufferHeight;
		*outBufferPitch = glBufferPitch; 
		
		if (bpp == 32)  {
			*outFormat = GL_RGBA;
		}
	}
	@catch (NSException *exception) {
		if (outDataPtr) {
			free(*outDataPtr);
			outDataPtr = 0;
		}
	}
	if (bitmap) {
// TODO: figure out why we would retain this if we copied over the data already
		[bitmap retain];
//		[bitmap release];
//		bitmap = 0;
	}
}

} // end namespace pdg


