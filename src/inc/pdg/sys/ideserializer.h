// -----------------------------------------------
//  ideserializer.h
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


#ifndef IDESERIALIZER_H_INCLUDED
#define IDESERIALIZER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/color.h"
#include "pdg/sys/coordinates.h"
#include "pdg/sys/pdgexception.h"
#include "pdg/sys/iserializer.h"

#ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
#include <string>
#endif

//#define PDG_DESERIALIZER_NO_THROW
#if defined( DEBUG ) || defined( PDG_SERIALIZATION_DEBUG )
  #define WANT_DERSERIALIZE_TRACE
#endif

#ifndef WANT_DERSERIALIZE_TRACE
  #define DESERIALIZE_IN(type, bytes)
  #define DESERIALIZE_OUT
#else
  #include <iostream>
  #define DESERIALIZE_IN(type, bytes)   if ( (IDeserializer::s_TraceDepth++ == 0) && ISerializer::s_DebugMode) { std::cout << "DES: "<< type << " " << statusDump(bytes) << "\n";}
  #define DESERIALIZE_OUT IDeserializer::s_TraceDepth--
#endif


namespace pdg {
	

#ifndef PDG_DESERIALIZER_NO_THROW
	class out_of_data : public PDGException {
		public: out_of_data(const char* msg) : PDGException(msg) {}
	};
	class sync_error : public PDGException {
		public: sync_error(const char* msg) : PDGException(msg) {}
	};
	class bad_tag : public PDGException {
		public: bad_tag(const char* msg) : PDGException(msg) {}
	};
	class unknown_object : public PDGException {
		public: unknown_object(const char* msg) : PDGException(msg) {}
	};
	#define MAY_THROW( __specs )
#else
	#define MAY_THROW( __specs )
#endif

	class ISerializable;

	//! \ingroup Serialization
	class IDeserializer {
	public:
			
		static int s_TraceDepth;

		typedef ISerializable* (*CreateSerializableFunc)();
		
	  #ifndef PDG_NO_64BIT
		//! Deserialize an 8 byte (64 bit) value
		/*! internal pointer is advanced 8 btyes
		 \return the deserialized 64 bit value
		 */
		virtual uint64 deserialize_8u() MAY_THROW( out_of_data ) = 0;
		int64          deserialize_8 () MAY_THROW( out_of_data );
	  #endif

		//! Deserialize a 4 byte (32 bit) value
		/*! internal pointer is advanced 4 btyes
		 \return the deserialized 32 bit value
		 */
		virtual uint32 deserialize_4u() MAY_THROW( out_of_data ) = 0;
		int32          deserialize_4 () MAY_THROW( out_of_data );
		
		//! Deserialize a 3 byte (24 bit) value
		/*! internal pointer is advanced 3 btyes
		 \return the deserialized 24 bit value
		 */
		virtual uint32 deserialize_3u() MAY_THROW( out_of_data ) = 0;
		
		//! Deserialize a 2 byte (16 bit) value
		/*! internal pointer is advanced 2 btyes
		 \return the deserialized 16 bit value
		 */
		virtual uint16 deserialize_2u() MAY_THROW( out_of_data ) = 0;
		int16          deserialize_2 () MAY_THROW( out_of_data );
		
		//! Deserialize an 1 byte (8 bit) value
		/*! internal pointer is advanced 1 btye
		 \return the deserialized 8 bit value
		 */
		virtual uint8 deserialize_1u() MAY_THROW( out_of_data ) = 0;
		int8          deserialize_1 () MAY_THROW( out_of_data );
		
		//! Serialize a floating point value into a buffer
		/*! internal pointer gets advanced by 4 bytes
		 \return the floating point value that was deserialized
		 */
		float         deserialize_f() MAY_THROW( out_of_data );

		//! Serialize a double precision floating point value into a buffer
		/*! internal pointer gets advanced by 8 bytes
		 \return the floating point value that was deserialized
		 */
		double        deserialize_d() MAY_THROW( out_of_data );

