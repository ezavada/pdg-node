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

#include <map>
#include <cstdio>

#ifndef PDG_NO_SERIALIZER_SANITY_CHECKS
#define SERIALIZER_SANITY_CHECKS
#endif


namespace pdg {

	enum {
		tag_veryLongLen	= 0xFF,
		tag_longLen		= 0xFE,
		tag_smallestLenTag  = tag_longLen,
		tag_pdgTaggedStream = 0x706467,	  // 'pdg'  -- a tag to identify a tagged pdg stream
		tag_string      = 0x737472,   // 'str'
		tag_mem			= 0x6d656d,   // 'mem'
		tag_object		= 0x6f626a,   // 'obj'
		tag_objectNil	= 0x6e696c,   // 'nil'  -- an nil object pointer
		tag_objectRef   = 0x726566,	  // 'ref'  -- an additional reference to an object that has already been streamed
		tag_pointerRef  = 0x707472	  // 'ptr'  -- a ptr to a non streamable object
	};
	

    // maps object unique ids to the object pointers
    typedef std::map<uint32, void*> ObjectRegistryT;
    extern ObjectRegistryT gObjectRegistry;


#ifdef PDG_DESERIALIZER_NO_THROW
	// we can't throw, so we report errors but don't exit
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except, _cleanup) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)\nOffset: %ld End: %ld", #_except, _msg, __FUNCTION__, __FILE__, __LINE__, (long)(p - mDataPtr), (long)(mDataEnd - mDataPtr)); \
		DEBUG_PRINT(buf); } // don't do the cleanup since we aren't exiting
#else
	// we throw exceptions on errors
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except, _cleanup) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)\nOffset: %ld End: %ld", #_except, _msg, __FUNCTION__, __FILE__, __LINE__, (long)(p - mDataPtr), (long)(mDataEnd - mDataPtr)); \
		_cleanup; throw _except(buf); }
#endif


	
} // end namespace pdg

#endif // SERIALIZER_IMPL_H_INCLUDED

