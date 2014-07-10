// -----------------------------------------------
// pdg.js
//
// main include file for Javascript version of PDG
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

var bindings;
var coordinates;
var embedded_pdg = (typeof process.pdg != "undefined");
var jsc = (typeof process.versions['jsc'] != "undefined");
var inbrowser = (typeof document != "undefined");
var netclient;
var netconnection;
var netserver;
var color;
if (embedded_pdg || jsc) {
	// special case for pdg embedded directly into custom standalone application
	bindings = process.pdg;
	coordinates = require('coordinates');
	color = require('color');
    if (!jsc) {  // these don't work on JavaScriptCore/iOS yet
        netconnection = require('netconnection');
        netclient = require('netclient');
        netserver = require('netserver');
    }
	process.pdg = undefined; // clean up the process variable
	delete process.pdg;
} else {
    console.log("PDG node add-in");
	// normal case, for pdg as a node JS add-on
	bindings = require('../build/Release/pdg');
	require('./dump');
	coordinates = require('./coordinates');
	color = require('./color');
	netconnection = require('./netconnection');
	netclient = require('./netclient');
	netserver = require('./netserver');
}
// make pdg a global
if (typeof(pdg) == 'undefined') {
	pdg = bindings;
}

// and also export it
module.exports = bindings;

// setup class hierarchy
bindings.EventManager.superclass = bindings.EventEmitter;
if (typeof bindings.Sound != "undefined") {  // might be non-gui build
	bindings.Sound.superclass = bindings.EventEmitter;
}
bindings.EventManager.superclass = bindings.EventEmitter;
bindings.TimerManager.superclass = bindings.EventEmitter;
bindings.Sprite.superclass = new Array( bindings.Animated, bindings.EventEmitter, bindings.ISerializable );
bindings.SpriteLayer.superclass = new Array( bindings.Animated, bindings.EventEmitter, bindings.ISerializable );
bindings.TileLayer.superclass = bindings.SpriteLayer;
bindings.ImageStrip.superclass = bindings.Image;


bindings.running = false;
bindings.quitting = false;

