// -----------------------------------------------
// test.js
//
// Smoke tests for pdg node module
//
// Written by Ed Zavada, 2014
// Copyright (c) 2014, Dream Rock Studios, LLC
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

var pdg = require('pdg');
var assert = require('assert')

// module
assert(typeof(pdg) === 'object');

// key singleton objects
assert(typeof(pdg.cfg) === 'object');
assert(typeof(pdg.evt) === 'object');
assert(typeof(pdg.fs) === 'object');
assert(typeof(pdg.lm) === 'object');
assert(typeof(pdg.res) === 'object');
assert(typeof(pdg.tm) === 'object');

// all the classes
assert(typeof(pdg.Animated) === 'function');
assert(typeof(pdg.Color) === 'function');
assert(typeof(pdg.ConfigManager) === 'function');
assert(typeof(pdg.CpArbiter) === 'function');
assert(typeof(pdg.CpConstraint) === 'function');
assert(typeof(pdg.CpSpace) === 'function');
assert(typeof(pdg.Deserializer) === 'function');
assert(typeof(pdg.EventEmitter) === 'function');
assert(typeof(pdg.EventManager) === 'function');
assert(typeof(pdg.FileManager) === 'function');
assert(typeof(pdg.IAnimationHelper) === 'function');
assert(typeof(pdg.IEventHandler) === 'function');
assert(typeof(pdg.ISerializable) === 'function');
assert(typeof(pdg.ISpriteCollideHelper) === 'function');
assert(typeof(pdg.Image) === 'function');
assert(typeof(pdg.ImageStrip) === 'function');
assert(typeof(pdg.LogManager) === 'function');
assert(typeof(pdg.MemBlock) === 'function');
assert(typeof(pdg.NetClient) === 'function');
assert(typeof(pdg.NetConnection) === 'function');
assert(typeof(pdg.NetServer) === 'function');
assert(typeof(pdg.Offset) === 'function');
assert(typeof(pdg.Point) === 'function');
assert(typeof(pdg.Quad) === 'function');
assert(typeof(pdg.Rect) === 'function');
assert(typeof(pdg.ResourceManager) === 'function');
assert(typeof(pdg.RotatedRect) === 'function');
assert(typeof(pdg.Serializer) === 'function');
assert(typeof(pdg.Sprite) === 'function');
assert(typeof(pdg.SpriteLayer) === 'function');
assert(typeof(pdg.TileLayer) === 'function');
assert(typeof(pdg.TimerManager) === 'function');
assert(typeof(pdg.Vector) === 'function');

if (process.versions['chipmunk']) {
	console.log('Physics: Chipmunk '+process.versions['chipmunk']);
} else {
	console.log('Physics: built-in (limited)');
}
console.log('pdg: '+process.versions['pdg']);
console.log('ok');
