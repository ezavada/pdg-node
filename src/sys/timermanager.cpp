// -----------------------------------------------
// timermanager.cpp
// 
// Implementation of core application/system interface
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


#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/timermanager.h"
#include "pdg/sys/userdata.h"
#include "pdg/sys/events.h"
#include "pdg/sys/os.h"

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

// define the following in your build environment, or uncomment it here to get
// debug output for the core events and timers
//#define PDG_DEBUG_TIMERS

#ifndef PDG_DEBUG_TIMERS
  #define TIMER_DEBUG_ONLY(_expression)
#else
  #define TIMER_DEBUG_ONLY DEBUG_ONLY
#endif


namespace pdg {



// ==========================================================================
// Timer Manager
// ==========================================================================

void 
TimerManager::startTimer(long id, uint32 delay, bool oneShot, UserData* userData) {
    // zero is a special value, don't allow it as a timer ID
    DEBUG_ASSERT( (id != 0), "TimerMgr::startTimer illegal timer id [0]" );
    if (id == 0) return;

    TIMER_DEBUG_ONLY( 
        if (oneShot) {
            OS::_DOUT("TimerMgr::startTimer oneshot [%ld] fires in [%ld] ms", id, delay); 
        } else {
            OS::_DOUT("TimerMgr::startTimer [%ld] interval [%ld]", id, delay); 
        }
    )
    Timer* t = findTimer(id);
    if (t) {
        TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::startTimer start existing timer [%ld]", id); )
        t->freeUserData();  // don't let the old user data leak when we replace it with new user data
    } else {
	    t = new Timer;
        CHECK_NEW(t, Timer);
        TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::startTimer created new timer [%p] id [%ld]", t, id); )
        t->prev = 0;        // creating new timer, must set it up in doubly linked list
        t->next = timers;
    	if (timers) {
    		timers->prev = t;
    	}
        timers = t;
	}
	// this is stuff we do regardless of whether it was a new or reused timer
    t->id = id;
    t->userData = userData;
    t->oneshot = oneShot;
    t->interval = delay;
    t->paused = false;
    t->fire = delay + OS::getMilliseconds();
}

// make sure a timer waits at an additional delay ms before next time it fires, regardless of current interval
// if the timer is paused this simply adds additional time to it when it is unpaused, but leaves it paused
void 
TimerManager::delayTimer(long id, unsigned long delay) {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::delayTimer [%ld] delay [%ld]", id, delay); )
    if (firing.id == id) {
        TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::delayTimer [%ld] already firing, save delay", id); )
        addDelay += delay;
    } else {
        Timer* t = timers;
        while (t) {
            if (t->id == id) {
                t->fire += delay;
                TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] now fires at [%u]", t, t->id, t->fire); )
                break;  // stop looking
            }
            t = t->next;
        }
    }
}

// tell the timer not to fire until OS::getMilliseconds() => msTime
// if it was already firing after that time, the time till firing
// is left unchanged
// if the timer is paused, this will unpause it
void 
TimerManager::delayTimerUntil(long id, uint32 msTime) {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::delayTimerUntil [%ld] when [%ld]", id, msTime); )
    if (firing.id == id) {
        TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::delayTimerUntil [%ld] already firing, save delay", id); )
        delayUntil = msTime;
        addDelay = 0;   // cancel any delay that was previously set
    } else {
        Timer* t = timers;
        while (t) {
            if (t->id == id) {
                t->paused = false;
                if (t->fire < msTime) {
                    t->fire = msTime;
                    TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] now fires at [%u]", t, t->id, t->fire); )
                }
                break;  // stop looking
            }
            t = t->next;
        }
    }
}