bindings.quit = function() {
	var _sig = methodSignature("", arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
	console.log("bindings.quit");
	bindings.quitting = true;
}

bindings.run = function() {
	var _sig = methodSignature("", arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
	if (!bindings.running) {
		bindings.__run();
	}
	bindings.running = true;
}

bindings.__run = function() {
	if (!bindings.quitting && !bindings._isQuitting() ) {
		bindings.idle();
		setImmediate(bindings.__run);
	} else {
		bindings._quit();
		process.nextTick(process.exit);
	}
}

bindings.idle = function() {
	var _sig = methodSignature("", arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
//	console.log("bindings.idle");
	bindings._idle();
}

if (!jsc) {
    
    // debugger support
    var _debuggerRunning = false;
    var exec = require('child_process').exec;
	var path = require('path');
    
    bindings.openDebugger = function() {
        var _sig = methodSignature("start node-inspector and open a debugger window in your browser", 
                                   arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
        if (!_debuggerRunning) {
            _debuggerRunning = true;
            process._debugProcess(process.pid);
            var child = exec('node-inspector --web-port=5859',
			  function (error, stdout, stderr) {
				console.log('stdout: ' + stdout);
				console.log('stderr: ' + stderr);
				if (error !== null) {
				  console.log('exec error: ' + error);
				}
				_debuggerRunning = false;
			  });
            var dir = path.dirname(process.execPath);
            var openCmd = 'open ';
            if (process.platform == 'win32') {
                openCmd = 'start ';
            }
            var openChild = exec(openCmd + path.join(dir, 'debug.html'), 
			  function (error, stdout, stderr) {
				console.log('stdout: ' + stdout);
				console.log('stderr: ' + stderr);
				if (error !== null) {
				  console.log('exec error: ' + error);
				}
			  });
        }
    }
    
    bindings._commandPort = 0;

    // console support
    bindings.openConsole = function() {
        var _sig = methodSignature("open a pdg console window", 
                arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
                
        if (!bindings._commandPort) {
        	bindings.openCommandPort();
        }
		var dir = path.dirname(process.execPath);
		var pdg_dir = process.env['PDG_ROOT'];
		if (pdg_dir) {
			pdg_dir = path.join(pdg_dir, 'tools');
		}
		var repl_script;
		if (pdg_dir) {
			repl_script = path.join(pdg_dir, 'repl');
		}
		var nodeCmd = path.join(pdg_dir, 'node');
		nodeCmd += ' ' + repl_script + ' ' + bindings._commandPort;
		var openCmd;
		if (process.platform == 'darwin') {
			openCmd = 'osascript -e \'tell app "Terminal" to do script "'+nodeCmd+'"\'';
		} else if (process.platform == 'win32') {
			openCmd = 'start '+nodeCmd;
		} else {
			openCmd = 'xterm '+nodeCmd;
		}
		var openChild = exec(openCmd, 
		  function (error, stdout, stderr) {
			console.log('stdout: ' + stdout);
			console.log('stderr: ' + stderr);
			if (error !== null) {
			  console.log('exec error: ' + error);
			}
		  });
    }
    
    // net/stream stuff not working on JSC yet
	bindings.openCommandPort = function (port) {
        var _sig = methodSignature("start a REPL server on a TCP port", 
                arguments, "undefined", 0, "([number int] port = 5757)"); if (_sig != null) return _sig;

		if (typeof(port) == 'undefined') {
			port = 5757;
		}

		var opts = { prompt: 'pdg '+process.versions['pdg']+'> ', useGlobal: true, ignoreUndefined: true, terminal: true};
		var repl = require('net-repl');
		var srv = repl.createServer(opts).listen(port);
		// TODO: emit pdg welcome; expose same globals as running pdg to repl
		bindings._commandPort = port;
	}
} // end !jsc

// save the original version of require
bindings._base_require = require;

var Module = bindings._base_require('module');
var _saved_resolveFilename = Module._resolveFilename;

bindings._loadScript = function(contents, file) {
	var myModule = new Module(file, module);
	Module._cache[file] = myModule;
	myModule.require = bindings._custom_require;
	try {
		// INTERNAL PRIVATE CALL, works with node.js 0.6.19 - 0.8.7
		console.log("bindings._loadScript() - About to compile script ["+file+"]");
		if (!jsc) {
			if ((file == "main.js") && global.v8debug) { 
				// this replacement of _resolveFilename tricks _compile into recognizing
				// our main.js and thus allow debug-brk to function
				console.log("DEBUG MODE: replacing Module._resolveFilename so debugger will work");
				Module._resolveFilename = function(request, parent) { 
					Module._resolveFilename = _saved_resolveFilename;
					return "main.js"; 
				}
			}
		}
		myModule._compile(contents, file);
		if (file == "main.js") { 
			// duplicate what node's module.js does with the main module
			process.mainModule = myModule;
			myModule.id = '.';
		}
	} catch (e) {
		delete Module._cache[file];
		throw(e);
	}
	return myModule.exports;
}

// replace it with our own special require function
// that can load from resource files
bindings._custom_require = function(file) {
	var ext = file.substring(file.lastIndexOf('.'));
	var resMgr = bindings.getResourceManager();
	var scriptSize = resMgr.getResourceSize(file);
	if ((ext == ".js" || ext == ".jsi") && scriptSize > 0) {
		// found it in the resources
		// check to make sure it hasn't previous been compiled and cached
		var cachedModule = Module._cache[file];
		if (cachedModule) {
//			console.log("resource file module "+file+" found in cache");
			return cachedModule.exports;
		}
		console.log("importing "+file+" from resources");
		var content = resMgr.getResource(file);
		return bindings._loadScript(content, file);
	} else {
//		console.log("PDG Requiring "+file);
//		return bindings._base_require(file);
		var tempModule = new Module(file, module);
		try {
			return tempModule.require(file);
		} catch(e) {
			return tempModule.require('./'+file);
		}
	}
}

require = bindings._custom_require;

console.binaryDump = function(buf, len, bytesPerLine) {
	var dumpStr;
	if (typeof bytesPerLine == "undefined") {
		dumpStr = bindings.getLogManager().binaryDump(buf, len);
	} else {
		dumpStr = bindings.getLogManager().binaryDump(buf, len, bytesPerLine);
	}
	console.log(dumpStr);
}

// basic coordinates
module.exports.Point = coordinates.Point;
module.exports.Offset = coordinates.Offset;
module.exports.Vector = coordinates.Vector;
module.exports.Rect = coordinates.Rect;
module.exports.Quad = coordinates.Quad;
module.exports.RotatedRect = coordinates.RotatedRect;

module.exports.lftTop = coordinates.lftTop;
module.exports.rgtTop = coordinates.rgtTop;
module.exports.rgtBot = coordinates.rgtBot;
module.exports.lftBot = coordinates.lftBot;

// color

module.exports.Color = color.Color;

// save off the pdg items that are pure JavaScript so they
// can be assigned as prototypes for newly created Javascript Objects when necessary
process._pdgScriptClasses = [];
process._pdgScriptClasses['Color'] = (new color.Color).__proto__;
process._pdgScriptClasses['Offset'] = (new coordinates.Offset).__proto__;
process._pdgScriptClasses['Point'] = (new coordinates.Offset).__proto__;
process._pdgScriptClasses['Vector'] = (new coordinates.Vector).__proto__;
process._pdgScriptClasses['Rect'] = (new coordinates.Rect).__proto__;
process._pdgScriptClasses['Quad'] = (new coordinates.Quad).__proto__;
process._pdgScriptClasses['RotatedRect'] = (new coordinates.RotatedRect).__proto__;

// network

if (!jsc) { // not supported on iOS/JavaScriptCore currently

	module.exports.NetConnection = netconnection.NetConnection;
	module.exports.NetClient = netclient.NetClient;
	module.exports.NetServer = netserver.NetServer;

	process._pdgScriptClasses['NetConnection'] = (new netconnection.NetConnection).__proto__;
	process._pdgScriptClasses['NetClient'] = (new netclient.NetClient).__proto__;
	process._pdgScriptClasses['NetServer'] = (new netserver.NetServer).__proto__;

	module.exports.MemBlock.prototype.toBuffer = function() {
		var _sig = methodSignature("", arguments, "[object Buffer]", 0, "()"); if (_sig != null) return _sig;
		return new Buffer(this.getData(), 'binary');
	}

	module.exports.openCommandPort = bindings.openCommandPort;
	module.exports.hasNetwork = true;
} else {
	module.exports.hasNetwork = false;
}	

// file system manager
module.exports.FileManager.prototype.findFiles = function(name) {
	var _sig = methodSignature("", arguments, "string[]", 0, "(string name)"); if (_sig != null) return _sig;
	var files = new Array;
	var fileMgr = bindings.getFileManager();
	var findInfo = fileMgr.findFirst(name);
	if (findInfo.found) do {
		if (findInfo.isDirectory == false) {
			files.push(findInfo.nodeName);
		}
	} while (fileMgr.findNext(findInfo));
	fileMgr.findClose(findInfo);
	return files;
}

module.exports.FileManager.prototype.findDirs = function(name) {
	var _sig = methodSignature("", arguments, "string[]", 0, "(string name)"); if (_sig != null) return _sig;
	var files = new Array; 
	var fileMgr = bindings.getFileManager();
	var findInfo = fileMgr.findFirst(name);
	if (findInfo.found) do {
		if ( (findInfo.isDirectory == true)
		   && (findInfo.nodeName != '.') 
		   && (findInfo.nodeName != '..') ) {
			files.push(findInfo.nodeName);
		}
	} while (fileMgr.findNext(findInfo));
	fileMgr.findClose(findInfo);
	return files;
}

module.exports.fs = bindings.getFileManager();
module.exports.evt = bindings.getEventManager();
module.exports.tm = bindings.getTimerManager();
module.exports.res = bindings.getResourceManager();
module.exports.cfg = bindings.getConfigManager();
module.exports.lm = bindings.getLogManager();
if (typeof bindings.GraphicsManager != "undefined") {  // might be non-gui build
	module.exports.gfx = bindings.getGraphicsManager();
	module.exports.hasGraphics = true;
} else {
	module.exports.hasGraphics = false;
}
if (typeof bindings.SoundManager != "undefined") {  // might be non-gui build
	module.exports.snd = bindings.getSoundManager();
	module.exports.hasSound = true;
} else {
	module.exports.hasSound = false;
}

// simple log writer
module.exports.log = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(4, "LOG", msg);
}
module.exports.info = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(5, "INFO", msg);
}
module.exports.warn = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(3, "WARN", msg);
}
module.exports.fatal = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(0, "FATAL", msg);
}
module.exports.error = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(1, "ERROR", msg);
}
module.exports.debug = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(7, "DEBUG", msg);
}
module.exports.trace = function(msg) {
	var _sig = methodSignature("", arguments, "undefined", 1, "(string msg)"); if (_sig != null) return _sig;
	bindings.getLogManager().writeLogEntry(9, "TRACE", msg);
}

