// -----------------------------------------------
// memstats.h
//
// utility structure for tracking memory usage by the game
//
// Written by Ed Zavada, 2014
// Copyright (c) 2014, Dream Rock Studios, LLC
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


#ifndef PDG_MEMSTATS_H_INCLUDED
#define PDG_MEMSTATS_H_INCLUDED

#include "pdg/sys/global_types.h"

namespace pdg {

struct MemStats {
	uint32 cxxObjsAllocated;      // number of C++ objects allocated/freed
	uint32 cxxObjsFreed;
	uint32 blocksAllocated;       // number of raw memory blocks/buffers allocated/freed
	uint32 blocksFreed;
	uint32 osEntitiesAllocated;   // number of OS platform entities allocated/freed
	uint32 osEntitiesFreed;
	uint32 filesOpened;           // number of files opened closed
	uint32 filesClosed;
	uint32 refCountsAdded;        // number of C++ RefCountedObject calls to addRef/release
	uint32 refCountsReleased;
	MemStats() : cxxObjsAllocated(0), cxxObjsFreed(0), 
		blocksAllocated(0), blocksFreed(0), 
		osEntitiesAllocated(0), osEntitiesFreed(0), 
		filesOpened(0), filesClosed(0),
		refCountsAdded(0), refCountsReleased(0) {}
	void clear() {
		cxxObjsAllocated = cxxObjsFreed = 0; 
		blocksAllocated = blocksFreed = 0;
		osEntitiesAllocated = osEntitiesFreed = 0;
		filesOpened = filesClosed = 0;
		refCountsAdded = refCountsReleased = 0;
	}
};

}
#endif // PDG_MEMSTATS_H_INCLUDED

