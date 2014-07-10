// -----------------------------------------------
// ieventhander.h
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


#ifndef PDG_IEVENTHANDLER_H_INCLUDED
#define PDG_IEVENTHANDLER_H_INCLUDED

#include "pdg_project.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

class EventEmitter;

// -----------------------------------------------------------------------------------
// IEventHandler interface
// -----------------------------------------------------------------------------------
/* Interface for any class which handles events.
 * Specific Subclasses should be written to implement particular handlers.
 *
 * For those coding in Ruby, there is an implementation of IEventHandler called
 * PDG::RubyEventHandler that manages translating events from C++ to Ruby. This lets
 * you write an event handlers in Ruby and register them with the Pixel Dust Framework
 * as if they were C++ objects.
 * The functionality of PDG::RubyEventHandler is identical to the pdg::IEventHandler
 * interface defined below, but the handleEvent() method is instead named handle_event()
 * in RubyEventHandler. (RubyEventHandler::handleEvent() is implemented in C++ and is
 * what handles the translation of events into Ruby and calls the derived handle_event()
 * method.)
 *
 * For those coding in Javascript, there is an implementation of IEventHandler 
 * that maps a function definition to the handleEvent call. So to create a handler:
 *
 *      var handler = new pdg.IEventHandler(function(event) {
 *            console.log("in my event handler with "+event);
 *      });
 *
 * \ingroup Events
 */
class IEventHandler  {
public:
	//! indicate that an additional reference to this object has been given out
	virtual void addRef() const throw() {} // = 0;
	//! a reference to this object is no longer used, free self if nothing references it anymore
	virtual void release() const throw() {} // = 0;
    /** <b>handle_event</b>(inEventType, inEventData) in Ruby - Developer implements to handle one or more types of events.
	 * The developer using the Pixel Dust Framework must implement this method
	 * to handle events. Each handler is invoked only for the event types it was
	 * actually registered for using EventManager::addHandler(). You can register
	 * a handler multiple times for different events, or register to receive all
	 * events.
	 * @warning You must catch any exceptions in handleEvent, it cannot throw exceptions.
	 * @param inEventType the type of event you are expected to handle
	 *       <br>(ie: pdg::eventType_Startup, pdg::eventType_Shutdown, etc...) <i>[C++]</i>
	 *       <br>(PDG::EventType_Startup, PDG::EventType_Shutdown, etc..) <i>[Ruby]</i>
	 * @param inEventData the data for the event you are expected to handle
	 * @return you should true from your handleEvent method if the event has been completely handled and no other
	 *       handlers should receive it, or false if other handlers should still get an opportunity to handle it
     * @see EventManager
	 */
    virtual bool handleEvent(EventEmitter* inEmitter, long inEventType, void* inEventData) throw() { return false; } // = 0;  // return true if completely handled

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mIEventHandlerScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
	IEventHandler() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mIEventHandlerScriptObj);
    	#endif
	}

    virtual ~IEventHandler() {
				#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
					CleanupIEventHandlerScriptObject(mIEventHandlerScriptObj);
				#endif
			}
/// @endcond
};




} // end namespace pdg


#endif // PDG_IEVENTHANDLER_H_INCLUDED

