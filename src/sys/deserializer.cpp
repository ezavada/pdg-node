// -----------------------------------------------
//  deserializer.cpp
//
// Written by Ed Zavada, 2010-2012
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


#include "pdg_project.h"

#include "pdg/sys/deserializer.h"
#include "pdg/sys/serializable.h"
#include "pdg/sys/os.h"

#include "serializer-impl.h"

#include <limits.h>
#include <cstring>
#include <vector>
#include <map>
#include <cstdlib>

#ifndef PDG_NO_SERIALIZER_SANITY_CHECKS
#define SERIALIZER_SANITY_CHECKS
#endif

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
  #include "pdg_script_bindings.h"
  #include "pdg_script_interface.h"
#endif

#ifdef PDG_DESERIALIZER_NO_THROW
	// we can't throw, so we report errors but don't exit
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)\nOffset: %ld End: %ld", #_except, _msg, __FUNCTION__, __FILE__, __LINE__, p - mDataPtr, mDataEnd - mDataPtr); \
		DEBUG_PRINT(buf); }
#else
	// we throw exceptions on errors
	#define STREAM_SAFETY_CHECK(_cond, _msg, _except) if (!(_cond)) { char buf[1024]; buf[0] = 0;\
		std::sprintf(buf, "%s: %s at %s (%s:%d)\nOffset: %ld End: %ld", #_except, _msg, __FUNCTION__, __FILE__, __LINE__, p - mDataPtr, mDataEnd - mDataPtr); \
		throw _except(buf); }
#endif

