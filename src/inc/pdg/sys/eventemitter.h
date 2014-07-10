// -----------------------------------------------
// eventemitter.h
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


#ifndef PDG_EVENT_EMITTER_H_INCLUDED
#define PDG_EVENT_EMITTER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include <map>
#include <list>

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

class IEventHandler;
class EventManager;
	
// -----------------------------------------------------------------------------------
// EventEmitter
// Associates Handlers with Event Types and supports posting an event
// which is then sent to all appropriate handlers
// -----------------------------------------------------------------------------------

enum { all_events = 0 };

	
/** Distributes events to event handlers.
 * The EventEmitter expects the game developer to implement IEventHandler subclasses
 * and register them to handle various events they are interested in.
 * @see IEventHandler
 * \ingroup Managers
 */
class EventEmitter {
friend class EventManager;
public:

	//! add a new handler for some event type, or for all events if no type specified
    virtual void addHandler(IEventHandler *inHandler, long inType = all_events);

	/** Remove a handler for some event type, or for all events if no type specified.
	 * If the handler is listed multiple times it will only remove it once.
	 * inType == all_events doesn't work quite like you might expect. If
	 * you have registered a handler for multiple events, but not with all_events,
	 * doing removeHandler(handler, all_events) will do nothing. Basically,
	 * all_events is a special event type that matches all event types when
 	 * considering whether to invoke a handler or not.
	 * It is safe to call remove handler from within an event handler's handleEvent() call.
	 */
	virtual void removeHandler(IEventHandler *inHandler, long inType = all_events);

	//! remove all handlers
	// if doRelease is false then release is not called on handlers
	virtual void clear(bool doRelease = true);

	//! temporarily ignore all events of a particular type, just drop them on the floor
	virtual void blockEvent(long inEventType);
	//! stop ignoring events of a particular type, no recovery of previously ignored events
    virtual void unblockEvent(long inEventType);

// lifecycle
    //! constructor for new event emitter
    EventEmitter() : mHandlers(), mHandlerRemoved(false) {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mEventEmitterScriptObj);
    	#endif
    };
/// @cond C++
    virtual ~EventEmitter();
/// @endcond

	//! post an event for immediate handling locally, or relay it to the EventManger
	// singleton to see if it can be handled by global handlers if it isn't handled locally 
	// returns true if event handled
	virtual bool postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter = 0);

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mEventEmitterScriptObj;
#endif

protected:

/// @cond INTERNAL   
	virtual bool emitEvent(EventEmitter* emitter, long inEventType, void* inEventData); // returns true if event handled

	virtual void cleanupRemovedHandlers();

	struct EventHandlerInfo {
		IEventHandler*  handler;
		bool			removed;
		EventHandlerInfo(IEventHandler *inHandler) : handler(inHandler), removed(false) {}
		EventHandlerInfo() : handler(), removed(false) {}
	};
	typedef std::list<EventHandlerInfo> HandlerListT;
	struct EventHandlingInfo {
		HandlerListT    handlerList;
		bool            blocked;
		bool			handlerRemoved;
		EventHandlingInfo(HandlerListT lst) : handlerList(lst), blocked(false), handlerRemoved(false) {}
		EventHandlingInfo() : handlerList(), blocked(false), handlerRemoved(false) {}
	};
	typedef std::map<long, EventHandlingInfo> HandlerMapT;

    HandlerMapT     mHandlers;
	bool			mHandlerRemoved;
/// @endcond
};

} // end namespace pdg


#endif // PDG_EVENT_MANAGER_H_INCLUDED