// pause all timers (no effect on timers added after pause)
void 
TimerManager::pause() {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::pause"); )
    Timer* t = timers;
    uint32 msTime = OS::getMilliseconds();
    while (t) {
        if (!t->paused) {
            t->paused = true;
            // convert from absolute ms time to milliseconds remaining
            if (t->fire > msTime) {
                t->fire = t->fire - msTime;
            } else {
                t->fire = 0;
            }
        }
        t = t->next;
    }
	allTimersPaused = true;
}

// unpause all timers
void 
TimerManager::unpause() {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::unpause"); )
    Timer* t = timers;
    uint32 msTime = OS::getMilliseconds();
    while (t) {
        if (t->paused) {
            // convert from  milliseconds remaining to absolute ms time
            if (t->fire) {
                t->fire = msTime + t->fire;
            } else {
                t->fire = msTime;
            }
            t->paused = false;
        }
        t = t->next;
    }
	allTimersPaused = false;
}

bool 
TimerManager::isPaused() // check to see if all timers are paused
{
	return allTimersPaused;
}

// pause a particular timer
void 
TimerManager::pauseTimer(long id) {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::pauseTimer [%ld]", id); )
    Timer* t = timers;
    uint32 msTime = OS::getMilliseconds();
    while (t) {
        if ((t->id == id) && !t->paused) {
            // convert from  milliseconds remaining to absolute ms time
            if (t->fire) {
                t->fire = msTime + t->fire;
                TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] now fires at [%u]", t, t->id, t->fire); )
            } else {
                t->fire = msTime;
            }
            t->paused = true;
            break;
        }
        t = t->next;
    }
}

// unpause a particular timer
void 
TimerManager::unpauseTimer(long id) {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::pauseTimer [%ld]", id); )
    Timer* t = timers;
    uint32 msTime = OS::getMilliseconds();
    while (t) {
        if ((t->id == id) && t->paused) {
            // convert from  milliseconds remaining to absolute ms time
            if (t->fire) {
                t->fire = msTime + t->interval;
                TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] now fires at [%u]", t, t->id, t->fire); )
            } else {
                t->fire = msTime;
            }
            t->paused = false;
            break;
        }
        t = t->next;
    }
}

bool TimerManager::isTimerPaused(long id) // check to see if a particular timer is paused
{
    Timer* t = timers;
	bool retVal = false;
    while (t) {
        if (t->id == id) {
            retVal = t->paused;
            break;  // stop looking
        }
        t = t->next;
    } 
	return retVal;
}

uint32 
TimerManager::getWhenTimerFiresNext(long id) {
    // return the time (OS::getMilliseconds()) when this timer will fire next, returns timer_Never if it won't fire
    if (firing.id == id) {
        if (firing.oneshot || firing.paused) {
            return timer_Never;
        } else {
            return firing.fire + firing.interval + addDelay;  // we are firing a repeating timer, this is when it will fire next 
        }
    } else {
        Timer* t = timers;
        while (t) {
            if (t->paused) {
                return timer_Never;
            } else if (t->id == id) {
                return t->fire;  // found the timer, return when it will next fire
            }
            t = t->next;
        }
    }
    return timer_Never;
}

void 
TimerManager::cancelTimer(long id) {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::cancelTimer [%ld]", id); )
    Timer* t = timers;
    while (t) {
        if (t->id == id) {
            // timer found, remove it
            if (t == timers) {
                // we are deleted the list head, replace it
                timers = t->next;
            }
            if (t->prev) {
                // the previous was point to us, point to next instead
                t->prev->next = t->next;
            }
            if (t->next) {
                // the next was pointed to us, point to prev instead
                t->next->prev = t->prev;
            }
            if (nextid == id) { // was this the timer that was to fire next
                next = t->next;  // it was, so update the next ptr and next id
                nextid = (next) ? next->id : 0;
            }
            if ((firing.id == id) && !firing.oneshot) { // was this the timer that was firing?
                // we don't do this for oneshot timers, because a oneshot timer is removed from
                // the list before it starts firing. Therefore, even if the ids match, the firing
                // oneshot is not actually the same timer that we found here
                TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::cancelTimer [%ld] already firing, setting delete flag", id); )
                deleted = true;
                delayUntil = 0;  // don't do any extra delays
                addDelay = 0;
            }
            TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::cancelTimer timer [%p] id [%ld] deleted", t, id); )
            delete t;
            break;  // stop looking
        }
        t = t->next;
    } 
}

