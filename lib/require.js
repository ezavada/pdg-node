// -----------------------------------------------
// require.js
//
// Browser side implementation of Node.js require() call
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

(function() {

var _cache = [];

// taken from Node
function Module(id, parent) {
  this.id = id;
  this.exports = {};
  this.parent = parent;
  if (parent && parent.children) {
    parent.children.push(this);
  }

  this.filename = null;
  this.loaded = false;
  this.children = [];
}

Module._compile = function(contents, file) {
	script = 'global.__filename = ' + JSON.stringify(file) + ';\n' +
               'global.exports = exports;\n' +
               'global.module = module;\n' +
               'global.__dirname = __dirname;\n' +
               'global.require = require;\n' +
               JSON.stringify(contents) + '\n' +
               'return exports;\n';
    this.exports = eval(script);
}

_loadScript = function(contents, file) {
	var myModule = new Module(file, module);
	_cache[file] = myModule;
	myModule.require = this._requireScript;
	try {  
		myModule._compile(contents, file);
	} catch (e) {
		delete _cache[file];
		throw(e);
	}
	return myModule.exports;
}

// replace it with our own special require function
// that can load from resource files
_requireScript = function(file) {
	// normalize the script name:
	// add .js if needed
	// remove leading ./
	if (file.substring(0, 2) == './') {
		file = file.substring(2);
	}
	// get the document path
	var path = document.location.pathname;
	path = path.substring(0, path.lastIndexOf('/')+1);
	// remove ../ from front of file name and a corresponding path segment
	// from path
	var origFile = file;
	var origPath = path;
	while (file.substring(0, 3) == '../') {
		file = file.substring(3);
		path = path.substring(0, path.length - 1);  // take off closing slash
		var pos = path.lastIndexOf('/');
		if (pos == -1) {
			throw("relative path goes past server root: "+origPath+origFile);
		}
		path = substring(0, pos + 1); // take off prior segment
	}
	// finally, make sure we have a suitable file extension
	if (file.lastIndexOf('.') == -1) {
		file += '.js';
	}
	// now we have an absolute path, check to see if it's in the cache
	var cachedModule = _cache[file];
	if (cachedModule) {
		return cachedModule.exports;
	} else if (document.location.protocol == 'file:') {
		// no way to make this work that I know of -- those
		// files had to be included in html so their defines are
		// in global namespace
		return window;
	} else {
		// not cached, do synchronous request to fetch it
		var request = new XMLHttpRequest();
		request.open('GET', path + file, false);
		request.send(); // because of "false" above, will block until the request is done 
		if (request.status != 200) {
			throw("require "+path+file+" failed. Status Code: "+request.status); 
		}
		return this._loadScript(request.responseText, file);
	}
}

if (window) {
	window.require = _requireScript;
}

})();

