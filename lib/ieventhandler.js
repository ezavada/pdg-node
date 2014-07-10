// -----------------------------------------------
// ieventhandler.js
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

// -----------------------------------------------------------------------------------
// EventHandler
// Wrapper for callbacks to handle events
// -----------------------------------------------------------------------------------

/** Handles event from EventEmitters.
 * The EventEmitter expects the game developer to implement IEventHandler subclasses
 * and register them to handle various events they are interested in.
 * @see EventManager
 * \ingroup Events
 */
classify('IEventHandler', function() {

    //! constructor for new event emitter
    def('initialize', function(func) {
    		this.func = func;
    	});

	//! handle an event
    def('handleEvent', function(emitter, inEventType, inEventData) {
    		var event = (typeof inEventData == 'undefined') ? {} : inEventData;
    		event.emitter = emitter;
    		event.eventType = inEventType;
    		return this.func(event);
    	});
});


if(!(typeof exports === 'undefined')) {
    exports.IEventHandler = IEventHandler;
}