// remove all timers
void 
TimerManager::cancelAllTimers() {
    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::cancelAllTimers"); )
    Timer* t = timers;
    Timer* nextT = 0;
    while (t) {
        nextT = t->next;
        TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::cancelAllTimers timer [%p] id [%ld] deleted", t, t->id); )
        delete t;
        t = nextT;
    }
    if ((firing.id != 0) && !firing.oneshot) {
        // we deleted the timer currently firing, except for oneshots, which are already removed from the list
        // and so we couldn't have deleted it here
        deleted = true;
        this->next = 0; // clear the cached value for next so we don't try to use it
        delayUntil = 0; // don't do any extra delays
        addDelay = 0;
    }
    timers = 0;
}

void 
TimerManager::checkTimers() {
    Timer* t = timers;
    uint32 ms = OS::getMilliseconds();
//    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers at ms [%ld]", ms); )
	try {
		while (t) {
			deleted = false;
			addDelay = 0;       // we are about to fire this timer, so reset values for delays
			delayUntil = 0;      // to this timer added while it is firing
			if (!t->paused && (ms >= t->fire)) {
				// fire this timer				
				TimerInfo ti;
				ti.id = t->id;
				ti.millisec = ms;
				ti.msElapsed = (ms - t->fire) + t->interval;
				if (t->userData) {
					ti.userData = t->userData->getData();
				} else {
					ti.userData = 0;
				}
				firing = *t;
				TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers firing timer [%ld]", ti.id); )
				DEBUG_ONLY( uint32 behindMs = ms - t->fire; if (behindMs > 100) OS::_DOUT("TimerMgr::timer fired %ld ms late! Targeted for %u", behindMs, t->fire); )
				// cache the next timer in case of deletion
				// the deletion code can update the ptr to next if it deletes the next timer
				next = t->next;
				nextid = (next) ? next->id : 0;
				// if the timer is a one shot, remove it from the timer list immediately
				if (t->oneshot) {
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers removing oneshot timer [%ld] from list", ti.id); )
					if (t == timers) {
						// we are deleted the list head, replace it
						timers = t->next;
					}
					if (t->prev) {
						// the previous was point to us, point to next instead
						t->prev->next = t->next;
					}
					if (t->next) {
						// the next was pointed to us, point to prev instead
						t->next->prev = t->prev;
					}
					// now delete it, we will never use this pointer again
					// but before we delete it, clear the user data so we don't
					// delete that before it can be used
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers saving off oneshot timer [%p] id [%ld] user data [%p] to avoid deletion", t, t->id, t->userData); )
					t->userData = 0;
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers deleting oneshot timer [%p] id [%ld]", t, t->id); )
					delete t;
					t = 0;
				}
				try {
					if (!postEvent(eventType_Timer, &ti)) {
					    DEBUG_ONLY( OS::_DOUT("WARNING: TimerMgr::checkTimers timer [%p] id [%ld] unhandled", (void*)t, ti.id); )
					}
				}
				catch (std::exception& e) {
				    DEBUG_ONLY( OS::_DOUT("Std Exception [%s] in TimerMgr::checkTimers.eventManager.postEvent", e.what()); )
				    throw TimerException( e, "TimerMgr::checkTimers.eventManager.postEvent", ti.id );
				}
				catch (...) {
					DEBUG_ONLY( OS::_DOUT("Exception in TimerMgr::checkTimers.eventManager.postEvent"); )
					throw TimerException( "TimerMgr::checkTimers.eventManager.postEvent", ti.id );
				}
				TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers done with Timer Event"); )
				firing.id = 0;
				nextid = 0;     // the nextid is no longer valid because we are done firing
				if (firing.oneshot) {
					// we deleted this already when we removed it from the list
					// but we didn't delete the user data, so be sure to take care of that
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers freeing deleted oneshot timer user data [%p]", firing.userData); )
					firing.freeUserData();
					t = next;
					continue;   // cycle to the next timer
				} else if (deleted) { // if we deleted the timer while firing it do nothing more with it
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers detected firing timer [%ld] was deleted", ti.id); )
					t = next;    // the timer was deleted, so use the cached value for next
					// special case where we cancel, reinstall, then delay a timer all within
					// the firing handler
					if (delayUntil) {
						// need to call delayTimerUntil on the new timer
						delayTimerUntil(ti.id, delayUntil + addDelay);
					} else if (addDelay) {
						// need to call delayTimer on the new timer
						delayTimer(ti.id, addDelay);
					}
					continue;   // cycle to the next timer
				} else {
					TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers resetting timer [%ld]", ti.id); )
					// repeating timer, reset the timer to fire again after interval
					// include any additional delay added by the timer's handler
					t->fire = ms + addDelay + t->interval;
					// if we have a new firing time from a delayUntil call, we must
					// see if that would extend the firing time
					if (t->fire < delayUntil) {
						// repeating timer, reset the timer to fire at the exact time
						// specified by delayTimerUntil() any additional delay added by the timer's handler
						t->fire = delayUntil + addDelay;
					}
					TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] now fires at [%u]", t, t->id, t->fire); )
				}
			}
			t = t->next;
		}
	} catch (std::exception& e) {
		DEBUG_ONLY( OS::_DOUT("Exception [%s] in TimerMgr::checkTimers", e.what()); )
		throw e;
	} catch (...) {
		DEBUG_ONLY( OS::_DOUT("Unspecified Exception in TimerMgr::checkTimers"); )
		throw;
	}
