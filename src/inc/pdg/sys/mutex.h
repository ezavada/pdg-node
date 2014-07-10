// -----------------------------------------------
// mutex.h
//
// Provides platform independent mutex and critical section classes.
//
// A mutex allows only one thread at a time to execute inside a 
// locked section
// 
// A critical section prevents the section from being interrupted
// by another thread, making the action atomic
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


#ifndef PDG_SYS_MUTEX_H_INCLUDED
#define PDG_SYS_MUTEX_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/semaphore.h"

#include <exception>


#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

#ifndef PDG_DEBUG_MUTEX
  // if the network layer is not being debugged, we ignore all 
  #define MUTEX_DEBUG_ONLY(_expression)
#else
  #define MUTEX_DEBUG_ONLY DEBUG_ONLY
  #include "pdg/sys/os.h"  // for debugging stuff
#endif


// ==========================================================================================
// MacOS Carbon API
// ==========================================================================================
#if defined ( PLATFORM_MACOS ) && PLATFORM_MAC_CARBON && !defined( PDG_NO_THREAD_SAFETY )

    #ifndef __MULTIPROCESSING__
    #include <Multiprocessing.h>
    #endif //__MULTIPROCESSING__

namespace pdg {

   // critical section for code that cannot be interrupted by anything
    class CriticalSection {
    public:
        CriticalSection()  { MacAPI::MPCreateCriticalRegion(&mCriticalSection); }
        ~CriticalSection() { MacAPI::MPDeleteCriticalRegion(mCriticalSection); }
        void enter()       { MacAPI::MPEnterCriticalRegion(mCriticalSection, kDurationForever); }
        void leave()       { MacAPI::MPExitCriticalRegion(mCriticalSection); }
    private:
        MacAPI::MPCriticalRegionID mCriticalSection;
    };
    
    // mutex class for things that can only be executed by one thread at a time
    class Mutex {
    public:
        Mutex()            { MacAPI::MPCreateBinarySemaphore(&mSemaphore); }
        ~Mutex()           { MacAPI::MPDeleteSemaphore(mSemaphore); }
        void aquire()      { MUTEX_DEBUG_ONLY( OS::_DOUT("Aquiring Mutex %p", &mSemaphore); )
                             MacAPI::MPWaitOnSemaphore(mSemaphore, kDurationForever); }
        bool tryAquire(int32 waitMilliseconds) { 
                MUTEX_DEBUG_ONLY( OS::_DOUT("tryAquire Mutex %p for %d ms", &mSemaphore, waitMilliseconds); )
                bool result = (MacAPI::MPWaitOnSemaphore(mSemaphore, waitMilliseconds) == noErr);
                MUTEX_DEBUG_ONLY( if (!result) { 
                    OS::_DOUT("tryAquire failed for Mutex %p after %d ms", &mSemaphore, waitMilliseconds); 
                })
                return result;
            }
        void release()     { MUTEX_DEBUG_ONLY( OS::_DOUT("Releasing Mutex %p", &mSemaphore); )
                             MacAPI::MPSignalSemaphore(mSemaphore); }
    private:
        MacAPI::MPSemaphoreID      mSemaphore;
    };
    
} // close namespace pdg
    
// ==========================================================================================
// POSIX
// ==========================================================================================
#elif defined ( PLATFORM_UNIX )  && !defined( PDG_NO_THREAD_SAFETY ) // && defined (_POSIX_THREADS)
    #include <pthread.h>
    #include <errno.h>

namespace pdg {

    // don't ever set AQUIRE_USEC_INTERVAL >= 1,000,000 or usleep() might fail
    const int32 AQUIRE_USEC_INTERVAL = 10000; // check for lock aquired every 10 milliseconds
    const int32 AQUIRE_MILLISEC_INTERVAL = AQUIRE_USEC_INTERVAL / 1000;
    
