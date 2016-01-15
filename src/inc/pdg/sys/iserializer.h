// -----------------------------------------------
//  iserializer.h
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


#ifndef ISERIALIZER_H_INCLUDED
#define ISERIALIZER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/color.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/pdgexception.h"

#ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
#include <string>
#endif

#if defined( DEBUG ) || defined( PDG_SERIALIZATION_DEBUG )
  #define WANT_SERIALIZE_TRACE
#endif

#ifndef WANT_SERIALIZE_TRACE
  #define SERIALIZE_START
  #define SERIALIZED(type, bytes)
#else
  #include <iostream>
  #define SERIALIZE_START 			ISerializer::s_TraceDepth++; if (ISerializer::s_TraceDepth == 1) { startMark(); } 
  #define SERIALIZED(type, bytes)   if ( (--ISerializer::s_TraceDepth == 0) && ISerializer::s_DebugMode) { std::cout << "SER: "<< type << " " << statusDump(bytesFromMark()) << "\n";}
#endif


namespace pdg {
	
#ifndef PDG_DESERIALIZER_NO_THROW
	class unknown_object : public PDGException {
		public: unknown_object(const char* msg) : PDGException(msg) {}
	};
	#define MAY_THROW( __specs )
#else
	#define MAY_THROW( __specs )
#endif

	class ISerializable;

	// ==============================================
	// Serializer interface
	// ==============================================
	
	//! An interface to represent serialization capabilities
	//! \ingroup Serialization
	
	class ISerializer {
	public:
		
		static int s_TraceDepth;
		static bool s_DebugMode;

		// --------------------------------------------
		// fixed size serialization methods
		// --------------------------------------------

	  #ifndef PDG_NO_64BIT
		//! Serialize an 8 byte (64 bit) value into a buffer
		/*! internal pointer gets advanced by 8 bytes
		 \param val the 64 bit value to serialize
		 */
		virtual void   serialize_8u(uint64 val) = 0;
		void           serialize_8 (int64 val);
	  #endif

		//! Serialize a 4 byte (32 bit) value into a buffer
		/*! internal pointer gets advanced by 4 bytes
		 \param val the 32 bit value to serialize
		 */
		virtual void   serialize_4u(uint32 val) = 0;
		void           serialize_4 (int32 val);
		
		//! Serialize a 3 byte (24 bit) value into a buffer
		/*! internal pointer gets advanced by 3 bytes
		 \param val the 24 bit value to serialize
		 */
		virtual void   serialize_3u(uint32 val) = 0;
		
		//! Serialize a 2 byte (16 bit) value into a buffer
		/*! internal pointer gets advanced by 2 bytes
		 \param val the 16 bit value to serialize
		 */
		virtual void   serialize_2u(uint16 val) = 0;
		void           serialize_2 (int16 val);
		
		//! Serialize an 1 byte (8 bit) value into a buffer
		/*! internal pointer gets advanced by 1 byte
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_1u(uint8 val) = 0;
		void           serialize_1 (int8 val);
		
		//! Serialize a floating point value into a buffer
		/*! internal pointer gets advanced by 4 bytes
		 \param val the floating point value to serialize
		 */
		void           serialize_f(float val);

		//! Serialize a double precision floating point value into a buffer
		/*! internal pointer gets advanced by 8 bytes
		 \param val the floating point value to serialize
		 */
		void           serialize_d(double val);

		// --------------------------------------------
		// variable size serialization methods
		// --------------------------------------------

		//! Serialize a boolean (1 bit) value into a buffer
		/*! internal pointer gets advanced by 1 or 0 bytes, depending on the bytes needed to store the value
		 \param val the boolean to serialize
		 This is optimized for series of boolean values, so that between 1 and 8 bools serialized 
		 sequentially only use a single byte
		 */
		virtual void   serialize_bool(bool val) = 0;

		//! Serialize an unsigned integer value into a buffer
		/*! internal pointer gets advanced by 1, 3, 5 bytes, depending on the bytes needed to store the value
		 \param num the unsigned value to serialize
		 */
		virtual void   serialize_uint(uint32 num) = 0;
		
		//! Serialize a pdg::Color
		/*! internal pointer gets advanced by total size of serialized color
		 \param c the Color to serialize
		 */
		virtual void   serialize_color(const Color& c) = 0;