//    TIMER_DEBUG_ONLY( OS::_DOUT("TimerMgr::checkTimers done"); )
}

uint32 
TimerManager::msTillNextFire() {
 // tells us how long it will be (in milliseconds) till the next timer fires
    Timer* t = timers;
    uint32 ms = OS::getMilliseconds();
    uint32 shortestTime = 0x7fffffff;
    while (t) {
		if (!t->paused && (ms >= t->fire)) {
            return 0;   // timer is overdue to fire
        }
        uint32 diff = t->fire - ms;
        if (diff < shortestTime) {
            shortestTime = diff;
        }
        t = t->next;
    }
    DEBUG_ASSERT(shortestTime <= 0x7fffffff, "ERROR: TimerManager::msTillNextFire() returned possible negative number");
    return shortestTime;
}

TimerManager::Timer* 
TimerManager::findTimer(long id) {
    Timer* t = timers;
    while (t) {
        if (t->id == id) {
            // timer found
			return t;
        }
        t = t->next;
    }
	return 0;
}


TimerManager::~TimerManager() {
    firing.userData = 0;    // make sure no-one tries to free the user data from the firing timer copy
    Timer* t = timers;
    // delete all the timers
    while (t) {
        Timer* nextT = t->next;
        delete t;
        t = nextT;
    }
    timers = 0;
}


TimerManager::Timer::Timer() 
 : userData(0) {
    // make sure data stuff is set up safely no matter what
}

TimerManager::Timer::~Timer() {
    freeUserData(); // make sure the user data gets freed
}

void
TimerManager::Timer::freeUserData() {
    TIMER_DEBUG_ONLY( OS::_DOUT("Timer [%p] id [%ld] freeUserData: userData [%p]", this, id, userData); )
    if (userData) {
        userData->release();
    }
    userData = 0;
}

TimerManager* TimerManager::createSingletonInstance() {
	return new TimerManager();
}




} // end namespace pdg

