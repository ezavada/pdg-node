// -----------------------------------------------
// pdg_jsc_support.cpp
// 
// Stuff to support Javascript bindings
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

#include "pdg_project.h"

#include "pdg_v8_support.h"
#include "pdg_script_macros.h"
#include "memblock.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstring>

namespace pdg {

static v8::Local<v8::Value>  MakeCppOffset(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Offset &o, bool canFail = false);
static v8::Local<v8::Value>  MakeCppPoint(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Point &p, bool canFail = false);
//static v8::Local<v8::Value>  MakeCppVector(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Vector &v, bool canFail = false);
static v8::Local<v8::Value>  MakeCppRect(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Rect &r, bool canFail = false);
static v8::Local<v8::Value>  MakeCppRect(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::RotatedRect &r, bool canFail = false);
static v8::Local<v8::Value>  MakeCppQuad(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Quad &q, bool canFail = false);
static v8::Local<v8::Value>  MakeCppColor(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Color &c, bool canFail = false);

#define X_Symbol _V8_STR("x")
#define Y_Symbol _V8_STR("y")
#define Top_Symbol _V8_STR("top")
#define Left_Symbol _V8_STR("left")
#define Bottom_Symbol _V8_STR("bottom")
#define Right_Symbol _V8_STR("right")
#define Height_Symbol _V8_STR("height")
#define Width_Symbol _V8_STR("width")
#define TopLeft_Symbol _V8_STR("topleft")
#define BottomRight_Symbol _V8_STR("bottomright")
#define Radians_Symbol _V8_STR("radians")
#define CenterOffset_Symbol _V8_STR("centeroffset")
#define Points_Symbol _V8_STR("points")

#define Red_Symbol _V8_STR("red")
#define Green_Symbol _V8_STR("green")
#define Blue_Symbol _V8_STR("blue")
#define Alpha_Symbol _V8_STR("alpha")

v8::Persistent<v8::Object> gOffsetPrototype;
v8::Persistent<v8::Object> gPointPrototype;
v8::Persistent<v8::Object> gVectorPrototype;
v8::Persistent<v8::Object> gRectPrototype;
v8::Persistent<v8::Object> gRotatedRectPrototype;
v8::Persistent<v8::Object> gQuadPrototype;
v8::Persistent<v8::Object> gColorPrototype;
v8::Persistent<v8::Object> gNetServerPrototype;
v8::Persistent<v8::Object> gNetClientPrototype;
v8::Persistent<v8::Object> gNetConnectionPrototype;

const char* v8_GetFunctionName(v8::Local<v8::Function> func) {
    if (func->IsNull()) return "NULL";
    static std::string result_;
    v8::String::Utf8Value funcNameStr(func->GetName()->ToString());
    result_ = *funcNameStr;
    return result_.c_str();
}

const char* v8_GetFunctionFileAndLine(v8::Local<v8::Function> func) {
    if (func->IsNull()) return "??";
    static std::ostringstream result_;
    v8::String::Utf8Value resNameStr(func->GetScriptOrigin().ResourceName()->ToString());
    result_.str("");  // clear the steam
    result_ << *resNameStr << ":" << func->GetScriptLineNumber()+1;
    return result_.str().c_str();
}
const char* v8_GetObjectClassName(v8::Local<v8::Object> obj) {
    if (obj->IsNull()) return "NULL";
    static std::string result_;
    v8::String::Utf8Value nameStr(obj->GetConstructorName());
    result_ = *nameStr;
    return result_.c_str();
}

void v8_ThrowArgCountException(v8::Isolate* isolate, int argc, int requiredCount, bool allowExtra) {
	std::ostringstream excpt_;
	excpt_ << "argument count mismatch: expected ";
	if (allowExtra) {
		excpt_ << "at least ";
	}
	excpt_ << requiredCount << ", but got ";
	if (allowExtra) {
		excpt_ << "only ";
	}
	excpt_ << argc << " arguments.";
	if (argc < requiredCount) {
		THROW_SYNTAX_ERR( excpt_.str().c_str() );
	} else {
		THROW_ERR( excpt_.str().c_str() );
	}
}

void v8_ThrowArgTypeException(v8::Isolate* isolate, int argn, const char* mustBeStr, v8::Value* valp) {
	std::ostringstream excpt_s;
    excpt_s << "argument " << argn << " must be ";
    if (valp && valp->IsUndefined()) {
    	excpt_s << "an object of type " << mustBeStr << ", but got undefined. Did you pass in \""
    		<< mustBeStr << "()\" instead of \"new " << mustBeStr << "()\"?";
    } else {
    	excpt_s << mustBeStr << ".";
    }
	THROW_TYPE_ERR( excpt_s.str().c_str() );
}

// these let us set a prototype that will be used when an object of a particulr JavaScript class is created
void v8_SetOffsetPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gOffsetPrototype.Reset(isolate, obj);
}

