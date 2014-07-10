// -----------------------------------------------
// graphics-macosx-opengl.cpp
// 
// Mac OS X implementation of common graphics functions
// on OpenGL based framework
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


#ifndef PDG_NO_GUI

#include "pdg_project.h"

#include <algorithm>
#include <string>
#include <cmath>

#include "graphics-macosx.h"
#include "internals.h"

#include "internals-macosx.h"
#include "include-opengl.h"
#include "textcache-opengl.h"

#ifdef PLATFORM_IOS
#include <CoreGraphics/CoreGraphics.h>
#endif

#define PDG_ITALICS_FACTOR 3.333f


extern "C" CGContextRef graphics_getCurrentCGContextRef();
extern "C" int pow2(int n);

// missing from CG headers
extern "C" bool CGFontGetGlyphsForUnichars(CGFontRef, pdg::utf16char[], CGGlyph[], size_t);

namespace pdg {

void graphics_CG_drawText(CGContextRef context, FontImplMac* font, int size, uint32 style, const char* text, int len);
    
void graphics_CG_drawText(CGContextRef context, FontImplMac* font, int size, uint32 style, const char* text, int len) {
	
	MacAPI::PrivateOSFontRef fontRef = font->getMacFont(size, style);
	
	MacAPI::CGContextSetFont(context, fontRef);
	MacAPI::CGContextSetFontSize(context, size * font->mScalingFactor );

	pdg::utf16string s16;
	OS::utf8to16(s16, std::string(text), len);
	
	// Create an array of Glyph's the size of text that will be drawn.
	MacAPI::CGGlyph glyphs[len];

	// convert characters to glyphs
	MacAPI::CGFontGetGlyphsForUnichars(fontRef, (short unsigned int*)s16.c_str(), glyphs, len);
	MacAPI::CGContextShowGlyphs(context, glyphs, len);
}

// factory call for creating a new platform specific Port object

Port* graphics_newPort(GraphicsManager* mgr) {
	PortImplMac* port = new PortImplMac(mgr);
	return port;
}

// internal implementation of text drawing
void graphics_drawText(PortImpl& port, const char* text, int len, const Quad& quad, int size, uint32 style, Color rgba) {
	FontImplMac* font = dynamic_cast<FontImplMac*> ( port.getCurrentFont(style) );
	if (!font) return;

	TextCacheEntry* textInfo = TextCacheEntry::findTextInCache(text, len, font, size, style);  // creates an entry if one doesn't already exist
	if (!textInfo) return;

	// it's possible that we've never measured this text
	if (textInfo->width == 0) {
		textInfo->width = port.getTextWidth(text, size, style, len);
	}
	if (textInfo->texture == 0) {
		// could be already in cache from width measurement call, but no texture generated yet
		int glBufferWidth = pow2(textInfo->width);
		int glBufferHeight = pow2(textInfo->charHeight);
		int glBufferPitch = ((glBufferWidth * 4) + 3) / 4;
		
		size_t dataSize = glBufferHeight * glBufferPitch;
		char* imageData = (char*) std::malloc( dataSize );
		if (!imageData) {
			delete textInfo;
			return;
		}
		
		MacAPI::CGColorSpaceRef colorSpace = MacAPI::CGColorSpaceCreateDeviceGray();
		MacAPI::CGContextRef cgl_ctx = MacAPI::CGBitmapContextCreate( imageData, glBufferWidth, glBufferHeight, 8, glBufferPitch, colorSpace, MacAPI::kCGImageAlphaNone);
		
		MacAPI::CGColorSpaceRelease( colorSpace );
		std::memset( imageData, 0x00, dataSize );
		MacAPI::CGContextSetTextPosition(cgl_ctx, 0, glBufferHeight - textInfo->ascent );
		
		if (style & Graphics::textStyle_Bold) {
			MacAPI::CGContextSetTextDrawingMode (cgl_ctx, MacAPI::kCGTextFillStroke);
		} else {
			MacAPI::CGContextSetTextDrawingMode (cgl_ctx, MacAPI::kCGTextFill);
		}
		MacAPI::CGContextSetGrayFillColor(cgl_ctx, 1.0f, 1.0f);
		MacAPI::CGContextSetGrayStrokeColor(cgl_ctx, 1.0f, 1.0f);
		graphics_CG_drawText(cgl_ctx, font, size, style, text, len);
		// draw underline if needed
		if (style & Graphics::textStyle_Underline ) {
			float underlineThickness = std::ceil((float)size / 12.0f);
			MacAPI::CGContextSetLineWidth(cgl_ctx, underlineThickness );
			MacAPI::CGPoint points[2];
			points[1].y = points[0].y = (underlineThickness * -1.25f) + (float)glBufferHeight - (float)textInfo->ascent;
			points[0].x = 0;
			points[1].x = textInfo->width;
			MacAPI::CGContextStrokeLineSegments(cgl_ctx, points, 2);
		}
		
		// create the Open GL texture
		glGenTextures(1, &textInfo->texture);
		glBindTexture(GL_TEXTURE_2D, textInfo->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, glBufferWidth, glBufferHeight, 0,
					 GL_ALPHA, GL_UNSIGNED_BYTE, imageData);
		std::free(imageData);
		imageData = 0;
		
		textInfo->tx = (float)textInfo->width/(float)glBufferWidth;
		textInfo->ty = (float)textInfo->charHeight/(float)glBufferHeight;
		if (style & Graphics::textStyle_Italic ) {
			textInfo->tx_topoffset = -(font->getFontHeight(size, style) / (PDG_ITALICS_FACTOR * (float)glBufferWidth));
		} else {
			textInfo->tx_topoffset = 0;
		}
	}

	Point topLeft, topRight, bottomLeft, bottomRight;
	topLeft = quad.points[lftTop];
	topRight = quad.points[rgtTop];
	bottomLeft = quad.points[lftBot];
	bottomRight = quad.points[rgtBot];
	
	port.setOpenGLModesForDrawing(true); // must use alpha for text
	glColor4f(rgba.red, rgba.green, rgba.blue, rgba.alpha);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textInfo->texture);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f (0.0, textInfo->ty);
	glVertex2f( bottomLeft.x, bottomLeft.y );
	glTexCoord2f (textInfo->tx_topoffset, 0.0);
	glVertex2f( topLeft.x, topLeft.y );
	glTexCoord2f (textInfo->tx, textInfo->ty);
	glVertex2f( bottomRight.x, bottomRight.y );
	glTexCoord2f (textInfo->tx + textInfo->tx_topoffset, 0.0);
	glVertex2f( topRight.x, topRight.y );
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
	
// ==================================================================
// PORT
// implementation of publicly declared methods (ie, exposed outside
// system framework by framework/inc/graphics.h)
// these implementations are Mac OS X dependent
// ==================================================================
	
int       
Port::getTextWidth(const char* text, int size, uint32 style, int len) {
//    PortImplMac& port = static_cast<PortImplMac&>(*this); // get us access to our private data
    if (text == 0) return 0;
    if (len == -1) {
        len = std::strlen(text);
    }
	if (len == 0) return 0;
	FontImplMac* font = dynamic_cast<FontImplMac*> ( getCurrentFont(style) );
	if (!font) return 0;
	
	TextCacheEntry* textInfo = TextCacheEntry::findTextInCache(text, len, font, size, style); // creates new entry if not already there
	if (!textInfo) return 0;
	if (textInfo->width == 0) {
		static MacAPI::CGContextRef sFontMeasuringContext = 0;
		static char sStaticJunkImageData[4];
		if (!sFontMeasuringContext) {
			MacAPI::CGColorSpaceRef colorSpace = MacAPI::CGColorSpaceCreateDeviceGray();
			sFontMeasuringContext = MacAPI::CGBitmapContextCreate( sStaticJunkImageData, 4, 1, 8, 4, colorSpace, MacAPI::kCGImageAlphaNone);
			MacAPI::CGColorSpaceRelease( colorSpace );
		}
		MacAPI::CGContextSetTextPosition(sFontMeasuringContext, 0, 0 );
		MacAPI::CGContextSetTextDrawingMode (sFontMeasuringContext, MacAPI::kCGTextInvisible);
		graphics_CG_drawText(sFontMeasuringContext, font, size, style, text, len);
		MacAPI::CGPoint textPt = MacAPI::CGContextGetTextPosition(sFontMeasuringContext);
		if (style & Graphics::textStyle_Underline ) {
			textPt.x += (font->getFontHeight(size, style) / PDG_ITALICS_FACTOR);
		}
		// save the new info in the cache
		textInfo->ascent = std::ceil(font->getFontAscent(size, style));
		textInfo->charHeight = textInfo->ascent + std::ceil(font->getFontDescent(size, style));
		textInfo->width = std::ceil(textPt.x);  // text position moved from offset 0 to new width
	}
    return textInfo->width;
}


FontImplMac::FontImplMac(Port* port, const char* fontName, float scalingFactor) 
	: FontImpl(port, fontName, scalingFactor) {
}

FontImplMac::~FontImplMac() {
	for (int i = 0; i < TEXT_INFO_CACHE_SIZE; i++) {
		if (mFontMetricsInfo[i]) {
			MacFontMetricsInfo* mfmi = (MacFontMetricsInfo*) mFontMetricsInfo[i];
			MacAPI::CFRelease(mfmi->mMacFont);
		}
	}
}

FontMetricsInfo* FontImplMac::getFontMetrics(int size, uint32 style) {
	MacFontMetricsInfo* mfmi = (MacFontMetricsInfo*) std::malloc(sizeof(MacFontMetricsInfo));
	mfmi->size = size;
	mfmi->style = style;
	MacAPI::CFStringRef name = MacAPI::CFStringCreateWithCString(0, getFontName(), kCFStringEncodingUTF8);
	mfmi->mMacFont = MacAPI::CGFontCreateWithFontName( name );
	if (!mfmi->mMacFont) {
		MacAPI::CGDataProviderRef fontDataProvider = 0;
		std::string basefilename = OS::getApplicationDataDirectory();
		basefilename += '/';
		basefilename += getFontName();
		// try various name permutations
		std::string filename = basefilename;
		fontDataProvider = MacAPI::CGDataProviderCreateWithFilename(filename.c_str());
		if (!fontDataProvider) {
			filename = basefilename + ".otf";
			fontDataProvider = MacAPI::CGDataProviderCreateWithFilename(filename.c_str());
		}
		if (!fontDataProvider) {
			filename = basefilename + ".ttf";
			fontDataProvider = MacAPI::CGDataProviderCreateWithFilename(filename.c_str());
		}
		if (!fontDataProvider) {
			filename = basefilename + ".dfont";
			fontDataProvider = MacAPI::CGDataProviderCreateWithFilename(filename.c_str());
		}
		// Create the font with the data provider, then release the data provider.
		mfmi->mMacFont = CGFontCreateWithDataProvider(fontDataProvider);
		MacAPI::CGDataProviderRelease(fontDataProvider);	
	}
	float unitsPerEm = MacAPI::CGFontGetUnitsPerEm( mfmi->mMacFont );
	float pixelsPerEm = 0.7518f * mScalingFactor;   // 1/1.33 Pixels per Em for Windows & MacOSX, 1/1.0 for traditional Mac OS;
	float ascent = ((float)MacAPI::CGFontGetAscent( mfmi->mMacFont ) * size * pixelsPerEm) / unitsPerEm ;
	float descent = (std::abs( (float)MacAPI::CGFontGetDescent( mfmi->mMacFont ) ) * size * pixelsPerEm) / unitsPerEm ;
	float leading = ((float)MacAPI::CGFontGetLeading( mfmi->mMacFont ) * size * pixelsPerEm) / unitsPerEm ;
	mfmi->height = ascent + descent + leading;
	mfmi->ascent = ascent;
	mfmi->descent = descent;
	mfmi->leading = leading;
	mfmi->usedWhen = 0;
	return mfmi;
}

MacAPI::PrivateOSFontRef FontImplMac::getMacFont(int size, uint32 style) {
	MacFontMetricsInfo* mfmi = (MacFontMetricsInfo*) fetchFontMetricsWithCaching(size, style);
	if (!mfmi) return 0;
	return mfmi->mMacFont;
}


// ==================================================================
// PORT IMPL MAC
// implementation of hidden implementation methods defined in PortImplMac
// these implementations are Mac OS X dependent
// ==================================================================

PortImplMac::PortImplMac(GraphicsManager* graphicsMgr)
: PortImpl(graphicsMgr) //,
//  mWindow(0)
{
}

PortImplMac::~PortImplMac()
{
}

	

// ==================================================================
// GRAPHICS MANAGER MAC
// implementation of publicly declared methods (ie, exposed outside
// system framework by framework/inc/graphics.h)
// these implementations are Mac OS X dependent
// ==================================================================

Font*   
GraphicsManagerMac::createFont(const char* fontName, float scalingFactor) {
	FontCacheEntry* fontInfo = FontCacheEntry::findFontInCache(fontName, scalingFactor);
    if (fontInfo->mFont == 0) {
		FontImplMac* font = new FontImplMac(getMainPort(), fontName, scalingFactor);
		font->addRef();
		fontInfo->mFont = font;
	}
	fontInfo->mFont->addRef();
	return fontInfo->mFont;
}

GraphicsManagerMac::GraphicsManagerMac() {
}

GraphicsManagerMac::~GraphicsManagerMac() {
}

GraphicsManager* GraphicsManager::createSingletonInstance() {
	GraphicsManager* gfxMgr = new GraphicsManagerMac();
	return gfxMgr;
}


} // end namespace pdg

#endif // PDG_NO_GUI
