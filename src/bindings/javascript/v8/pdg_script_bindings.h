// -----------------------------------------------
// pdg_script_bindings.h
// 
// Stuff to support Javascript bindings via Google's V8 Engine
//
// Written by Ed Zavada, 2012-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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


#ifndef PDG_SCRIPT_BINDINGS_H_INCLUDED
#define PDG_SCRIPT_BINDINGS_H_INCLUDED

#include "pdg_project.h"

// don't do anything unless we are actually targeting Javascript
#ifdef PDG_COMPILING_FOR_JAVASCRIPT

#define PDG_USING_V8

#include <v8.h>

#define SCRIPT_OBJECT_REF   	v8::Persistent<v8::Object>
#define SCRIPT_CLEANUP_PARAM	v8::Handle<v8::Object> obj
#define INIT_SCRIPT_OBJECT(obj)

namespace pdg {

// Destructors for these object will call these matching
// cleanup function whenever one of them is deleted.

void CleanupMemBlockScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupIEventHandlerScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupEventEmitterScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupSoundScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupFontScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupPortScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupImageScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupImageStripScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupIAnimationHelperScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupAnimatedScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupSpriteScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupSpriteLayerScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupTileLayerScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupISpriteCollideHelperScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupISpriteDrawHelperScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupISerializableScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupSerializerScriptObject(SCRIPT_CLEANUP_PARAM);
void CleanupDeserializerScriptObject(SCRIPT_CLEANUP_PARAM);

// initializes the bindings for the pdg module
void initBindings(v8::Handle<v8::Object> target);

} // end namespace pdg

// called by the bindings when pdg is completely setup in the scripting language
// implemented in pdg_node.cpp, but done this way so bindings are not Node.js dependent
extern "C" void scriptSetupCompleted();

#endif // PDG_COMPILING_FOR_JAVASCRIPT

#endif // PDG_SCRIPT_BINDINGS_H_INCLUDED
