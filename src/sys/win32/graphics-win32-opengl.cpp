// -----------------------------------------------
// graphics-win32-opengl.cpp
//
// Windows implementation of common graphics functions
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


#include "pdg_project.h"

#ifndef PDG_NO_GUI

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include <algorithm>
#include <string>

#include "graphics-win32.h"
#include "pdg-main.h"
#include "textcache-opengl.h"
#include "pdg/sys/resource.h"
#include "internals.h"

#include "internals-win32.h"

extern int pow2(int n);

namespace pdg {

#define PDG_ITALICS_FACTOR 3.333f

// some routines to deal with the low level OS cursor

void platform_setHardwareCursorVisible(bool inVisible) {
	WinAPI::ShowCursor(inVisible);
}

HCURSOR gHourGlassCursor = NULL;

void platform_setHardwareBusyCursor() {
    if (!gHourGlassCursor) {
	    gHourGlassCursor = WinAPI::LoadCursor(NULL, IDC_WAIT);
	}
    GraphicsManagerWin* winGfxMgr = static_cast<GraphicsManagerWin*>(GraphicsManager::getSingletonInstance());
    winGfxMgr->privateSetWin32HardwareCursor(gHourGlassCursor);
}

void platform_setHardwareNormalCursor() {
    GraphicsManagerWin* winGfxMgr = static_cast<GraphicsManagerWin*>(GraphicsManager::getSingletonInstance());
    winGfxMgr->privateSetWin32HardwareCursor(NULL);
}


HBITMAP CreateOffscreenBitmap8( HDC hDC, int width, int height ) {
    // Creating new bitmap and receive pointer to it's bits.
    HBITMAP hTargetBitmap;
    void *pBuffer;

    width = ((width+3)/4)*4;

    // Initilize DIBINFO structure
    BITMAPINFO dibInfo;
    ZeroMemory( &dibInfo, sizeof(BITMAPINFO) );

    dibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
    dibInfo.bmiHeader.biWidth = width;
    dibInfo.bmiHeader.biHeight = height;
    dibInfo.bmiHeader.biBitCount = 8;
    dibInfo.bmiHeader.biPlanes = 1;
    dibInfo.bmiHeader.biSizeImage = width * height;

    // Create bitmap and receive pointer to points into pBuffer
    hTargetBitmap = WinAPI::CreateDIBSection( hDC, &dibInfo, DIB_RGB_COLORS, (void**)&pBuffer, NULL, 0);

    return hTargetBitmap;
}

// factory call for creating a new platform specific Port object

Port* graphics_newPort(GraphicsManager* mgr) {
	PortImplWin* port = new PortImplWin(mgr);
	return port;
}

// internal implementation of text drawing
void graphics_drawText(PortImpl& portimpl, const char* text, int len, const Quad& quad, int size, uint32 style, Color rgba) {
    PortImplWin& port = static_cast<PortImplWin&>(portimpl); // get us access to our private data
 	FontImplWin* font = dynamic_cast<FontImplWin*> ( port.getCurrentFont(style) );
	if (!font) return;

	TextCacheEntry* textInfo = TextCacheEntry::findTextInCache(text, len, font, size, style);  // creates an entry if one doesn't already exist
	if (!textInfo) return;

	// it's possible that we've never measured this text
	if (textInfo->width == 0) {
		textInfo->width = port.getTextWidth(text, size, style, len);
	}
	if (textInfo->texture == 0) {
		// could be already in cache from width measurement call, but no texture generated yet
		// convert the string to the unicode representation that Windows supports
		std::string src(text);
		utf16string wtext;
		OS::utf8to16(wtext, src);
		len = wtext.length();
		const WCHAR* theText = (WCHAR*)wtext.c_str();
		WinAPI::HDC dc = graphics_getPortDC(&port);
		// create font
		WinAPI::HFONT wfont = font->getWindowsFont(size, style);

		// create memory dc
		HDC hMemDC	= WinAPI::CreateCompatibleDC(dc);
		WinAPI::SelectObject(hMemDC, wfont);

		int glBufferWidth = pow2(textInfo->width);
		int glBufferHeight = pow2(textInfo->charHeight);

		HBITMAP hBitmap = CreateOffscreenBitmap8(hMemDC, glBufferWidth, glBufferHeight);
		WinAPI::SelectObject(hMemDC, hBitmap);

		BITMAP      bm;
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
		RECT rRect;
		rRect.left = 0;
		rRect.top =0;
		rRect.bottom = bm.bmHeight;
		rRect.right = bm.bmWidth;
		char* imageData = (char*) bm.bmBits;
		size_t dataSize = bm.bmHeight * bm.bmWidth;
		std::memset( imageData, 0x00, dataSize );
		int yOffset = glBufferHeight - textInfo->charHeight;

		HBRUSH hBrush = CreateSolidBrush(RGB(10,10,10));
//		WinAPI::FillRect(hMemDC, &rRect, hBrush);
		WinAPI::SetBkMode(hMemDC, TRANSPARENT);
		WinAPI::SetTextColor(hMemDC, RGB(128, 128, 128) );
		WinAPI::ExtTextOutW(hMemDC, 0, yOffset, ETO_CLIPPED, &rRect, theText, len, NULL);  // Unicode vers supported on Win98

		// create the Open GL texture
		glGenTextures(1, &textInfo->texture);
		glBindTexture(GL_TEXTURE_2D, textInfo->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, glBufferWidth, glBufferHeight, 0,
					 GL_ALPHA, GL_UNSIGNED_BYTE, imageData);

		WinAPI::DeleteObject(hBitmap);
		WinAPI::DeleteDC(hMemDC);
//		std::free(imageData);
//		imageData = 0;

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
// these implementations are Windows dependent
// ==================================================================
	
int       
Port::getTextWidth(const char* text, int size, uint32 style, int len) {
    PortImplWin& port = static_cast<PortImplWin&>(*this); // get us access to our private data
    if (text == 0) return 0;
    if (len == -1) {
        len = std::strlen(text);
    }
	if (len == 0) return 0;
	FontImplWin* font = dynamic_cast<FontImplWin*> ( getCurrentFont(style) );
	if (!font) return 0;
    int textWidth = 0;
  	WinAPI::HDC dc = graphics_getPortDC(this);
	// create font
	WinAPI::HFONT wfont = font->getWindowsFont(size, style);

	// set the new font, preserving old
	WinAPI::HGDIOBJ oldFont = WinAPI::SelectObject(dc, wfont);
	std::string src(text);
	utf16string wtext;
	OS::utf8to16(wtext, src, len);
	const WinAPI::WCHAR* theText = (WinAPI::WCHAR*)wtext.c_str();
	WinAPI::SIZE textSize;
	int cbLen = wtext.length();
	WinAPI::GetTextExtentPoint32W(dc, theText, cbLen, &textSize);  // Unicode vers supported on Win98
	textWidth = textSize.cx;
	// put back the font and clean up created font
    WinAPI::SelectObject(dc, oldFont);
    return textWidth;
}


FontImplWin::FontImplWin(Port* port, const char* fontName, float scalingFactor)
: FontImpl(port, fontName, scalingFactor) {
}

FontImplWin::~FontImplWin() {
	for (int i = 0; i < TEXT_INFO_CACHE_SIZE; i++) {
		if (mFontMetricsInfo[i]) {
			WinFontMetricsInfo* mfmi = (WinFontMetricsInfo*) mFontMetricsInfo[i];
			WinAPI::DeleteObject(mfmi->mWinFont);
		}
	}
}

FontMetricsInfo* FontImplWin::getFontMetrics(int size, uint32 style) {
	WinFontMetricsInfo* mfmi = (WinFontMetricsInfo*) std::malloc(sizeof(WinFontMetricsInfo));
	PortImplWin& port = static_cast<PortImplWin&> (*mPort);
	mfmi->size = size;
	mfmi->style = style;
	WinAPI::HDC dc = graphics_getPortDC(mPort);
	// create font
	mfmi->mWinFont = port.createWindowsFont(mFontName.c_str(), size, style);
	// set the new font, preserving old
	WinAPI::HGDIOBJ oldFont = WinAPI::SelectObject(dc, mfmi->mWinFont);
	// get the text metrics
	WinAPI::TEXTMETRICA textinfo;
	WinAPI::GetTextMetricsA(dc, &textinfo); // calling ASCII version on Win98 will be ok
	// put back the font and clean up created font
	WinAPI::SelectObject(dc, oldFont);
	float ascent = (float)textinfo.tmAscent * mScalingFactor;
	float descent = std::abs( (float) textinfo.tmDescent * mScalingFactor );
	float leading = (float)textinfo.tmExternalLeading * mScalingFactor;
	mfmi->height = ascent + descent + leading;
	mfmi->ascent = ascent;
	mfmi->descent = descent;
	mfmi->leading = leading;
	mfmi->usedWhen = 0;
	return mfmi;
}


WinAPI::HFONT FontImplWin::getWindowsFont(int size, uint32 style) {
	WinFontMetricsInfo* wfmi = (WinFontMetricsInfo*) fetchFontMetricsWithCaching(size, style);
	if (!wfmi) return 0;
	return wfmi->mWinFont;
}


// ==================================================================
// PORT IMPL WIN
// implementation of hidden implementation methods defined in PortImplWin
// these implementations are Windows dependent
// ==================================================================

PortImplWin::PortImplWin(GraphicsManager* graphicsMgr)
: PortImpl(graphicsMgr), mWindow(0)
{
}

PortImplWin::~PortImplWin()
{
}

WinAPI::HFONT
PortImplWin::createWindowsFont(const char* inFontName, int size, uint32 style) {
    // create font
    WinAPI::LOGFONTA LF; // NO UNICODE ON WIN98, so using Ascii version
    WinAPI::LOGFONTA* pLF = &LF;
    std::memset(pLF, 0, sizeof(WinAPI::LOGFONTA));
	std::strncpy(pLF->lfFaceName, inFontName, LF_FACESIZE);

	// we use this calculation if we are on a device other than the screen
//	HDC hMainDC = mBackBuffer.GetDC();
//	int logpixelsy = WinAPI::GetDeviceCaps(hMainDC, LOGPIXELSY);
//	pLF->lfHeight = -(size * logpixelsy)/72;

	// we use this calculation because we assume the screen is 72 DPI
    pLF->lfHeight = -size;

    // default to plain
    pLF->lfWeight = FW_NORMAL;
    pLF->lfItalic=0;
    pLF->lfUnderline=0;
   #ifdef PDG_NO_WIN32_GDI
    pLF->lfQuality = NONANTIALIASED_QUALITY;
   #endif   // can't use antialiasing if we aren't using GDI to draw directly into the back buffer

    // add styles as appropriate
    if (style & Graphics::textStyle_Bold) {
	   pLF->lfWeight = FW_BOLD;
    }
    if (style & Graphics::textStyle_Italic) {
       pLF->lfItalic=1;
    }
    if (style & Graphics::textStyle_Underline) {
       pLF->lfUnderline=1;
    }
    WinAPI::HFONT font = WinAPI::CreateFontIndirectA(pLF); // NO UNICODE ON WIN98
    return font;
}


// ==================================================================
// GRAPHICS MANAGER WIN
// implementation of publicly declared methods (ie, exposed outside
// system framework by framework/inc/graphics.h)
// these implementations are Windows dependent
// ==================================================================

Font*
GraphicsManagerWin::createFont(const char* fontName, float scalingFactor) {
	if (!getMainPort()) return 0;  // can't create a font unless main port is set
	FontCacheEntry* fontInfo = FontCacheEntry::findFontInCache(fontName, scalingFactor);
	if (fontInfo->mFont == 0) {
		FontImplWin* font = new FontImplWin(getMainPort(), fontName, scalingFactor);
		font->addRef();
		fontInfo->mFont = font;
	}
	fontInfo->mFont->addRef();
	return fontInfo->mFont;
}


void
GraphicsManagerWin::privateSetWin32HardwareCursor(HCURSOR winCursor) {
    mCurrentWin32HardwareCursor = winCursor;
    privateMaintainWin32HardwareCursor();
}

// called whenever Windows sends us WM_SETCURSOR
bool
GraphicsManagerWin::privateMaintainWin32HardwareCursor() {
    if (mCurrentWin32HardwareCursor != NULL) {
        WinAPI::SetCursor(mCurrentWin32HardwareCursor);
        return true;
    } else {
        return false;
    }
}

void
GraphicsManagerWin::privateSetWindowIcons(DWORD iconIdDefault, DWORD iconId16Bit, DWORD iconId4Bit) {
    mIconId8Bit = iconIdDefault;
    if (iconId16Bit != 0) {
        mIconId16Bit = iconId16Bit;
    } else {
        mIconId16Bit = iconIdDefault; // none specified, use the default icon
    }
    if (iconId4Bit != 0) {
        mIconId4Bit = iconId4Bit;
    } else {
        mIconId4Bit = iconIdDefault; // none specified, use the default icon
    }
}

GraphicsManagerWin::GraphicsManagerWin(HINSTANCE appInstance, LRESULT (CALLBACK *windowProc)(HWND, UINT, WPARAM, LPARAM))
 : mAppInstance(appInstance),
   mCurrentWin32HardwareCursor(0),
   mWindowProc(windowProc),
   mIconId8Bit(0),
   mIconId16Bit(0),
   mIconId4Bit(0)
{
}

GraphicsManagerWin::~GraphicsManagerWin() {
}


extern LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern HINSTANCE gAppInstance;


GraphicsManager* GraphicsManager::createSingletonInstance() {
	GraphicsManager* gfxMgr =  new GraphicsManagerWin(gAppInstance, WindowProc);
	// graphics manager needs to be connected to the resource manager

	ResourceManager* resMgr = ResourceManager::getSingletonInstance();
	return gfxMgr;

}



} // end namespace pdg

#endif // PDG_NO_GUI
