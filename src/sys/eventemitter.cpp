// -----------------------------------------------
// eventemitter.cpp
// 
// Implementation of core application/system interface
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


#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/core.h"
#include "pdg/sys/os.h"

#include <typeinfo>

// define the following in your build environment, or uncomment it here to get
// debug output for the core events and timers
//#define PDG_DEBUG_EVENTS

#ifndef PDG_DEBUG_EVENTS
  // if the network layer is not being debugged, we ignore all 
  #define EVENT_DEBUG_ONLY(_expression)
#else
  #define EVENT_DEBUG_ONLY DEBUG_ONLY
#endif

namespace pdg {


// ==========================================================================
// Event Emitter
// ==========================================================================


// EventEmitter destructor
EventEmitter::~EventEmitter() {
	// cleanup all handlers and release them
	clear();
}

// EventManger methods
void 
EventEmitter::addHandler(IEventHandler *inHandler, long inType) {
    EVENT_DEBUG_ONLY( OS::_DOUT("Begin registering handler %p for event type %d [%s]", inHandler, 
    			inType, getEventName(inType)); )
    if (inHandler == NULL) {
        DEBUG_ONLY( OS::_DOUT("Attempt to register NULL handler for event type %d [%s]",
        			inType, getEventName(inType)); )
        return;
    }
    // see if this type is already registered
    HandlerMapT::iterator it = mHandlers.find(inType);
    if (it == mHandlers.end()) {
        // new type
        HandlerListT list;
        list.push_back(inHandler);
        mHandlers.insert( HandlerMapT::value_type(inType, EventHandlingInfo(list)) );
    } else {
        DEBUG_ONLY(
			HandlerListT& list = (*it).second.handlerList;
			HandlerListT::iterator listIt = list.begin();
            while (listIt != list.end()) {
            	IEventHandler* handler = (*listIt).handler;
                if (handler == inHandler) {
                    OS::_DOUT("addHandler: handler %p is already registered for event type %d [%s]",
                    		inHandler, inType, getEventName(inType));
                }
                DEBUG_ASSERT(handler != inHandler, "EventEmitter::addHandler Added same handler twice for same event type");
                ++listIt;
            }
        )
        // established type, so add it to the list
        (*it).second.handlerList.push_back(inHandler);
    }
    EVENT_DEBUG_ONLY( OS::_DOUT("Done registering handler %p for event type %d [%s]", inHandler, 
    		inType, getEventName(inType)); )
    inHandler->addRef();
    EVENT_DEBUG_ONLY( OS::_DOUT("Add ref'd handler %p", inHandler); )
}


// inType == all_events doesn't work quite like you might expect. If
// you have registered a handler for multiple events, but not with all_events,
// doing removeHandler(handler, all_events) will do nothing. Basically,
// all_events is a special event type that matches all event types when
// considering whether to invoke a handler or not.
// It is safe to call remove handler from within a handleEvent() call
void 
EventEmitter::removeHandler(IEventHandler *inHandler, long inType) {
    if (inHandler == NULL) {
        DEBUG_ONLY( OS::_DOUT("EventEmitter::removeHandler *inHandler was NULL"); )
        return;
    }
    // see if this type is already registered
    HandlerMapT::iterator it = mHandlers.find(inType);
    if (it == mHandlers.end()) {
        DEBUG_ONLY( OS::_DOUT("EventEmitter::removeHandler could not find inType [%d] [%s] in mHandlers", 
        	inType, getEventName(inType)); )
    } else {
        // found the type, remove the handler from the list
        HandlerListT& list = (*it).second.handlerList;
        HandlerListT::iterator listIt = list.begin();
        while (listIt != list.end()) {
            IEventHandler* handler = (*listIt).handler;
            if (handler == inHandler) {
                (*listIt).removed = true;            // mark it removed
				(*it).second.handlerRemoved = true;  // mark the map for this type as having a dead handler
				mHandlerRemoved = true;              // mark that we need to cleanup removed handlers
				handler->release();				     // we add ref'd when we put it in, release it now -- must do immediately in case this removeHandler is called from a destructor
				(*listIt).handler = 0;				 // don't reuse it now that we have released it
				EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::removeHandler: marked as removed handler [%p]", handler);)
				break;
            }
            ++listIt;
        }
    }
}

bool 
EventEmitter::postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter) {
    EVENT_DEBUG_ONLY( OS::_DOUT("EventEmitter::postEvent event type [%d][%s] with data [%p]", inEventType, 
                                getEventName(inEventType), inEventData); )
	if (fromEmitter == 0) {
		fromEmitter = this;
	}
	bool wasHandled = emitEvent(fromEmitter, inEventType, inEventData);
	if (!wasHandled) {
        EVENT_DEBUG_ONLY( OS::_DOUT("EventEmitter::postEvent event [%d][%s] unhandled, passing to EventManager", 
        					inEventType,  getEventName(inEventType)); )
		wasHandled = EventManager::getSingletonInstance()->postEvent(inEventType, inEventData, fromEmitter);
	}
	return wasHandled;
}

