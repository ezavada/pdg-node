// -----------------------------------------------
// memblock.h
//
// utility class to pass buffers around between
// script calls 
//
// Written by Ed Zavada, 2012-2015
// Copyright (c) 2015, Dream Rock Studios, LLC
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

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

#include <cstdlib>
#include <string>

namespace pdg {


// used to pass buffers around between javascript calls
struct MemBlock {
	char* 	ptr;
	size_t	bytes;
	bool	owned;
    MemBlock(char* p, size_t n, bool own);
    MemBlock(size_t n);
    std::string& getData();
    size_t  getDataSize();
    unsigned char getByte(size_t i);
    std::string& getBytes(size_t start, size_t len);
    ~MemBlock();
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mMemBlockScriptObj;
  #endif
};


} // end pdg namespace

#endif // PDG_MEMBLOCK_H_INCLUDED
