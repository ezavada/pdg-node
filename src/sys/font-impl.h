// -----------------------------------------------
// font-impl.h
// 
// implementation class for fonts
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


#ifndef PDG_FONT_IMPL_H_INCLUDED
#define PDG_FONT_IMPL_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"
#include "pdg/sys/graphics.h"

#include <string>

namespace pdg {
	
	// ==================================================================
	// FONT
	// implementation of publicly declared methods (ie, exposed outside
	// system framework by framework/inc/graphics.h)
	// these implementations are not OS dependent
	// ==================================================================

#define NUM_TEXT_STYLES				(Graphics::textStyle_Bold + Graphics::textStyle_Italic + Graphics::textStyle_Underline + 1)
#define TEXT_STYLES_MASK			7L
#define TEXT_INFO_CACHE_SIZE		24
	
	struct FontMetricsInfo {
		uint32		usedWhen;
		int			size;
		uint32		style;
		float		height;
		float		ascent;
		float		descent;
		float		leading;
	};
	
	class FontImpl : public Font {
	public:
		virtual const char* getFontName() const;
		
		virtual float    getFontHeight(int size, uint32 style = Graphics::textStyle_Plain);
		virtual float    getFontLeading(int size, uint32 style = Graphics::textStyle_Plain);
		virtual float    getFontAscent(int size, uint32 style = Graphics::textStyle_Plain);
		virtual float    getFontDescent(int size, uint32 style = Graphics::textStyle_Plain);
		
		const FontMetricsInfo* fetchFontMetricsWithCaching(int size, uint32 style);

		virtual FontMetricsInfo* getFontMetrics(int size, uint32 style) = 0;  // this must be implemented by subclasses that know how to get font metrics
		
		FontImpl(Port* port, const char* fontName, float scalingFactor);
		virtual ~FontImpl();
		
		Port* mPort;
		std::string mFontName;
		float mScalingFactor;
		FontMetricsInfo* mFontMetricsInfo[TEXT_INFO_CACHE_SIZE];
		int mCacheIndex;
		uint32 mCacheWhen;
	};
	
} // end namespace pdg

#endif // PDG_FONT_IMPL_H_INCLUDED
