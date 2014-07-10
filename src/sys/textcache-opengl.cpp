// -----------------------------------------------
// textcache-opengl.cpp
//
// Caching for OpenGL text drawing
//
// Written by Ed Zavada, 2010-2012
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

#include "pdg/msvcfix.h"

#include "textcache-opengl.h"
#include "font-impl.h"

#include <algorithm>

#define MAX_CACHED_STRING_TEXTURES 250

namespace pdg {


static FontCacheEntry* sFirstFontCacheEntry = 0;

static TextCacheEntry* sFirstTextCacheEntry = 0;
static uint32 sTextCacheWhen = 0;
static int sTextCacheSize = 0;

TextCacheEntry::TextCacheEntry( const char* inText, int inLen, FontImpl* font, int inSize, uint32 inStyle)
: nextEntry(0), usedWhen(0), mFont(font), mText(inText), len(inLen), size(inSize), style(inStyle), textChecksum(0),
texture(0), charHeight(0), ascent(0), width(0), tx(0.0), ty(0.0), tx_topoffset(0.0)
{
	TextCacheEntry::addEntryToCache(this);
	// pre-fill these values
	ascent = std::ceil(font->getFontAscent(size, style));
	charHeight = ascent + std::ceil(font->getFontDescent(size, style));
	textChecksum = TextCacheEntry::calcChecksum(inText, inLen);
}

TextCacheEntry*
TextCacheEntry::findTextInCache(const char* text, int len, FontImpl* font, int size, uint32 style) {
	// try to find in cache
	style &= TEXT_STYLES_MASK;
	TextCacheEntry* entry = sFirstTextCacheEntry;
	int textChecksum = TextCacheEntry::calcChecksum(text, len);
	while (entry) {
		if (   (entry->textChecksum == textChecksum)
			&& (entry->len == len)
			&& (entry->size == size)
			&& (entry->style == style)
			&& (entry->mFont == font) ) {
			if (std::strcmp(text, entry->mText.c_str()) == 0) {
					// same text string
				entry->usedWhen = sTextCacheWhen++;
				return entry;
			}
		}
		entry = entry->nextEntry;
	}
	return new TextCacheEntry(text, len, font, size, style);
}

void
TextCacheEntry::addEntryToCache(TextCacheEntry* textInfo) {
	if (sTextCacheSize >= MAX_CACHED_STRING_TEXTURES) {
		// cache full, delete the least recently used cache entry
		uint32 smallestWhen = sTextCacheWhen;
		TextCacheEntry* entry = sFirstTextCacheEntry;
		TextCacheEntry* prev = 0;
		TextCacheEntry* smallestPrev = 0;
		while (entry) {
			if (entry->usedWhen < smallestWhen) {
				smallestPrev = prev;
				smallestWhen = entry->usedWhen;
			}
			prev = entry;
			entry = entry->nextEntry;
		}
		if (smallestPrev) {
			entry = smallestPrev->nextEntry;
			smallestPrev->nextEntry = entry->nextEntry;
		} else {
			entry = sFirstTextCacheEntry;
			sFirstTextCacheEntry = entry->nextEntry;
		}
		glDeleteTextures(1, &entry->texture); // clean up the OpenGL texture now that we aren't caching it anymore
		delete entry;
		sTextCacheSize--;
	}
	// now add the new entry
	textInfo->nextEntry = sFirstTextCacheEntry;
	textInfo->usedWhen = sTextCacheWhen++;
	textInfo->style &= TEXT_STYLES_MASK;
	sFirstTextCacheEntry = textInfo;
	sTextCacheSize++;
}

int
TextCacheEntry::calcChecksum(const char* text, int len) {
	int sum = 0;
	const char* p = text;
	while (len) {
		len--;
		sum += *p;
	}
	return sum;
}

FontCacheEntry::FontCacheEntry( const char* inFontName, float inScalingFactor )
	: nextEntry(0), mFontName(inFontName), scalingFactor(inScalingFactor), mFont(0)
{
	addEntryToCache(this);
}

FontCacheEntry*
FontCacheEntry::findFontInCache(const char* inFontName, float inScalingFactor ) {
	// try to find in cache
	FontCacheEntry* entry = sFirstFontCacheEntry;
	while (entry) {
		if (   (entry->scalingFactor == inScalingFactor)
			&& (std::strcmp(inFontName, entry->mFontName.c_str()) == 0) ) {
			// same font
			return entry;
		}
		entry = entry->nextEntry;
	}
	return new FontCacheEntry(inFontName, inScalingFactor);
}

void
FontCacheEntry::addEntryToCache(FontCacheEntry* fontInfo) {
	// now add the new entry
	fontInfo->nextEntry = sFirstFontCacheEntry;
	if (fontInfo->mFont) {
		fontInfo->mFontName = fontInfo->mFont->getFontName();
	}
	sFirstFontCacheEntry = fontInfo;
}


} // end namespace pdg

#endif // PDG_NO_GUI
