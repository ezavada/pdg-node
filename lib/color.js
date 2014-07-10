// -----------------------------------------------
// color.js
//
// Definitions for rgb color
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

// this is one of the few javascript files that are built-in
// to the pdg standalone app but also part of the pure 
// javascript implementation

if ((typeof process != "undefined") && ((typeof process.pdg != "undefined")
	|| (typeof process.versions['jsc'] != "undefined"))) {
	// node in standalone, classify becomes a built-in
	require('classify');
} else {
	// node add-on or in-browser
	require('./classify');
	require('./dump');
}

var colorNames = new Array("aqua", "black", "blue", "fuchsia", 
	"gray", "grey", "green", "lime", "maroon", "navy", "olive", "purple", 
	"red", "silver", "teal", "white", "yellow");
var colorValues = new Array( 0x00ffff, 0x000000, 0x0000ff, 0xff00ff, 
	0x808080, 0x808080, 0x008000, 0x00ff00, 0x800000, 0x000080, 0x808000, 0x800080,
	0xff0000, 0xc0c0c0, 0x008080, 0xffffff, 0xffff00 );

// -----------------------------------------------------------------------------------
// Color
// -----------------------------------------------------------------------------------
// An RGB Color with alpha

classify('Color', function() {
	//! new Color() create black
	//! new Color(c): create a color from a single 32 bit value
	//! new Color(name): create a color from a css color name
	//! new Color(r, g, b): create a color
	//! new Color(r, g, b, a): create a color with alpha
    def('initialize', function(ir, ig, ib, ia) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("create and color and set rgb values", arguments, "undefined", 2, "({ () | (number c) | (string colorstr) | (number r, number g, number b, number alpha = 1) })");
			}
			if (arguments.length == 1) {
				var c = ir;
				if (ir instanceof String || typeof(ir) == "string") {
					if (ir[0] == '#') {
						// convert string with #hhhhhh or #hhh (css style) value
						var red = 0, green = 0, blue = 0;
						if (ir.length == 4) {  // 4 char str
							red = parseInt(ir[1], 16);
							green = parseInt(ir[2], 16);
							blue = parseInt(ir[3], 16);
							red = (red << 4) | red;
							green = (green << 4) | green;
							blue = (blue << 4) | blue;
						} else if (ir.length == 7) { // 7 char str
							red = parseInt(ir.substr(1, 2), 16);
							green = parseInt(ir.substr(3, 2), 16);
							blue = parseInt(ir.substr(5, 2), 16);
						} else {
							throw("Color from String must be: '#rgb', '#rrggbb', or css color name" );
						}
						c = (red << 16) | (green << 8) | blue | 0xff000000;
					} else {
						//look up the color from the name
						c = 0;
						var found = false;
						for (var i = 0; i < 17; i++) {
							if (ir == colorNames[i]) {
								c = colorValues[i] | 0xff000000;
								found = true;
								break;
							}
						}
						if (!found) {
							throw("Invalid color name \"" + ir + "\". Valid names are: " + colorNames);
						}
					}
				}
				this.alpha = ((c>>24)&0xff)/255.0;
				this.red = ((c>>16)&0xff)/255.0; 
				this.green = ((c>>8)&0xff)/255.0; 
				this.blue = (c&0xff)/255.0; 
			} else {
				this.red = (ir) ? ir : 0; 
				this.green = (ig) ? ig : 0;
				this.blue = (ib) ? ib : 0;
				this.alpha = (ia) ? ia : 1.0;
			}
		});
	// operators
	//! return true if this point is equal to the other
	def('equals', function(color) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this color is equal to the other (ignoring alpha)", arguments, "boolean", 1, "([object Color] color)");
			}
			return ((this.red == color.red) && (this.green == color.green) && (this.blue == color.blue));
		});
	def('notEquals', function(color) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("return true if this color is not equal to the other (ignoring alpha)", arguments, "boolean", 1, "([object Color] color)");
			}
			return ((this.red != color.red) || (this.green != color.green) || (this.blue != color.blue));
		});
	def('assign', function(color) {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("set this color equal to the given color", arguments, "[object Color]", 1, "([object Color] color)");
			}
			this.alpha = color.alpha; 
			this.red = color.red; 
			this.green = color.green;
			this.blue = color.blue;
			return this;
		});
	def('convertToGrayscale', function() {
			if ((arguments.length == 1) && (arguments[0] == null)) { 
				return methodSignature("convert this color to a matching shade of grey", arguments, "undefined", 0, "()");
			}
			var v = (red + green + blue)/3; 
			red = v;
			green = v;
			blue = v;
		});
	def('toString', function() {
			return "Color("+this.red+","+this.green+","+this.blue+")";
		});
});


if(!(typeof exports === 'undefined')) {
    exports.Color = Color;
}