void v8_SetPointPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gPointPrototype.Reset(isolate, obj);
}

void v8_SetVectorPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gVectorPrototype.Reset(isolate, obj);
}

void v8_SetRectPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gRectPrototype.Reset(isolate, obj);
}

void v8_SetRotatedRectPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gRotatedRectPrototype.Reset(isolate, obj);
}

void v8_SetQuadPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gQuadPrototype.Reset(isolate, obj);
}

void v8_SetColorPrototype(v8::Local<v8::Object> obj) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	gColorPrototype.Reset(isolate, obj);
}

v8::Local<v8::Object> v8_MakeJavascriptOffset(v8::Isolate* isolate, pdg::Offset& o) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gOffsetPrototype);
    if (!gOffsetPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(X_Symbol,NUM2VAL(o.x));
	obj->Set(Y_Symbol,NUM2VAL(o.y));
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptPoint(v8::Isolate* isolate, pdg::Point& p) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gPointPrototype);
  	if (!gPointPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(X_Symbol,NUM2VAL(p.x));
	obj->Set(Y_Symbol,NUM2VAL(p.y));
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptVector(v8::Isolate* isolate, pdg::Vector& v) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gVectorPrototype);
  	if (!gVectorPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(X_Symbol,NUM2VAL(v.x));
	obj->Set(Y_Symbol,NUM2VAL(v.y));
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptRect(v8::Isolate* isolate, pdg::Rect& r) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gRectPrototype);
  	if (!gRectPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(Left_Symbol,NUM2VAL(r.left));
	obj->Set(Top_Symbol,NUM2VAL(r.top));
	obj->Set(Right_Symbol,NUM2VAL(r.right));
	obj->Set(Bottom_Symbol,NUM2VAL(r.bottom));
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptRect(v8::Isolate* isolate, pdg::RotatedRect& r) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gRotatedRectPrototype);
  	if (!gRotatedRectPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(Left_Symbol,NUM2VAL(r.left));
	obj->Set(Top_Symbol,NUM2VAL(r.top));
	obj->Set(Right_Symbol,NUM2VAL(r.right));
	obj->Set(Bottom_Symbol,NUM2VAL(r.bottom));
	obj->Set(Radians_Symbol,NUM2VAL(r.radians));
	v8::Local<v8::Object> r_p_ = v8_MakeJavascriptOffset(isolate, r.centerOffset);
	obj->Set(CenterOffset_Symbol, r_p_);
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptQuad(v8::Isolate* isolate, pdg::Quad& q) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Array> arr = v8::Array::New(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gQuadPrototype);
  	if (!gQuadPrototype.IsEmpty()) obj->SetPrototype(proto);
  	for (int i = 0; i<4; i++) {
		v8::Local<v8::Object> q_p_ = v8_MakeJavascriptPoint(isolate, q.points[i]);
		arr->Set(v8::Integer::New(isolate, i), q_p_);
	}
	obj->Set(Points_Symbol, arr);
  	return scope.Escape(obj);
}

v8::Local<v8::Object> v8_MakeJavascriptColor(v8::Isolate* isolate, pdg::Color& c) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> proto = v8::Local<v8::Object>::New(isolate, gColorPrototype);
  	if (!gColorPrototype.IsEmpty()) obj->SetPrototype(proto);
	obj->Set(Red_Symbol,NUM2VAL(c.red));
	obj->Set(Green_Symbol,NUM2VAL(c.green));
	obj->Set(Blue_Symbol,NUM2VAL(c.blue));
	obj->Set(Alpha_Symbol,NUM2VAL(c.alpha));
  	return scope.Escape(obj);
}

