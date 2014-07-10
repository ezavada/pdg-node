// -----------------------------------------------
// color.cpp
// 
// color functionality
//
// Written by Ed Zavada, 2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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

#include "pdg/sys/color.h"
#include "pdg/sys/os.h"

#include <sstream>

namespace pdg {

Color Color::makeColor(const char* cssColorValue) {
	if (cssColorValue[0] == '#') {
		// convert string with #hhhhhh or #hhh (css style) value
		int red = 0, green = 0, blue = 0;
		if (cssColorValue[4] == 0) {  // 3 char str
			sscanf(cssColorValue, "#%1x%1x%1x", &red, &green, &blue);
			red = (red << 8) & red;
			green = (green << 8) & green;
			blue = (blue << 8) & blue;
		} else if (cssColorValue[7] == 0) { // 7 char str
			sscanf(cssColorValue, "#%2x%2x%2x", &red, &green, &blue);
		}
		DEBUG_ONLY( else {
			DEBUG_PRINT("Color from String must be: '#rgb', '#rrggbb', or css color name" );
		} )
		return Color(red, green, blue);
	} else {
		// convert string with standard css color names 
		static const int NUM_COLORS = 17;
		static const char* colorNames[NUM_COLORS] = {"aqua", "black", "blue", "fuchsia", 
			"gray", "grey", "green", "lime", "maroon", "navy", "olive", "purple", 
			"red", "silver", "teal", "white", "yellow"};
		static uint32 colorValues[NUM_COLORS] = { 0x00ffff, 0x000000, 0x0000ff, 0xff00ff, 
			0x808080, 0x808080, 0x008000, 0x00ff00, 0x800000, 0x000080, 0x808000, 0x800080,
			0xff0000, 0xc0c0c0, 0x008080, 0xffffff, 0xffff00 };
		for (int i = 0; i<NUM_COLORS; i++) {
			if (strcmp(cssColorValue, colorNames[i]) == 0) {
				uint32 c = colorValues[i] | 0xff000000;
				return Color(c);
			}
		}
		DEBUG_ONLY(
			std::ostringstream msg;
			msg << "Invalid color name \"" << cssColorValue << "\". Valid names are:";
			for (int i = 0; i<NUM_COLORS; i++) {
				msg << " " << colorNames[i];
			}
			DEBUG_PRINT(msg.str().c_str());
		)
        return Color();
	}

}

} //end namespace pdg
