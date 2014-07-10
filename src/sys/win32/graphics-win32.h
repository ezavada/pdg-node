// -----------------------------------------------
// graphics-win32-glfw.h
//
// Windows specific definitions for graphics manager functions
// implemented using GLFW
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

#ifndef PDG_GRAPHICS_WIN32_GLFW_H_INCLUDED
#define PDG_GRAPHICS_WIN32_GLFW_H_INCLUDED

#ifndef PDG_NO_GUI

#include "pdg/sys/os.h"

#include "graphics-opengl.h"

#include <string>

#define WinAPI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

namespace pdg {


struct WinFontMetricsInfo : public FontMetricsInfo {
	WinAPI::HFONT mWinFont;
};

// mostly what this class does is make the constructors and destructors public
// so the Graphics Manger can create Ports
class PortImplWin : public PortImpl {
public:
    PortImplWin(GraphicsManager* graphicsMgr);
    virtual ~PortImplWin();

	WinAPI::HFONT createWindowsFont(const char* inFontName, int size, uint32 style);
    void setPortRects(Rect portRect) { mDrawingRect = portRect; mClipRect = portRect; }
public: // exposed to other parts of the sys framework implementation, but not outside it
    WinAPI::HWND        mWindow;
};

class FontImplWin : public FontImpl {
public:
	FontImplWin(Port* port, const char* fontName, float scalingFactor);
	virtual ~FontImplWin();
	virtual FontMetricsInfo* getFontMetrics(int size, uint32 style);
	WinAPI::HFONT getWindowsFont(int size, uint32 style);
};

// === graphics manager ====

class GraphicsManagerWin : public GraphicsManager {
public:
    virtual Font*   createFont(const char* fontName, float scalingFactor);

    GraphicsManagerWin(WinAPI::HINSTANCE appInstance, WinAPI::LRESULT (CALLBACK *windowProc)(WinAPI::HWND, WinAPI::UINT, WinAPI::WPARAM, WinAPI::LPARAM));
    virtual ~GraphicsManagerWin();

    Port*           privateCreateInWindowPort(const Rect& rect, WinAPI::HWND containerWindow, int bpp = 0);
    void            privateSetWin32HardwareCursor(WinAPI::HCURSOR winCursor);
    bool            privateMaintainWin32HardwareCursor();
    void            privateSetWindowIcons(WinAPI::DWORD iconIdDefault, WinAPI::DWORD iconId16Bit = 0, WinAPI::DWORD iconId4Bit = 0);

	WinAPI::HINSTANCE   mAppInstance;
    WinAPI::HCURSOR     mCurrentWin32HardwareCursor;
    WinAPI::LRESULT     (CALLBACK *mWindowProc)(WinAPI::HWND, WinAPI::UINT, WinAPI::WPARAM, WinAPI::LPARAM);

    WinAPI::DWORD   mIconId8Bit;
    WinAPI::DWORD   mIconId16Bit;
    WinAPI::DWORD   mIconId4Bit;
};

} // end namespace pdg

#endif // PDG_NO_GUI

#endif // PDG_GRAPHICS_WIN32_GLFW_H_INCLUDED