		//! Serialize a pdg::Offset
		/*! internal pointer gets advanced by total size of serialized Offset
		 \param o the Offset to serialize
		 */
		virtual void   serialize_offset(const Offset& o) = 0;

		//! Serialize a pdg::Point
		/*! internal pointer gets advanced by total size of serialized Point
		 \param p the Point to serialize
		 */
		void           serialize_point(const Point& p);

		//! Serialize a pdg::Vector
		/*! internal pointer gets advanced by total size of serialized Vector
		 \param p the Vector to serialize
		 */
		void           serialize_vector(const Vector& v);

		//! Serialize a pdg::Rect
		/*! internal pointer gets advanced by total size of serialized rect
		 \param r the Rect to serialize
		 */
		virtual void   serialize_rect(const Rect& r) = 0;

		//! Serialize a pdg::RotatedRect
		/*! internal pointer gets advanced by total size of serialized rect
		 \param r the Rect to serialize
		 */
		virtual void   serialize_rotr(const RotatedRect& rr) = 0;

		//! Serialize a pdg::Quad
		/*! internal pointer gets advanced by total size of serialized quad
		 \param q the Quad to serialize
		 */
		virtual void   serialize_quad(const Quad& q) = 0;

		//! Serialize an string value into a buffer
		/*! internal pointer gets advanced by serialized size of string
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_str(const char* str) = 0;
		
	  #ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
		//! Serialize a string value from a std::string
		/*! internal pointer gets advanced by serialized size of string
		 \param str, a std::string to be serialized
		 */
		void           serialize_string(const std::string& str);
	  #endif
		
		//! Serialize an arbitrary block of memory
		/*! internal pointer gets advanced by serialized size of the memory block
		 \param val the 8 bit value to serialize
		 */
		virtual void   serialize_mem(const void* mem, uint32 memLen) = 0;
		
		//! Serialize a serializable object
		/*! internal pointer gets advanced by total size of serialized obj
		 \param obj the object to serialize
		 This works by writing the object's unique tag, then serializing the size of the object via obj->getSerializedSize()
		 and then finally calling obj->serialize() to actually serialize the object's data
		 */
		virtual void   serialize_obj(const ISerializable* obj) = 0;
		
		//! Serialize a reference to a non-serializable object
		// You must have called IDeserializer::registerObject() to give the object a unique ID 
		// before serializing it, otherwise an unknown_object exception will be thrown.
		template<typename T> void serialize_ref(const T* obj) MAY_THROW( unknown_object );

		// --------------------------------------------
		// size checking methods for variable size
		// --------------------------------------------

#ifndef PDG_NO_64BIT
		uint32 sizeof_8u(uint64 val) const  { return 8; }
		uint32 sizeof_8 (int64 val) const   { return 8; }
#endif
        uint32 sizeof_4u(uint32 val) const  { return 4; }
		uint32 sizeof_4 (int32 val) const   { return 4; }
		uint32 sizeof_3u(uint32 val) const  { return 3; }
		uint32 sizeof_2u(uint16 val) const  { return 2; }
		uint32 sizeof_2 (int16 val) const   { return 2; }
		uint32 sizeof_1u(uint8 val) const   { return 1; }
		uint32 sizeof_1 (int8 val) const    { return 1; }
		uint32 sizeof_f(float val) const    { return 4; }
		uint32 sizeof_d(double val) const   { return 8; }

		//! How many bytes are used to serialize a particular boolean value
		virtual uint32 sizeof_bool(bool val) const = 0;

		//! How many bytes are used to serialize a particular unsigned integer value
		virtual uint32 sizeof_uint(uint32 num) const = 0;

		//! How many bytes are used to serialize a particular pdg::Color value
		virtual uint32 sizeof_color(const Color& c) const = 0;

		//! How many bytes are used to serialize a particular pdg::Offset, Point or Vector value
		virtual uint32 sizeof_offset(const Offset& o) const = 0;
		uint32         sizeof_vector(const Vector& v) const { return sizeof_offset(v); }
		uint32         sizeof_point(const Point& p) const { return sizeof_offset(p); }

		//! How many bytes are used to serialize a particular pdg::Rect value
		virtual uint32 sizeof_rect(const Rect& r) const = 0;

		//! How many bytes are used to serialize a particular pdg::RotatedRect value
		virtual uint32 sizeof_rotr(const RotatedRect& rr) const = 0;

		//! How many bytes are used to serialize a particular pdg::Quad value
		virtual uint32 sizeof_quad(const Quad& q) const = 0;

