// -----------------------------------------------
// eventmanager.h
//
// core application/system interface
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


#ifndef PDG_EVENT_MANAGER_H_INCLUDED
#define PDG_EVENT_MANAGER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/eventemitter.h"
#include "pdg/sys/singleton.h"
#include "pdg/sys/userdata.h"

// define PDG_NO_EVENT_QUEUE in your build environment if you don't
// need multithreaded safe event queues
#ifndef PDG_NO_EVENT_QUEUE
#include <queue>
#include "pdg/sys/mutex.h"
#endif // PDG_NO_EVENT_QUEUE

//! \defgroup Managers
//! Collection of classes, types and constants that are the main points of interface to the Pixel Dust Game Framework

namespace pdg {

class UserData;	

// -----------------------------------------------------------------------------------
// EventManager
// Associates Handlers with Event Types and supports posting an event
// which is then sent to all appropriate handlers
// -----------------------------------------------------------------------------------

/** Distributes events to event handlers.
 * The EventManager is the core of the Pixel Dust Games Framework. All inputs to your game
 * arrive as events via the Event Manager. This includes network messages and system level
 * notifications in addition to user inputs such as keystrokes and mouse movements.<br>
 * The EventManager expects the game developer to implement IEventHandler subclasses
 * and register them to handle various events they are interested in.
 * @see IEventHandler
 * \ingroup Managers
 */

class EventManager : public EventEmitter, public Singleton<EventManager> {
friend class Singleton<EventManager>;
public:

	//! remove all handlers and also removes unprocessed enqueued events
	// if doRelease is false then release is not called on handlers
	virtual void clear(bool doRelease = true);

	//! temporarily ignore all events of a particular type, just drop them on the floor
	virtual void blockEvent(long inEventType);
	
	//! stop ignoring events of a particular type, no recovery of previously ignored events
    virtual void unblockEvent(long inEventType);

	//! post an event for handling, you probably don't need to do this directly
	// returns true if event handled
	virtual bool postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter = 0);

	//! post an event for handling but give another emitter first chance at handing it
	// you probably don't need to do this directly
	// returns true if event handled
	virtual bool postEventToEmitter(long inEventType, void* inEventData, EventEmitter* toEmitter);

#ifndef PDG_NO_EVENT_QUEUE
    //! mutexed for posting events between threads, makes copy of the data
    void enqueueEvent(long inEventType, UserData* inEventData, EventEmitter* inEmitter);
    
	//! mutexed for posting events between threads, must call std::free() on the data
    //! returns false when there are no more events in the queue
    bool getQueuedEvent(long& outEventType, UserData*& outEventData, EventEmitter*& outEmitter);
  #endif // PDG_NO_EVENT_QUEUE

// lifecycle
/// @cond C++
    virtual ~EventManager();
/// @endcond

protected:
/// @cond INTERNAL
	static EventManager* createSingletonInstance();
    //! call EventManager::getSingletonInstance instead
    EventManager() {};

  #ifndef PDG_NO_EVENT_QUEUE
	struct EventQueueEntry {
		long			eventType;
		UserData*   	userData;
		EventEmitter*	emitter;
		EventQueueEntry(long inEventType, UserData* inUserData, EventEmitter* inEmitter) 
			: eventType(inEventType), userData(inUserData), emitter(inEmitter) {}
		EventQueueEntry() : eventType(0), userData(0), emitter(0) {}
	};
	typedef std::queue<EventQueueEntry> EventQueueT;

    EventQueueT     mEventQueue;
    Mutex           mEventQueueMutex;
  #endif // PDG_NO_EVENT_QUEUE
/// @endcond
};

const char* getEventName(long eventType);

} // end namespace pdg


#endif // PDG_EVENT_MANAGER_H_INCLUDED

