// -----------------------------------------------
// pdg_js_macros.h
// 
// Macros used to generate Javascript bindings
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


#ifndef PDG_JS_MACROS_H_INCLUDED
#define PDG_JS_MACROS_H_INCLUDED

#include "pdg_script_macros.h"

#define _V8_NATIVE_CONSTRUCTOR_IMPL(klass)	\
klass* New_##klass(SCRIPT_ARGS) {

//#define V8SYMBOL(s)	SYMBOL(s)

//#define V8STR(s)	STR2VAL(s)
//#define V8INT(n)	INT2VAL(n)
//#define V8UINT(n)	UINT2VAL(n)
//#define V8NUM(n)	NUM2VAL(n)
//#define V8BOOL(b)	BOOL2VAL(b)

//#define V8POINT(p)  POINT2VAL(p)

#define EXTRACT_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, wrapper)  \
	klass* paramName = 0;												 	CR \
	if (args[n-1]->IsObject()) {										 	CR \
		v8::Handle<v8::Object> paramName##_ = args[n-1]->ToObject();		CR \
		wrapper* paramName##__ = jswrap::ObjectWrap::Unwrap<wrapper>(paramName##_);	CR \
		if (!paramName##__) {												CR \
			v8::Handle<v8::Value> protoVal_ = paramName##_->GetPrototype(); CR \
			if (protoVal_->IsObject()) {									CR \
				paramName##_ = protoVal_->ToObject();						CR \
				paramName##__ = jswrap::ObjectWrap::Unwrap<wrapper>(paramName##_);	CR \
			}																CR \
		}																	CR \
		if (paramName##__) {												CR \
			paramName = paramName##__->getNativeObject();					CR \
			paramName->m##klass##ScriptObj = v8::Persistent<v8::Object>::New(args[n-1]->ToObject()); CR \
		}																	CR \
	}

#define REQUIRE_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_OR_NULL_ARG(n, paramName, klass, wrapper)   \
	EXTRACT_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, wrapper)   CR \
	if (!paramName && !args[n-1]->IsNull()) {                                    CR \
		THROW_TYPE_ERR("argument "#n" must be null or an object derived from "#klass" ("#paramName")"); CR \
	}

#define REQUIRE_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, wrapper)   \
	EXTRACT_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, wrapper)   CR \
	if (!paramName) {                                                            CR \
		THROW_TYPE_ERR("argument "#n" must be an object derived from "#klass" ("#paramName")"); CR \
	}

#define EXTRACT_CUSTOM_NATIVE_OBJECT_ARG(n, paramName, klass, wrapper)   \
	klass* paramName = 0;												 	    CR \
	if (args[n-1]->IsObject()) {                                                CR \
		v8::Handle<v8::Object> paramName##_ = args[n-1]->ToObject();            CR \
		wrapper* paramName##__ = jswrap::ObjectWrap::Unwrap<wrapper>(paramName##_);     CR \
	    if (paramName##__) {                                                    CR \
	    	paramName = paramName##__->getNativeObject();                       CR \
	    }                                                                       CR \
	}

#define REQUIRE_CUSTOM_NATIVE_OBJECT_ARG(n, paramName, klass, wrapper)   \
	EXTRACT_CUSTOM_NATIVE_OBJECT_ARG(n, paramName, klass, wrapper)               CR \
	if (!paramName) {                                                            CR \
		THROW_TYPE_ERR("argument "#n" must be an object of type "#klass" ("#paramName")"); CR \
	}

#define EXTRACT_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass)  \
	EXTRACT_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, klass##Wrap)

#define REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass)   \
	REQUIRE_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_ARG(n, paramName, klass, klass##Wrap)
	
#define REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_OR_NULL_ARG(n, paramName, klass)   \
	REQUIRE_CUSTOM_NATIVE_OBJECT_OR_SUBCLASS_OR_NULL_ARG(n, paramName, klass, klass##Wrap)
	
#define REQUIRE_NATIVE_OBJECT_ARG(n, paramName, klass)   \
	REQUIRE_CUSTOM_NATIVE_OBJECT_ARG(n, paramName, klass, klass##Wrap)


#define REQUIRE_FUNCTION_ARG(n, paramName)   \
	if (!args[n-1]->IsFunction()) {                                         CR \
		THROW_TYPE_ERR("argument "#n" must be a function ("#paramName")");  CR \
	}                                                                       CR \
	v8::Handle<v8::Function> paramName = v8::Handle<v8::Function>::Cast(args[n-1]);

#define REQUIRE_OBJECT_ARG(n, paramName)   \
       if (!args[n-1]->IsObject()) {                                          CR \
               THROW_TYPE_ERR("argument "#n" must be an object ("#paramName")");  CR \
       }                                                                      CR \
       v8::Handle<v8::Object> paramName = args[n-1]->ToObject();


#endif // PDG_JS_MACROS_H_INCLUDED