		//! How many bytes are used to serialize a particular string
		virtual uint32 sizeof_str(const char* str) const = 0;

	  #ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
		//! How many bytes are used to serialize a particular std::string
		uint32         sizeof_string(const std::string& str) const;
	  #endif

		//! How many bytes are used to serialize a particular block of memory
		virtual uint32 sizeof_mem(const void* mem, uint32 memLen) const = 0;

		//! How many bytes are used to serialize a particular object, including tags and size data
		/*! Since objects are only serialized once, it is critical that you call this on objects in the same 
		    order you actually serialize those object. The first call for a particular object will return the
		    number of bytes for the serialized object stream plus tag overhead. Successive calls will return
		    the number of bytes used to store a reference to the object, usually a much smaller number.
			A NULL object pointer will always take a constant number of bytes needed for the null object tag
		 */
		virtual uint32 sizeof_obj(const ISerializable* obj) = 0;

		template<typename T> uint32 sizeof_ref(const T* obj) const MAY_THROW( unknown_object );

        //! Turn on or off the sync tags for this steam
        // This which makes the steam slightly smaller and faster. This must be done
        // before anything is written to the stream.
        // They default to ON since they provide relatively inexpensive sanity checks. 
        ISerializer& setSendTags(bool sendThem) { mSendTags = sendThem; return *this; }

	protected:
		virtual char* statusDump(int hiliteBytes = 0) = 0;
		virtual void startMark() = 0;
		virtual int bytesFromMark() = 0;
		virtual void serialize_ptr(const void* ptr) = 0;
		virtual uint32 sizeof_ptr(const void* ptr) const = 0;


		bool mSendTags;
		
		ISerializer() : mSendTags(true) {}
	};

	template<typename T> void ISerializer::serialize_ref(const T* obj) {
	    serialize_ptr(static_cast<const void*>(obj));
	}
	template<typename T> uint32 ISerializer::sizeof_ref(const T* obj) const {
	    return sizeof_ptr(static_cast<const void*>(obj));
	}

  #ifndef PDG_NO_64BIT
	inline void    
	ISerializer::serialize_8(int64 val) { 
		serialize_8u(val); 
	}
  #endif

	inline void    
	ISerializer::serialize_d(double val) { 
		SERIALIZE_START;
      #ifndef PDG_NO_64BIT
		serialize_8u(*(uint64*)&val); 
	  #else
	  	// for platforms without int64, we have to break it into 2 steps
	  	// which means worrying about endianness
	  	#ifdef PLATFORM_BIG_ENDIAN
	  		serialize_4u(((uint32*)&val)[0]);
	  		serialize_4u(((uint32*)&val)[1]);
	  	#else
	  		serialize_4u(((uint32*)&val)[1]);
	  		serialize_4u(((uint32*)&val)[0]);
	  	#endif
	  #endif
		SERIALIZED("d   ", 8);
	}

	inline void    
	ISerializer::serialize_4(int32 val) { 
		SERIALIZE_START;
		serialize_4u(val); 
		SERIALIZED("4   ", 4);
	}

	inline void    
	ISerializer::serialize_2(int16 val) { 
		SERIALIZE_START;
		serialize_2u(val); 
		SERIALIZED("2   ", 2);
	}

	inline void    
	ISerializer::serialize_1(int8 val) { 
		SERIALIZE_START;
		serialize_1u(val);
		SERIALIZED("1   ", 1);
	}

	inline void    
	ISerializer::serialize_f(float val) { 
		SERIALIZE_START;
		serialize_4u(*(uint32*)&val); 
		SERIALIZED("f   ", 4);
	}

	inline void    
	ISerializer::serialize_point(const Point& p) { 
		SERIALIZE_START;
		serialize_offset(p); 
		SERIALIZED("p   ", 8);
	}

	inline void    
	ISerializer::serialize_vector(const Vector& v) { 
		SERIALIZE_START;
		serialize_offset(v); 
		SERIALIZED("v   ", 8);
	}

  #ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
	inline void    
	ISerializer::serialize_string(const std::string& str) { 
		serialize_str(str.c_str()); 
	}

	inline uint32 
	ISerializer::sizeof_string(const std::string& str) const {
		return sizeof_str(str.c_str()); 
	}

  #endif

} // end namespace pdg

#endif // ISERIALIZER_H_INCLUDED