// replace console log
module.exports.captureConsole = function() {
	var _sig = methodSignature("", arguments, "undefined", 1, "()"); if (_sig != null) return _sig;
	console.log = module.exports.log
	console.info = module.exports.info
	console.warn = module.exports.warn
	console.error = module.exports.error
}

// event manager

// create an IEventHandler with the function and add it to the Event Manager
module.exports.on = function(eventType, func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 2, "([number int] eventType, function func)"); if (_sig != null) return _sig;
	var handler = new bindings.IEventHandler(func);
	bindings.getEventManager().addHandler(handler, eventType);
	handler.cancel = function() {
		bindings.getEventManager().removeHandler(handler, eventType);
	};
	return handler;
}

// onStartup(function)
// module.exports.onStartup = function(func) {
// 	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
// 	return this.on(bindings.eventType_Startup, func);
// }
// onShutdown(function)
module.exports.onShutdown = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_Shutdown, func);
}
// onTimer(function)
module.exports.onTimer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_Timer, func);
}
// onKeyDown(function)
module.exports.onKeyDown = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_KeyDown, func);
}
// onKeyUp(function)
module.exports.onKeyUp = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_KeyUp, func);
}
// onKeyPress(function)
module.exports.onKeyPress = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_KeyPress, func);
}
// onMouseDown(function)
module.exports.onMouseDown = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_MouseDown, func);
}
// onMouseUp(function)
module.exports.onMouseUp = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_MouseUp, func);
}
// onMouseMove(function)
module.exports.onMouseMove = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.eventType_MouseMove, func);
}