bool 
EventEmitter::emitEvent(EventEmitter* emitter, long inEventType, void* inEventData) {
    if (inEventType == all_events) {
        DEBUG_ONLY( OS::_DOUT("EventEmitter::emitEvent ignoring attempt to post illegal event of type all_events"); )
        return false;
    }
    if (mHandlers.size() == 0) {
        EVENT_DEBUG_ONLY( OS::_DOUT("EventEmitter::emitEvent but no handlers at all have been registered"); )
        return false;
    }
    EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::emitEvent: posting event type [%d][%s] with data [%p]", inEventType, 
                                getEventName(inEventType), inEventData);)
    bool wasHandled = false;
    // --- typed handlers ---
    HandlerMapT::iterator it = mHandlers.find(inEventType);
    if (it != mHandlers.end()) {
        // found a list of handlers for this type
        if ((*it).second.blocked) {
            DEBUG_ONLY( OS::_DOUT("EventEmitter::emitEvent got blocked event type [%d][%s] data [%p], IGNORING",
                        inEventType, getEventName(inEventType), inEventData); )
            return false; // go no further with this event
        } else {
            HandlerListT& list = (*it).second.handlerList;
            if (list.size() > 0) {
                HandlerListT::reverse_iterator listIt = list.rbegin();
                while (listIt != list.rend() && !wasHandled) {
                    IEventHandler* handler = (*listIt).handler;
					bool removed = (*listIt).removed;
					listIt++;
					if (handler && !removed) {
                        EVENT_DEBUG_ONLY( OS::_DOUT("EventEmitter::emitEvent: trying typed handler [%p] [%s]", 
                        	handler, typeid_name(handler)); )
				        wasHandled = handler->handleEvent(emitter, inEventType, inEventData);
					}
                }
            }
        }
    }
    // --- untyped handlers ---
    EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::emitEvent: all typed handlers tried: %s", wasHandled ? "handled" : "not handled");)
    if (!wasHandled) {
        // no need to check for blocked events here, blocked events always have a handler entry
        // in the type map, even if there are no handlers assigned to that type, so they
        // will always be dealt with in the typed handlers section
        EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::emitEvent: event still not handled, trying untyped handlers");)
        it = mHandlers.find(all_events);
        if (it != mHandlers.end()) {
            // we have a list of handlers interested in all events
            HandlerListT& list = (*it).second.handlerList;
            if (list.size() > 0) {
                HandlerListT::reverse_iterator listIt = list.rbegin();
                while (listIt != list.rend() && !wasHandled) {
                    IEventHandler* handler = (*listIt).handler;
					bool removed = (*listIt).removed;
					++listIt;
					if (handler && !removed) {
                        EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::emitEvent: trying untyped handler [%p]", handler);)
				        wasHandled = handler->handleEvent(emitter, inEventType, inEventData);
					}
                }
            }
        }
    	EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::emitEvent: all untyped handlers tried: %s", wasHandled ? "handled" : "not handled");)
    }
	cleanupRemovedHandlers();
	return wasHandled;
}    
    
