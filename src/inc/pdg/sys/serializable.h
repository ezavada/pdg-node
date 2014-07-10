// -----------------------------------------------
// serializable.h
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


#ifndef SERIALIZABLE_H_INCLUDED
#define SERIALIZABLE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/refcounted.h"
#include "pdg/sys/global_types.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

//! \defgroup Serialization
//! Collection of classes, types and constants used for serializing and deserialing data

// reserved class tags (all tags in range 0xFFFFFF00 to 0xFFFFFFFF are reserved by PDG)

#define CLASSTAG_SPRITE  		0xFFFFFF01
#define CLASSTAG_SPRITE_LAYER 	0xFFFFFF02
#define CLASSTAG_TILE_LAYER 	0xFFFFFF03
#define CLASSTAG_ANIMATED  		0xFFFFFF04
#define CLASSTAG_ANIM_HELPER    0xFFFFFF05
#define CLASSTAG_COLLIDE_HELPER 0xFFFFFF06
#define CLASSTAG_DRAW_HELPER    0xFFFFFF07
#define CLASSTAG_IMAGE    		0xFFFFFF08

#define CLASSTAG_ACTION  		0xFFFFFF10
#define CLASSTAG_REQUEST 		0xFFFFFF11
#define CLASSTAG_UPDATE  		0xFFFFFF12

#define CLASSTAG_SCENARIO_INFO 	0xFFFFFF20
#define CLASSTAG_GAME_INFO     	0xFFFFFF21
#define CLASSTAG_PLAYER_INFO   	0xFFFFFF22
#define CLASSTAG_AI_INFO       	0xFFFFFF23
#define CLASSTAG_GAME_ENGINE   	0xFFFFFF24


// use this just to declare a class tag for a class that derives directly from Serializable<>
#define SERIALIZABLE_TAG( _classTag ) \
		virtual uint32 getMyClassTag() const { return _classTag; }

// use this to declare a creator, independently of a tag, for a class that derives INDIRECTLY from Serializable<>
#define SERIALIZABLE_CREATOR( _className ) \
		static pdg::ISerializable* CreateInstance() { return new _className(); }\
        _className() {}

// use this to declare the serialization methods, for any direct or indirect derivation of ISerializable or Serializable()
#define SERIALIZABLE_METHODS() \
		virtual uint32 getSerializedSize(pdg::ISerializer* serializer) const;\
		virtual void serialize(pdg::ISerializer* serializer) const;\
		virtual void deserialize(pdg::IDeserializer* deserializer);

// use this to do the whole thing with a single macro, for a class that derives INDIRECTLY from Serializable<>
#define MAKE_SERIALIZABLE( _classTag, _className ) \
		virtual uint32 getMyClassTag() const { return _classTag; }\
		static pdg::ISerializable* CreateInstance() { return new _className(); }\
		virtual uint32 getSerializedSize(pdg::ISerializer* serializer) const;\
		virtual void serialize(pdg::ISerializer* serializer) const;\
		virtual void deserialize(pdg::IDeserializer* deserializer);\
		_className() {}


namespace pdg {
	
	class ISerializer;
	class IDeserializer;
	
	// ==============================================
	// Serializable class
	// ==============================================
	
	//! Represents an object which can be serialized
	
	/*! Subclasses must implement all the methods
		derive from the serializable template like this:
	 
	    #define CLASS_TAG_MY_SERIALIZABLECLASS 500
	 
		class MySerializableClass : public Serializable<MySerializableClass> { 
		    SERIALIZABLE_TAG( CLASS_TAG_MYSERIALIZABLECLASS )
			... 
		};
	 
		If your class derives indirectly from Serializable, then you will need
	    to also add SERIALIZABLE_CREATOR() like this:
	 
		class MyDerivedSerializableClass : public MySerializableClass {
			SERIALIZABLE_TAG( CLASS_TAG_MYDERIVEDCLASS )
			...
			SERIALIZABLE_CREATOR( MyDerivedSerializableClass )
		};
	 
		\ingroup Serialization
	 */
	
	class ISerializable : public RefCountedObj {
	public:

		//! Get number of bytes needed to hold serialized state
		/*! \return required number of bytes, or zero if error
		 */
		virtual uint32 getSerializedSize(ISerializer* serializer) const = 0;
		
		//! Write self out to a serializer
		/*! \param serializer pointer to a serializer
		 \sa deserialize() and getSerializedSize()
		 */
		virtual void serialize(ISerializer* serializer) const = 0;
		
		//! Read self from a deserializer
		/*! \param buffer pointer to a deserializer
		 \sa serialize() and getSerializedSize();
		 */
		virtual void deserialize(IDeserializer* deserializer) = 0;
		
		//! Get a tag value that uniquely represents this class. 
		/*! This value is used to determine which class factory to call to
		 instantiate an object during deserialization, so this value
		 cannot change during the lifetime of the serialized stream
		 Use SERIALIZABLE_TAG() to declare this in your classes
		 */
		virtual uint32 getMyClassTag() const = 0;
		
		ISerializable() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mISerializableScriptObj);
    	#endif
		}
		virtual ~ISerializable() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		CleanupISerializableScriptObject(mISerializableScriptObj);
    	#endif
		};

	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
		SCRIPT_OBJECT_REF mISerializableScriptObj;
	#endif

	};

	
	template <class T> class Serializable : public ISerializable {
	public:
				
		// ----------------------------------------------------------------------
		// serializable registry and factory methods, needed for deserialization
		// ----------------------------------------------------------------------
		
		static ISerializable* CreateInstance() { return new T; }
		
		virtual ~Serializable() {};
	};


	
} // end namespace pdg

#endif // SERIALIZABLE_H_INCLUDED

