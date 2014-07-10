// -----------------------------------------------
// graphics-macosx.h
// 
// Mac OS X definitions for graphics manager functions
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


#ifndef PDG_GRAPHICS_MACOSX_H_INCLUDED
#define PDG_GRAPHICS_MACOSX_H_INCLUDED

#include "pdg_project.h"

#ifndef PDG_NO_GUI

#include "pdg/sys/os.h"

#include "graphics-opengl.h"

#include <string>

#ifndef PLATFORM_IOS
#define _UINT32
#include <ApplicationServices/ApplicationServices.h>
#else
#include <CoreGraphics/CoreGraphics.h>
#endif
//#define PrivateOSWindowRef WindowRef	
//#define PrivateOSPortRef CGContextRef
#define PrivateOSFontRef CGFontRef

namespace pdg {


struct MacFontMetricsInfo : public FontMetricsInfo {
	MacAPI::PrivateOSFontRef mMacFont;
};

// mostly what this class does is make the constructors and destructors public
// so the Graphics Manger can create Ports
class PortImplMac : public PortImpl {
public:
    PortImplMac(GraphicsManager* graphicsMgr);
    virtual ~PortImplMac();
    
    void setPortMacFont(int size, uint32 style);
    void setPortRects(Rect portRect) { mDrawingRect = portRect; mClipRect = portRect; }
};

class FontImplMac : public FontImpl {
public:
	FontImplMac(Port* port, const char* fontName, float scalingFactor);
	virtual ~FontImplMac();
	virtual FontMetricsInfo* getFontMetrics(int size, uint32 style);
	MacAPI::PrivateOSFontRef getMacFont(int size, uint32 style);
};

// === graphics manager ====

class GraphicsManagerMac : public GraphicsManager {
public:
    virtual Font*   createFont(const char* fontName, float scalingFactor);

    GraphicsManagerMac();
    virtual ~GraphicsManagerMac();
};

} // end namespace pdg

#endif // PDG_NO_GUI

#endif // PDG_GRAPHICS_MACOSX_H_INCLUDED