    // critical section for code that cannot be interrupted by anything
    class CriticalSection {
    public:
        CriticalSection()  { PosixAPI::pthread_mutex_init(&mCriticalSection, 0); }
        ~CriticalSection() { PosixAPI::pthread_mutex_destroy(&mCriticalSection); }
        void enter()       { PosixAPI::pthread_mutex_lock(&mCriticalSection); }
        void leave()       { PosixAPI::pthread_mutex_unlock(&mCriticalSection); }
    private:
        PosixAPI::pthread_mutex_t mCriticalSection;
    };

    // mutex class for things that can only be executed by one thread at a time
    class Mutex {
    public:
        Mutex()            { PosixAPI::pthread_mutex_init(&mMutex, 0 /*PTHREAD_MUTEX_DEFAULT*/ ); }
        ~Mutex()           { PosixAPI::pthread_mutex_destroy(&mMutex); }
        void aquire()      { MUTEX_DEBUG_ONLY( OS::_DOUT("Aquiring Mutex %p", &mMutex); )
                             PosixAPI::pthread_mutex_lock(&mMutex); }
        bool tryAquire(int32 waitMilliseconds) { 
                MUTEX_DEBUG_ONLY( OS::_DOUT("tryAquire Mutex %p for %d ms", &mMutex, waitMilliseconds); )
                while (PosixAPI::pthread_mutex_trylock(&mMutex) == EBUSY) {
                    if (waitMilliseconds <= 0) {
                        MUTEX_DEBUG_ONLY( OS::_DOUT("tryAquire failed for Mutex %p after %d ms", &mMutex, waitMilliseconds); )
                        return false;   // we couldn't get the lock before we timed out
                    } else {
                        long sleepTime;   // lock was busy, so sleep for a bit and see if we can get it later
                        if (waitMilliseconds < AQUIRE_MILLISEC_INTERVAL) {  // less than the interval remaining on our timer
                            sleepTime = waitMilliseconds * 1000;
                            waitMilliseconds = -1;      // so sleep for a precise amount and then check one last time
                        } else {
                            sleepTime = AQUIRE_USEC_INTERVAL;   // sleep for a bit and see if we can get it later
                            waitMilliseconds -= AQUIRE_MILLISEC_INTERVAL;
                        }
                        PosixAPI::usleep(sleepTime);  // note: don't call this with sleepTime >= 1,000,000 
                    }
                }
                return true;   // we got the lock
            }
        void release()     { MUTEX_DEBUG_ONLY( OS::_DOUT("Releasing Mutex %p", &mMutex); )
                             PosixAPI::pthread_mutex_unlock(&mMutex); }
    private:
        PosixAPI::pthread_mutex_t mMutex;
    };
    
} // close namespace pdg

// ==========================================================================================
// Windows
// ==========================================================================================
#elif defined ( PLATFORM_WIN32 ) && !defined( PDG_NO_THREAD_SAFETY )
    #if defined ( COMPILER_MWERKS )   // CodeWarrior won't compile windows.h with ANSI Strict on
        #pragma ANSI_strict off
    #endif
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #if defined ( COMPILER_MWERKS )
        #pragma ANSI_strict reset
    #endif

namespace pdg {

    // critical section for code that cannot be interrupted by anything
    class CriticalSection {
    public:
        CriticalSection()  { WinAPI::InitializeCriticalSection(&mCriticalSection); }
        ~CriticalSection() { WinAPI::DeleteCriticalSection(&mCriticalSection); }
        void enter()       { WinAPI::EnterCriticalSection(&mCriticalSection); }
        void leave()       { WinAPI::LeaveCriticalSection(&mCriticalSection); }
    private:
        WinAPI::CRITICAL_SECTION mCriticalSection;
    };

