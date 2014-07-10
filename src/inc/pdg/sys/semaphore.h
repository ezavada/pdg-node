// -----------------------------------------------
// semaphore.h
//
// Provides platform independent semaphore classes.
//
// This semphore is a binary flag that is signaled (set true) when
// a certain condition is met. Other threads that are blocked waiting
// for that condition are then unblocked.
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


#ifndef PDG_SEMAPHORE_H_INCLUDED
#define PDG_SEMAPHORE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/global_types.h"

const int32 FOREVER = 0x7fffffff;

// ==========================================================================================
// MacOS Carbon API
// ==========================================================================================
#if defined ( PLATFORM_MACOS ) && PLATFORM_MAC_CARBON && !defined( PDG_NO_THREAD_SAFETY )

    #ifndef __MULTIPROCESSING__
    #include <Multiprocessing.h>
    #endif //__MULTIPROCESSING__
    
namespace pdg {

    // mutex class for things that can only be executed by one thread at a time
    class Semaphore {
    public:
        Semaphore()        { MacAPI::MPCreateSemaphore(1, 0, &mSemaphore); }
        ~Semaphore()       { MacAPI::MPDeleteSemaphore(mSemaphore); }
        void awaitSignal() { MacAPI::MPWaitOnSemaphore(mSemaphore, FOREVER); }
        bool awaitSignal(int32 waitMilliseconds) { // return true if signal was set, false if timeout
                return (MacAPI::MPWaitOnSemaphore(mSemaphore, waitMilliseconds) == 0); // noErr
            }
        void signal()      { MacAPI::MPSignalSemaphore(mSemaphore); }
    private:
        MacAPI::MPSemaphoreID      mSemaphore;
    };

} // close namespace pdg

// ==========================================================================================
// POSIX
// ==========================================================================================
#elif defined ( PLATFORM_UNIX ) && !defined( PDG_NO_THREAD_SAFETY ) // && defined (_POSIX_THREADS)
    #include <pthread.h>
    #include <sys/time.h>
    #include <unistd.h>

namespace pdg {

    // semaphore class for having threads block waiting for a signal from other threads
    class Semaphore {
    public:
        Semaphore() { 
                    PosixAPI::pthread_cond_init(&mCondition, 0 /*PTHREAD_CONDITION_DEFAULT*/ ); 
                    PosixAPI::pthread_mutex_init(&mMutex, 0 /*PTHREAD_MUTEX_DEFAULT*/); 
                    signaled = false;
               }
        ~Semaphore() { 
                    PosixAPI::pthread_cond_destroy(&mCondition); 
                    PosixAPI::pthread_mutex_destroy(&mMutex); 
               }
        void awaitSignal() { 
                    PosixAPI::pthread_mutex_lock(&mMutex); 
                    if (!signaled) { 
                        PosixAPI::pthread_cond_wait(&mCondition, &mMutex); // unlocks the mutex while we wait
                    }  // mutex is locked again when we exit pthread_cond_wait
                    signaled = false; 
                    PosixAPI::pthread_mutex_unlock(&mMutex); 
               }
        bool awaitSignal(int32 waitMilliseconds) { // returns true if signal was set, false if timed out
                    PosixAPI::pthread_mutex_lock(&mMutex); 
                    bool result = signaled;
                    if (!signaled) {
                        // convert from milliseconds to wait to absolute timespec
                        struct timespec ts;
                        struct timeval tp;
                        gettimeofday(&tp, 0);
                        tp.tv_usec += (waitMilliseconds % 1000) * 1000;
                        if (tp.tv_usec > 1000000) {
                            tp.tv_usec -= 1000000;
                            tp.tv_sec++;
                        }
                        ts.tv_sec   = tp.tv_sec + (waitMilliseconds/1000);
                        ts.tv_nsec  = tp.tv_usec * 1000;
                        PosixAPI::pthread_cond_timedwait(&mCondition, &mMutex, &ts); // unlocks the mutex while we wait
                        result = signaled;   // mutex is locked again when we exit pthread_cond_timedwait
                    }
                    signaled = false; 
                    PosixAPI::pthread_mutex_unlock(&mMutex); 
                    return result;
                }
        void signal() { 
                    PosixAPI::pthread_mutex_lock(&mMutex); 
                    signaled = true;
                    PosixAPI::pthread_cond_signal(&mCondition); 
                    PosixAPI::pthread_mutex_unlock(&mMutex); 
                }
    private:
        PosixAPI::pthread_mutex_t mMutex;   // need both a mutex and a condition on pthreads
        PosixAPI::pthread_cond_t mCondition;
        bool signaled;
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
    #undef NOMINMAX
    #define NOMINMAX
    #include <windows.h>
    #if defined ( COMPILER_MWERKS )
        #pragma ANSI_strict reset
    #endif

namespace pdg {

    // semaphore class for having threads block waiting for a signal from other threads
    class Semaphore {
    public:
        Semaphore()         { mSemaphore = WinAPI::CreateSemaphore(0, 0, 1, 0); } // max count = 1, initial = 0
        ~Semaphore()        { WinAPI::CloseHandle(mSemaphore); }
        void awaitSignal()  { WinAPI::WaitForSingleObject(mSemaphore, INFINITE); } // wait until count > 0, count -= 1
        bool awaitSignal(int32 waitMilliseconds) {  // returns true if signal was set, false if timeout
                return (WinAPI::WaitForSingleObject(mSemaphore, waitMilliseconds) == NO_ERROR);
            }
        void signal()       { WinAPI::ReleaseSemaphore(mSemaphore, 1, 0); } // add one to the count, up to max
    private:
        WinAPI::HANDLE mSemaphore;
    };
    
} // close namespace pdg

// ==========================================================================================
// Unknown platform, or no thread safety
// ==========================================================================================
#else

namespace pdg {

    // semaphore class for having threads block waiting for a signal from other threads
    class Semaphore {
    public:
        void awaitSignal()      {}
        bool awaitSignal(int32) { return true;}
        void signal()     {}
    };
    
} // close namespace pdg

#endif



#endif // PDG_SEMAPHORE_H_INCLUDED