v8::Local<v8::Value> MakeCppOffset(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Offset& o, bool canFail) {
    v8::EscapableHandleScope scope(isolate);
	if (val->IsArray()) {
		v8::Local<v8::Array> arr_ = v8::Local<v8::Array>::Cast(val);
		if (arr_->Length() == 2) {
			// convert 2 element array to point assuming x,y order
			o.x = arr_->Get(0)->NumberValue();
			o.y = arr_->Get(1)->NumberValue();
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Point from Array must be: 2 Numbers [x, y]");
		}
	} else if (val->IsObject()) {
		// convert object with "x" and "y" values
		v8::Local<v8::Object> obj_ = val->ToObject();
		if (obj_->Has(X_Symbol) && obj_->Has(Y_Symbol)) {
			o.x = obj_->Get(X_Symbol)->NumberValue();
			o.y = obj_->Get(Y_Symbol)->NumberValue();
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Point from Object must be: {x:Number, y:Number}");
		}
	} else {
		return scope.Escape( BOOL2VAL(false) );
  	}
//	SCRIPT_DEBUG_ONLY( OS::_DOUT( "Point: (x:%.1f, y:%.1f)", o.x, o.y); )
	return scope.Escape( BOOL2VAL(true) );
}

v8::Local<v8::Value> MakeCppPoint(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Point& p, bool canFail) {
	return MakeCppOffset(isolate, val, p, canFail);
}

// v8::Local<v8::Value> MakeCppVector(v8::Local<v8::Value> val, pdg::Vector& v, bool canFail) {
// 	return MakeCppOffset(val, v, canFail);
// }
// 

v8::Local<v8::Value> MakeCppRect(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Rect& r, bool canFail) {
    v8::EscapableHandleScope scope(isolate);
  	pdg::Point p;
	if (val->IsArray()) {
		v8::Local<v8::Array> arr_ = v8::Local<v8::Array>::Cast(val);
		if (arr_->Length() == 2) {
			// convert 2 element array of points assuming leftTop, rightBottom order
			v8::Local<v8::Value> resVal = MakeCppPoint(isolate, arr_->Get(0), p, canFail);
			if (resVal->IsTrue()) {
				r.left = p.x;
				r.top = p.y;			
				resVal = MakeCppPoint(isolate, arr_->Get(1), p, canFail);
				if (!resVal->IsTrue()) {
				    return scope.Escape(resVal);
				}
				r.right = p.x;
				r.bottom = p.y;
			} else {
				// convert 2 element array to width, height
				r.top = 0;
				r.left = 0;
				r.right = arr_->Get(0)->NumberValue();
				r.bottom = arr_->Get(1)->NumberValue();
			}
		} else if (arr_->Length() == 4) {
			// convert 4 element array to left, top, right, bottom (ie: x1, y1, x2, y2)
			r.left = arr_->Get(0)->NumberValue();
			r.top = arr_->Get(1)->NumberValue();
			r.right = arr_->Get(2)->NumberValue();
			r.bottom = arr_->Get(3)->NumberValue();
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Rect from Array must be: 2 points [topLeft, botRight], or "
					"2 Numbers [width, height], or 4 Numbers [left, top, right, bottom]");
		}
	} else if (val->IsObject()) {
		v8::Local<v8::Object> obj_ = val->ToObject();
		if (obj_->Has(Top_Symbol) && obj_->Has(Left_Symbol) 
		  && obj_->Has(Right_Symbol) && obj_->Has(Bottom_Symbol)) {
			// convert object with "top" "left" "right" and "bottom" values
			r.left = obj_->Get(Left_Symbol)->NumberValue();
			r.top = obj_->Get(Top_Symbol)->NumberValue();
			r.right = obj_->Get(Right_Symbol)->NumberValue();
			r.bottom = obj_->Get(Bottom_Symbol)->NumberValue();
		} else if (obj_->Has(Width_Symbol) && obj_->Has(Height_Symbol)) {
			// convert object with "width" and "height" values (and optional "topleft")
			if (obj_->Has(TopLeft_Symbol)) {
				v8::Local<v8::Value> resVal = MakeCppPoint(isolate, obj_->Get(TopLeft_Symbol), p, canFail);
				if (!resVal->IsTrue()) {
				    return scope.Escape(resVal);
				}
				r.left = p.x;
				r.top = p.y;
			} else {
				r.left = 0;
				r.top = 0;
			}
			r.setWidth( obj_->Get(Width_Symbol)->NumberValue() );
			r.setHeight( obj_->Get(Height_Symbol)->NumberValue() );
		} else if (obj_->Has(TopLeft_Symbol) && obj_->Has(BottomRight_Symbol)) {
			// convert object with "topleft" and "bottomright" values
			v8::Local<v8::Value> resVal = MakeCppPoint(isolate, obj_->Get(TopLeft_Symbol), p, canFail);
			if (!resVal->IsTrue()) {
				return scope.Escape(resVal);
			}
			r.left = p.x;
			r.top = p.y;
			resVal = MakeCppPoint(isolate, obj_->Get(BottomRight_Symbol), p, canFail);
			if (!resVal->IsTrue()) {
			    return scope.Escape(resVal);
			}
			r.right = p.x;
			r.bottom = p.y;
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Rect from Object must be: \n"
				"  { top:n, right:n, bottom:n, left:n [, radians:n] }, or \n"
				"  { height:n, width:n [, topleft : {x:n, y:n} ] [, radians:n] }, or \n"
				"  { topleft: {x:n, y:n}, bottomright: {x:n, y:n} [, radians:n] }" );
		}
	} else {
		return scope.Escape( BOOL2VAL(false) );
	}