var _lastAutoTimerId = 0x7000000;

// TimerManager.onTimeout(function, delayMs)
module.exports.TimerManager.prototype.onTimeout = function(func, delay) {
	var _sig = methodSignature("setup handler to be called once after delay ms", arguments, "[object IEventHandler]", 2, "(function func, [number int] delay)"); if (_sig != null) return _sig;
	var timerId = _lastAutoTimerId++;
	this.startTimer(timerId, delay, bindings.timer_OneShot);
	var handler = new bindings.IEventHandler(function(event) {
		if (event.id != timerId) return false; // timer event was not for us
		func(event);
//		this.removeHandler(handler, eventType); // this was a one-shot, so remove the handler too
		return true;  // we are the only handler to handle this event
	}.bind(this));
	this.addHandler(handler, bindings.eventType_Timer);
	handler.cancel = function() {
		this.removeHandler(handler, eventType);
		this.cancelTimer(timerId);
	}.bind(this);
	handler.timer = timerId;  // so we can pass it to timer manager functions
	return handler;
}

// TimerManager.onInterval(function, intervalMs)
module.exports.TimerManager.prototype.onInterval = function(func, interval) {
	var _sig = methodSignature("setup handler to be called regularly at interval ms", arguments, "[object IEventHandler]", 2, "(function func, [number int] interval)"); if (_sig != null) return _sig;
	var timerId = _lastAutoTimerId++;
	this.startTimer(timerId, interval, bindings.timer_Repeating);
	var handler = new bindings.IEventHandler(function(event) {
		if (event.id != timerId) return false; // timer event was not for us
		func(event);
		return true; // we are the only handler to handle this event
	}.bind(this));
	this.addHandler(handler, bindings.eventType_Timer);
	handler.cancel = function() {
		this.removeHandler(handler, eventType);
		this.cancelTimer(timerId);
	}.bind(this);
	handler.timer = timerId;  // so we can pass it to timer manager functions
	return handler;
}

