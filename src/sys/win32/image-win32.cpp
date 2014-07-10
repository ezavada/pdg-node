// -----------------------------------------------
// image-win32.cpp
//
// Windows implementation of graphics functionality
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

#include <windows.h>
#include <GL/gl.h>
#include "png/png.h"
#include "internals.h"

extern "C" {
void pdg_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
void pdg_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void pdg_png_flush_data(png_structp png_ptr);
}

#define PNG_SIG_BYTES 8

typedef struct pdg_png_data_t {
	unsigned char* imageData;
	png_size_t imageDataLen;
	png_size_t currOffset;
} pdg_png_data;

namespace pdg {

void platform_initImageData(unsigned char* imageData, long imageDataLen, 
		unsigned char** outDataPtr, long* outWidth, long* outHeight, 
		long* outBufferWidth, long* outBufferHeight, long* outBufferPitch, 
		int* outFormat) {
	*outDataPtr = 0;
	*outWidth = 0;
	*outHeight = 0;
	*outBufferWidth = 0;
	*outBufferHeight = 0;
	*outBufferPitch = 0;
	*outFormat = GL_RGBA;

	bool is_png = !png_sig_cmp(imageData, 0, PNG_SIG_BYTES);
	if (!is_png) return;

	// create lib png structures
	png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return;
	}
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return;
	}

	png_bytep* row_pointers = 0;

    // define a block to handle cleanup after an error
	if (setjmp(png_jmpbuf(png_ptr))) {
	    if (row_pointers) {
	        delete [] row_pointers;
	    }
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return;
	}

    // setup an io pointer for reading from memory
	pdg_png_data_t read_data;
	read_data.imageData = imageData;
	read_data.imageDataLen = imageDataLen;
	read_data.currOffset = PNG_SIG_BYTES;

	png_set_read_fn(png_ptr, &read_data, pdg_png_read_data);

	// we have skipped the sig bytes
	png_set_sig_bytes(png_ptr, PNG_SIG_BYTES);

	// now read the PNG header
	png_read_info(png_ptr, info_ptr);
	png_uint_32 width, height;
	int bit_depth, color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	// do appropriate transformations based on the info in the header
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}

	*outWidth = width;
	*outHeight = height;
	*outBufferWidth = width;
	*outBufferHeight = height;
	png_uint_32 pitch;
	if (color_type & PNG_COLOR_MASK_ALPHA) {
		*outFormat = GL_RGBA;
		pitch = width * 4;
	} else {
		*outFormat = GL_RGB;
		pitch = width * 3;
	}
	*outBufferPitch = pitch;
	void* dataP = malloc(pitch * height);
	*outDataPtr = (unsigned char*) dataP;
	if (dataP) {
		// define the row pointers
		png_bytep* row_pointers = new png_bytep[height];
		for (png_uint_32 i = 0; i < height; i++) {
			row_pointers[i] = &((png_bytep)dataP)[pitch * i];
		}
		// now read in the image
		png_read_image(png_ptr, row_pointers);
		delete [] row_pointers;
	}
	// read comments, etc...
	png_read_end(png_ptr, end_info);
	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}

} // end namespace pdg

void pdg_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	pdg_png_data* read_io_ptr = (pdg_png_data*) png_get_io_ptr(png_ptr);
	if (read_io_ptr->currOffset >= read_io_ptr->imageDataLen) {
		// we are at end of file
        png_error(png_ptr, "Read Error");
        return;
	}
	if (read_io_ptr->currOffset + length > read_io_ptr->imageDataLen) {
		// don't read past end of file
		length = read_io_ptr->imageDataLen - read_io_ptr->currOffset;
	}
	memcpy(data, &read_io_ptr->imageData[read_io_ptr->currOffset], length);
	read_io_ptr->currOffset += length;
}



#endif // PDG_NO_GUI


