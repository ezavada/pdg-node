// -----------------------------------------------
//  serializer-impl.h
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


#ifndef SERIALIZER_IMPL_H_INCLUDED
#define SERIALIZER_IMPL_H_INCLUDED

#include "pdg_project.h"


#ifndef PDG_NO_SERIALIZER_SANITY_CHECKS
#define SERIALIZER_SANITY_CHECKS
#endif


namespace pdg {

	enum {
		tag_veryLongLen	= 0xFF,
		tag_longLen		= 0xFE,
		tag_smallestLenTag  = tag_longLen,
		tag_string      = 0x737472,   // 'str'
		tag_mem			= 0x6d656d,   // 'mem'
		tag_object		= 0x6f626a,   // 'obj'
		tag_objectNil	= 0x6e696c,   // 'nil'  -- an nil object pointer
		tag_objectRef   = 0x726566	  // 'ref'  -- an additional reference to an object that has already been streamed
	};
	
	
	
} // end namespace pdg

#endif // SERIALIZER_IMPL_H_INCLUDED

