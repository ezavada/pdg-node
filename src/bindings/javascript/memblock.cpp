// -----------------------------------------------
// memblock.cpp
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

#include "pdg_project.h"

#include "memblock.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

#include <cstdlib>
#include <string>

namespace pdg {


std::string&    
MemBlock::getData() { 
    static std::string _workstr;
    _workstr.resize(bytes);
    char* p = const_cast<char*>(_workstr.data());
    for (size_t i = 0; i < bytes; i++) {
        p[i] = ptr[i];
    }
    return _workstr;
}

size_t  
MemBlock::getDataSize() { 
    return bytes; 
}

unsigned char 
MemBlock::getByte(size_t i) { 
    return (i<bytes) ? ptr[i] : 0; 
}

MemBlock::MemBlock(char* p, size_t n, bool own) 
	: ptr(p), bytes(n), owned(own) 
{
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mMemBlockScriptObj);
  #endif
}

MemBlock::MemBlock(size_t n) 
	: ptr(0), bytes(n), owned(true) 
{
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mMemBlockScriptObj);
  #endif
	ptr = (char*)std::malloc(bytes);
}

MemBlock::~MemBlock() { 
	if (ptr && owned) { 
		std::free(ptr);
		ptr = 0; 
	}
  #ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupMemBlockScriptObject(mMemBlockScriptObj);
  #endif
}

// Not thread safe -- runs in a single threaded JavaScript environment
std::string& 
MemBlock::getBytes(size_t start, size_t len) {
    static std::string _workstr;
    _workstr.resize((start > bytes) ? 0 : (start + len > bytes) ? bytes - start : len);
    char* p = const_cast<char*>(_workstr.data());
    for (size_t i = 0; i < len; i++) {
        p[i] = ptr[start+i];
    } 
    return _workstr;
}


} // end pdg namespace

