// -----------------------------------------------
//  serializer.h
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


#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/iserializer.h"

#include <vector>

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {
	
	// ==============================================
	// Serializer class
	// ==============================================
	
	//! A container for static methods that do the actual serialization
	
	class Serializer : public ISerializer {
	public:
		
		// --------------------------------------------
		// fixed size serialization methods
		// --------------------------------------------
		
	  #ifndef PDG_NO_64BIT
		//! Serialize an 8 byte (64 bit) value into a buffer
		/*! internal pointer gets advanced by 8 bytes
		 \param val the 64 bit value to serialize
		 */
		virtual void   serialize_8u(uint64 val);
	  #endif

		//! Serialize a 4 byte (32 bit) value into a buffer
		/*! internal pointer gets advanced by 4 bytes
		 \param val the 32 bit value to serialize
		 */
		virtual void   serialize_4u(uint32 val);
		
		//! Serialize a 3 byte (24 bit) value into a buffer
		/*! internal pointer gets advanced by 3 bytes
		 \param val the 24 bit value to serialize
		 */
		virtual void   serialize_3u(uint32 val);
		
		//! Serialize a 2 byte (16 bit) value into a buffer
		/*! internal pointer gets advanced by 2 bytes
		 \param val the 16 bit value to serialize
		 */
		virtual void   serialize_2u(uint16 val);
		
		//! Serialize an 1 byte (8 bit) value into a buffer
		/*! internal pointer gets advanced by 1 byte
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_1u(uint8 val);
		
		// --------------------------------------------
		// variable size serialization methods
		// --------------------------------------------

		//! Serialize a boolean (1 bit) value into a buffer
		/*! internal pointer gets advanced by 1 or 0 bytes, depending on the bytes needed to store the value
		 \param val the boolean to serialize
		 This is optimized for series of boolean values, so that between 1 and 8 bools serialized 
		 sequentially only use a single byte
		 */
		virtual void   serialize_bool(bool val);

		//! Serialize an unsigned integer value into a buffer
		/*! internal pointer gets advanced by 1, 3 or 5 bytes, depending on the bytes needed to store the value
		 \param num the unsigned value to serialize
		 */
		virtual void   serialize_uint(uint32 num);
		
		//! Serialize an string value into a buffer
		/*! internal pointer gets advanced by serialized size of string
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_str(const char* str);
		
		//! Serialize an arbitrary block of memory
		/*! internal pointer gets advanced by serialized size of the memory block
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_mem(const void* mem, uint32 memLen);
		
		//! Serialize a serializable object
		/*! internal pointer gets advanced by total size of serialized obj
		 \param obj the object to serialize
		 This works by writing the object's unique tag, then serializing the size of the object via obj->getSerializedSize()
		 and then finally calling obj->serialize() to actually serialize the object's data
		 */
		virtual void   serialize_obj(const ISerializable* obj);
		
		//! Serialize a pdg::Color
		/*! internal pointer gets advanced by total size of serialized color
		 \param c the Color to serialize
		 */
		virtual void   serialize_color(const Color& c);

		//! Serialize a pdg::Offset
		/*! internal pointer gets advanced by total size of serialized Offset
		 \param o the Offset to serialize
		 */
		virtual void   serialize_offset(const Offset& o);

		//! Serialize a pdg::Rect
		/*! internal pointer gets advanced by total size of serialized rect
		 \param r the Rect to serialize
		 */
		virtual void   serialize_rect(const Rect& r);

		//! Serialize a pdg::RotatedRect
		/*! internal pointer gets advanced by total size of serialized rect
		 \param r the Rect to serialize
		 */
		virtual void   serialize_rotr(const RotatedRect& rr);

		//! Serialize a pdg::Quad
		/*! internal pointer gets advanced by total size of serialized quad
		 \param q the Quad to serialize
		 */
		virtual void   serialize_quad(const Quad& q);

		// --------------------------------------------
		// size checking methods
		// --------------------------------------------

		//! How many bytes are used to serialize a particular boolean value
		virtual uint32 serializedSize(bool val) const;

		//! How many bytes are used to serialize a particular unsigned integer value
		virtual uint32 serializedSize(uint32 num) const;

		//! How many bytes are used to serialize a particular string
		virtual uint32 serializedSize(const char* str) const;

		//! How many bytes are used to serialize a particular block of memory
		virtual uint32 serializedSize(const void* mem, uint32 memLen) const;

		//! How many bytes are used to serialize a particular object, including tags and size data
		/*! Since objects are only serialized once, it is critical that you call this on objects the same order that
		    you actually serialize those object. The first call for a particular object will return the
		    number of bytes for the serialized object stream plus tag overhead. Successive calls will return
		    the number of bytes used to store a reference to the object, usually a much smaller number
			A NULL object pointer will always take a constant number of bytes needed for the null object tag
		 */
		virtual uint32 serializedSize(const ISerializable* obj);  // non const method

		//! How many bytes are used to serialize a particular pdg::Color value
		virtual uint32 serializedSize(const Color& c) const;

		//! How many bytes are used to serialize a particular pdg::Offset, Point or Vector value
		virtual uint32 serializedSize(const Offset& o) const;

		//! How many bytes are used to serialize a particular pdg::Rect value
		virtual uint32 serializedSize(const Rect& r) const;

		//! How many bytes are used to serialize a particular pdg::RotatedRect value
		virtual uint32 serializedSize(const RotatedRect& rr) const;

		//! How many bytes are used to serialize a particular pdg::Quad value
		virtual uint32 serializedSize(const Quad& q) const;

		// --------------------------------------------
		// data methods
		// --------------------------------------------
		
		uint8* getDataPtr() { return mDataPtr; }
		uint32 getDataSize() { return (mDataPtr) ? p - mDataPtr : 0; }
		
		// --------------------------------------------
		// constructors
		// --------------------------------------------
		
		Serializer();
		virtual ~Serializer();

		#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
			SCRIPT_OBJECT_REF mSerializerScriptObj;
		#endif

	protected:
		
		virtual char* statusDump(int hiliteBytes = 0);
		virtual void startMark();
		virtual int bytesFromMark();

		// Make sure there is space to write 
		virtual void  ensureSpace(uint32 bytes);
		
		uint8* mDataPtr;
		uint8* p;   // current position in pointer
		uint32 mAllocatedSize;
		uint32 mBlockSize;
		
		// used to track objects that have been streamed
		std::vector<const ISerializable*> mSerializedInstances;
		std::vector<const ISerializable*> mSerializedSizeInstances;
		
		int mSerializeObjDepth;
		uint8* mMark;
		uint8* mLastBoolPtr;
		int mBoolBitOffset;
		mutable int mBoolSizeCount;
	};

		
} // end namespace pdg

#endif // SERIALIZER_H_INCLUDED

