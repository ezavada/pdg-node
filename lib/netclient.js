// -----------------------------------------------
// netclient.js
//
// Client side implementation of pdg network interface
//
// Written by Ed Zavada, 2012
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

// this is one of the few javascript files that are built-in
// to the pdg standalone app but also part of the pure 
// javascript implementation

if ((typeof process != "undefined") && (typeof process.pdg != "undefined")) {
	// node in standalone, classify becomes a built-in
	require('netconnection');
} else {
	require('./netconnection');
}
var net = require('net');

classify('NetClient', function() {

	//! new Client(): create a network client for your game
    def('initialize', function(opts) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create a network client", arguments, "undefined", 0, "(object opts = null)");
			}
			this.connection = false;
			this._errorCallback = false;
			this._connectCallback = false;
			this._allowDatagram = true;
			if (typeof opts != 'undefined') {
				if (typeof opts.noDatagram != 'undefined') {
					this._allowDatagram = !opts.noDatagram
				}
			}
		});

	//! attempt to connect to a server for your game
	//! /param serverInfo the info for the server you want to connect to. Should be an object with
	//! /param clientKey the private key that identifies this as an authorized client
	//!        addr and port members, ie: { host: "localhost", port: 5454 }
	def('connect', function(serverInfo, callback, clientKey) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("connect to a server for your game", arguments, "[object NetClient]", 2, "(object serverInfo, function callback, string clientKey = '')");
			}
			// have serverInfo, try to connect to it via TCP
			this._connectCallback = callback;
			var socket = net.connect({port: serverInfo.port, host: serverInfo.host},
				function() { //'connect' listener
				  try {
					this.connection = new pdg.NetConnection(socket);
					this.connection._clientInit(this, clientKey);
				  } catch(e) { 
				    if (this._errorCallback) {
				      this._errorCallback(e);
				    } else {
				  	  console.error("NetClient Exception connecting to "+serverInfo.host+":"+serverInfo.port+": "+JSON.stringify(e)); 
				  	}
				  }
				}.bind(this));
			socket.on('error', function(error) {
			  socket.end();
			  if (this.connection) {
				this.connection._handleError(error);
			  } else if (this._errorCallback) {
				this._errorCallback(error, this);
			  }
			}.bind(this));				
			return this;
		});

	//! setup error handler
	def('onError', function(callback) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("setup error handler", arguments, "[object NetClient]", 1, "(function callback)");
			}
			this._errorCallback = callback;
			return this;
		});

});

if(!(typeof exports === 'undefined')) {
    exports.NetClient = NetClient;
}
