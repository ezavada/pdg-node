// -----------------------------------------------
// font.h
//
// font functionality
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


#ifndef PDG_FONT_H_INCLUDED
#define PDG_FONT_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/refcounted.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

// -----------------------------------------------------------------------------------
//! Graphics
//! A collection of static enumerations that are used when drawing
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

namespace Graphics {

	//! text drawing and measurement
	//! \ingroup Graphics

	enum Style {
		//! Basic Styles can be combined, ie: textStyle_Bold + textStyle_Italic + textStyle_Underline
		textStyle_Plain = 0,
		textStyle_Bold = 1,
		textStyle_Italic = 2,
		textStyle_Underline = 4,

		//! next three are mutually exclusive, but can be combined with basic styles
		textStyle_Centered = 16,
		textStyle_LeftJustified = 0,
		textStyle_RightJustified = 32

	};

};


// -----------------------------------------------------------------------------------
//! Font
//! You cannot create fonts directly, they must be loaded
//! by the Graphics Manager
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

class Font : public RefCountedObj {
public:
	virtual const char* getFontName() const = 0;

    //! total height of the font, including leading between lines
	//! adding this to the baseline gives the next baseline position
    virtual float    getFontHeight(int size, uint32 style = Graphics::textStyle_Plain) = 0;
	//! the amount of space between two lines on the font, between the bottom of the decender of the
	//! line above and the top of the ascender of the line below
    virtual float    getFontLeading(int size, uint32 style = Graphics::textStyle_Plain) = 0;
    //! the height from the baseline to the top of the tallest character in the font
    virtual float    getFontAscent(int size, uint32 style = Graphics::textStyle_Plain) = 0;
	//! the height from the baseline to the bottom of the lowest descending character in the font
    virtual float    getFontDescent(int size, uint32 style = Graphics::textStyle_Plain) = 0;

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mFontScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
	//! @cond INTERNAL
    Font() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mFontScriptObj);
    	#endif
    };
    virtual ~Font() {
//                DEBUG_ONLY( OS::_DOUT("dt Font %p", this); )
			#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
				CleanupFontScriptObject(mFontScriptObj);
			#endif
	};
	//! @endcond
};


} // end namespace pdg

#endif // PDG_FONT_H_INCLUDED