if (typeof bindings.Sound != "undefined") {  // might be non-gui build
    
    // Sound.on(eventCode, function)
    //
    // creates an IEventHander for the sound events with the function
    // and add it to the sound.
    module.exports.Sound.prototype.on = function(eventCode, func) {
        var _sig = methodSignature("", arguments, "[object IEventHandler]", 2, "([number int] eventCode, function func)"); if (_sig != null) return _sig;
        var handler = new bindings.IEventHandler(function(event) {
                                                 if (event.eventCode != eventCode) return false;
                                                 return func(event);
                                                 }.bind(this));
        this.addHandler(handler, bindings.eventType_SoundEvent);
        handler.cancel = function() {
            this.removeHandler(handler, eventType);
        }.bind(this);
        return handler;
    }
    
    // Sound.onDonePlaying(function)
    module.exports.Sound.prototype.onDonePlaying = function(func) {
        var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
        return this.on(bindings.soundEvent_DonePlaying, func);
    }
    // Sound.onLooping(function)
    module.exports.Sound.prototype.onLooping = function(func) {
        var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
        return this.on(bindings.soundEvent_Looping, func);
    }
    // Sound.onFailedToPlay(function)
    module.exports.Sound.prototype.onFailedToPlay = function(func) {
        var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
        return this.on(bindings.soundEvent_FailedToPlay, func);
    }
    
} // !sound undefined

// Sprite.on(eventCode, function)
//
// creates an IEventHander for the sprite events with the function
// and add it to the sprite.
module.exports.Sprite.prototype.on = function(eventCode, func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 2, "([number int] eventCode, function func)"); if (_sig != null) return _sig;
	var handler = new bindings.IEventHandler(function(event) {
		if (event.eventType == bindings.eventType_SpriteTouch) {
			if (event.touchType != eventCode) return false;
		}
		if (event.action != eventCode) return false;
		return func(event);
	}.bind(this));
	var evtCode;
	if (eventCode <= bindings.action_CollideWall) {
		evtCode = bindings.eventType_SpriteCollide;
	} else if (eventCode <= bindings.action_FadeOutComplete) {
		evtCode = bindings.eventType_SpriteAnimate;
	} else {
		evtCode = bindings.eventType_SpriteTouch;
	}
	this.addHandler(handler, evtCode);
	handler.cancel = function() {
		this.removeHandler(handler, eventType);
	}.bind(this);
	return handler;
}

// Sprite.onCollideSprite(function)
module.exports.Sprite.prototype.onCollideSprite = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_CollideSprite, func);
}
// Sprite.onCollideWall(function)
module.exports.Sprite.prototype.onCollideWall = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_CollideWall, func);
}
// Sprite.onOffscreen(function)
module.exports.Sprite.prototype.onOffscreen = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_Offscreen, func);
}
// Sprite.onOnscreen(function)
module.exports.Sprite.prototype.onOnscreen = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_onOnscreen, func);
}
// Sprite.onExitLayer(function)
module.exports.Sprite.prototype.onExitLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_ExitLayer, func);
}
// Sprite.onAnimationLoop(function)
module.exports.Sprite.prototype.onAnimationLoop = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationLoop, func);
}
// Sprite.onAnimationEnd(function)
module.exports.Sprite.prototype.onAnimationEnd = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationEnd, func);
}
// Sprite.onFadeComplete(function)
module.exports.Sprite.prototype.onFadeComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeComplete, func);
}
// Sprite.onFadeInComplete(function)
module.exports.Sprite.prototype.onFadeInComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeInComplete, func);
}
// Sprite.onFadeOutComplete(function)
module.exports.Sprite.prototype.onFadeOutComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeOutComplete, func);
}
// Sprite.onMouseEnter(function)
module.exports.Sprite.prototype.onMouseEnter = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseEnter, func);
}
// Sprite.onMouseLeave(function)
module.exports.Sprite.prototype.onMouseLeave = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseLeave, func);
}
// Sprite.onMouseDown(function)
module.exports.Sprite.prototype.onMouseDown = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseDown, func);
}
// Sprite.onMouseUp(function)
module.exports.Sprite.prototype.onMouseUp = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseUp, func);
}
// Sprite.onMouseClick(function)
module.exports.Sprite.prototype.onMouseClick = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseClick, func);
}



// SpriteLayer.on(eventCode, function)
//
// creates an IEventHander for the layer events with the function
// and add it to the layer. 
module.exports.SpriteLayer.prototype.on = function(eventCode, func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 2, "([number int] eventCode, function func)"); if (_sig != null) return _sig;
	var handler = new bindings.IEventHandler(function(event) {
		if (event.eventType == bindings.eventType_SpriteTouch) {
			if (event.touchType != eventCode) return false;
		}
		if (event.action != eventCode) return false;
		return func(event);
	}.bind(this));
	var evtCode;
	if (eventCode <= bindings.action_CollideWall) {
		evtCode = bindings.eventType_SpriteCollide;
	} else if (eventCode <= bindings.action_FadeOutComplete) {
		evtCode = bindings.eventType_SpriteAnimate;
	} else if (eventCode >= bindings.action_ErasePort) {
		evtCode = bindings.eventType_SpriteLayer;
	} else {
		evtCode = bindings.eventType_SpriteTouch;
	}
	this.addHandler(handler, evtCode);
	handler.cancel = function() {
		this.removeHandler(handler, eventType);
	}.bind(this);
	return handler;
}
module.exports.TileLayer.prototype.on = module.exports.SpriteLayer.prototype.on;

