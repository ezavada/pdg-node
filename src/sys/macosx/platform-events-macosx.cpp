// -----------------------------------------------
// platform-events-macosx.cpp
// 
// Mac OS X implementation of platform specific event handling functions
// Cocoa implementation
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

// this isn't used for Mac OS X, and is just here for completeness
// All the MacOS X specific initialization, etc.. is handled by a
// custom version of pdg-lib.cpp -- pdg-lib-macosx.mm


namespace pdg {

void platform_init(int argc, const char* argv[]) {
}

void platform_pollEvents() {
}

void platform_cleanup() {
}

#ifndef PLATFORM_IOS
void platform_getDeviceOrientation(float* outRoll, float* outPitch, float* outYaw, bool absolute) {
    *outPitch = 0.0f;
    *outRoll = 0.0f;
    *outYaw = 0.0f;
}
#endif // !PLATFORM_IOS


} // end namespace pdg

