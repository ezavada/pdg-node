// -----------------------------------------------
//  serializer.cpp
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

#include "pdg/sys/serializer.h"
#include "pdg/sys/serializable.h"
#include "pdg/sys/os.h"

#include "serializer-impl.h"

#include <limits.h>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include <fstream>

#define SERIALIZER_MALLOC_BLOCK_SIZE 1024

namespace pdg {

int ISerializer::s_TraceDepth = 0;
bool ISerializer::s_DebugMode = false;


#ifndef PDG_NO_64BIT
//! Serialize an 8 byte (64 bit) value into a buffer
void   Serializer::serialize_8u(uint64 val) {
	ensureSpace(8);
	SERIALIZE_START;
	*p++ = (val >> 56) & 0xff;
	*p++ = (val >> 48) & 0xff;
	*p++ = (val >> 40) & 0xff;
	*p++ = (val >> 32) & 0xff;
	*p++ = (val >> 24) & 0xff;
	*p++ = (val >> 16) & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
	SERIALIZED("8u  ", 8);
}
#endif

//! Serialize a 4 byte (32 bit) value into a buffer
void   Serializer::serialize_4u(uint32 val) {
	ensureSpace(4);
	SERIALIZE_START;
	*p++ = (val >> 24) & 0xff;
	*p++ = (val >> 16) & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
	SERIALIZED("4u  ", 4);
}

//! Serialize a 3 byte (24 bit) value into a buffer
void   Serializer::serialize_3u(uint32 val) {
	ensureSpace(3);
	SERIALIZE_START;
	*p++ = (val >> 16) & 0xff;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
	SERIALIZED("3u  ", 3);
}

//! Serialize a 2 byte (16 bit) value into a buffer
void   Serializer::serialize_2u(uint16 val) {
	ensureSpace(2);
	SERIALIZE_START;
	*p++ = (val >> 8) & 0xff;
	*p++ = val & 0xff;
	SERIALIZED("2u  ", 2);
}

//! Serialize an 1 byte (8 bit) value into a buffer
void   Serializer::serialize_1u(uint8 val) {
	ensureSpace(1);
	SERIALIZE_START;
	*p++ = val & 0xff;
	SERIALIZED("1u  ", 1);
}

void   
Serializer::serialize_bool(bool val) {
	ensureSpace(1);
	SERIALIZE_START;
	if (mLastBoolPtr) {
		uint8 byte = *mLastBoolPtr;
		if (val) {
			byte |= (1 << mBoolBitOffset);
			*mLastBoolPtr = byte;
		}
		mBoolBitOffset++;
		if (mBoolBitOffset > 7) {
			// filled the current byte with bits, reset
			mLastBoolPtr = 0;
			mBoolBitOffset = 0;
		}
		SERIALIZED("bool", 0);
	} else {
		mLastBoolPtr = p;
		mBoolBitOffset = 1;
		*p++ = val ? 1 : 0;
		SERIALIZED("bool", 1);
	}
	// if we actually serialized a boolean, reset the size count
	// to where we actually are, so interspersed calls to 
	// getSerializedSize(bool) and serialize_bool() will work
	mBoolSizeCount = mBoolBitOffset;
}

// write a length value into the buffer in a way that is more efficient for shorter lengths
void 
Serializer::serialize_uint(uint32 len) {
	ensureSpace(5);
	SERIALIZE_START;
	if (len < tag_smallestLenTag) {
		serialize_1(len);
	} else if (len <= USHRT_MAX) {
		// large value
		serialize_1(tag_longLen);
		serialize_2(len);
	} else {
		// very large value
		serialize_1(tag_veryLongLen);
		serialize_4(len);
	}
	SERIALIZED("uint", 0);
}

void
Serializer::serialize_mem(const void* mem, uint32 memLen) {
	ensureSpace(memLen + 8);
	SERIALIZE_START;
#ifdef SERIALIZER_SANITY_CHECKS
	serialize_3u(tag_mem);
#endif
	if (mem == 0) memLen = 0;
	serialize_uint(memLen);
	if (memLen > 0) {
		std::memcpy(p, mem, memLen);
		p += memLen;
	}
	SERIALIZED("mem ", 0);
}

void   
Serializer::serialize_str(const char* str) { 
	uint32 len = std::strlen(str);
	ensureSpace(len + 8);
	SERIALIZE_START;
#ifdef SERIALIZER_SANITY_CHECKS
	serialize_3u(tag_string);
#endif
	serialize_uint(len);
	if (len > 0) {
		ensureSpace(len);
		std::memcpy(p, str, len);
		p += len;
	}
	SERIALIZED("str ", 0);
}

void
Serializer::serialize_obj(const ISerializable* obj) {
	SERIALIZE_START;
	if (obj == 0) {
		serialize_3u(tag_objectNil);
		SERIALIZED("obj ", 0);
		return;
	}
	// see if the object has already been serialized
	for (uint16 i = 0; i < mSerializedInstances.size(); i++) {
		if (mSerializedInstances[i] == obj) {
			serialize_3u(tag_objectRef);
			serialize_uint(i);
			SERIALIZED("obj ", 0);
			return;
		}
	}
	// wasn't previously serialized, do so now
	mSerializeObjDepth++;
	mSerializedInstances.push_back(obj);
//	obj->addRef();
	serialize_3u(tag_object);
	uint32 tag = obj->getMyClassTag();
	serialize_4(tag);
	SERIALIZED("obj ", 0);   // stop here so we can see the object being serialized
#ifdef SERIALIZER_SANITY_CHECKS
	serialize_2(mSerializedInstances.size());
#endif
	uint32 objLen = obj->getSerializedSize(this);
	serialize_uint(objLen);
	obj->serialize(this);
	mSerializeObjDepth--;
}

void   
Serializer::serialize_color(const Color& c) {
	SERIALIZE_START;
	uint8 v = c.alpha * 255.0f;
	bool hasAlpha = (v != 255);
	serialize_bool(hasAlpha);
	if (hasAlpha) {
		serialize_1u(v);
	}
	serialize_1u(c.red * 255.0f);
	serialize_1u(c.green * 255.0f);
	serialize_1u(c.blue * 255.0f);	
	SERIALIZED("colr", 0);
}

void   
Serializer::serialize_offset(const Offset& o) {
	SERIALIZE_START;
	bool nonZero = ((o.x != 0.0f) || (o.y != 0.0f));
	serialize_bool(nonZero);
	if (nonZero) {
		double ix;
		double fx = modf(o.x, &ix);
		double iy;
		double fy = modf(o.y, &iy);
		bool xIsInt = ( (ix >= 0.0f) && (ix <= 65535.0f) && ((int)(fabs(fx)*1000.0f) == 0) );
		bool yIsInt = ( (iy >= 0.0f) && (iy <= 65535.0f) && ((int)(fabs(fy)*1000.0f) == 0) );
		serialize_bool(xIsInt);
		serialize_bool(yIsInt);	
		if (xIsInt) {
			serialize_uint((uint32)ix);
		} else {
			serialize_f(o.x);
		}
		if (yIsInt) {
			serialize_uint((uint32)iy);
		} else {
			serialize_f(o.y);
		}
	}
	SERIALIZED("off ", 0);
}

void   
Serializer::serialize_rect(const Rect& r) {
	SERIALIZE_START;
	serialize_point(r.leftTop());
	serialize_offset(Offset(r.width(), r.height()));
	SERIALIZED("rect", 0);
}

void   
Serializer::serialize_rotr(const RotatedRect& rr) {
	SERIALIZE_START;
	serialize_rect(rr);
	serialize_f(rr.radians);
	serialize_offset(rr.centerOffset);
	SERIALIZED("rotr", 0);
}

void   
Serializer::serialize_quad(const Quad& q) {
	SERIALIZE_START;
	serialize_point(q.points[0]);
	serialize_point(q.points[1]);
	serialize_point(q.points[2]);
	serialize_point(q.points[3]);
	SERIALIZED("quad", 0);
}

uint32
Serializer::serializedSize(const void* mem, uint32 memLen) const {
	uint32 lenOfLen = serializedSize(memLen);
#ifdef SERIALIZER_SANITY_CHECKS
	lenOfLen += 3;
#endif
	if (mem == 0) memLen = 0;
	return lenOfLen + memLen;
}

//! How many bytes are used to serialize a particular boolean value
uint32 
Serializer::serializedSize(bool val) const {
	uint32 result = (mBoolSizeCount == 0) ? 1 : 0;
	mBoolSizeCount++;
	if (mBoolSizeCount > 7) {
		mBoolSizeCount = 0;
	}
	return result;
}

// how many bytes are used to serialize this particular length value
uint32 
Serializer::serializedSize(uint32 num) const {
	if (num < tag_smallestLenTag) {
		return 1;
	} else if (num <= USHRT_MAX) {
		return 3;
	} else {
		return 5;
	}
}

//! How many bytes are used to serialize a particular pdg::Color value
uint32 
Serializer::serializedSize(const Color& c) const {
	uint8 v = c.alpha * 255.f;
	bool hasAlpha = (v != 255);
	uint32 result = hasAlpha ? serializedSize(hasAlpha) + 4 : 3;
	return result;
}

//! How many bytes are used to serialize a particular pdg::Offset, Point or Vector value
uint32 
Serializer::serializedSize(const Offset& o) const {
	bool nonZero = ((o.x != 0.0f) || (o.y != 0.0f));
	uint32 result = serializedSize(nonZero);
	if (nonZero) {
		double ix;
		double fx = modf(o.x, &ix);
		double iy;
		double fy = modf(o.y, &iy);
		bool xIsInt = ( (ix >= 0.0f) && (ix <= 65535.0f) && ((int)(fabs(fx)*1000.0f) == 0) );
		bool yIsInt = ( (iy >= 0.0f) && (iy <= 65535.0f) && ((int)(fabs(fy)*1000.0f) == 0) );
		result += serializedSize(xIsInt);
		result += serializedSize(yIsInt);	
		if (xIsInt) {
			result += serializedSize((uint32)ix);
		} else {
			result += 4;
		}
		if (yIsInt) {
			result += serializedSize((uint32)iy);
		} else {
			result += 4;
		}
	}
	return result;
}

//! How many bytes are used to serialize a particular pdg::Rect or RotatedRect value
uint32 
Serializer::serializedSize(const Rect& r) const {
	uint32 result = serializedSize(r.leftTop());
	result += serializedSize(Offset(r.width(), r.height()));
	return result;
}

//! How many bytes are used to serialize a particular pdg::Rect or RotatedRect value
uint32 
Serializer::serializedSize(const RotatedRect& rr) const {
	uint32 result = serializedSize((const Rect&)rr);
	result += serializedSize(rr.centerOffset) + 4;
	return result;
}

//! How many bytes are used to serialize a particular pdg::Quad value
uint32 
Serializer::serializedSize(const Quad& q) const {
	uint32 result = serializedSize(q.points[0]);
	result += serializedSize(q.points[1]);
	result += serializedSize(q.points[2]);
	result += serializedSize(q.points[3]);
	return result;
}


uint32 
Serializer::serializedSize(const char* str) const { 
	uint32 strLen = std::strlen(str); 
	uint32 lenOfLen = serializedSize(strLen);
#ifdef SERIALIZER_SANITY_CHECKS
	lenOfLen += 3;
#endif
	return lenOfLen + strLen;
}

uint32
Serializer::serializedSize(const ISerializable* obj) {
	if (obj == 0) {
		return 3;
	}
	if (mSerializeObjDepth == 0) {
		// when this is called before actually serializing objects, we need
		// to get size estimates based on what would be serialized verses
		// just stored as references
		for (uint16 i = 0; i < mSerializedSizeInstances.size(); i++) {
			if (mSerializedSizeInstances[i] == obj) {
				return serializedSize((uint32)i) + 3;
			}
		}
		// wasn't previously serialized, calculate full serialized size
		mSerializedSizeInstances.push_back(obj);
//		obj->addRef();
	} else {
		// when called while serializing objects, we need
		// to get size estimates based on what has actually been
		// serialized, not based on what has been sized
		// see if the object has already been serialized
		for (uint16 i = 0; i < mSerializedInstances.size(); i++) {
			if (mSerializedInstances[i] == obj) {
				return serializedSize((uint32)i) + 3;
			}
		}
	}
	uint32 objLen = obj->getSerializedSize(this);
	uint32 len = 3 + 4;
#ifdef SERIALIZER_SANITY_CHECKS
	len += 2;
#endif
	len += serializedSize(objLen);
	len += objLen;
    return len;
}

void  Serializer::ensureSpace(uint32 bytes) {
	uint32 currSize = getDataSize();
	uint32 blocksNeeded = ((currSize + bytes) / mBlockSize) + 1;
	uint32 blocksAllocated = mAllocatedSize / mBlockSize;
	if (blocksNeeded > blocksAllocated) {
		mAllocatedSize = blocksNeeded * mBlockSize;
		uint32 offset = p - mDataPtr;
		mDataPtr = (uint8*)std::realloc(mDataPtr, mAllocatedSize);
		p = mDataPtr + offset;
	}
}

// --------------------------------------------
// constructors
// --------------------------------------------

Serializer::Serializer()
 :  mDataPtr(0),
	p(0),
	mAllocatedSize(0),
	mBlockSize(SERIALIZER_MALLOC_BLOCK_SIZE),
	mSerializedInstances(),
	mSerializedSizeInstances(),
	mSerializeObjDepth(0),
	mMark(0),
	mLastBoolPtr(0),
	mBoolBitOffset(0),
	mBoolSizeCount(0)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mSerializerScriptObj);
#endif
}

Serializer::~Serializer() {
	if (mDataPtr) {
		std::free(mDataPtr);
		mDataPtr = 0;
	}
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupSerializerScriptObject(mSerializerScriptObj);
#endif
}

char* Serializer::statusDump(int hiliteBytes) {
	static char buf[1024];
	char* targBuf = buf + 22;
	uint32 blockNum = (mMark - mDataPtr)/16;
	uint32 blockStart = 16 * blockNum;
	uint32 blockLen = 20;
	if ((blockStart + blockLen) > mAllocatedSize) blockLen = mAllocatedSize - blockStart;
	long hiliteStart = (mMark - mDataPtr) - blockStart;
	std::sprintf(buf, "@%05lu/%05lu:  %04lX | ", p - mDataPtr, mAllocatedSize, blockStart);
	OS::binaryDump(targBuf, 1000, (char*)(mDataPtr + blockStart), blockLen, 20, hiliteStart, hiliteBytes);
	return buf;
}

void Serializer::startMark() {
	if (!p) {
		ensureSpace(1);
	}
	mMark = p;
}

int Serializer::bytesFromMark() { 
	return (mMark && p) ? (p - mMark) : 0;
}

} // end namespace pdg
