// -----------------------------------------------
//  deserializer.h
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


#ifndef DESERIALIZER_H_INCLUDED
#define DESERIALIZER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/ideserializer.h"

#include <vector>

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
  #include "pdg_script_bindings.h"
  #ifdef PDG_COMPILING_FOR_JAVASCRIPT
    #include "pdg_javascript.h"
  #endif
#endif

namespace pdg {
	
	//! A class to serialize data into memory
	//! \ingroup Serialization
	class Deserializer : public IDeserializer {
	public:
			
	  #ifndef PDG_NO_64BIT
		//! Deserialize an 8 byte (64 bit) value
		/*! internal pointer is advanced 8 btyes
		 \return the deserialized 64 bit value
		 */
		virtual uint64 deserialize_8u();
	  #endif

		//! Deserialize a 4 byte (32 bit) value
		/*! internal pointer is advanced 4 btyes
		 \return the deserialized 32 bit value
		 */
		virtual uint32 deserialize_4u();
		
		//! Deserialize a 3 byte (24 bit) value
		/*! internal pointer is advanced 3 btyes
		 \return the deserialized 24 bit value
		 */
		virtual uint32 deserialize_3u();
		
		//! Deserialize a 2 byte (16 bit) value
		/*! internal pointer is advanced 2 btyes
		 \return the deserialized 16 bit value
		 */
		virtual uint16 deserialize_2u();
		
		//! Deserialize an 1 byte (8 bit) value
		/*! internal pointer is advanced 1 btye
		 \return the deserialized 8 bit value
		 */
		virtual uint8 deserialize_1u();
		
		//! Deserialize a boolean (1 bit) value
		/*! internal pointer gets advanced by 1 or 0 bytes, depending on bytes required to store the value
		 \return the boolean that was deserialized
		 This is optimized for series of boolean values, so that between 1 and 8 bools serialized 
		 sequentially only use a single byte
		 */
		virtual bool   deserialize_bool();

		//! Deserialize an unsigned value
		/*! internal pointer is advanced by 1, 3 or 5 bytes, depending on bytes required to store value
		 \return the unsigned value that was deserialized
		 */
		virtual uint32 deserialize_uint();

		//! Deserialize a pdg::Color value
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the Color that was deserialized
		 */
		virtual Color deserialize_color() MAY_THROW( out_of_data );

		//! Deserialize a pdg::Offset value
		/*! internal pointer is advanced by 1 to 9 bytes, depending on bytes required to store value
		 \return the Offset that was deserialized
		 */
		virtual Offset deserialize_offset() MAY_THROW( out_of_data );

		//! Deserialize a pdg::Rect
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the Rect that was deserialized
		 */
		virtual Rect   deserialize_rect() MAY_THROW( out_of_data );

		//! Deserialize a pdg::RotatedRect
		/*! internal pointer is advanced by ? bytes, depending on bytes required to store value
		 \return the RotatedRect that was deserialized
		 */
		virtual RotatedRect   deserialize_rotr() MAY_THROW( out_of_data );

		//! Deserialize a pdg::Quad
		/*! internal pointer is advanced by 1, 3 or 5 bytes, depending on bytes required to store value
		 \return the Quad that was deserialized
		 */
		virtual Quad   deserialize_quad() MAY_THROW( out_of_data );

		//! Deserialize a string value
		/*! \param outStr, a buffer to hold the resulting string
		 \param strMaxLen, the max length of the output string, including the NUL terminator, that the buffer can accomodate
		 If the serialized string is longer than outStrMaxLen, it will be truncated and the internal pointer p will still be
		 advanced past the string to the start of the next entity in the internal buffer.
		 \return the length of the string in bytes
		 */
		virtual uint32 deserialize_str(char* outStr, uint32 strMaxLen);

		//! get length of a string value, as if it were deserialized
		/*! internal pointer is NOT advanced. Use this to pre-flight allocation for longer strings
		 \return the length of the serialized string in bytes
		 */
		virtual uint32 deserialize_strGetLen();

		//! Deserialize an arbitrary block of memory
		/*! \param p a reference to a buffer pointer; pointer gets advanced past mem block to start of next entity in buffer
		 \param outMem, a buffer to hold the resulting block of memory
		 \param memMaxLen, the max length of the output block, that the buffer can accomodate
		 If the serialized string is longer than outStrMaxLen, it will be truncated and the buffer pointer p will still be
		 advanced past the string to the start of the next entity in the buffer.
		 \return the length of the string in bytes
		 */
		virtual uint32 deserialize_mem(void* outMem, uint32 memMaxLen);

		//! Deserialize a string value
		/*! internal pointer is NOT advanced. Use this to pre-flight allocation for blocks of memory
		 \return the length of the serialized block of memory in bytes
		 */
		virtual uint32 deserialize_memGetLen();

		//! Deserialize a serializable object
		/*! internal pointer gets advanced past object to start of next entity in buffer
		 \return NULL, or a newly created Serializable object
		 NULL return happens if serialized object reference was null, or if the object could not be deserialized
		 If object could not be deserialized, either because the subclass was unknown or because the data was corrupt,
		 the internal pointer will still be advanced past the object to the start of the next entity in the buffer.
		 */
		virtual ISerializable* deserialize_obj();

		
		// --------------------------------------------
		// data methods
		// --------------------------------------------
		
		void setDataPtr(void* ptr, uint32 ptrSize);
		
		// --------------------------------------------
		// constructors
		// --------------------------------------------
		
		Deserializer();
		Deserializer(void* ptr, uint32 ptrSize);
		virtual ~Deserializer();
		
	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
		SCRIPT_OBJECT_REF mDeserializerScriptObj;
		
		static void registerScriptClass(uint32 classTag, FUNCTION_REF constructorFunc);
	#endif

	protected:
		
		virtual char* statusDump(int hiliteBytes = 0);

		uint8* mDataPtr;
		uint8* mDataEnd;
		uint8* p;   // current position in pointer
		uint32 mDataSize;
		uint8  mLastBoolByte;
		int mBoolBitOffset;
		
		std::vector<ISerializable*> mDeserializedInstances;
	};
	
} // end namespace pdg

#endif // DESERIALIZER_H_INCLUDED