    // mutex class for things that can only be executed by one thread at a time
    class Mutex {
    public:
        Mutex()            { mSemaphore = WinAPI::CreateSemaphore(0, 1, 1, 0); }
        ~Mutex()           { WinAPI::CloseHandle(mSemaphore); }
        void aquire()      { MUTEX_DEBUG_ONLY( OS::_DOUT("Aquiring Mutex %p", &mSemaphore); )
                             WinAPI::WaitForSingleObject(mSemaphore, INFINITE); }
        bool tryAquire(int32 waitMilliseconds) { 
                MUTEX_DEBUG_ONLY( OS::_DOUT("tryAquire Mutex %p for %d ms", &mSemaphore, waitMilliseconds); )
                bool result = (WinAPI::WaitForSingleObject(mSemaphore, waitMilliseconds) == NO_ERROR);
                MUTEX_DEBUG_ONLY( if (!result) { 
                    OS::_DOUT("tryAquire failed for Mutex %p after %d ms", &mSemaphore, waitMilliseconds); 
                })
                return result;
            }
        void release()     { MUTEX_DEBUG_ONLY( OS::_DOUT("Releasing Mutex %p", &mSemaphore); )
                             WinAPI::ReleaseSemaphore(mSemaphore, 1, 0); }
    private:
        WinAPI::HANDLE mSemaphore;
    };
    
} // close namespace pdg

// ==========================================================================================
// Unknown platform, or no thread safety
// ==========================================================================================
#else

namespace pdg {

    // critical section for code that cannot be interrupted by anything
    class CriticalSection {
    public:
        void enter() {}
        void leave() {}
    };

    // mutex class for things that can only be executed by one thread at a time
    class Mutex {
    public:
        void aquire()      {}
        bool tryAquire(int32) { return true;}
        void release()     {}
    };

} // close namespace pdg

#endif


// ==========================================================================================
// All platforms
// ==========================================================================================

namespace pdg {

// for expection safe critical sections
class AutoCriticalSection {
public:
    AutoCriticalSection(CriticalSection* inSection) 
        : mCriticalSection(inSection) {
            if (mCriticalSection) {
                inSection->enter();
            }
        }
    ~AutoCriticalSection() {
            if (mCriticalSection) {
                mCriticalSection->leave();
            }
        }
    CriticalSection* mCriticalSection;
};

#ifndef PDG_NO_EXCEPTIONS

class timeout : public std::exception {	
public:
	virtual const char* what() const throw() { return "timeout"; }
};

// for exception safe Mutexes
class AutoMutex {
public:
    AutoMutex(Mutex* inMutex) throw()
        : mMutex(inMutex) {
            if (mMutex) { 
                inMutex->aquire();  // don't do a potentially costly tryAquire() if aquire is good enough
            }
        }
    AutoMutex(Mutex* inMutex, int32 millisecondWait) throw(timeout)
        : mMutex(inMutex) {
            if (mMutex) { 
                if (millisecondWait == FOREVER) {
                    inMutex->aquire();  // don't do a potentially costly tryAquire() if aquire is good enough
                } else if (!inMutex->tryAquire(millisecondWait)) {
                    mMutex = 0;  // to make sure that whatever happens, we don't release it again
                    throw (new timeout);
                }
            }
        }
    ~AutoMutex() {
            if (mMutex) {
                mMutex->release();
            }
        }
    Mutex* mMutex;
};

#endif // PDG_NO_EXCEPTIONS

// for exception safe Mutexes in situations where we can't throw an exception
class AutoMutexNoThrow {
public:
    AutoMutexNoThrow(Mutex* inMutex) throw() : mMutex(inMutex), mAquired(false) {}
    ~AutoMutexNoThrow() {
            if (mMutex && mAquired) {
                mMutex->release();
            }
        }
    void aquire() {
            if (mMutex) {
                mMutex->aquire();
                mAquired = true;
            }
        }
    bool tryAquire(int32 waitNanosecs) throw() {
            if (mMutex) {
                mAquired = mMutex->tryAquire(waitNanosecs);
                return mAquired;
            } else {
                return true;
            }
        }
    Mutex* mMutex;
    bool mAquired;
};

} // close namespace pdg

#endif // PDG_SYS_MUTEX_H_INCLUDED