		//! Deserialize a boolean (1 bit) value
		/*! internal pointer gets advanced by 1 or 0 bytes, depending on bytes required to store the value
		 \return the boolean that was deserialized
		 This is optimized for series of boolean values, so that between 1 and 8 bools serialized 
		 sequentially only use a single byte
		 */
		virtual bool   deserialize_bool() MAY_THROW( out_of_data ) = 0;

		//! Deserialize an unsigned value
		/*! internal pointer is advanced by 1, 3 or 5 bytes, depending on bytes required to store value
		 \return the unsigned value that was deserialized
		 */
		virtual uint32 deserialize_uint() MAY_THROW( out_of_data ) = 0;

		//! Deserialize a pdg::Color value
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the Color that was deserialized
		 */
		virtual Color deserialize_color() MAY_THROW( out_of_data ) = 0;

		//! Deserialize a pdg::Offset value
		/*! internal pointer is advanced by 1 to 9 bytes, depending on bytes required to store value
		 \return the Offset that was deserialized
		 */
		virtual Offset deserialize_offset() MAY_THROW( out_of_data ) = 0;

		//! Deserialize a pdg::Point value
		/*! internal pointer is advanced by 1 to 9 bytes, depending on bytes required to store value
		 \return the Point that was deserialized
		 */
		Point          deserialize_point() MAY_THROW( out_of_data );

		//! Deserialize a pdg::Vector value
		/*! internal pointer is advanced by 1 to 9 bytes, depending on bytes required to store value
		 \return the Vector that was deserialized
		 */
		Vector         deserialize_vector() MAY_THROW( out_of_data );

		//! Deserialize a pdg::Rect
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the Rect that was deserialized
		 */
		virtual Rect   deserialize_rect() MAY_THROW( out_of_data ) = 0;

		//! Deserialize a pdg::RotatedRect
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the RotatedRect that was deserialized
		 */
		virtual RotatedRect   deserialize_rotr() MAY_THROW( out_of_data ) = 0;

		//! Deserialize a pdg::Quad
		/*! internal pointer is advanced by 1, 3 or 5 bytes, depending on bytes required to store value
		 \return the Quad that was deserialized
		 */
		virtual Quad   deserialize_quad() MAY_THROW( out_of_data ) = 0;


		//! Deserialize a string value
		/*! \param outStr, a buffer to hold the resulting string
		 \param strMaxLen, the max length of the output string, including NUL terminator, that the buffer can accomodate
		 If the serialized string is longer than strMaxLen, it will be truncated and the internal pointer p will 
		 still be advanced past the string to the start of the next entity in the internal buffer.
		 \return the length of the string in bytes (not including NUL terminator)
		 */
		virtual uint32 deserialize_str(char* outStr, uint32 strMaxLen) MAY_THROW( (out_of_data, bad_tag) ) = 0;

		//! get length of a string value, including NUL terminator, as if it were deserialized
		/*! internal pointer is NOT advanced. Use this to pre-flight allocation for longer strings
		 \return the length of the serialized string plus nul terminator in bytes
		 */
		virtual uint32 deserialize_strGetLen() MAY_THROW( (out_of_data, bad_tag) ) = 0;

	  #ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
		//! Deserialize a string value into a std::string
		/*! \param outStr, a std::string to hold the resulting string
		 \return none
		 */
		void           deserialize_string(std::string& outStr) MAY_THROW( (out_of_data, bad_tag) );
	  #endif
		
		//! Deserialize an arbitrary block of memory
		/*! \param p a reference to a buffer pointer; pointer gets advanced past mem block to start of next entity in buffer
		 \param outMem, a buffer to hold the resulting block of memory
		 \param memMaxLen, the max length of the output block, that the buffer can accomodate
		 If the serialized memory is longer than memMaxLen, it will be truncated and the buffer pointer p will 
		 still be advanced past the memory block to the start of the next entity in the buffer.
		 \return the length of the string in bytes
		 */
		virtual uint32 deserialize_mem(void* outMem, uint32 memMaxLen) MAY_THROW( (out_of_data, bad_tag) ) = 0;

		//! Deserialize an string value
		/*! internal pointer is NOT advanced. Use this to pre-flight allocation for blocks of memory
		 \return the length of the serialized block of memory in bytes
		 */
		virtual uint32 deserialize_memGetLen() MAY_THROW( (out_of_data, bad_tag) ) = 0;

