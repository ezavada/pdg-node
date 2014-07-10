// -----------------------------------------------
// internals-macosx.h
// 
// Mac OS X bindings between Obj-C and C++ code
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

// -----------------------------------------------
// Wrapper Functions called by Objective C
// -----------------------------------------------

typedef struct CGContext *CGContextRef;

//extern long gInitialPortWidth;
//extern long gInitialPortHeight;

#ifdef __cplusplus
#define IN_PDG_NAMESPACE(item) pdg::item
#else
#define IN_PDG_NAMESPACE(item) item
#endif // cplusplus	


typedef signed char	BOOL; 
typedef unsigned short unichar;
#if __cplusplus
extern "C" {
#endif
	
	// -----------------------------------------------------
	// Functions defined in Objective C and called from C++
	// -----------------------------------------------------
	
	int pow2(int n);
	CGContextRef graphics_getCurrentCGContextRef(void);
	bool macosx_getApplicationSupportDirectory(const char* appName, char* buffer, int size);
	void macosx_getSystemVersion( int &major, int &minor, int &bugfix );

#if __cplusplus
}
#endif