namespace pdg {

int IDeserializer::s_TraceDepth = 0;

// maps class tags to class factory functions
typedef std::map<uint32, IDeserializer::CreateSerializableFunc> SerializableRegistryT;
SerializableRegistryT gSerializableRegistry;

#ifdef PDG_COMPILING_FOR_JAVASCRIPT
  typedef std::map< uint32, SAVED_FUNCTION > JavascriptRegistryT;
  JavascriptRegistryT gJavascriptRegistry;
#endif // PDG_COMPILING_FOR_JAVASCRIPT

#ifndef PDG_NO_64BIT
//! Deserialize an 8 byte (64 bit) value
uint64 Deserializer::deserialize_8u() {
	DESERIALIZE_IN("8u  ", 8);
	STREAM_SAFETY_CHECK(p + 8 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + 8 > mDataEnd) return 0;
	uint64 val = 0;
	val |= ((uint64)*p++ << 56);
	val |= ((uint64)*p++ << 48);
	val |= ((uint64)*p++ << 40);
	val |= ((uint64)*p++ << 32);
	val |= (*p++ << 24);
	val |= (*p++ << 16);
	val |= (*p++ << 8);
	val |= *p++;
	DESERIALIZE_OUT;
	return val;
}
#endif

//! Deserialize a 4 byte (32 bit) value
uint32 Deserializer::deserialize_4u() {
	DESERIALIZE_IN("4u  ", 4)
	STREAM_SAFETY_CHECK(p + 4 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + 4 > mDataEnd) return 0;
	uint32 val = 0;
	val |= (*p++ << 24);
	val |= (*p++ << 16);
	val |= (*p++ << 8);
	val |= *p++;
	DESERIALIZE_OUT;
	return val;
}

//! Deserialize a 3 byte (24 bit) value
uint32 Deserializer::deserialize_3u() {
	DESERIALIZE_IN("3u  ", 3)
	STREAM_SAFETY_CHECK(p + 3 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + 3 > mDataEnd) return 0;
	uint32 val = 0;
	val |= (*p++ << 16);
	val |= (*p++ << 8);
	val |= *p++;
	DESERIALIZE_OUT;
	return val;
}

//! Deserialize a 2 byte (16 bit) value
uint16 Deserializer::deserialize_2u() {
	DESERIALIZE_IN("2u  ", 2)
	STREAM_SAFETY_CHECK(p + 2 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + 2 > mDataEnd) return 0;
	uint16 val = 0;
	val |= (*p++ << 8);
	val |= *p++;
	DESERIALIZE_OUT;
	return val;
}

//! Deserialize an 1 byte (8 bit) value
uint8 Deserializer::deserialize_1u() {
	DESERIALIZE_IN("1u  ", 1)
	STREAM_SAFETY_CHECK(p + 1 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	DESERIALIZE_OUT;
	if (p + 1 > mDataEnd) return 0;
	return *p++;
}
	
bool Deserializer::deserialize_bool() {
	DESERIALIZE_IN("bool", mBoolBitOffset ? 0 : 1);
	if (mBoolBitOffset == 0) {
		STREAM_SAFETY_CHECK(p + 1 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
		mLastBoolByte = *p++;
	}
	bool val = ((mLastBoolByte & (1 << mBoolBitOffset)) != 0);
	mBoolBitOffset++;
	if (mBoolBitOffset > 7) {
		// all the bits from this byte have been read
		mLastBoolByte = 0;
		mBoolBitOffset = 0;
	}
	DESERIALIZE_OUT;
	return val;
}

//! Deserialize an unsigned int value 
uint32 Deserializer::deserialize_uint() {
	DESERIALIZE_IN("uint", 0)
	STREAM_SAFETY_CHECK(p + 1 <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + 1 > mDataEnd) return 0;
	uint32 val = *p++;
	if (val == tag_longLen) {
		val = deserialize_2();
	} else if (val == tag_veryLongLen) {
		val = deserialize_4();
	}
	DESERIALIZE_OUT;
	return val;
}

Color Deserializer::deserialize_color() {
	DESERIALIZE_IN("colr", 0)
	bool hasAlpha = deserialize_bool();
	Color c;
	if (hasAlpha) {
		c.alpha = (float)deserialize_1u()/255.0f;
	} else {
		c.alpha = 1.0f;
	}
	c.red = (float)deserialize_1u()/255.0f;
	c.green = (float)deserialize_1u()/255.0f;
	c.blue = (float)deserialize_1u()/255.0f;	
	DESERIALIZE_OUT;
	return c;
}

Offset Deserializer::deserialize_offset() {
	DESERIALIZE_IN("off ", 0)
	Offset o(0, 0);
	bool nonZero = deserialize_bool();
	if (nonZero) {
		bool xIsInt = deserialize_bool();
		bool yIsInt = deserialize_bool();	
		o.x = (xIsInt) ? deserialize_uint() : deserialize_f();
		o.y = (yIsInt) ? deserialize_uint() : deserialize_f();
	}
	DESERIALIZE_OUT;
	return o;
}

Rect Deserializer::deserialize_rect() {
	DESERIALIZE_IN("rect", 0)
	Point rightTop = deserialize_point();
	Offset leftBotOffset = deserialize_offset();
	Rect r;
	r.top = rightTop.y;
	r.left = rightTop.x;
	r.right = r.left + leftBotOffset.x;
	r.bottom = r.top + leftBotOffset.y;
	DESERIALIZE_OUT;
	return r;
}

RotatedRect Deserializer::deserialize_rotr() {
	DESERIALIZE_IN("rotr", 0)
	RotatedRect rr = deserialize_rect();
	rr.radians = deserialize_f();
	rr.centerOffset = deserialize_offset();
	DESERIALIZE_OUT;
	return rr;
}

Quad Deserializer::deserialize_quad() {
	DESERIALIZE_IN("quad", 0)
	Quad q;
	q.points[0] = deserialize_point();
	q.points[1] = deserialize_point();
	q.points[2] = deserialize_point();
	q.points[3] = deserialize_point();
	DESERIALIZE_OUT;
	return q;
}

uint32
Deserializer::deserialize_str(char* outStr, uint32 strMaxLen) {
	DESERIALIZE_IN("str ", 0)
#ifdef SERIALIZER_SANITY_CHECKS
	uint32 tag = deserialize_3u();
	STREAM_SAFETY_CHECK(tag_string == tag, "OUT OF SYNC: expected String", bad_tag);
#endif
	uint32 strLen = deserialize_uint();
	if (!strLen) return 0;  // nothing in buffer, nothing to copy
	uint32 copyLen = (strLen < (strMaxLen - 1)) ? strLen : strMaxLen - 1;  // strMaxLen includes NUL terminator space
	STREAM_SAFETY_CHECK(p + copyLen <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + copyLen > mDataEnd) {  // copyLen always <= strLen
		p = mDataEnd;
		copyLen = 0;
	} else {
		std::memcpy(outStr, p, copyLen);
		p += strLen;  // always advance to end of entire string in buffer, not just by amount we copied
	}
	DESERIALIZE_OUT;
	return copyLen;
}

uint32 
Deserializer::deserialize_strGetLen() { 
	uint8* ps = p;	
#ifdef SERIALIZER_SANITY_CHECKS
	uint32 tag = deserialize_3u();
	STREAM_SAFETY_CHECK(tag_string == tag, "OUT OF SYNC: expected String", bad_tag);
#endif
	uint32 len = deserialize_uint(); 
	p = ps;
	return len + 1; // add one for the NUL terminator
}

uint32
Deserializer::deserialize_mem(void* outMem, uint32 memMaxLen) {
	DESERIALIZE_IN("str ", 0)
#ifdef SERIALIZER_SANITY_CHECKS
	uint32 tag = deserialize_3u();
	STREAM_SAFETY_CHECK(tag_mem == tag, "OUT OF SYNC: expected Memory", bad_tag);
#endif
	uint32 memLen = deserialize_uint();
	if (!memLen) return 0;  // nothing in buffer, nothing to copy
	uint32 copyLen = (memLen < memMaxLen) ? memLen : memMaxLen;
	STREAM_SAFETY_CHECK(p + copyLen <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
	if (p + copyLen > mDataEnd) {  // copyLen always <= memLen
		p = mDataEnd;
		copyLen = 0;
	} else {
		std::memcpy(outMem, p, copyLen);
		p += memLen;  // always advance to end of entire mem block in buffer, not just by amount we copied
	}
	DESERIALIZE_OUT;
	return copyLen;
}


uint32
Deserializer::deserialize_memGetLen() {
	uint8* ps = p;	
#ifdef SERIALIZER_SANITY_CHECKS
	uint32 tag = deserialize_3u();
	STREAM_SAFETY_CHECK(tag_mem == tag, "OUT OF SYNC: expected Memory", bad_tag);
#endif
	uint32 memLen = deserialize_uint();
	p = ps;
	return memLen;
}


ISerializable*
Deserializer::deserialize_obj() {
	DESERIALIZE_IN("obj ", 0)
	ISerializable* obj = 0;
	uint32 serializationType = deserialize_3u();
	if (serializationType == tag_objectNil) {
		DESERIALIZE_OUT;
		return 0;
	} else if (serializationType == tag_objectRef) {
		uint32 index = deserialize_uint();
		obj = mDeserializedInstances[index];
		STREAM_SAFETY_CHECK(obj, "Invalid Instance Ref!! Obsolete stream?", sync_error);
		obj->addRef();
		DESERIALIZE_OUT;
	} else if (serializationType == tag_object) {
		uint32 classTag = deserialize_4();
      #ifdef PDG_COMPILING_FOR_JAVASCRIPT
       #ifdef PDG_USING_V8
      	// first we try to find a javascript constructor function in our class registry
        v8::Handle<v8::Function> constructor_ = gJavascriptRegistry[classTag];
        if (!constructor_.IsEmpty()) {
        	// we found one, so call it with no params to create a new JS Object
        	v8::Handle<v8::Value> resVal_ = constructor_->CallAsConstructor(0, 0);
        	if (resVal_.IsEmpty() || !resVal_->IsObject()) {
				v8::String::Utf8Value funcNameStr(constructor_->GetName()->ToString());
				v8::String::Utf8Value resNameStr(constructor_->GetScriptOrigin().ResourceName()->ToString());
				std::cerr << "fatal error: return value from constructor Function was not an object ("
					<< *funcNameStr << " at " << *resNameStr << ":" 
					<< constructor_->GetScriptLineNumber()+1 << ")\n";
				// this is a programming error on the part of the person writing the deserialization
				// code, since they've registered something that is not a constructor
				exit(1);
			} else {
				// if we succeeded, try to unwrap it to get to the underlying native ISerializable object
				v8::Handle<v8::Object> obj_ = resVal_->ToObject();
				ISerializableWrap* obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
				if (!obj__) { 
					// usually we can't unwrap it directly, because it will be a subclass of ISerializable
					// so we grab the prototype (obj.__proto__), which should be the ISerializable
					v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
					if (protoVal_->IsObject()) {
						obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(protoVal_->ToObject());
					}
				}
				if (obj__) {
					// at this point we know we have a wrapped ISerializable, so grab the pointer
					// to the native object and make sure the native object has a reference to the
					// newly created JS object
					obj = obj__->getNativeObject();
					obj->mISerializableScriptObj = v8::Persistent<v8::Object>::New(obj_);
					// JS object is now completely set up and ready to be deserialized
				}
			}
        }
       #endif // PDG_USING_V8
       #ifdef PDG_USING_JAVASCRIPT_CORE
         // TODO: implement serialization with JavaScriptCore
		 std::cerr << "fatal error: JS object streaming not implemented with JavaScriptCore yet!";
		 exit(1);
       #endif // PDG_USING_JAVASCRIPT_CORE
      #endif
		DESERIALIZE_OUT;  // we stop here so that when the object deserializes we can see the bits of of it
        if (!obj) {
			IDeserializer::CreateSerializableFunc create_func = gSerializableRegistry[classTag];
			if (create_func) { 
				obj = create_func();
			}
		}
		STREAM_SAFETY_CHECK(obj, "Unregistered Class!!", unknown_object);
		if (obj) {
			obj->addRef();
		}
		mDeserializedInstances.push_back(obj);
	  #ifdef SERIALIZER_SANITY_CHECKS
		uint16 numSerializedClasses = deserialize_2();
		STREAM_SAFETY_CHECK(numSerializedClasses == mDeserializedInstances.size(), "Serialized Class Count Incorrect", sync_error);
	  #endif
		uint32 objLen = deserialize_uint();
	    STREAM_SAFETY_CHECK(p + objLen <= mDataEnd, "OUT OF SYNC: insufficient data remaining in buffer", out_of_data);
		if (p + objLen > mDataEnd) return 0;
		if (obj) {
			obj->deserialize(this);
		} else {
			p += objLen; // skip the object we couldn't deserialize
		}
	} else {
		STREAM_SAFETY_CHECK(false, "OUT OF SYNC: expected Object or Object Ref", bad_tag);
	}
	return obj;
}

void Deserializer::setDataPtr(void* ptr, uint32 ptrSize) {
	mDataPtr = (uint8*)ptr; 
	mDataSize = ptrSize;
	mDataEnd = (uint8*)ptr + ptrSize; 
	DEBUG_PRINT("Deserializer Initialized with Ptr [%p] size [%ld] bytes", mDataPtr, mDataEnd - mDataPtr);
	p = (uint8*)ptr;
}
	
Deserializer::Deserializer()
 :  mDataPtr(0),
	mDataEnd(0),
	p(0),
	mDataSize(0),
	mLastBoolByte(0),
	mBoolBitOffset(0),
	mDeserializedInstances()
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mDeserializerScriptObj);
#endif
}

Deserializer::Deserializer(void* ptr, uint32 ptrSize)
 :  mDataPtr(0),
	mDataEnd(0),
	p(0),
	mDataSize(0),
	mLastBoolByte(0),
	mBoolBitOffset(0),
	mDeserializedInstances()
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mDeserializerScriptObj);
#endif
	setDataPtr(ptr, ptrSize);
}

