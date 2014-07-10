// -----------------------------------------------
// pdg_main.js
//
// override the normal loading of script specified on command line
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
// jsminify this file, js2c: jsmin

var pdg = require('pdg');
pdg.argv = process.argv;  // save the arguments

pdg._debug_log = function() {};
if (process.env.NODE_DEBUG && /pdg/.test(process.env.NODE_DEBUG)) {
  pdg._debug_log = function(x) {
    console.error(x);
  };
}

pdg.startRepl = function() {
	var _sig = methodSignature("", arguments, "undefined", 0, "()"); if (_sig != null) return _sig;
	// go into interactive mode
	console.log("PDG terminal v"+process.versions['pdg']);

	// REPL
	var opts = {
	  useGlobal: true,
	  ignoreUndefined: true
	};
	if (parseInt(process.env['NODE_NO_READLINE'], 10)) {
	  opts.terminal = false;
	}
	if (parseInt(process.env['NODE_DISABLE_COLORS'], 10)) {
	  opts.useColors = false;
	}
	repl.start(opts);
	global.pdg = pdg;
}

process._forceRepl = false;
//process.argv.length = 1;

var path = require('path');
var fs = require('fs');

var net = require("net"),
	repl = require("repl");

var resMgr = pdg.getResourceManager();

var foundMain = false;

var resFile = 0;
var mainScript;
var mainFile;
var mainFilename = 'main.js';

// see if we were passed a script name on the command line
for (var i = 1; i < pdg.argv.length; i++) {
	if (pdg.argv[i] == 'main.js') {
		// artificially added, ignore it
		break;
	}
	if (pdg.argv[i].substr(-3) == '.js') {
		pdg._debug_log("Found script name on command line: "+pdg.argv[i]);
		mainFilename = pdg.argv[i];
		foundMain = true;
		break;
	} else if ((pdg.argv[i] == '-e') && (pdg.length > i+1)) {
		pdg._debug_log("Found [-e "+pdg.arv[i+1]+"] on command line");
		mainFilename = pdg.argv[i+1];
		foundMain = true;
		break;
	}
}
if (foundMain) {
	if (fs.existsSync(mainFilename)) {
		foundMain = true;
		mainScript = fs.readFileSync(mainFilename, 'utf8');
		mainFile = mainFilename;
	} else {
		console.error("Could not find file to execute: "+mainFilename+" (cwd: "+process.cwd()+")");
		foundMain = false;
		process.exit(1);
	}
}

// figure out our possible paths for main.js
// get where binary resides and look there
var binPath = path.resolve(path.dirname(process.execPath));
var execName = path.basename(process.argv[0]);
var defaultResourceFile = binPath + "/" + execName + ".dat";
if (fs.existsSync(defaultResourceFile)) {
	resFile = resMgr.openResourceFile(defaultResourceFile);
	pdg._debug_log("Found default resource file "+defaultResourceFile);
} else if (fs.existsSync(binPath + "/main.dat")) {
	resFile = resMgr.openResourceFile(binPath+"/main.dat");
	pdg._debug_log("Found resource file "+binPath+"/main.dat");
}
if (!foundMain) {
	if (resMgr.getResourceSize(mainFilename) > 0) {
		foundMain = true;
		mainScript = resMgr.getResource(mainFilename);
		mainFile = "(resources):"+mainFilename;
	}
}

// update current working directory if needed for Mac OS X bundle
var cwd = process.cwd();
if (cwd == "" || cwd == "/") {
	// for blank or root cwd, check to see if this is a Mac Bundle
	var bundleSubstr = binPath.substring(binPath.length - 19);
//	pdg._debug_log("Checking for Mac Bundle [" + binPath + "] [" + bundleSubstr + "]");
	if (bundleSubstr == ".app/Contents/MacOS") {
		var newPath = path.resolve(binPath, "../../../");
		pdg._debug_log("Changing Working directory to: " + newPath);
		process.chdir(newPath);
		cwd = process.cwd();
	}
}

var paths = new Array();
if (!foundMain) {
	paths.push(binPath);
	paths.push(binPath+"/js");
	// if it was invoked from somewhere different, maybe a symbolic link, look there too
	var execPath = path.resolve(path.dirname(process.argv[0])); 
	if (execPath != binPath) {
		paths.push(execPath);
		paths.push(execPath+"/js");
	}
	// if one of the above wasn't the working directory, look in working directory too
	if ((execPath != cwd) && (binPath != cwd)) {
		paths.push(cwd);
		paths.push(cwd+"/js");
	}
	for (var i = 0; i < paths.length; i++) {
		var file = paths[i] + "/" + mainFilename;
		pdg._debug_log("checking: "+ file);
		if (fs.existsSync(file)) {
			foundMain = true;
			mainScript = fs.readFileSync(file, 'utf8');
			mainFile = file;
		}
	}
}

// if we were able to load a main.js from one of the many places we looked, then
// go ahead and run it on the next tick.
if (foundMain) {
	pdg._debug_log("pdg_main: found main file: " + mainFile);

	// setup a unix socket for live interactive commands
// 	connections = 0;
// 	net.createServer(function (socket) {
// 	  connections += 1;
// 	  repl.start({
// 		prompt: "pdg> ",
// 		input: socket,
// 		output: socket,
// 		terminal: true,
// 		useGlobal: true
// 	  }).on('exit', function() {
// 		socket.end();
// 	  })
// 	}).listen("/tmp/pdg-cmd-sock");

	// run main.js next time we go through the event loop
	process.nextTick(function() {
		pdg._loadScript(mainScript, mainFilename);
	});
} else {
	console.log("Couldn't find script "+mainFilename+" in resources ("+defaultResourceFile+";"
		+ binPath + "/main.dat;" + resMgr.getResourcePaths() + ") or the common paths ("
		+ paths.join(";") + ")\n\n");

	pdg.startRepl();
}
