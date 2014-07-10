// -----------------------------------------------
// memblock.h
//
// utility class to pass buffers around between
// script calls 
//
// Written by Ed Zavada, 2012-2013
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

#ifndef PDG_MEMBLOCK_H_INCLUDED
#define PDG_MEMBLOCK_H_INCLUDED

#include "pdg_project.h"

#include "pdg_script_bindings.h"

#include <cstdlib>

namespace pdg {


// used to pass buffers around between javascript calls
struct MemBlock {
	char* 	ptr;
	size_t	bytes;
	bool	owned;
    MemBlock(char* p, size_t n, bool own);
    MemBlock(size_t n);
    ~MemBlock();
	SCRIPT_OBJECT_REF mMemBlockScriptObj;
};

inline
MemBlock::MemBlock(char* p, size_t n, bool own) 
	: ptr(p), bytes(n), owned(own) 
{
	INIT_SCRIPT_OBJECT(mMemBlockScriptObj);
}

inline
MemBlock::MemBlock(size_t n) 
	: ptr(0), bytes(n), owned(true) 
{
	INIT_SCRIPT_OBJECT(mMemBlockScriptObj);
	ptr = (char*)std::malloc(bytes);
}

inline 
MemBlock::~MemBlock() { 
	if (ptr && owned) { 
		std::free(ptr);
		ptr = 0; 
	}
	CleanupMemBlockScriptObject(mMemBlockScriptObj);
}



} // end pdg namespace

#endif // PDG_MEMBLOCK_H_INCLUDED