// 	SCRIPT_DEBUG_ONLY( OS::_DOUT( "Rect: (t:%.1f, l:%.1f, r:%.1f, b:%.1f)",
// 			r.top, r.left, r.right, r.bottom ); )
	return scope.Escape( BOOL2VAL(true) );
}

v8::Local<v8::Value> MakeCppRect(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::RotatedRect& rr, bool canFail) {
    v8::EscapableHandleScope scope(isolate);
  	pdg::Rect r;
  	if (MakeCppRect(isolate, val, r, canFail)->IsTrue()) {
  		// always accept a rectangle as a Quad
  		rr = pdg::RotatedRect(r);
  		// check for optional rotation
  		if (val->IsObject()) {
			v8::Local<v8::Object> obj_ = val->ToObject();
			if (obj_->Has(Radians_Symbol)) {
				float radians = obj_->Get(Radians_Symbol)->NumberValue();
				rr.radians = radians;
			}
			pdg::Point centerPtOffset;
			if (obj_->Has(CenterOffset_Symbol)) {
				MakeCppPoint(isolate, obj_->Get(CenterOffset_Symbol), centerPtOffset, canFail);
			}
			rr.centerOffset = centerPtOffset;
		}
  	} else if (canFail) {
		return scope.Escape( BOOL2VAL(false) );
  	} else {
		THROW_SYNTAX_ERR("Rect from Object must be: \n"
			"  { top:n, right:n, bottom:n, left:n [, radians:n] [, centeroffset: Point] }, or \n"
			"  { height:n, width:n [, topleft : {x:n, y:n} ] [, radians:n] [, centeroffset: Point] }, or \n"
			"  { topleft: {x:n, y:n}, bottomright: {x:n, y:n} [, radians:n] [, centeroffset: Point] }" );
  	}
// 	SCRIPT_DEBUG_ONLY( OS::_DOUT( "RotatedRect: (t:%.1f, l:%.1f, r:%.1f, b:%.1f, r:%.1f, co.x:%.1f, co.y:%.1f)",
// 			rr.top, rr.left, rr.right, rr.bottom, rr.radians, rr.centerOffset.x, rr.centerOffset.y ); )
	return scope.Escape( BOOL2VAL(true) );
}
  	
