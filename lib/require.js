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

window.require = function(module) {
	var url = window.require.resolve(module);

	if (require.cache[url]) {

		// already cached, just return it
		return require.cache[url];

	} else {
	
        var exports = new Object();
        var source = "";
        var type = null;

		window.require.cache[url] = exports;

	    if (typeof FS=="object") {

            // We have emscripten runtime environment, which emulates
            // a file system via a number of different methods. So we will
            // just use it to read the file.
            var source = FS.readFile(url, { encoding: 'utf8' });
            var type = url.split('.').pop();

        } else {
	
	        // use a synchronous XMLHttpRequest      
	        var request = new XMLHttpRequest();
            request.open('GET', url, false);
            request.send();
            if (request.status != 200) {
                throw( Error("require() "+url+" failed. Status Code: "+request.status +' ('+this.statusText+')') ); 
            }
            source = this.responseText;
            type = (this.getResponseHeader('content-type').indexOf('application/json') != -1) ? 'json' : 'js';
        
        }

        // we have the script by one of the above methods

        if (type == "json") {

            exports = JSON.parse(source);

        } else {

            var code = source.match(/^\s*(?:(['"]use strict['"])(?:;\r?\n?|\r?\n))?\s*((?:.*\r?\n?)*)/);
            eval('(function(){'+code[1]+';var exports=window.require.cache[\''+url+'\'];\n\n'+code[2]+'\n})();\n//@ sourceURL='+url+'\n');

        }
	    return exports;
	}
}

window.require.resolve = function(module) {
	var r = module.match(/^(\.{0,2}\/)?([^\.]*)(\..*)?$/);
	return (r[1]?r[1]:'/js_modules/')+r[2]+(r[3]?r[3]:(r[2].match(/\/$/)?'index.js':'.js'));
}

window.require.cache = new Object();
})();

