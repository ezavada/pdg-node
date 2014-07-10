// -----------------------------------------------
// msvc.h
//
// Fixes to make MSVC 6 act like an ANSI C++ compiler
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2004-2012, Dream Rock Studios, LLC
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

#ifndef PDG_MSVCFIX_H_INCLUDED
#define PDG_MSVCFIX_H_INCLUDED

#define MSVC_VERSION_2012 1700

// ---------------------------------------------------
// VISUAL C++ .NET < 2012
// ---------------------------------------------------

#if defined( _MSC_VER ) && ( _MSC_VER < MSVC_VERSION_2012 )

// disable exception specification warnings
#pragma warning ( disable : 4290 )

// disable int->bool performance warnings
#pragma warning ( disable : 4800 )

// disable 'this' : used in base member initializer list warnings
#pragma warning ( disable : 4355 )

// disable This function or variable may be unsafe, consider using ... instead
#pragma warning ( disable : 4996 )

// disable name was marked as #pragma deprecated
#pragma warning ( disable : 4995 )

// disable conversion warnings
#pragma warning ( disable : 4244 )

// disable truncation from 'double' to 'float'
#pragma warning ( disable : 4305 )

#include <stdio.h>
#include <stdarg.h>

inline float fmaxf(float a, float b) { return a > b ? a : b; }
inline float fminf(float a, float b) { return a < b ? a : b; }

namespace std {
	inline int    snprintf(char * s, size_t max, const char * fmt, ...) { int n; va_list lst; va_start(lst, fmt); n = vsnprintf(s, max, fmt, lst); va_end(lst); return n; }
	using ::vsnprintf;
}

#define PDG_VS_NEED__IMP__VSNPRINTF

// hack to work around broken for loop scope, forces for loops
// to be inside a reduced scope
#ifndef for
#define for    if (0) {} else for
#endif // for

#endif // _MSC_VER 7 or later before VC++ 2012

// ---------------------------------------------------
// VISUAL C++ 2012 and later
// ---------------------------------------------------

#if defined( _MSC_VER ) && ( _MSC_VER >= MSVC_VERSION_2012 )

// disable exception specification warnings
#pragma warning ( disable : 4290 )

// disable int->bool performance warnings
#pragma warning ( disable : 4800 )

// disable truncation from 'double' to 'float'
#pragma warning ( disable : 4305 )

// disable Constant overflow warnings
#pragma warning ( disable : 4056 )

#include <stdio.h>
#include <stdarg.h>
namespace std {
    inline int snprintf(char* str, size_t size, const char* format, ...) {
        int count;
        va_list ap;
        va_start(ap, format);
        count = vsnprintf(str, size, format, ap);
        va_end(ap);
        return count;
    }
} // end namespace std

#endif // _MSC_VER 2012 and later

// ---------------------------------------------------
// MetroWorks
// ---------------------------------------------------

#if defined (__MWERKS__)
    #undef strdup
#endif // __MWERKS__

// ---------------------------------------------------
// GCC
// ---------------------------------------------------

// GCC fix
#if defined (__GNUC__)
#include <stdio.h>
namespace std {
	using ::snprintf;
}
#endif // __GNUC__

#endif // PDG_MSVCFIX_H_INCLUDED