Deserializer::~Deserializer() {
	if (mDataPtr) {
		std::free(mDataPtr);
		mDataPtr = 0;
	}
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupDeserializerScriptObject(mDeserializerScriptObj);
#endif
}
	
char* Deserializer::statusDump(int hiliteBytes) {
	static char buf[1024];
	char* targBuf = buf + 22;
	long blockNum = (p - mDataPtr)/16;
	long blockStart = 16 * blockNum;
	long blockLen = 20;
	if ((mDataPtr + blockStart + blockLen) > mDataEnd) blockLen = (long)(mDataEnd - (mDataPtr + blockStart));
	long hiliteStart = (p - mDataPtr) - blockStart;
	std::sprintf(buf, "@%05lu/%05lu:  %04lX | ", p - mDataPtr, mDataEnd - mDataPtr, blockStart);
	OS::binaryDump(targBuf, 1000, (char*)(mDataPtr + blockStart), blockLen, 20, hiliteStart, hiliteBytes);
	return buf;
}

void
IDeserializer::registerClass(uint32 classTag, CreateSerializableFunc classNewFunc) {
	gSerializableRegistry[classTag] = classNewFunc;
}

#ifdef PDG_COMPILING_FOR_JAVASCRIPT
void 
Deserializer::registerScriptClass(uint32 classTag, FUNCTION_REF constructorFunc) {
  #ifdef PDG_USING_V8
	gJavascriptRegistry[classTag] = v8::Persistent<v8::Function>::New(constructorFunc);
  #endif
  #ifdef PDG_USING_JAVASCRIPT_CORE
  	gJavascriptRegistry[classTag] = constructorFunc;
  	JSValueProtect(gMainContext, constructorFunc);
  #endif
}
#endif //PDG_COMPILING_FOR_JAVASCRIPT

} // end namespace pdg
