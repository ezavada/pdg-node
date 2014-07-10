// -----------------------------------------------
// log.h
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


#ifndef PDG_LOG_H_INCLUDED
#define PDG_LOG_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/msvcfix.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/singleton.h"

#include <cstdio>
#include <string>
#include <iostream>

namespace pdg {

// -----------------------------------------------------------------------------------
// LogManager
// used for doing logging with variable debug levels
// -----------------------------------------------------------------------------------

class LogManager : public Singleton<LogManager> {
friend class Singleton<LogManager>;
public:
    enum {
        init_CreateUniqueNewFile = 0,
        init_OverwriteExisting = 1,
        init_AppendToExisting = 2,
        init_StdOut = 3,
        init_StdErr = 4
    };

    // initialize adds ".log" extension to the inLogNameBase
    virtual void  initialize(const char* inLogNameBase, int initMode = init_CreateUniqueNewFile) = 0;
    virtual void  writeLogEntry(int8 level, const char* category, const char* message) = 0;
    virtual void  setLogLevel(int8 level) = 0;
    virtual int8  getLogLevel() const = 0;

// lifecycle
/// @cond C++
    virtual ~LogManager() {};
/// @endcond

protected:
/// @cond INTERNAL
	static LogManager* createSingletonInstance();
	LogManager() {};	// call LogManager::getSingletonInstance() instead
/// @endcond
};

// -----------------------------------------------------------------------------------
// log
// utility class that makes writing log messages much easier
// -----------------------------------------------------------------------------------

// don't build script interface to log utility class
/// @cond C++
#ifndef SWIG

/*
// use examples:

using namespace pdg;

log::category category_MuLib("MULIB");
log::category category_Blue("Blue");

log << error << category_MuLib << "an MuLib error message" << endlog;

log << "this is a level " << 5 << " blue message" << category_Blue << endlog;

DEBUG_ONLY (
   log << verbose << "this will get compiled out in non-debug builds" << endlog;
)  

log << verbose << "this will be around even in non-debug builds" << endlog;

*/

enum EEndLogT {
   endlog
};    

class log {
public:

    enum ELogLevelT { 
        fatal = 0, 
        error = 2, 
        inform = 4, 
        detail = 6, 
        verbose = 8, 
        trace = 10,
        none = -1
    };

    struct category {
        category(const char* inCat) {mCat = inCat;}
        const char* mCat;
    };

    struct level {
        level(int n) {mLevel = n;}
        int mLevel;
    };
    
  #if COMPILER_TYPE_LONGLONG // support 64 bit integers where possible
    typedef long long number;
  #else  // don't have both long long and long at same time to prevent ambiguity
    typedef long number;
  #endif // COMPILER_TYPE_LONGLONG

    log& operator<< (log::category inCat) { setCategory(inCat); return *this; }
    log& operator<< (log::level inLevel)  { mLevel = inLevel.mLevel; return *this; }
    log& operator<< (ELogLevelT inLevel)  { mLevel = inLevel; return *this; }
    log& operator<< (const char* inStr)   { mMessage.append(inStr); return *this; }
    log& operator<< (std::string& inStr)  { mMessage.append(inStr); return *this; }
    log& operator<< (void* p)             { char tmp[32]; std::snprintf(tmp, 32, "%p", p); tmp[31]=0; mMessage.append(tmp); return *this; }
  #if COMPILER_TYPE_LONGLONG // support 64 bit integers where possible
    log& operator<< (number n)            { char tmp[64]; std::snprintf(tmp, 64, "%lld", n); tmp[63]=0; mMessage.append(tmp); return *this; }
  #else  // don't have both long long and long at same time to prevent ambiguity
    log& operator<< (number n)            { char tmp[32]; std::snprintf(tmp, 32, "%ld", n); tmp[31]=0; mMessage.append(tmp); return *this; }
  #endif // COMPILER_TYPE_LONGLONG
    log& operator<< (bool b)              { mMessage.append(b ? "true" : "false"); return *this; }
   
    log& operator<< (EEndLogT) 
        {
            if (mLogMgr) {
                mLogMgr->writeLogEntry( mLevel, mCategory.c_str(), mMessage.c_str() );
            } else {
                std::cout << mLevel << '\t' << mCategory.c_str() << '\t' << mMessage.c_str() << '\n';
            }
            mLevel = mDefaultLevel;
            mCategory.assign(mDefaultCategory);
            mMessage.assign("");
            return *this;
        }

    void setDefaultCategory(const char* inCatStr) { mDefaultCategory.assign(inCatStr); }
    void setDefaultLevel(int inLevel) { mDefaultLevel = inLevel; }
    
    void setCategory(category &inCat) { mCategory.assign(inCat.mCat); }
    
    void setLogManager(LogManager* inLogMgr) { mLogMgr = inLogMgr; }
    
    log(LogManager* inLogMgr = 0) : mLevel(inform), mDefaultLevel(inform), mLogMgr(inLogMgr) {}

protected:
    std::string mCategory;
    std::string mMessage;
    int mLevel;
    
    std::string mDefaultCategory;
    int mDefaultLevel;
    
    LogManager* mLogMgr;
};
#endif // !SWIG
/// @endcond

}  // close namespace pdg

#endif // PDG_LOG_H_INCLUDED