		//! Deserialize a serializable object
		/*! internal pointer gets advanced past object to start of next entity in buffer
		 \return NULL, or a newly created Serializable object
		 NULL return happens if serialized object reference was null, or if the object could not be deserialized
		 If object could not be deserialized, either because the subclass was unknown or because the data was corrupt,
		 the internal pointer will still be advanced past the object to the start of the next entity in the buffer.
		 */
		virtual ISerializable* deserialize_obj() MAY_THROW( (out_of_data, bad_tag, sync_error, unknown_object) ) = 0;

		// ------------------------------------------------------------------
		// class registry methods
		// ------------------------------------------------------------------
		
		//! Register a class that is derived from Serializable.
		/*! this is the prefered way of registering classes for serialization
		 */
		template <class T> static void registerClass() { T tmp; IDeserializer::registerClass(tmp.getMyClassTag(), T::CreateInstance); }
		
		//! Register a class that is derived from ISerializable
		/*! \param classTag a 32 bit value uniquely identifying the class
		   \param classNewFunc the factory function that can create new instances of the class
		   You should derive classes from Serializable rather than ISerializable, and use the template based registerClass()
		   to register them unless you have a really good reason not to.
		 */
		static void registerClass(uint32 classTag, CreateSerializableFunc classNewFunc);

	protected:
		virtual char* statusDump(int hiliteBytes = 0) = 0;
	};
	
  #ifndef PDG_NO_64BIT
	inline int64
	IDeserializer::deserialize_8() { 
		DESERIALIZE_IN("8   ", 8);
		int64 val = (int64) deserialize_8u(); 
		DESERIALIZE_OUT;
		return val;
	}
  #endif
	
	inline double
	IDeserializer::deserialize_d() { 
		DESERIALIZE_IN("d   ", 8);
      #ifndef PDG_NO_64BIT
		uint64 n = deserialize_8u(); 
	  #else
	  	// for platforms without int64, we have to break it into 2 steps
	  	// which means worrying about endianness
	    uint32 n[2];
	  	#ifdef PLATFORM_BIG_ENDIAN
	  		n[0] = deserialize_4u();
	  		n[1] = deserialize_4u();
	  	#else
	  		n[1] = deserialize_4u();
	  		n[0] = deserialize_4u();
	  	#endif
	  #endif
	  	DESERIALIZE_OUT;
		return *(double*)&n; 
	}

	inline int32
	IDeserializer::deserialize_4 () { 
		DESERIALIZE_IN("4   ", 4);
		int32 val = (int32) deserialize_4u();
		DESERIALIZE_OUT;
		return val;
	}
	
	inline int16
	IDeserializer::deserialize_2 () { 
		DESERIALIZE_IN("2   ", 2);
		int16 val = (int16) deserialize_2u(); 
		DESERIALIZE_OUT;
		return val;
	}
	
	inline int8
	IDeserializer::deserialize_1 () { 
		DESERIALIZE_IN("1   ", 1);
		int8 val = (int8) deserialize_1u(); 
		DESERIALIZE_OUT;
		return val;
	}
	
	inline float
	IDeserializer::deserialize_f() { 
		DESERIALIZE_IN("f   ", 4);
		uint32 n = deserialize_4u(); 
		DESERIALIZE_OUT;
		return *(float*)&n; 	
	}

	inline Point
	IDeserializer::deserialize_point()  { 
		DESERIALIZE_IN("p   ", 0);
		Point p = deserialize_offset(); 
		DESERIALIZE_OUT;
		return p;
	}

	inline Vector
	IDeserializer::deserialize_vector() { 
		DESERIALIZE_IN("v   ", 0);
		Vector v = deserialize_offset(); 
		DESERIALIZE_OUT;
		return v;
	}

  #ifndef PDG_SERIALIZE_NO_STD_STRING_SUPPORT
	inline void
	IDeserializer::deserialize_string(std::string& outStr) { 
		outStr.resize(deserialize_strGetLen()); 
		deserialize_str(const_cast<char*>(outStr.data()), outStr.size());
	}
  #endif

} // end namespace pdg


#endif // IDESERIALIZER_H_INCLUDED

