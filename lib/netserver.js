// -----------------------------------------------
// netserver.js
//
// Server side implementation of pdg network interface
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
	require('./pdg');
}
var net = require('net');
var dgram = require('dgram');

var NET_SERVER_LOG;

if ((process.env.PDG_DEBUG && process.env.PDG_DEBUG.indexOf('NET_') != -1)
  || (process.env.NODE_DEBUG && process.env.NODE_DEBUG.indexOf('NET_') != -1)) {
	console.log('Found NODE_DEBUG or PDG_DEBUG=NET_CONNECT (or NET_DATA) in environment. Logging network connections.');
	NET_SERVER_LOG = function(msg) {
		console.log(msg);
	};
} else {
 	NET_SERVER_LOG = function(msg) {};
}

classify('NetServer', function() {

	//! new NetServer(serverInfo): create a network server for your game
	//! /param opts the options for the server
    def('initialize', function(opts) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create a network server", arguments, "undefined", 2, "(object opts = null)");
			}
			this.serverInfo = opts;
			this.serverPort = 5000;
			this.serverAddr = '0.0.0.0';
			this.handshakeTimeout = 5000; // 5 second timeout on handshake
			this.reservationRequired = false;
			this._fixedPort = false;
			this.allowDatagram = true;
			if (typeof opts != 'undefined') {
				if (typeof opts.serverAddr != 'undefined') {
					this.serverAddr = opts.serverAddr;
				}
				if (typeof opts.serverPort != 'undefined') {
					this.serverPort = opts.serverPort
				}
				if (typeof opts.fixedPort != 'undefined') {
					this._fixedPort = opts.fixedPort
				}
				if (typeof opts.noDatagram != 'undefined') {
					this.allowDatagram = !opts.noDatagram
				}
				if (typeof opts.reservationRequired != 'undefined') {
					this.reservationRequired = opts.reservationRequired;
				}
				if (typeof opts.handshakeTimeout != 'undefined') {
					this.handshakeTimeout = opts.handshakeTimeout;
				}
			}
			this._lastPort = this.serverPort + 100;
			this._listener = false;
			this.listening = false;
			this.connections = [];
			this._connectCallback = false;
			this._errorCallback = false;
			this._reservations = [];
			this._dgramSock = false;
			this._dgramAlive = false;
		});

	//! list for incoming connections from your game clients
	//! /param callback function to call each time a new connection is made
	def('listen', function(callback) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("open the server for incoming connections", arguments, "[object NetServer]", 2, "(function callback)");
			}
			if (this.listening) {
				throw("server already listening on " + this._listener.address() );
			}
			this._connectCallback = callback;
			if (!this._listener) {
				this._listener = net.createServer();
  				this._listener.on('error', this._handleError.bind(this));
  				this._listener.on('close', this._handleClose.bind(this));
  				this._listener.on('connection', this._handleConnect.bind(this));
 				this._listener.on('listening', this._handleListening.bind(this));
			}
			this._tryListen();
			return this;
		});

	//! send a message to all 
	//! returns number of connections the message was sent to
	//! filter - bool filter([object NetConnection] connection)
	def('broadcast', function(message, filter) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("send a message to all connections, with optional filter", arguments, "number", 2, "(object message, function filter = null)");
			}
			var msgCount = 0;
			if (typeof filter != 'function') {
				// send the message to everyone, no exceptions
				for (var i = 0; i < connections.length; i++) {
					connections[i].send(message);
				}
				msgCount = connections.length;
			} else {
				// send the message only to connections for which the filter func returns true
				for (var i = 0; i < connections.length; i++) {
					var connection = connections[i];
					if (filterFunc(connection)) {
						connection.send(message);
						msgCount++;
					}
				}
			}
			return msgCount;
		});

	//! expect a client with a particular key to connect
	//! /param clientKey the key the client should have
	//! /param clientIpAddr the IP address the client should be coming from
	//! /param reservationTTL the time in seconds for the reservation to last
	//! /param singleUse true if the reservation is cleared as soon as the client connects
	def('expectClient', function(clientKey, clientIpAddr, reservationTTL, singleUse) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("client reservation", arguments, "[object NetServer]", 1, "(string clientKey, string clientIpAddr = '*', [number int] reservationTTL = FOREVER, boolean singleUse = false)");
			}
			if (typeof(clientIpAddr) == 'undefined') {
				clientIpAddr = '*'; // accept any IP
			}
			if (typeof(reservationTTL) == 'undefined') {
				reservationTTL = -1; // never expires
			}
			if (typeof(singleUse) == 'undefined') {
				singleUse = false;
			}
			var t = pdg.tm.getMilliseconds();
			var reservation = { key: clientKey, ip: clientIpAddr, until: t + (reservationTTL*1000), once: singleUse };
			for (var i in this._reservations) {
				var rt = this._reservations[i].until;
				if ((rt != -1) && (rt < t)) {
					// expired reservation, replace it with this one
					this._reservations[i] = reservation;
					return this;
				}
			}
			this._reservations.push(reservation);
			return this;
		});

	//! setup error handler
	def('onError', function(callback) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("setup error handler", arguments, "[object NetServer]", 1, "(function callback)");
			}
			this._errorCallback = callback;
			return this;
		});

	//! close the connection
	def('shutdown', function(closeExisting, kill) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("close the listener and don't accept new connections", arguments, "undefined", 0, "(boolean closeExisting = true, boolean kill = false)");
			}
			if (typeof kill != 'undefined') {
				kill = false;
			}
			if (this._listener) {
				this._listener.close();
			}
			if (typeof(closeExisting) == 'undefined') {
				closeExisting = true;
			}
			if (closeExisting) {
				this._closeAllConnections(kill);
			}
			this.listening = false;
		});


