// -----------------------------------------------
// color.h
//
// RGB color handling with alpha value
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


#ifndef PDG_COLOR_H_INCLUDED
#define PDG_COLOR_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"

namespace pdg {

//! \defgroup Graphics
//! Collection of classes, types and constants that are used for drawing

// -----------------------------------------------------------------------------------
// Color definitions for common color values
// -----------------------------------------------------------------------------------

#define PDG_WHITE_COLOR     ::pdg::Color(1.0f, 1.0f, 1.0f)
#define PDG_BLACK_COLOR     ::pdg::Color(0.0f, 0.0f, 0.0f)

// primary colors
#define PDG_RED_COLOR       ::pdg::Color(1.0f, 0.0f, 0.0f)
#define PDG_GREEN_COLOR     ::pdg::Color(0.0f, 1.0f, 0.0f)
#define PDG_BLUE_COLOR      ::pdg::Color(0.0f, 0.0f, 1.0f)

// secondary colors
#define PDG_YELLOW_COLOR    ::pdg::Color(1.0f, 1.0f, 0.0f)
#define PDG_CYAN_COLOR      ::pdg::Color(0.0f, 1.0f, 1.0f)
#define PDG_MAGENTA_COLOR   ::pdg::Color(1.0f, 0.0f, 1.0f)

// shades of gray, larger numbers have more gray and are thus darker
#define PDG_GRAY_90_COLOR   ::pdg::Color(0.1f, 0.1f, 0.1f)
#define PDG_GRAY_80_COLOR   ::pdg::Color(0.2f, 0.2f, 0.2f)
#define PDG_GRAY_75_COLOR   ::pdg::Color(0.25f, 0.25f, 0.25f)
#define PDG_GRAY_70_COLOR   ::pdg::Color(0.3f, 0.3f, 0.3f)
#define PDG_GRAY_60_COLOR   ::pdg::Color(0.4f, 0.4f, 0.4f)
#define PDG_GRAY_50_COLOR   ::pdg::Color(0.5f, 0.5f, 0.5f)
#define PDG_GRAY_40_COLOR   ::pdg::Color(0.6f, 0.6f, 0.6f)
#define PDG_GRAY_30_COLOR   ::pdg::Color(0.7f, 0.7f, 0.7f)
#define PDG_GRAY_25_COLOR   ::pdg::Color(0.75f, 0.75f, 0.75f)
#define PDG_GRAY_20_COLOR   ::pdg::Color(0.8f, 0.8f, 0.8f)
#define PDG_GRAY_10_COLOR   ::pdg::Color(0.9f, 0.9f, 0.9f)

// 17 standard CSS Colors
#define PDG_CSS_AQUA_COLOR    	::pdg::Color(0.0f, 1.0f, 1.0f) 
#define PDG_CSS_BLACK_COLOR    	::pdg::Color(0.0f, 0.0f, 0.0f) 
#define PDG_CSS_BLUE_COLOR    	::pdg::Color(0.0f, 0.0f, 1.0f) 
#define PDG_CSS_FUCHSIA_COLOR   ::pdg::Color(1.0f, 0.0f, 1.0f) 
#define PDG_CSS_GRAY_COLOR    	::pdg::Color(0.5f, 0.5f, 0.5f) 
#define PDG_CSS_GREY_COLOR    	::pdg::Color(0.5f, 0.5f, 0.5f) 
#define PDG_CSS_GREEN_COLOR    	::pdg::Color(0.0f, 0.5f, 0.0f) 
#define PDG_CSS_LIME_COLOR    	::pdg::Color(0.0f, 1.0f, 0.0f) 
#define PDG_CSS_MAROON_COLOR    ::pdg::Color(0.5f, 0.0f, 0.0f) 
#define PDG_CSS_NAVY_COLOR    	::pdg::Color(0.0f, 0.0f, 0.5f) 
#define PDG_CSS_OLIVE_COLOR    	::pdg::Color(0.5f, 0.5f, 0.0f) 
#define PDG_CSS_PURPLE_COLOR    ::pdg::Color(0.5f, 0.0f, 0.5f) 
#define PDG_CSS_RED_COLOR    	::pdg::Color(1.0f, 0.0f, 0.0f) 
#define PDG_CSS_SILVER_COLOR    ::pdg::Color(0.75f, 0.75f, 0.75f) 
#define PDG_CSS_TEAL_COLOR    	::pdg::Color(0.0f, 0.5f, 0.5f) 
#define PDG_CSS_WHITE_COLOR    	::pdg::Color(1.0f, 1.0f, 1.0f) 
#define PDG_CSS_YELLOW_COLOR    ::pdg::Color(1.0f, 1.0f, 0.0f) 

//synonyms
#define PDG_GRAY_COLOR      PDG_GRAY_50_COLOR

// -----------------------------------------------------------------------------------
// Color
// A definitition for a color in RGB space, with alpha channel
// -----------------------------------------------------------------------------------
//! A color in RGB color space, with optional alpha
//! \ingroup Graphics
struct Color {
	//! amount of red in the color, from 0.0 (none) to 1.0 (max red)
    float    red;
	//! amount of green in the color, from 0.0 (none) to 1.0 (max green)
    float    green;
	//! amount of blue in the color, from 0.0 (none) to 1.0 (max blue)
    float    blue;
	//! the alpha value for the color
	float	 alpha;
	//! create a new black color object
    Color() : red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f) {}
	//! create a new color with the RGB values specified
    Color( int r, int g, int b ): red((float)r/255.0f), green((float)g/255.0f), blue((float)b/255.0f), alpha(1.0f) {}
	Color( float r, float g, float b ): red(r), green(g), blue(b), alpha(1.0f) {}
	//! create a new color with the RGBA values specified
    Color( int r, int g, int b, int a ): red((float)r/255.0f), green((float)g/255.0f), blue((float)b/255.0f), alpha((float)a/255.0f) {}
	Color( float r, float g, float b, float a ): red(r), green(g), blue(b), alpha(a) {}
	//! create a new color with the ARGB values specified in a single unsigned 32 bit value
    explicit Color( uint32 c ):  red((float)((c>>16)&0xff)/255.0f), 
						green((float)((c>>8)&0xff)/255.0f), 
						blue((float)(c&0xff)/255.0f),
						alpha((float)((c>>24)&0xff)/255.0f)  {}
	//! set this color equal to another
    Color&  operator=(const Color& c) { red = c.red; green = c.green; blue = c.blue; alpha = c.alpha; return *this; }
	//! set to ARGB color value
    Color&  operator=(uint32 c) {   alpha = (float)((c>>24)&0xff)/255.0f; 
    								red = (float)((c>>16)&0xff)/255.0f; 
    								green = (float)((c>>8)&0xff)/255.0f; 
    								blue = (float)(c&0xff)/255.0f; 
    								return *this; 
    							}
	//! check to see if this color is the same as another, IGNORES ALPHA
    bool    operator==(const Color& c) { return ((uint8)(255.0f * red) == (uint8)(255.0f * c.red) && (uint8)(255.0f * green) == (uint8)(255.0f * c.green) && (uint8)(255.0f * blue) == (uint8)(255.0f * c.blue)); }
    bool    operator==(uint32 c) { return ((c&0xffffff) == (((((uint32)(255.0f * red))&0xff)<<16) | ((((uint32)(255.0f * green))&0xff)<<8) | (((uint32)(255.0f * blue))&0xff)) ); }

	//! convert a color to a matching shade of grey
    void    convertToGrayscale() { float v = (red + green + blue)/3; red = v; green = v; blue = v; }
    //! create a color from floating point RGB, values are 0.0 to 1.0
    static Color makeColor(float rf, float gf, float bf) { return Color(rf, gf, bf); }
    //! create a color from floating point RGBA, values are 0.0 to 1.0
    static Color makeColor(float rf, float gf, float bf, float af) { return Color(rf, gf, bf, af); }
	//! create a color from a CSS color string value, eg: "#111", "#237D1A", "black", etc...
	static Color makeColor(const char* cssColorValue);
};


} // end namespace pdg

#endif // PDG_COLOR_H_INCLUDED
