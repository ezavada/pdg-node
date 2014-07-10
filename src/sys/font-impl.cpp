// -----------------------------------------------
// font-impl.cpp
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


#include "pdg_project.h"

#include "font-impl.h"

#include <cstdlib>

namespace pdg {

	// ==================================================================
	// FONT
	// implementation of publicly declared methods (ie, exposed outside
	// system framework by framework/inc/graphics.h)
	// these implementations are not OS dependent
	// ==================================================================

	const char* FontImpl::getFontName() const {
		return mFontName.c_str();
	}

	float    FontImpl::getFontHeight(int size, uint32 style) {
		const FontMetricsInfo* fmi = fetchFontMetricsWithCaching(size, style);
		if (!fmi) return 0.0f;
		return fmi->height;
	}

	float    FontImpl::getFontLeading(int size, uint32 style) {
		const FontMetricsInfo* fmi = fetchFontMetricsWithCaching(size, style);
		if (!fmi) return 0.0f;
		return fmi->leading;
	}

	float    FontImpl::getFontAscent(int size, uint32 style) {
		const FontMetricsInfo* fmi = fetchFontMetricsWithCaching(size, style);
		if (!fmi) return 0.0f;
		return fmi->ascent;
	}

	float    FontImpl::getFontDescent(int size, uint32 style) {
		const FontMetricsInfo* fmi = fetchFontMetricsWithCaching(size, style);
		if (!fmi) return 0.0f;
		return fmi->descent;
	}

	const FontMetricsInfo* FontImpl::fetchFontMetricsWithCaching(int size, uint32 style) {
		// try to find in cache
		style &= TEXT_STYLES_MASK;
		for (int i = 0; i < mCacheIndex; i++) {
			if (mFontMetricsInfo[i] && (mFontMetricsInfo[i]->size == size) && (mFontMetricsInfo[i]->style == style)) {
				mFontMetricsInfo[i]->usedWhen = mCacheWhen++;
				return mFontMetricsInfo[i];
			}
		}
		// not found, must ask font system to generate them for us
		FontMetricsInfo* fmi = getFontMetrics(size, style);
		fmi->usedWhen = mCacheWhen++;
		// and then add them to the cache
		if (mCacheIndex < TEXT_INFO_CACHE_SIZE) {
			// cache is not full, just add to end
			mFontMetricsInfo[mCacheIndex] = fmi;
			mCacheIndex++;
		} else {
			// cache is full, find least recently used
			uint32 when = mCacheWhen;
			int which = 0;
			for (int i = 0; i < TEXT_INFO_CACHE_SIZE; i++) {
				if (mFontMetricsInfo[i] && (mFontMetricsInfo[i]->usedWhen < when)) {
					when = mFontMetricsInfo[i]->usedWhen;
					which = i;
				}
			}
			mFontMetricsInfo[which] = fmi;
		}
		return fmi;
	}

	FontImpl::FontImpl(Port* port, const char* fontName, float scalingFactor) 
	: mPort(port), mFontName(fontName),
	  mScalingFactor(scalingFactor),
	  mCacheIndex(0),
	  mCacheWhen(0)
	{
		for (int i = 0; i < TEXT_INFO_CACHE_SIZE; i++) {
			mFontMetricsInfo[i] = 0;
		}
	}

	FontImpl::~FontImpl() {
		for (int i = 0; i < TEXT_INFO_CACHE_SIZE; i++) {
			if (mFontMetricsInfo[i]) {
				std::free(mFontMetricsInfo[i]);
				mFontMetricsInfo[i] = 0;
			}
		}
	}

} // end namespace pdg