v8::Local<v8::Value> MakeCppQuad(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Quad& q, bool canFail) {
    v8::EscapableHandleScope scope(isolate);
  	pdg::Rect r;
  	if (MakeCppRect(isolate, val, r, canFail)->IsTrue()) {
  		// always accept a rectangle as a Quad
  		q = pdg::Quad(r);
  		// check for optional rotation
  		if (val->IsObject()) {
			v8::Local<v8::Object> obj_ = val->ToObject();
			if (obj_->Has(Radians_Symbol)) {
				float radians = obj_->Get(Radians_Symbol)->NumberValue();
				pdg::Point centerPtOffset;
				if (obj_->Has(CenterOffset_Symbol)) {
					MakeCppPoint(isolate, obj_->Get(CenterOffset_Symbol), centerPtOffset, canFail);
				}
				q.rotate(radians, centerPtOffset);
			}
		}
  	} else if (val->IsArray()) {
		v8::Local<v8::Array> arr_ = v8::Local<v8::Array>::Cast(val);
		if (arr_->Length() == 4) {
			// convert array of points into quad
			v8::Local<v8::Value> resVal;
			for (int i = 0; i < 4; i++) {
				resVal = MakeCppPoint(isolate, arr_->Get(i), q.points[i], canFail);
				if (!resVal->IsTrue())  {
				    return scope.Escape(resVal);
				}
			}
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Quad from Array must be: Array[4] of points");
		}
	} else {
		// TODO: handle JavaScript pdg.Quad() object, ie:
		// { points: [ { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 } ] }
		return scope.Escape( BOOL2VAL(false) );
	}
// 	SCRIPT_DEBUG_ONLY( OS::_DOUT( "Quad: (%.1f, %.1f), (%.1f, %.1f), (%.1f, %.1f), (%.1f, %.1f)",
// 			q.points[0].x, q.points[0].y, q.points[1].x, q.points[1].y, 
// 			q.points[2].x, q.points[2].y, q.points[3].x, q.points[3].y ); )
	return scope.Escape( BOOL2VAL(true) );
}

v8::Local<v8::Value> MakeCppColor(v8::Isolate* isolate, v8::Local<v8::Value> val, pdg::Color& c, bool canFail) {
    v8::EscapableHandleScope scope(isolate);
  	if (val->IsUint32()) {
		// convert uint32 to color by breaking apart elements
		c = val->Uint32Value();
  	} else if (val->IsArray()) {
		// convert 3 or 4 element array to color assuming RGBA order
		v8::Local<v8::Array> arr_ = v8::Local<v8::Array>::Cast(val);
		if (arr_->Length() == 3 || arr_->Length() == 4) {
			c.red = arr_->Get(0)->NumberValue();
			c.green = arr_->Get(1)->NumberValue();
			c.blue = arr_->Get(2)->NumberValue();
			if (arr_->Length() == 4) {
				c.alpha = arr_->Get(3)->NumberValue();
			} else {
				c.alpha = 1.0f;
			}
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Color from Array must be: 3 or 4 Numbers [red, green, blue] or "
					"[red, green, blue, alpha]");
		}
	} else if (val->IsString()) {
		v8::String::Utf8Value strVal(val->ToString());
		const char* str = *strVal;
		if (str[0] == '#') {
			// convert string with #hhhhhh or #hhh (css style) value
			int red = 0, green = 0, blue = 0;
			if (str[4] == 0) {  // 3 char str
				sscanf(str, "#%1x%1x%1x", &red, &green, &blue);
				red = (red << 8) & red;
				green = (green << 8) & green;
				blue = (blue << 8) & blue;
			} else if (str[7] == 0) { // 7 char str
				sscanf(str, "#%2x%2x%2x", &red, &green, &blue);
			} else {
				THROW_SYNTAX_ERR("Color from String must be: '#rgb', '#rrggbb', or css color name" );
			}
			c = Color(red, green, blue);
		} else {
			// convert string with common css color names 
			const int NUM_COLORS = 17;
			const char* colorNames[NUM_COLORS] = {"aqua", "black", "blue", "fuchsia", 
				"gray", "grey", "green", "lime", "maroon", "navy", "olive", "purple", 
				"red", "silver", "teal", "white", "yellow"};
			uint32 colorValues[NUM_COLORS] = { 0x00ffff, 0x000000, 0x0000ff, 0xff00ff, 
				0x808080, 0x808080, 0x008000, 0x00ff00, 0x800000, 0x000080, 0x808000, 0x800080,
				0xff0000, 0xc0c0c0, 0x008080, 0xffffff, 0xffff00 };
			bool found = false;
			for (int i = 0; i<NUM_COLORS; i++) {
				if (strcmp(str, colorNames[i]) == 0) {
					c = colorValues[i] | 0xff000000;
					found = true;
					break;
				}
			}
			if (!found) {
				if (canFail) {
			        return scope.Escape( BOOL2VAL(false) );
				} else {
					std::ostringstream msg;
					msg << "Invalid color name \"" << str << "\". Valid names are:";
					for (int i = 0; i<NUM_COLORS; i++) {
						msg << " " << colorNames[i];
					}
					THROW_RANGE_ERR(msg.str().c_str());
				}
			}
		}
	} else if (val->IsObject()) {
		// convert object with "red" "green" "blue" and optional "alpha" values
		v8::Local<v8::Object> obj_ = val->ToObject();
		if (obj_->Has(Red_Symbol) && obj_->Has(Green_Symbol) 
		  && obj_->Has(Blue_Symbol)) {
			c.red = obj_->Get(Red_Symbol)->NumberValue();
			c.green = obj_->Get(Green_Symbol)->NumberValue();
			c.blue = obj_->Get(Blue_Symbol)->NumberValue();
			if (obj_->Has(Alpha_Symbol)) {
				c.alpha = obj_->Get(Alpha_Symbol)->NumberValue();
			} else {
				c.alpha = 1.0f;
			}
		} else if (canFail) {
			return scope.Escape( BOOL2VAL(false) );
		} else {
			THROW_SYNTAX_ERR("Color from Object must be: { red:n, green:n, blue:n [, alpha:n] }" );
		}
	} else {
		return scope.Escape( BOOL2VAL(false) );
	}
