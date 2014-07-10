// -----------------------------------------------
// eventmanager.cpp
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

#include "pdg/sys/eventmanager.h"
#include "pdg/sys/os.h"
#include "pdg/sys/events.h"

#ifndef PDG_NO_EVENT_QUEUE
#include "pdg/sys/mutex.h"
#endif // PDG_NO_EVENT_QUEUE

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

const char* getEventName(long eventType) {
    switch (eventType) {
    case all_events:
    	return "ALL EVENTS";
	case eventType_Startup:
	    return "eventType_Startup";
    case eventType_Shutdown:
	    return "eventType_Shutdown";
    case eventType_Timer:
	    return "eventType_Timer";
    case eventType_KeyDown:
	    return "eventType_KeyDown";
    case eventType_KeyUp:
	    return "eventType_KeyUp";
    case eventType_KeyPress:
	    return "eventType_KeyPress";
    case eventType_MouseDown:
	    return "eventType_MouseDown";
	case eventType_MouseUp:
	    return "eventType_MouseUp";
	case eventType_MouseMove:
	    return "eventType_MouseMove";
    case eventType_NetConnect:
	    return "eventType_NetConnect";
    case eventType_NetDisconnect:
	    return "eventType_NetDisconnect";
    case eventType_NetData:
	    return "eventType_NetData";
	case eventType_NetError:
	    return "eventType_NetError";
	case eventType_SoundEvent:
	    return "eventType_SoundEvent";
	case eventType_PortResized:
	    return "eventType_PortResized";
	case eventType_ScrollWheel:
		return "eventType_ScrollWheel";
	case eventType_SpriteAnimate:
		return "eventType_SpriteAnimate";
	case eventType_SpriteLayer:
		return "eventType_SpriteLayer";
	case eventType_SpriteTouch:
		return "eventType_SpriteTouch";
	case eventType_SpriteCollide:
		return "eventType_SpriteCollide";
	case eventType_MouseEnter:
		return "eventType_MouseEnter";
	case eventType_MouseLeave:
		return "eventType_MouseLeave";
	case eventType_PortDraw:
		return "eventType_PortDraw";
	}
	return "** UNKNOWN EVENT TYPE **";
}

// ==========================================================================
// Event Manager
// ==========================================================================

// EventEmitter destructor
EventManager::~EventManager() {
	// cleanup all handlers and unprocessed events
	clear();
}

bool 
EventManager::postEvent(long inEventType, void* inEventData, EventEmitter* fromEmitter) {
    if (fromEmitter == 0) {
    	fromEmitter = this;
    }
    if (mHandlers.size() == 0) {
        DEBUG_ONLY( OS::_DOUT("EventMgr::postEvent but no handlers at all have been registered"); )
        return false;
    }
    bool wasHandled = emitEvent(fromEmitter, inEventType, inEventData);
    // pretend mouse move events are always handled, so we don't overwhelm our logs
    // also don't give unhandled warnings for anything we are going to post to another
    // emitter, because they will just send back to us eventually if they don't have a
    // handler for it
    bool warnUnhandled = !wasHandled;
    if ((inEventType == eventType_MouseMove) || (inEventType == eventType_KeyUp) 
      || (inEventType == eventType_SpriteLayer) || (inEventType == eventType_PortDraw) ) {
        warnUnhandled = false;
    }
    if (warnUnhandled) {
        DEBUG_ONLY( OS::_DOUT("EventMgr::postEvent got unhandled event type [%d][%s] data [%p]", inEventType, 
                              getEventName(inEventType), inEventData); )
    }
    cleanupRemovedHandlers();
    return wasHandled;
}

bool 
EventManager::postEventToEmitter(long inEventType, void* inEventData, EventEmitter* toEmitter) {
    if ((toEmitter == 0) || (toEmitter == this)) {
    	return postEvent(inEventType, inEventData);
	} else {
        bool wasHandled = toEmitter->emitEvent(toEmitter, inEventType, inEventData);
        toEmitter->cleanupRemovedHandlers();
        return wasHandled;
    }
}

void 
EventManager::clear(bool doRelease) {
    EventEmitter::clear(doRelease);
#ifndef PDG_NO_EVENT_QUEUE
    AutoMutex mutex(&mEventQueueMutex);
    DEBUG_ONLY( if (mEventQueue.size() != 0) OS::_DOUT("WARNING: unhandled events in queue when EventMgr::clear() called"); )
    while (mEventQueue.size() > 0) {
        EventQueueEntry& evt = mEventQueue.front();
        // release the user data if there was any
        if (evt.userData) {
        	evt.userData->release();
        	evt.userData = 0;
        }
        mEventQueue.pop();
    }
#endif
}

void 
EventManager::blockEvent(long inEventType) {
	EventEmitter::blockEvent(inEventType);
// TODO: block other singleton emitters such as NetworkManager and SoundManager?
}

void 
EventManager::unblockEvent(long inEventType) {
	EventEmitter::unblockEvent(inEventType);
// TODO: unblock other singleton emitters such as NetworkManager and SoundManager?
}

#ifndef PDG_NO_EVENT_QUEUE
void 
EventManager::enqueueEvent(long inEventType, UserData* inEventData, EventEmitter* inEmitter) {
 // mutexed for posting events between threads
    AutoMutex mutex(&mEventQueueMutex);
    DEBUG_ASSERT(inEventData != 0, "bad event data ptr");
    EventQueueEntry entry(inEventType, inEventData, inEmitter);
    mEventQueue.push(entry);
}

bool 
EventManager::getQueuedEvent(long& outEventType, UserData*& outEventData, EventEmitter*& outEmitter) {
 // mutexed for posting events between threads
    AutoMutex mutex(&mEventQueueMutex);
    if (mEventQueue.size() == 0) {
        return false;
    } else {
        EventQueueEntry& evt = mEventQueue.front();
        outEventType = evt.eventType;
        outEventData = evt.userData;
        outEmitter = evt.emitter;
        mEventQueue.pop();
        return true;
    }
}
#endif // PDG_NO_EVENT_QUEUE

EventManager* EventManager::createSingletonInstance() {
	return new EventManager();
}



} // end namespace pdg

