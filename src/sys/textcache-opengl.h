// -----------------------------------------------
// textcache-opengl.h
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


#ifndef PDG_TEXTCACHE_OPENGL_H_INCLUDED
#define PDG_TEXTCACHE_OPENGL_H_INCLUDED

#include "pdg_project.h"

#ifndef PDG_NO_GUI

#include <string>

#include "include-opengl.h"
#include "font-impl.h"

namespace pdg {

	
struct FontCacheEntry {

	// cache management
	FontCacheEntry* nextEntry;
	
	// here's what we use to find the item in the cache
	std::string	 mFontName;
	float scalingFactor;
	
	// and here's the information we cache
	FontImpl* mFont;
	
	// constructor, find in cache
	
	FontCacheEntry( const char* inFontName, float inScalingFactor );

	static FontCacheEntry*  findFontInCache(const char* inFontName, float inScalingFactor );
	
private:
	FontCacheEntry() {}
	FontCacheEntry(FontCacheEntry&) {}
	static void             addEntryToCache(FontCacheEntry* textInfo);
};

struct TextCacheEntry {
	
	// cache management
	TextCacheEntry* nextEntry;
	uint32		 usedWhen;
	
	// here's what we use to find the item in the cache
	FontImpl*    mFont;
	std::string  mText;
	int			 len;
	int			 size;
	uint32		 style;
	int			 textChecksum;
	
	// and here's the information we cache
	GLuint		 texture;
	int			 charHeight;
	int			 ascent;
	int			 width;
	float		 tx;
	float		 ty;
	float		 tx_topoffset;
	
	TextCacheEntry( const char* inText, int inLen, FontImpl* font, int inSize, uint32 inStyle);
	
	static TextCacheEntry*   findTextInCache(const char* text, int len, FontImpl* font, int size, uint32 style);
	
private:
	TextCacheEntry() {}
	TextCacheEntry(TextCacheEntry&) {}
	static void              addEntryToCache(TextCacheEntry* textInfo);
	static int				 calcChecksum(const char* inText, int inLen);
};


} // end namespace pdg

#endif // PDG_NO_GUI
#endif // PDG_TEXTCACHE_OPENGL_H_INCLUDED