// ==============================================================
// protected - you shouldn't need to call these directly


	def('_checkClientIP', function(clientIpAddr) {
			if (this.reservationRequired) {
				var t = pdg.tm.getMilliseconds();
				var foundExpired = false;
				var foundMatch = false;
				for (var i in this._reservations) {
					var rt = this._reservations[i].until;
					if ((rt != -1) && (rt < t)) {
						foundExpired = true;  // clean up later
						this._reservations[i].until = 0;
					} else if ((this._reservations[i].ip == '*') || (this._reservations[i].ip == clientIpAddr)) {
						NET_SERVER_LOG('IP match for '+clientIpAddr+' reservation '+i+' == '+this._reservations[i].ip);
						foundMatch = true;
						if (this._reservations[i].once) {
							// we have an IP match for this client, so give it extra time
							// to complete the handshake
							this._reservations[i].until += 2000;
						}
					}
				}
				if (foundExpired) {
					for (var i in this._reservations) {
						if (this._reservations[i].until == 0) {
							NET_SERVER_LOG('removed expired reservation '+i);
							this._reservations.splice(i, 1);
						}
					}
				}
				return foundMatch;
			} else {
				return true;
			}
		});

	def('_checkClientKey', function(clientKey, clientIpAddr) {
			if (this.reservationRequired) {
				var t = pdg.tm.getMilliseconds();
				var foundExpired = false;
				var foundMatch = false;
				for (var i in this._reservations) {
					var rt = this._reservations[i].until;
					if ((rt != -1) && (rt < t)) {
						foundExpired = true;  // clean up later
						this._reservations[i].until = 0;
					} else if (this._reservations[i].key == clientKey) {
						if ((this._reservations[i].ip == '*') || (this._reservations[i].ip == clientIpAddr)) {
							NET_SERVER_LOG('Key + IP match for '+clientKey+' @'+clientIpAddr+' reservation '+i+' == '+this._reservations[i].key+' @'+this._reservations[i].ip);
							foundMatch = true;
							if (this._reservations[i].once) {
								foundExpired = true;	// single use key, expire it now that it has been used
								this._reservations[i].until = 0;	
							}
						}
					}
				}
				if (foundExpired) {
					for (var i in this._reservations) {
						if (this._reservations[i].until == 0) {
							NET_SERVER_LOG('removed expired/used reservation '+i);
							this._reservations.splice(i, 1);
						}
					}
				}
				if (!foundMatch) {
					NET_SERVER_LOG('No Reservation Found for '+clientKey+' @'+clientIpAddr);
				}
				return foundMatch;
			} else {
				return true;
			}
		});


	def('_handleClose', function() {
			this.listening = false;
			if (this.connections.length != 0) {
				NET_SERVER_LOG('Server close, killing all connections');
				this._closeAllConnections(true);
			}
		});
	def('_closeAllConnections', function(kill) {
			if (this._dgramSock) {
				this._dgramSock.close();
			}
			this._dgramSock = false;
			this._dgramAlive = false;
			var connectionsCopy = this.connections;
			for (var i = 0; i < connectionsCopy.length; i++) {
				var connection = connectionsCopy[i];
				connection.close(kill);
				connection._dgramSock = false;
				connection._dgramAlive = false;
			}
		});	
	def('_handleConnectionClose', function(connection) {
			var idx = this.connections.indexOf(connection);
			if (idx != -1) {
				this.connections.splice(idx, 1);
			}
		});
	def('_handleConnectionHandshakeClose', function(connection) {
			// if connection is not alive, it means we killed it, so just ignore this
			if (connection._alive) {
				NET_SERVER_LOG('Server got connection close during handshake from '+connection.remoteAddr);
				connection.close(true);
			}
		});
	def('_handleError', function(error) {
			NET_SERVER_LOG('Got error ' + error);
			if ((error.code == 'EADDRINUSE') && (!this._fixedPort) && (this.serverPort < this._lastPort)) {
				NET_SERVER_LOG('Retrying on new port');
				this.serverPort++;
				this._tryListen();
			} else if (this._errorCallback) {
				try {
					NET_SERVER_LOG('Doing Error Callback');
					this._errorCallback(error, this);
				}
				catch(e) {
					console.error('NetServer Error Callback Exception: '+JSON.stringify(e));
				}
			} else {
				// make sure somebody knows what happened
				console.error('NetServer Error: '+JSON.stringify(e));
			}
		});
	def('_handleListening', function() {
			var addr = this._listener.address().address;
			var port = this._listener.address().port;
			NET_SERVER_LOG('Server now listening on ' + addr +':'+ port+'/tcp');
			this.listening = true;
 			if (this.allowDatagram) {
 				NET_SERVER_LOG('Server attempting to create Datagram socket on ' + addr +':'+ port);
				this._dgramSock = dgram.createSocket("udp4");
				this._dgramSock.on("message", this._handleDgram.bind(this));
				this._dgramSock.on("error", this._handleError.bind(this));
				this._dgramSock.on("listening", this._handleDgramListening.bind(this));
				this._dgramSock.bind(port, addr);
 			}
		});
	def('_handleDgramListening', function() {
			var addr = this._listener.address().address;
			var port = this._listener.address().port;
			NET_SERVER_LOG('Server now listening on ' + addr +':'+ port+'/udp');
			this._dgramAlive = true;
		});
	// called by NetConnection once handshake is finished
	def('_handshakeComplete', function(connection) {
			NET_SERVER_LOG('Server handshake complete for ' + connection.remoteAddr);
			try {
				if (this._connectCallback(connection)) {
					this.connections.push(connection);
					connection.socket.on('close', function() {
						this._handleConnectionClose(connection);
					}.bind(this)); // we need to be notified
				} else {
					// if we aren't going to accept this connection, then
					// immediately shutdown the socket and allow no further
					// communication
					NET_SERVER_LOG('Application layer refused connection from '+connection.remoteAddr);
					connection.close(true);
				}
			}
			catch(e) {
				console.error('NetServer Connect Callback Exception: '+JSON.stringify(e));
				NET_SERVER_LOG('Application layer exception on connect callback for connection from '+connection.remoteAddr);
				connection.close(true);
			}
		});
	def('_handleConnect', function(socket) {
			NET_SERVER_LOG('incoming connection from '+socket.remoteAddress);
			try {
				if (this._checkClientIP(socket.remoteAddress)) {
					var connection = new pdg.NetConnection(socket, this);
					connection._serverInit(this); // wait for handshake to complete
					if (this.handshakeTimeout > 0) {
						pdg.tm.onTimeout(function(evt) {
							// when called, connection will be "this"
							if (this._alive && !this._handshakeComplete) {
								NET_SERVER_LOG('Handshake Timeout ['+this._server.handshakeTimeout+'ms] for connection from '+this.remoteAddr);
								this.close(true); // dead client connection, kill it
							}
							return true;
						}.bind(connection), this.handshakeTimeout);
					}
					connection.socket.on('close', function() {
						this._handleConnectionHandshakeClose(connection);
					}.bind(this)); // we need to be notified
				} else {
					// IP not in our reservation list
					NET_SERVER_LOG('Dumping connection with no reservation from '+socket.remoteAddress);
					socket.destroy();
				}
			} catch(e) { 
				this._handleError("Error handling incoming connection @"+this.serverAddr+":"+this.serverPort+" from "+socket.remoteAddress+": "+e); 
			}
		});
	def('_tryListen', function() {
			if (!this.listening) {
				NET_SERVER_LOG('Server trying to listen @'+this.serverAddr+":"+this.serverPort+"/tcp");
				this._listener.listen(this.serverPort, this.serverAddr);
			} else {
				NET_SERVER_LOG('ignoring _tryListen(), already listening');
			}
		});

	def('_handleDgram', function(msg, rinfo) {
		try {
			// find which connection based on remoteAddr and port
			for (var i = 0; i < this.connections.length; i++) {
				if ((rinfo.address == this.connections[i].remoteAddr) && (rinfo.port == this.connections[i].remotePort)) {
					this.connections[i]._handleDgram(msg, rinfo);
					return;
				}
			}
			NET_SERVER_LOG('Server ignoring dgram from unknown client ' + rinfo.address+':'+rinfo.port);
	    } catch(e) { 
	    	this._handleError(e); 
	    }
		});

});

if(!(typeof exports === 'undefined')) {
    exports.NetServer = NetServer;
}