void 
EventEmitter::cleanupRemovedHandlers() {
	if (mHandlerRemoved) {
		EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::cleanupRemovedHandlers: some handlers where removed, doing cleanup");)
		HandlerMapT::iterator it = mHandlers.begin();
		while (it != mHandlers.end()) {
			if ((*it).second.handlerRemoved) {
				HandlerListT& list = (*it).second.handlerList;
				HandlerListT::iterator listIt = list.begin();
				while (listIt != list.end()) {
					HandlerListT::iterator oldListIt = listIt;
					++listIt;
					if ((*oldListIt).removed) {
						DEBUG_ONLY( 
						    IEventHandler* handler = (*oldListIt).handler;
							DEBUG_ASSERT( handler == 0, "handler in list should have been set to 0 by removeHandler()" );
						)
						EVENT_DEBUG_ONLY(
							OS::_DOUT("EventEmitter::cleanupRemovedHandlers: cleanup removed handler");
						)
						list.erase(oldListIt);
					}
				}
				(*it).second.handlerRemoved = false;
			}
			it++;
		}
		mHandlerRemoved = false;
	}
}

void 
EventEmitter::clear(bool doRelease) {
    if (doRelease) {
		HandlerMapT::iterator it = mHandlers.begin();
		while (it != mHandlers.end()) {
			// iterate through entire list releasing references to handlers
			HandlerListT& list = (*it).second.handlerList;
			HandlerListT::iterator listIt = list.begin();
			while (listIt != list.end()) {
				IEventHandler* handler = (*listIt).handler;
				if (handler) {
					handler->release();				     // we add ref'd when we put it in, release it now -- must do immediately in case this removeHandler is called from a destructor
					(*listIt).handler = 0;				 // don't reuse it now that we have released it
					EVENT_DEBUG_ONLY(OS::_DOUT("EventEmitter::clear: removed handler [%p]", handler);)
				}
				++listIt;
			}
			++it;
		}
    } else {
    	DEBUG_ONLY( if (mHandlers.size() != 0) 
    		OS::_DOUT("WARNING: handlers in list when EventMgr::clear(dontRelease) called"); )
	}
    // now free all the list data
    mHandlers.clear();
}

void 
EventEmitter::blockEvent(long inEventType) {
    if (inEventType == all_events) {
        DEBUG_ONLY( 
            OS::_DOUT("ERROR: EventEmitter::blockEvent all_events is not an acceptable type"); 
            OS::_DEBUGGER("LOGIC ERROR");
        ) 
        return;
    }
    // temporarily ignore all events of a particular type
    DEBUG_ONLY( OS::_DOUT("EventEmitter::blockEvent for event type [%d][%s]", inEventType, 
                            getEventName(inEventType)); )
    HandlerMapT::iterator it = mHandlers.find(inEventType);
    if (it != mHandlers.end()) {
        // found a list of handlers for this type
        EventHandlingInfo& info = (*it).second;
        info.blocked = true;
    } else {
        // no list of handlers found, create a new one and tag it as blocked
        EventHandlingInfo newInfo;
        newInfo.blocked = true;
        mHandlers.insert( HandlerMapT::value_type(inEventType, newInfo) );
    }
}

void 
EventEmitter::unblockEvent(long inEventType) {
    if (inEventType == all_events) {
        DEBUG_ONLY( 
            OS::_DOUT("EventEmitter: EventMgr::unblockEvent all_events is not an acceptable type"); 
            OS::_DEBUGGER("LOGIC ERROR");
        )
        return;
    }
    // stop ignoring events of a particular type
    DEBUG_ONLY( OS::_DOUT("EventEmitter::unblockEvent for event type [%d][%s]", inEventType, 
                            getEventName(inEventType)); )
    
    HandlerMapT::iterator it = mHandlers.find(inEventType);
    if (it != mHandlers.end()) {
        // found a list of handlers for this type
        EventHandlingInfo& info = (*it).second;
        info.blocked = false;
    } else {
        DEBUG_ONLY( OS::_DOUT("WARNING: unblockEvent for event type [%d][%s] with no handlers",
                    inEventType, getEventName(inEventType)); )
    }
}



} // end namespace pdg

