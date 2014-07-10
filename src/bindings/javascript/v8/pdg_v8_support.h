// -----------------------------------------------
// pdg_v8_support.h
// 
// Stuff to support JavaScript V8 bindings
//
// Written by Ed Zavada, 2013
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


#ifndef PDG_V8_SUPPORT_H_INCLUDED
#define PDG_V8_SUPPORT_H_INCLUDED

#include "pdg_project.h"

// don't do anything unless we are actually targetting Javascript
#ifdef PDG_COMPILING_FOR_JAVASCRIPT

#include <v8.h>

#include "pdg/sys/color.h"
#include "pdg/sys/coordinates.h"

/*
#include <time.h>
*/

namespace pdg {

v8::Handle<v8::Value> v8_ThrowArgCountException(int argc, int requiredCount = 0, bool allowExtra = false);
v8::Handle<v8::Value> v8_ThrowArgTypeException(int argn, const char* mustBeStr, v8::Value* valp = 0);

// these let us set a prototype that will be used when an object of a particulr JavaScript class is created
void v8_SetOffsetPrototype(v8::Persistent<v8::Object> obj);
void v8_SetPointPrototype(v8::Persistent<v8::Object> obj);
void v8_SetVectorPrototype(v8::Persistent<v8::Object> obj);
void v8_SetRectPrototype(v8::Persistent<v8::Object> obj);
void v8_SetRotatedRectPrototype(v8::Persistent<v8::Object> obj);
void v8_SetQuadPrototype(v8::Persistent<v8::Object> obj);
void v8_SetColorPrototype(v8::Persistent<v8::Object> obj);

v8::Handle<v8::Object> v8_MakeJavascriptOffset(pdg::Offset& p);
v8::Handle<v8::Object> v8_MakeJavascriptPoint(pdg::Point& p);
v8::Handle<v8::Object> v8_MakeJavascriptVector(pdg::Vector& p);
v8::Handle<v8::Object> v8_MakeJavascriptRect(pdg::Rect& r);
v8::Handle<v8::Object> v8_MakeJavascriptRect(pdg::RotatedRect& r);
v8::Handle<v8::Object> v8_MakeJavascriptQuad(pdg::Quad& q);
v8::Handle<v8::Object> v8_MakeJavascriptColor(pdg::Color& c);

Offset  	v8_ValueToOffset(v8::Handle<v8::Value> val);
Point  		v8_ValueToPoint(v8::Handle<v8::Value> val);
Vector  	v8_ValueToVector(v8::Handle<v8::Value> val);
Rect  		v8_ValueToRect(v8::Handle<v8::Value> val);
RotatedRect v8_ValueToRotatedRect(v8::Handle<v8::Value> val);
Quad  		v8_ValueToQuad(v8::Handle<v8::Value> val);
Color  		v8_ValueToColor(v8::Handle<v8::Value> val);

v8::Handle<v8::Object> v8_ObjectCreateEmpty(void* privateDataPtr = 0);

/*bool V8_ValueIsObjectWithProperty(v8::Handle<v8::Value> objVal, JSStringRef propSymbol, v8::Handle<v8::Value>* exception);

bool V8_ValueIsFunction(v8::Handle<v8::Value> funcVal, v8::Handle<v8::Value>* exception);

JSObjectRef V8_ValueToFunction(v8::Handle<v8::Value> funcVal, v8::Handle<v8::Value>* exception);

v8::Handle<v8::Value> v8_ValueGetObjectProperty(v8::Handle<v8::Value> objVal, JSStringRef propSymbol, v8::Handle<v8::Value>* exception);

v8::Handle<v8::Value> v8_MakeValueFromCString(const char* s);

v8::Handle<v8::Object> v8_ObjectCreateEmpty(void* privateDataPtr = 0);

v8::Handle<v8::Value> v8_ExecuteScriptFile(const char *scriptPath, v8::Handle<v8::Value>* exception);

v8::Handle<v8::Value> v8_TimeToValue(time_t t, v8::Handle<v8::Value>* exception);

char* v8_CreateStringWithContentsOfFile(const char* fileName, const char* openMode = "r");

*/

bool v8_ValueIsOffset(v8::Handle<v8::Value> val);
bool v8_ValueIsPoint(v8::Handle<v8::Value> val);
bool v8_ValueIsVector(v8::Handle<v8::Value> val);
bool v8_ValueIsRect(v8::Handle<v8::Value> val, bool arrayCheck = true);
bool v8_ValueIsRotatedRect(v8::Handle<v8::Value> val);
bool v8_ValueIsQuad(v8::Handle<v8::Value> val);
bool v8_ValueIsColor(v8::Handle<v8::Value> val);

// This is here to avoid making these bindings node.js specific
// the implementation resides elsewhere (pdg_node.cpp for a node.js 
// module and it just calls node::FatalException).
// It can be implemented to do something different if these
// bindings are not being used in a node.js plugin.
void FatalException(v8::TryCatch &try_catch);


} // end namespace pdg

#endif // PDG_COMPILING_FOR_JAVASCRIPT

#endif // PDG_V8_SUPPORT_H_INCLUDED
