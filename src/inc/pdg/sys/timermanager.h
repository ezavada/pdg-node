// -----------------------------------------------
// timermanager.h
//
// timer handling
//
// Written by Ed Zavada, 2004-2012
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


#ifndef PDG_TIMER_MANAGER_H_INCLUDED
#define PDG_TIMER_MANAGER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/platform.h"
#include "pdg/sys/global_types.h"
#include "pdg/sys/singleton.h"
#include "pdg/sys/eventemitter.h"

#include <exception>
#include <cstdio>

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

namespace pdg {

class UserData;

// -----------------------------------------------------------------------------------
// TimerManager
// Setup a new timer that will fire at a particular time, or fire repeatedly
// -----------------------------------------------------------------------------------

enum { timer_OneShot = true, timer_Repeating = false, timer_Never = 0xffffffff };

// rethrow an exception that is thrown when a timer fires
// but with added information about the type of exception
class TimerException : public std::exception {
public:
	TimerException(std::exception& e, const char* loc, long id) : timerID(id) { describe(e.what(), loc, id); }
	TimerException(const char* loc, long id) : timerID(id) { describe("", loc, id); }
	virtual const char* what() const throw() { return description; }
	long getTimerId() { return timerID; }
private:
    void describe(const char* whatStr, const char* locStr, long idVal) {
        std::snprintf(description, 256, "Timer [%ld] Exception (%s) @ [%s]", idVal, whatStr, locStr);
    }
    char description[256];
	long timerID;
};

/** Manage timers that can fire at a particular time, or fire repeatedly.
 * All TimerManager functions are safe to perform while a timer is firing.
 * Functions that alter a timer can be safely used even from within the
 * handler of the timer being changed (ie: while the target timer is in
 * the process of firing)
 * \ingroup Managers
 */
class TimerManager : public EventEmitter, public Singleton<TimerManager> {
friend class Singleton<TimerManager>;
public:
	//! start new timer that will fire after delay ms
    void startTimer(long id, uint32 delay, bool oneShot = true, UserData* userData = 0);
    //! remove a timer
	void cancelTimer(long id);
	//! remove all timers
    void cancelAllTimers();
	//! make sure a timer waits at least delay ms before next time it fires, regardless of current interval
    void delayTimer(long id, uint32 delay);
	//! tell the timer not to fire until OS::getMilliseconds() => msTime
    void delayTimerUntil(long id, uint32 msTime);
	//! pause all timers (no effect on timers added after pause)
    void pause();
	//! unpause all timers
    void unpause();
	//! check to see if all timers are paused
	bool isPaused();
	//! pause a particular timer
    void pauseTimer(long id);
	//! unpause a particular timer
    void unpauseTimer(long id);
	//! check to see if a particular timer is paused
	bool isTimerPaused(long id);
	//! return the time (OS::getMilliseconds()) when this timer will fire next, returns timer_Never if it won't fire
    uint32 getWhenTimerFiresNext(long id);

	//! fire any timers that are ready to fire
    void checkTimers();

	//! tells us how long it will be (in milliseconds) till the next timer fires
    uint32 msTillNextFire();

// lifecycle
/// @cond C++
    ~TimerManager();
/// @endcond

protected:
/// @cond INTERNAL
	static TimerManager* createSingletonInstance();

	//! create a new timer manager that posts timer events to a particular event manager
    TimerManager()	// call TimerManger::getSingletonInstance instead
     : timers(0), firing(), nextid(0), next(0), 
       allTimersPaused(false) { firing.id = 0; }

    struct Timer {
        long id; // id of the timer
        uint32 fire; // when the timer will fire (or time remaining if paused)
        uint32 interval; // how often this timer fires
        UserData* userData; // any data the user may have associated with this timer
        bool oneshot;   // true if this should be deleted after firing
        bool paused;    // true if this timer is paused
        Timer* next;    // next Timer in the list
        Timer* prev;    // prev Timer in the list
        Timer();
        ~Timer();
        void freeUserData();
    };

	Timer* findTimer(long id);

	Timer* timers;              // doubly linked list of timers
    Timer firing;    // a copy of the timer currently being fired
    long    nextid;     // id of the next timer to fire, only valid when firing.id != 0
    Timer*  next;       // this will be the next timer to fire, only valid when firing.id != 0
    bool deleted;     // did we cancel the timer that is firing from within its handler?
    uint32 addDelay;  // addition delay added to timer that is firing from within its handler
    uint32 delayUntil; // new time when current timer is supposed to fire
	bool allTimersPaused;
/// @endcond
};

} // end namespace pdg


#endif // PDG_TIMER_MANAGER_H_INCLUDED

