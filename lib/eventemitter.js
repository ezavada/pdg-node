// -----------------------------------------------
// eventemitter.js
//
// pure javascript implementation of 
// core application/system interface
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

require('./classify');
require('./pdg-defs.js');

// -----------------------------------------------------------------------------------
// EventManager
// Associates Handlers with Event Types and supports posting an event
// which is then sent to all appropriate handlers
// -----------------------------------------------------------------------------------


// TODO: hook these up to pdg.warn and pdg.debug
function warn(str) {
	console.log("WARN: "+str);
}

function debug(str) {
//	console.log("DEBUG: "+str);
}

/** Distributes events to event handlers.
 * The EventEmitter expects the game developer to implement IEventHandler subclasses
 * and register them to handle various events they are interested in.
 * @see IEventHandler
 * \ingroup Managers
 */
classify('EventEmitter', function() {

    //! constructor for new event emitter
    def('initialize', function(parentEmitter) {
    		this.mHandlers = new Array();
    		if (typeof parentEmitter != 'undefined') {
    			this.mParent = parentEmitter;
    		} else {
    			this.mParent = false;
    		}
    	});

	//! add a new handler for some event type, or for all events if no type specified
    def('addHandler', function(inHandler, inType) {
    		var handlerType = (inType) ? inType : all_events;
    		var handlers = this.mHandlers[handlerType];
    		if (!handlers) {
    			// type wasn't previously registered -- add it to mHandlers
    			handlers = new Array();
    			this.mHandlers[handlerType] = handlers;
    		}
    		handlers.push(inHandler);
    		debug("EventEmitter::addHandler: registered handler "+inHandler+
    				" for event type "+handlerType+" ["+this._getEventName(handlerType)+"]");
    	});

	/** Remove a handler for some event type, or for all events if no type specified.
	 * If the handler is listed multiple times it will only remove it once.
	 * inType == all_events doesn't work quite like you might expect. If
	 * you have registered a handler for multiple events, but not with all_events,
	 * doing removeHandler(handler, all_events) will do nothing. Basically,
	 * all_events is a special event type that matches all event types when
 	 * considering whether to invoke a handler or not.
	 * It is safe to call remove handler from within an event handler's handleEvent() call.
	 */
	def('removeHandler', function(inHandler, inType) {
    		var handlerType = (inType) ? inType : all_events;
    		var handlers = this.mHandlers[handlerType];
    		if (!handlers) {
    			warn("EventEmitter::removeHandler could not find inType "+
    					handlerType+" ["+this._getEventName(handlerType)+"] in mHandlers");
    			return;
    		}
    		var idx = handlers.lastIndexOf(inHandler);
    		if (idx == -1) {
    			warn("EventEmitter::removeHandler could not find handler "+inHandler+
    					" for event type "+ handlerType+" ["+this._getEventName(handlerType)+
    					"] in mHandlers");
    			return;
    		}
			this.handlerRemoved = true;
			handlers.handlerRemoved = true;
			handlers[idx].removed = true;
			debug("EventEmitter::removeHandler: marked handler " + inHandler +
					" for type "+ handlerType+" ["+this._getEventName(handlerType)+"] as removed");
		});

	//! remove all handlers
	def('clear', function() {
    		this.mHandlers = new Array();
		});

	//! temporarily ignore all events of a particular type, just drop them on the floor
	def('blockEvent', function(inEventType) {
    		var handlerType = (inType) ? inType : all_events;
    		var handlers = this.mHandlers[handlerType];
    		if (!handlers) {
    			// type wasn't previously registered -- add it to mHandlers so we can record the block
    			handlers = new Array();
    			this.mHandlers[handlerType] = handlers;
    		}
    		handlers.blocked = true;
		});

	//! stop ignoring events of a particular type, no recovery of previously ignored events
    def('unblockEvent', function(inEventType) {
    		var handlerType = (inType) ? inType : all_events;
    		var handlers = this.mHandlers[handlerType];
    		if (handlers) {
    			// only unblock if it actually exists
    			handlers.blocked = false;
    		}
		});

	//! post an event for immediate handling locally, or relay it to the EventManger
	// singleton to see if it can be handled by global handlers if it isn't handled locally
	// returns true if event handled
	def('postEvent', function(inEventType, inEventData, fromEmitter) {
			if (!fromEmitter) {
				fromEmitter = this;
			}
			if (!this._emitEvent(fromEmitter, inEventType, inEventData)) {
				// relay to our parent if we have one
				if (this.mParent) {
					mParent.postEvent(inEventType, inEventData, fromEmitter);
				}
			}
		});



// ==============================================================
// protected - you shouldn't need to call these directly, call postEvent instead

	def('_emitEvent', function(emitter, inEventType, inEventData) {
			var wasHandled = false;
			// --- typed handlers ---
			var handlers = this.mHandlers[inEventType];
			if (handlers) {
				if (handlers.blocked) {
					return false; // go no further with this event
				}
				for (var i = 0; i < handlers.length; i++) {
					var handler = handlers[i];
					if (!handler.removed) {
						wasHandled = handler.handleEvent(emitter, inEventType, inEventData);
						if (typeof wasHandled != 'boolean') {
							throw('Handler must return true if event handled or false if unhandled');
						}
						if (wasHandled) {
							break;
						}
					}
				}
			}
			// --- untyped handlers ---
			if (!wasHandled) {
				handlers = this.mHandlers[all_events];
				if (handlers) {
					for (var i = 0; i < handlers.length; i++) {
						var handler = handlers[i];
						wasHandled = handler.handleEvent(emitter, inEventType, inEventData);
						if (typeof wasHandled != 'boolean') {
							throw('Handler must return true if event handled or false if unhandled');
						}
						if (wasHandled) {
							break;
						}
					}
				}
			}
			this._cleanupRemovedHandlers();
			return wasHandled;
		});
	
	def('_cleanupRemovedHandlers', function() {
			if (this.handlerRemoved) {
				for (var i = 0; i < this.mHandlers.length; i++) {
					var handlers = this.mHandlers[i];
					if (handlers.handlerRemoved) {
						for (var i = 0; i < handlers.length; i++) {
							if (handlers[i].removed) {
								handlers.splice(i, 1);  // remove the handler at idx
							}
							i--;
						}
						handlers.handlerRemoved = false;
					}
				}
			}
			this.handlerRemoved = false;
		});
		
	def('_getEventName', function(eventType) {
			return ""+eventType;
		});

});

if(!(typeof exports === 'undefined')) {
    exports.EventEmitter = EventEmitter;
}