// 	SCRIPT_DEBUG_ONLY( OS::_DOUT( "Color: (r:%.1f, g:%.1f, b:%.1f, a:%.1f)", 
// 			c.red, c.green, c.blue, c.alpha ); )
	return scope.Escape( BOOL2VAL(true) );
}

bool v8_ValueIsOffset(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Offset o;
	return MakeCppOffset(isolate, val, o, true)->IsTrue();
}

bool v8_ValueIsPoint(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	return v8_ValueIsOffset(isolate, val);
}

bool v8_ValueIsVector(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	return v8_ValueIsOffset(isolate, val);
}

bool v8_ValueIsRect(v8::Isolate* isolate, v8::Local<v8::Value> val, bool arrayCheck) {
	pdg::Rect r;
	return MakeCppRect(isolate, val, r, true)->IsTrue();
}

bool v8_ValueIsRotatedRect(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::RotatedRect rr;
	return MakeCppRect(isolate, val, rr, true)->IsTrue();
}

bool v8_ValueIsQuad(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Quad q;
	return MakeCppQuad(isolate, val, q, true)->IsTrue();
}

bool v8_ValueIsColor(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Color c;
	return MakeCppColor(isolate, val, c, true)->IsTrue();
}

Offset  	
v8_ValueToOffset(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Offset o;
	MakeCppOffset(isolate, val, o, true);
	return o;
}

Point  		
v8_ValueToPoint(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	return v8_ValueToOffset(isolate, val);
}

Vector  	
v8_ValueToVector(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	return v8_ValueToOffset(isolate, val);
}

Rect  		
v8_ValueToRect(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Rect r;
	MakeCppRect(isolate, val, r, true);
	return r;
}

RotatedRect 
v8_ValueToRotatedRect(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::RotatedRect rr;
	MakeCppRect(isolate, val, rr, true);
	return rr;
}

Quad  		
v8_ValueToQuad(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Quad q;
	MakeCppQuad(isolate, val, q, true);
	return q;
}

Color  		
v8_ValueToColor(v8::Isolate* isolate, v8::Local<v8::Value> val) {
	pdg::Color c;
	MakeCppColor(isolate, val, c, true);
	return c;
}

v8::Local<v8::Object> v8_ObjectCreateEmpty(v8::Isolate* isolate, void* privateDataPtr) {
    v8::EscapableHandleScope scope(isolate);
  	v8::Local<v8::Object> obj;
  	if (privateDataPtr) {
		v8::Local<v8::ObjectTemplate> t = v8::ObjectTemplate::New(isolate); 
        t->SetInternalFieldCount(1);
        obj = t->NewInstance();
        assert(obj->InternalFieldCount() > 0);
  		obj->SetAlignedPointerInInternalField(0, privateDataPtr);
  	} else {
  		obj = v8::Object::New(isolate);
  	}
	return scope.Escape( obj );
}

} // end namespace pdg