// SpriteLayer.onCollideSprite(function)
module.exports.SpriteLayer.prototype.onCollideSprite = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_CollideSprite, func);
}
module.exports.TileLayer.prototype.onCollideSprite = module.exports.SpriteLayer.prototype.onCollideSprite;
// SpriteLayer.onCollideWall(function)
module.exports.SpriteLayer.prototype.onCollideWall = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_CollideWall, func);
}
module.exports.TileLayer.prototype.onCollideWall = module.exports.SpriteLayer.prototype.onCollideWall;
// SpriteLayer.onOffscreen(function)
module.exports.SpriteLayer.prototype.onOffscreen = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_Offscreen, func);
}
module.exports.TileLayer.prototype.onOffscreen = module.exports.SpriteLayer.prototype.onOffscreen;
// SpriteLayer.onOnscreen(function)
module.exports.SpriteLayer.prototype.onOnscreen = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_onOnscreen, func);
}
module.exports.TileLayer.prototype.onOnscreen = module.exports.SpriteLayer.prototype.onOnscreen;
// SpriteLayer.onExitLayer(function)
module.exports.SpriteLayer.prototype.onExitLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_ExitLayer, func);
}
module.exports.TileLayer.prototype.onExitLayer = module.exports.SpriteLayer.prototype.onExitLayer;
// SpriteLayer.onAnimationLoop(function)
module.exports.SpriteLayer.prototype.onAnimationLoop = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationLoop, func);
}
module.exports.TileLayer.prototype.onAnimationLoop = module.exports.SpriteLayer.prototype.onAnimationLoop;
// SpriteLayer.onAnimationEnd(function)
module.exports.SpriteLayer.prototype.onAnimationEnd = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationEnd, func);
}
module.exports.TileLayer.prototype.onAnimationEnd = module.exports.SpriteLayer.prototype.onAnimationEnd;
// SpriteLayer.onFadeComplete(function)
module.exports.SpriteLayer.prototype.onFadeComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeComplete, func);
}
module.exports.TileLayer.prototype.onFadeComplete = module.exports.SpriteLayer.prototype.onFadeComplete;
// SpriteLayer.onFadeInComplete(function)
module.exports.SpriteLayer.prototype.onFadeInComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeInComplete, func);
}
module.exports.TileLayer.prototype.onFadeInComplete = module.exports.SpriteLayer.prototype.onFadeInComplete;
// SpriteLayer.onFadeOutComplete(function)
module.exports.SpriteLayer.prototype.onFadeOutComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_FadeOutComplete, func);
}
module.exports.TileLayer.prototype.onFadeOutComplete = module.exports.SpriteLayer.prototype.onFadeOutComplete;
// SpriteLayer.onMouseEnter(function)
module.exports.SpriteLayer.prototype.onMouseEnter = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseEnter, func);
}
module.exports.TileLayer.prototype.onMouseEnter = module.exports.SpriteLayer.prototype.onMouseEnter;
// SpriteLayer.onMouseLeave(function)
module.exports.SpriteLayer.prototype.onMouseLeave = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseLeave, func);
}
module.exports.TileLayer.prototype.onMouseLeave = module.exports.SpriteLayer.prototype.onMouseLeave;
// SpriteLayer.onMouseDown(function)
module.exports.SpriteLayer.prototype.onMouseDown = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseDown, func);
}
module.exports.TileLayer.prototype.onMouseDown = module.exports.SpriteLayer.prototype.onMouseDown;
// SpriteLayer.onMouseUp(function)
module.exports.SpriteLayer.prototype.onMouseUp = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseUp, func);
}
module.exports.TileLayer.prototype.onMouseUp = module.exports.SpriteLayer.prototype.onMouseUp;
// SpriteLayer.onMouseClick(function)
module.exports.SpriteLayer.prototype.onMouseClick = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.touch_MouseClick, func);
}
module.exports.TileLayer.prototype.onMouseClick = module.exports.SpriteLayer.prototype.onMouseClick;
// SpriteLayer.onErasePort(function)
module.exports.SpriteLayer.prototype.onErasePort = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_ErasePort, func);
}
module.exports.TileLayer.prototype.onErasePort = module.exports.SpriteLayer.prototype.onErasePort;
// SpriteLayer.onPreDrawLayer(function)
module.exports.SpriteLayer.prototype.onPreDrawLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_PreDrawLayer, func);
}
module.exports.TileLayer.prototype.onPreDrawLayer = module.exports.SpriteLayer.prototype.onPreDrawLayer;
// SpriteLayer.onPostDrawLayer(function)
module.exports.SpriteLayer.prototype.onPostDrawLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_PostDrawLayer, func);
}
module.exports.TileLayer.prototype.onPostDrawLayer = module.exports.SpriteLayer.prototype.onPostDrawLayer;
// SpriteLayer.onDrawPortComplete(function)
module.exports.SpriteLayer.prototype.onDrawPortComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_DrawPortComplete, func);
}
module.exports.TileLayer.prototype.onDrawPortComplete = module.exports.SpriteLayer.prototype.onDrawPortComplete;
// SpriteLayer.onAnimationStart(function)
module.exports.SpriteLayer.prototype.onAnimationStart = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationStart, func);
}
module.exports.TileLayer.prototype.onAnimationStart = module.exports.SpriteLayer.prototype.onAnimationStart;
// SpriteLayer.onPreAnimateLayer(function)
module.exports.SpriteLayer.prototype.onPreAnimateLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_PreAnimateLayer, func);
}
module.exports.TileLayer.prototype.onPreAnimateLayer = module.exports.SpriteLayer.prototype.onPreAnimateLayer;
// SpriteLayer.onPostAnimateLayer(function)
module.exports.SpriteLayer.prototype.onPostAnimateLayer = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_PostAnimateLayer, func);
}
module.exports.TileLayer.prototype.onPostAnimateLayer = module.exports.SpriteLayer.prototype.onPostAnimateLayer;
// SpriteLayer.onAnimationComplete(function)
module.exports.SpriteLayer.prototype.onAnimationComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_AnimationComplete, func);
}
module.exports.TileLayer.prototype.onAnimationComplete = module.exports.SpriteLayer.prototype.onAnimationComplete;
// SpriteLayer.onZoomComplete(function)
module.exports.SpriteLayer.prototype.onZoomComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_ZoomComplete, func);
}
module.exports.TileLayer.prototype.onZoomComplete = module.exports.SpriteLayer.prototype.onZoomComplete;
// SpriteLayer.onLayerFadeInComplete(function)
module.exports.SpriteLayer.prototype.onLayerFadeInComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_LayerFadeInComplete, func);
}
module.exports.TileLayer.prototype.onLayerFadeInComplete = module.exports.SpriteLayer.prototype.onLayerFadeInComplete;
// SpriteLayer.onLayerFadeOutComplete(function)
module.exports.SpriteLayer.prototype.onLayerFadeOutComplete = function(func) {
	var _sig = methodSignature("", arguments, "[object IEventHandler]", 1, "(function func)"); if (_sig != null) return _sig;
	return this.on(bindings.action_LayerFadeOutComplete, func);
}
module.exports.TileLayer.prototype.onLayerFadeOutComplete = module.exports.SpriteLayer.prototype.onLayerFadeOutComplete;

// debugger support

if (!jsc) {
    module.exports.onKeyPress(function(evt) {
        if (evt.ctrl && evt.unicode == bindings.key_Delete) {
            // start the debugger
            bindings.openDebugger();
            return true; // we handled this event, don't pass it on
        } else if (evt.alt && evt.unicode == bindings.key_Escape) {
            // open up a console
            bindings.openConsole();
            return true; // we handled this event, don't pass it on
        }
        return false;
    });
}

// finish script setup (jsc does this in pdg_main.js)
if (!jsc && !inbrowser) {
	if (typeof(bindings._finishedScriptSetup) == 'function') {
		bindings._finishedScriptSetup();  // let C++ finish initializing now that JS is all set up
	}
}
