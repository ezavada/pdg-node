// -----------------------------------------------
// global_types.h
// 
// Definitions for universally-used common types
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


#ifndef PDG_GLOBAL_TYPES_H_INCLUDED
#define PDG_GLOBAL_TYPES_H_INCLUDED

#include "pdg_project.h"

// -----------------------------------------------------------------------------------
// some basic type definitions for sized types
// -----------------------------------------------------------------------------------

#ifdef PDG_DECORATE_GLOBAL_TYPES
	#define uint8 pdg_uint8
	#define  int8 pdg_int8
	#define uint16 pdg_uint16
	#define  int16 pdg_int16
	#define uint32 pdg_uint32
	#define  int32 pdg_int32

	#ifndef __cplusplus
		// these are only global types if we aren't using C++, in C++ these are in namespace pdg
		#define  utf16char pdg_utf16char
		#define  utf32char pdg_utf32char
	#endif

#endif

typedef unsigned char  	    uint8;
typedef signed char          int8;
typedef unsigned short 	    uint16;
typedef signed short         int16;

#if !defined( _UINT32 ) || defined( PDG_DECORATE_GLOBAL_TYPES )
typedef unsigned long       uint32;
#define _UINT32  // this keeps Apple headers from blowing up
#endif
typedef signed long          int32;

#ifndef PDG_NO_64BIT
typedef unsigned long long  uint64;
typedef signed long long     int64;
#endif

#ifdef __cplusplus
namespace pdg {
#endif

typedef uint16          utf16char;
typedef uint32          utf32char;

#ifdef __cplusplus
} // end namespace pdg
#endif
		
#endif // PDG_GLOBAL_TYPES_H_INCLUDED


