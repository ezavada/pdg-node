// -----------------------------------------------
// log-impl.h
// 
// logging support
//
// Written by Ed Zavada, 2001-2012
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


#ifndef PDG_LOG_IMPL_H_INCLUDED
#define PDG_LOG_IMPL_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/log.h"
#include "pdg/sys/mutex.h"

// C++ standard lib includes
#include <fstream>

#include <cstdio>
#include <time.h>

namespace pdg {

class LogManagerImpl : public LogManager {
public:

// LogManager methods
    virtual void  initialize(const char* inLogNameBase, int initMode); // adds ".log" extension to base name
    virtual void  writeLogEntry(int8 level, const char* category, const char* message);
    virtual void  setLogLevel(int8 level);
    virtual int8  getLogLevel() const;

// lifecycle
    LogManagerImpl() : mWriteMutex(), mFile(), mStream(&mFile), mInited(false), 
    					mLevel(0), mInitMode(init_StdOut) {};
    virtual ~LogManagerImpl() { if (mFile.is_open()) {mFile.close();} }
protected:
    Mutex           mWriteMutex;
    std::ofstream   mFile;
    std::ostream*	mStream;
    bool            mInited;
    int8            mLevel;
	std::string		mBaseLogName;
	int				mInitMode;
	std::string		mLogFilename; // for reopening
};

} // close namespace pdg

#endif // PDG_LOG_IMPL_H_INCLUDED

