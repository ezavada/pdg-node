// -----------------------------------------------
// platform-events-win32.cpp
// 
// Windows implementation of platform specific event handling functions
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

#include "internals.h"

#define WinAPI
#include <windows.h>

namespace pdg {

void platform_init(int argc, const char* argv[]) {
  #if !defined( COMPILER_MWERKS ) && !defined( COMPILER_GCC )
	  #ifndef PDG_USE_WINDOWS_IME
		ImmDisableIME(0);	// don't use Windows IME for most apps
	  #endif // PDG_USE_WINDOWS_IME
  #endif // COMPILER_MWERKS
  
}

void platform_pollEvents() {
	// handle Windows Events
	MSG msg;
	if (WinAPI::PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		WinAPI::TranslateMessage(&msg);
		WinAPI::DispatchMessage(&msg);
	}
}

void platform_cleanup() {
  #ifndef PDG_NO_SOUND
	// Clean up COM
    CoUninitialize();
  #endif // PDG_NO_SOUND
}

void platform_getDeviceOrientation(float* outRoll, float* outPitch, float* outYaw, bool absolute) {
    *outPitch = 0.0f;
    *outRoll = 0.0f;
    *outYaw = 0.0f;
}


} // end namespace pdg

