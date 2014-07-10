// -----------------------------------------------
// pdg_script_impl.cpp
//
// wrapper definitions for pdg classes 
// that are exposed to scripting languages without
// any specific language dependencies
//
// This file is parsed by tools/gen-script_interface.sh
// to generate an interface file
//
// This should only be done when the wrappers need 
// to change.
//
// Written by Ed Zavada, 2013
// Copyright (c) 2013, Dream Rock Studios, LLC
// All Rights Reserved Worldwide
// -----------------------------------------------


#include "pdg_script_macros.h"

%#include "pdg_project.h"

%#define PDG_COMPILING_SCRIPT_IMPL

%#include "pdg_script_impl.h"
%#include "pdg_script_interface.h"

%#include "internals.h"
%#include "pdg-lib.h"

%#include <sstream>
%#include <cmath>
%#include <cstdlib>


//%#define PDG_DEBUG_SCRIPTING

%#ifndef PDG_DEBUG_SCRIPTING
  // if the javascript bindings are not being debugged, we ignore whatever
  // is inside these blocks
  %#define SCRIPT_DEBUG_ONLY(_expression)
%#else
  %#define SCRIPT_DEBUG_ONLY DEBUG_ONLY
%#endif

namespace pdg {

bool s_HaveSavedError = false;

// ========================================================================================
//MARK: MemBlock
// ========================================================================================

BINDING_INITIALIZER_IMPL(MemBlock)
    EXPORT_CLASS_SYMBOLS("MemBlock", MemBlock, , ,
    	// method section
		HAS_METHOD(MemBlock, "getData", GetData)
		HAS_METHOD(MemBlock, "getDataSize", GetDataSize)
		HAS_METHOD(MemBlock, "getByte", GetByte)
		HAS_METHOD(MemBlock, "getBytes", GetBytes)
    );
	END
METHOD_IMPL(MemBlock, GetData)
	METHOD_SIGNATURE("", [string Binary], 0, ())
    REQUIRE_ARG_COUNT(0);
	VALUE resultVal = EncodeBinary(self->ptr, self->bytes);
	RETURN(resultVal);
	END
METHOD_IMPL(MemBlock, GetDataSize)
	METHOD_SIGNATURE("", number, 0, ())
	REQUIRE_ARG_COUNT(0);
	RETURN_UINT32(self->bytes);
	END
METHOD_IMPL(MemBlock, GetByte)
	METHOD_SIGNATURE("", number, 1, ([number uint] i))
    REQUIRE_ARG_COUNT(1);
    REQUIRE_UINT32_ARG(1, i);
	RETURN_UNSIGNED(self->ptr[i]);
	END
METHOD_IMPL(MemBlock, GetBytes)
	METHOD_SIGNATURE("", [string Binary], 2, ([number uint] start, [number uint] len))
    REQUIRE_ARG_COUNT(2);
    REQUIRE_UINT32_ARG(1, start);
    REQUIRE_UINT32_ARG(2, len);
	VALUE resultVal = EncodeBinary(self->ptr + start, len);
	RETURN(resultVal);
	END

CLEANUP_IMPL(MemBlock)

NATIVE_CONSTRUCTOR_IMPL(MemBlock)
	return new MemBlock(0, 0, false);
	END

// ========================================================================================
//MARK: Config Manager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(ConfigManager)
    EXPORT_CLASS_SYMBOLS("ConfigManager", ConfigManager, , ,
    	// method section
		HAS_METHOD(ConfigManager, "useConfig", UseConfig)
		HAS_METHOD(ConfigManager, "getConfigString", GetConfigString)
		HAS_METHOD(ConfigManager, "getConfigLong", GetConfigLong)
		HAS_METHOD(ConfigManager, "getConfigFloat", GetConfigFloat)
		HAS_METHOD(ConfigManager, "getConfigBool", GetConfigBool)
		HAS_METHOD(ConfigManager, "setConfigString", SetConfigString)
		HAS_METHOD(ConfigManager, "setConfigLong", SetConfigLong)
		HAS_METHOD(ConfigManager, "setConfigFloat", SetConfigFloat)
		HAS_METHOD(ConfigManager, "setConfigBool", SetConfigBool)
    );
	END
METHOD_IMPL(ConfigManager, UseConfig)
	METHOD_SIGNATURE("", boolean, 1, (string inConfigName))
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inConfigName);
	bool result = self->useConfig(inConfigName);
	RETURN_BOOL(result);
	END
METHOD_IMPL(ConfigManager, GetConfigString)	
	METHOD_SIGNATURE("", string, 1, (string inConfigItemName))
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	std::string outVal;
	bool found = self->getConfigString(inConfigItemName, outVal);
	if (found) {
		RETURN_STRING(outVal.c_str());
	} else {
		RETURN_UNDEFINED;
	}
	END
METHOD_IMPL(ConfigManager, GetConfigLong)	
	METHOD_SIGNATURE("", number, 1, (string inConfigItemName))
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	long outVal;
	bool found = self->getConfigLong(inConfigItemName, outVal);
	if (found) {
		RETURN_INTEGER(outVal);
	} else {
		RETURN_UNDEFINED;
	}
	END
METHOD_IMPL(ConfigManager, GetConfigFloat)	
	METHOD_SIGNATURE("", number, 1, (string inConfigItemName))
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	float outVal;
	bool found = self->getConfigFloat(inConfigItemName, outVal);
	if (found) {
		RETURN_NUMBER(outVal);
	} else {
		RETURN_UNDEFINED;
	}
	END
METHOD_IMPL(ConfigManager, GetConfigBool)	
	METHOD_SIGNATURE("", boolean, 1, (string inConfigItemName))
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	bool outVal;
	bool found = self->getConfigBool(inConfigItemName, outVal);
	if (found) {
		RETURN_BOOL(outVal);
	} else {
		RETURN_UNDEFINED;
	}
	END
METHOD_IMPL(ConfigManager, SetConfigString)	
	METHOD_SIGNATURE("", undefined, 2, (string inConfigItemName, string inValue))
    REQUIRE_ARG_COUNT(2);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	REQUIRE_STRING_ARG(2, inValue);
	std::string theString(inValue);
	self->setConfigString(inConfigItemName, theString);
	NO_RETURN;
	END
METHOD_IMPL(ConfigManager, SetConfigLong)	
	METHOD_SIGNATURE("", undefined, 2, (string inConfigItemName, [number int] inValue))
    REQUIRE_ARG_COUNT(2);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	REQUIRE_INT32_ARG(2, inValue);
	self->setConfigLong(inConfigItemName, inValue);
	NO_RETURN;
	END
METHOD_IMPL(ConfigManager, SetConfigFloat)	
	METHOD_SIGNATURE("", undefined, 2, (string inConfigItemName, number inValue))
    REQUIRE_ARG_COUNT(2);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	REQUIRE_NUMBER_ARG(2, inValue);
	self->setConfigFloat(inConfigItemName, inValue);
	NO_RETURN;
	END
METHOD_IMPL(ConfigManager, SetConfigBool)	
	METHOD_SIGNATURE("", undefined, 2, (string inConfigItemName, boolean inValue))
    REQUIRE_ARG_COUNT(2);
	REQUIRE_STRING_ARG(1, inConfigItemName);
	REQUIRE_BOOL_ARG(2, inValue);
	self->setConfigBool(inConfigItemName, inValue);
	NO_RETURN;
	END

NATIVE_CONSTRUCTOR_IMPL(ConfigManager)
	return ConfigManager::getSingletonInstance();
	END


// ========================================================================================
//MARK: Log Manager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(LogManager)
    EXPORT_CLASS_SYMBOLS("LogManager", LogManager,
    	// constant section
		HAS_CONSTANT(LogManager, init_CreateUniqueNewFile)
		HAS_CONSTANT(LogManager, init_OverwriteExisting)
		HAS_CONSTANT(LogManager, init_AppendToExisting)
		HAS_CONSTANT(LogManager, init_StdOut)
		HAS_CONSTANT(LogManager, init_StdErr)
	   ,// property section
		HAS_PROPERTY(LogManager, LogLevel)
	   ,// method section
		HAS_METHOD(LogManager, "initialize", Initialize)
		HAS_METHOD(LogManager, "writeLogEntry", WriteLogEntry)
		HAS_METHOD(LogManager, "binaryDump", BinaryDump)
    );
	END
PROPERTY_IMPL(LogManager, LogLevel, INT32)
METHOD_IMPL(LogManager, Initialize)
	METHOD_SIGNATURE("", undefined, 2, (string inLogNameBase, [number int] initMode = LogManager.init_StdOut)); 
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, inLogNameBase);
	OPTIONAL_INT32_ARG(2, initMode, pdg::LogManager::init_StdOut);
	self->initialize(inLogNameBase, initMode);
	NO_RETURN;
	END
METHOD_IMPL(LogManager, WriteLogEntry)
	METHOD_SIGNATURE("", undefined, 3, ([number int] level, string category, string message)); 
    REQUIRE_ARG_COUNT(3);
	REQUIRE_INT32_ARG(1, level);
	REQUIRE_STRING_ARG(2, category);
	REQUIRE_STRING_ARG(3, message);
	self->writeLogEntry(level, category, message);
	NO_RETURN;
	END
STATIC_METHOD_IMPL(LogManager, BinaryDump)
	METHOD_SIGNATURE("", string, 3, ({[string Binary]|[object MemBlock]} inData, [number int] length = 0, [number int] bytesPerLine = 20)); 
    REQUIRE_ARG_MIN_COUNT(1);
    OPTIONAL_INT32_ARG(2, length, 0);
    OPTIONAL_INT32_ARG(3, bytesPerLine, 20);
	int dataSize = 0;
	char* inData = 0;
    if (VALUE_IS_STRING(ARGV[0])) {
     	size_t bytes = 0;
    	uint8* ptr = (uint8*) DecodeBinary(ARGV[0], &bytes);
		inData = (char*)ptr;
		dataSize = (length == 0) ? bytes : length;
	} else {
    	REQUIRE_NATIVE_OBJECT_ARG(1, memBlock, MemBlock);
    	inData = memBlock->ptr;
    	dataSize = (length == 0) ? memBlock->bytes : length;
    }
    int outBufSize = (4 * dataSize) + (6 * dataSize/bytesPerLine) + (4 * bytesPerLine) + 32;
    char* outBuf = new char[outBufSize];
    OS::binaryDump(outBuf, outBufSize, inData, dataSize, bytesPerLine);
	VALUE dumpStrVal = STR2VAL(outBuf);
    delete outBuf;
	RETURN( dumpStrVal );
	END

log& main_getDebugLog();  // defined in main-{platform}.cpp

NATIVE_CONSTRUCTOR_IMPL(LogManager)
    LogManager *theLogMgr = LogManager::getSingletonInstance();
	pdg::log& debugLog = main_getDebugLog();
	debugLog.setLogManager(theLogMgr);
	return theLogMgr;
	END


// ========================================================================================
//MARK: IEventHandler
// ========================================================================================

BINDING_INITIALIZER_IMPL(IEventHandler)
    EXPORT_CLASS_SYMBOLS("IEventHandler", IEventHandler, , , );
	END

CLEANUP_IMPL(IEventHandler)


// ========================================================================================
//MARK: Event Emitter Base Class
// ========================================================================================

#define HAS_EMITTER_METHODS(klass) \
    HAS_METHOD(klass, "addHandler", AddHandler)        \
    HAS_METHOD(klass, "removeHandler", RemoveHandler)  \
    HAS_METHOD(klass, "clear", Clear)                  \
    HAS_METHOD(klass, "blockEvent", BlockEvent)        \
    HAS_METHOD(klass, "unblockEvent", UnblockEvent)    \

#define EMITTER_BASE_CLASS_IMPL(klass) CR \
  METHOD_IMPL(klass, AddHandler) CR \
	METHOD_SIGNATURE("add a new handler for some event type, or for all events if no type specified. " CR \
	" \\param inHandler the object to handle events"  \
	" \\param inEventType the type of event to handle",  \
		undefined, 2, ([object IEventHandler] inHandler, [number int] inEventType = all_events)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_ARG(1, inHandler, IEventHandler); CR \
	DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], IEventHandler); CR \
	OPTIONAL_INT32_ARG(2, inType, pdg::all_events); CR \
	self->addHandler(inHandler, inType); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, RemoveHandler) CR \
	METHOD_SIGNATURE("remove a handler for some event type, or for all events (see note) if no type specified. "  CR \
	"If the handler is listed multiple times it will only remove it once.\n"  CR \
	"NOTE: inType == all_events doesn't work quite like you might expect. If "  CR \
	"you have registered a handler for multiple events, but not with all_events, " CR \
	"doing removeHandler(handler, all_events) will do nothing. Basically, " CR \
	"all_events is a special event type that matches all event types when " CR \
 	"considering whether to invoke a handler or not.\n" CR \
	"It is safe to call remove handler from within an event handler's handleEvent() call." CR \
	" \\param inHandler the object to handle events"  \
	" \\param inEventType the type of event to stop handling (see note)",  \
	 	undefined, 2, ([object IEventHandler] inHandler, [number int] inEventType = all_events)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_ARG(1, inHandler, IEventHandler); CR \
	OPTIONAL_INT32_ARG(2, inType, pdg::all_events); CR \
	self->removeHandler(inHandler, inType); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, Clear) CR \
	METHOD_SIGNATURE("remove all handlers", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->clear(); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, BlockEvent) CR \
	METHOD_SIGNATURE("temporarily ignore all events of a particular type. " CR \
	"Events that are blocked are NOT cached for later, they are just dropped." CR \
	" \\param inEventType the type of event to block",  \
		undefined, 1, ([number int] inEventType)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, inEventType); CR \
	self->blockEvent(inEventType); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, UnblockEvent) CR \
	METHOD_SIGNATURE("stop ignoring events of a particular type " CR \
	" \\param inEventType the type of event to unblock",  \
		undefined, 1, ([number int] inEventType)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, inEventType); CR \
	self->unblockEvent(inEventType); CR \
	NO_RETURN; CR \
	END

BINDING_INITIALIZER_IMPL(EventEmitter)
    EXPORT_CLASS_SYMBOLS("EventEmitter", EventEmitter, , ,
    	// method section
		HAS_EMITTER_METHODS(EventEmitter)
    );
	END
EMITTER_BASE_CLASS_IMPL(EventEmitter)

NATIVE_CONSTRUCTOR_IMPL(EventEmitter)
    return new EventEmitter();
	END


// ========================================================================================
//MARK: Event Manager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(EventManager)
    EXPORT_CLASS_SYMBOLS("EventManager", EventManager, , ,
    	// method section
		HAS_EMITTER_METHODS(EventManager)
		HAS_METHOD(EventManager, "isKeyDown", IsKeyDown)
		HAS_METHOD(EventManager, "isRawKeyDown", IsRawKeyDown)
		HAS_METHOD(EventManager, "isButtonDown", IsButtonDown)
		HAS_METHOD(EventManager, "getDeviceOrientation", GetDeviceOrientation) // not yet implemented in C++
    );
	END

EMITTER_BASE_CLASS_IMPL(EventManager)
STATIC_METHOD_IMPL(EventManager, IsRawKeyDown)
	METHOD_SIGNATURE("", boolean, 1, ([number int] keyCode));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, keyCode);
	RETURN_BOOL( OS::isRawKeyDown(keyCode) );
	END
STATIC_METHOD_IMPL(EventManager, IsButtonDown)
	METHOD_SIGNATURE("", boolean, 1, ([number int] buttonNumber = 0));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_INT32_ARG(1, buttonNumber, 0);
	RETURN_BOOL( OS::isButtonDown(buttonNumber) );
	END

NATIVE_CONSTRUCTOR_IMPL(EventManager)
	return EventManager::getSingletonInstance();
	END

// ========================================================================================
//MARK: Resource Manager
// ========================================================================================

//DECLARE_SYMBOL(name);


SINGLETON_INITIALIZER_IMPL(ResourceManager)
    EXPORT_CLASS_SYMBOLS("ResourceManager", ResourceManager,
	   ,// property section
		HAS_PROPERTY(ResourceManager, Language)
	   ,// method section
		HAS_METHOD(ResourceManager, "openResourceFile", OpenResourceFile)
		HAS_METHOD(ResourceManager, "closeResourceFile", CloseResourceFile)
		HAS_METHOD(ResourceManager, "getImage", GetImage)
		HAS_METHOD(ResourceManager, "getImageStrip", GetImageStrip)
// Temporarily removing this from interface until it actually works to load sounds
// that are inside the resource file
//	%#ifndef PDG_NO_SOUND CR
//		HAS_METHOD(ResourceManager, "getSound", GetSound)
//	%#endif CR // !PDG_NO_SOUND
		HAS_METHOD(ResourceManager, "getString", GetString)
		HAS_METHOD(ResourceManager, "getResourceSize", GetResourceSize)
		HAS_METHOD(ResourceManager, "getResource", GetResource)
		HAS_METHOD(ResourceManager, "getResourcePaths", GetResourcePaths)
    );
	END

PROPERTY_IMPL(ResourceManager, Language, STRING)
METHOD_IMPL(ResourceManager, OpenResourceFile)
	METHOD_SIGNATURE("", number, 1, (string filename));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, filename);
	int refNum = self->openResourceFile(filename);
	RETURN_INTEGER(refNum);
	END
METHOD_IMPL(ResourceManager, CloseResourceFile)
	METHOD_SIGNATURE("", undefined, 1, ([number int] refNum));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, refNum);
	self->closeResourceFile(refNum);
	NO_RETURN;
	END
METHOD_IMPL(ResourceManager, GetString)
	METHOD_SIGNATURE("", string, 2, ([number int] id, [number int] substring = -1));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_INT32_ARG(1, id);
	OPTIONAL_INT32_ARG(2, substring, -1);
	std::string ioStr;
	const char* outStr = self->getString(ioStr, id, substring);
	RETURN_STRING(outStr);
	END
METHOD_IMPL(ResourceManager, GetResourceSize)
	METHOD_SIGNATURE("", number, 1, (string resourceName));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, resourceName);
	unsigned long resSize = self->getResourceSize(resourceName);
	RETURN_UNSIGNED(resSize);
	END
METHOD_IMPL(ResourceManager, GetResource)
	METHOD_SIGNATURE("", [string Binary], 1, (string resourceName));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, resourceName);
	OPTIONAL_INT32_ARG(2, maxSize, -1);
	unsigned long bufferSize;
	if (maxSize < 0) {
		bufferSize = self->getResourceSize(resourceName);
	} else {
		bufferSize = maxSize;
	}
	uint8* buffer = (uint8*) std::malloc(bufferSize);
	bool loaded = self->getResource(resourceName, buffer, bufferSize);
	if (!loaded) {
		std::free(buffer);
		RETURN_FALSE;
	}
	VALUE resultVal = EncodeBinary(buffer, bufferSize);
	std::free(buffer);
	RETURN(resultVal);
	END
METHOD_IMPL(ResourceManager, GetResourcePaths)
	METHOD_SIGNATURE("", string, 0, ());
    REQUIRE_ARG_COUNT(0);
	std::string outStr = self->getResourcePaths();
	RETURN_STRING(outStr.c_str());
	END

NATIVE_CONSTRUCTOR_IMPL(ResourceManager)
	return ResourceManager::getSingletonInstance();
	END

// ========================================================================================
//MARK: ISerializable
// ========================================================================================

#define HAS_SERIALIZABLE_METHODS(klass) \
    HAS_GETTER(klass, MyClassTag)             \
    HAS_GETTER(klass, SerializedSize)         \
    HAS_METHOD(klass, "serialize", Serialize)                     \
    HAS_METHOD(klass, "deserialize", Deserialize)                   \


#define SERIALIZABLE_BASE_CLASS_IMPL(klass) CR \
  GETTER_IMPL(klass, MyClassTag, UINT32) CR \
  METHOD_IMPL(klass, GetSerializedSize) CR \
	METHOD_SIGNATURE("get size of this object's data for the given stream", CR \
		[number uint], 1, ([object Serializer] serializer)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, serializer, Serializer); CR \
    uint32 dataSize = self->getSerializedSize(serializer); CR \
	RETURN_UINT32(dataSize); CR \
	END CR \
  METHOD_IMPL(klass, Serialize) CR \
	METHOD_SIGNATURE("write this object's data into the given stream", CR \
		undefined, 1, ([object Serializer] serializer)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, serializer, Serializer); CR \
    self->serialize(serializer); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, Deserialize) CR \
	METHOD_SIGNATURE("read this object's data from the given stream", CR \
		undefined, 1, ([object Deserializer] deserializer)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, deserializer, Deserializer); CR \
    try { CR \
    	self->deserialize(deserializer); CR \
		NO_RETURN; CR \
    } catch(out_of_data& e) { CR \
    	THROW_ERR(e.what()); CR \
	} catch(bad_tag& e) { CR \
    	THROW_ERR(e.what()); CR \
	} catch(sync_error& e) { CR \
    	THROW_ERR(e.what()); CR \
	} catch(unknown_object& e) { CR \
    	THROW_ERR(e.what()); CR \
	} CR \
	END CR \


BINDING_INITIALIZER_IMPL(ISerializable) 
    EXPORT_CLASS_SYMBOLS("ISerializable", ISerializable, , , );
	END


CLEANUP_IMPL(ISerializable)


// ========================================================================================
//MARK: Serializer
// ========================================================================================


BINDING_INITIALIZER_IMPL(Serializer)
    EXPORT_CLASS_SYMBOLS("Serializer", Serializer, , ,
    	// method section
	  %#ifndef PDG_NO_64BIT CR
		HAS_METHOD(Serializer, "serialize_8", Serialize_8)   // no 64 bit int in Script
		HAS_METHOD(Serializer, "serialize_8u", Serialize_8u)
	  %#endif CR
		HAS_METHOD(Serializer, "serialize_d", Serialize_d)
		HAS_METHOD(Serializer, "serialize_f", Serialize_f)
		HAS_METHOD(Serializer, "serialize_4", Serialize_4)
		HAS_METHOD(Serializer, "serialize_4u", Serialize_4u)
		HAS_METHOD(Serializer, "serialize_3u", Serialize_3u)
		HAS_METHOD(Serializer, "serialize_2", Serialize_2)
		HAS_METHOD(Serializer, "serialize_2u", Serialize_2u)
		HAS_METHOD(Serializer, "serialize_1", Serialize_1)
		HAS_METHOD(Serializer, "serialize_1u", Serialize_1u)
		HAS_METHOD(Serializer, "serialize_bool", Serialize_bool)
		HAS_METHOD(Serializer, "serialize_uint", Serialize_uint)
 		HAS_METHOD(Serializer, "serialize_color", Serialize_color)
 		HAS_METHOD(Serializer, "serialize_offset", Serialize_offset)
 		HAS_METHOD(Serializer, "serialize_point", Serialize_point)
 		HAS_METHOD(Serializer, "serialize_vector", Serialize_vector)
 		HAS_METHOD(Serializer, "serialize_rect", Serialize_rect)
 		HAS_METHOD(Serializer, "serialize_rotr", Serialize_rotr)
 		HAS_METHOD(Serializer, "serialize_quad", Serialize_quad)
		HAS_METHOD(Serializer, "serialize_str", Serialize_str)
		HAS_METHOD(Serializer, "serialize_mem", Serialize_mem)
		HAS_METHOD(Serializer, "serialize_obj", Serialize_obj)
		HAS_METHOD(Serializer, "serializedSize", SerializedSize)
		HAS_METHOD(Serializer, "getDataSize", GetDataSize)
		HAS_METHOD(Serializer, "getDataPtr", GetDataPtr)
    );
	END
METHOD_IMPL(Serializer, Serialize_d)
	METHOD_SIGNATURE("", undefined, 1, (number val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, val);
	self->serialize_d(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_f)
	METHOD_SIGNATURE("", undefined, 1, (number val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, val);
	self->serialize_f(val);
	NO_RETURN;
	END
%#ifndef PDG_NO_64BIT
METHOD_IMPL(Serializer, Serialize_8)
	METHOD_SIGNATURE("", undefined, 1, (number val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, val);
	self->serialize_8(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_8u)
	METHOD_SIGNATURE("", undefined, 1, (number val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, val);
	self->serialize_8u(val);
	NO_RETURN;
	END
%#endif
METHOD_IMPL(Serializer, Serialize_4)
	METHOD_SIGNATURE("", undefined, 1, ([number int] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, val);
	self->serialize_4(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_4u)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_UINT32_ARG(1, val);
	self->serialize_4u(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_3u)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_UINT32_ARG(1, val);
	self->serialize_3u(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_2)
	METHOD_SIGNATURE("", undefined, 1, ([number int] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, val);
	self->serialize_2(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_2u)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_UINT32_ARG(1, val);
	self->serialize_2u(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_1)
	METHOD_SIGNATURE("", undefined, 1, ([number int] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, val);
	self->serialize_1(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_1u)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_UINT32_ARG(1, val);
	self->serialize_1u(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_bool)
	METHOD_SIGNATURE("", undefined, 1, (boolean val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_BOOL_ARG(1, val);
	self->serialize_bool(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_uint)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_UINT32_ARG(1, val);
	self->serialize_uint(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_color)
	METHOD_SIGNATURE("", undefined, 1, ([object Color] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_COLOR_ARG(1, val);
	self->serialize_color(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_offset)
	METHOD_SIGNATURE("", undefined, 1, ([object Offset] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_OFFSET_ARG(1, val);
	self->serialize_offset(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_point)
	METHOD_SIGNATURE("", undefined, 1, ([object Point] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_POINT_ARG(1, val);
	self->serialize_point(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_vector)
	METHOD_SIGNATURE("", undefined, 1, ([object Vector] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_VECTOR_ARG(1, val);
	self->serialize_vector(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_rect)
	METHOD_SIGNATURE("", undefined, 1, ([object Rect] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_RECT_ARG(1, r);
	self->serialize_rect(r);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_rotr)
	METHOD_SIGNATURE("", undefined, 1, ([object RotatedRect] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_ROTATED_RECT_ARG(1, val);
	self->serialize_rotr(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_quad)
	METHOD_SIGNATURE("", undefined, 1, ([object Quad] val));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_QUAD_ARG(1, val);
	self->serialize_quad(val);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_str)
	METHOD_SIGNATURE("", undefined, 1, (string str));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, str);
	self->serialize_str(str);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, Serialize_mem)
	METHOD_SIGNATURE("", undefined, 1, ({[string Binary]|[object MemBlock]} mem));
    REQUIRE_ARG_COUNT(1);
    bool isStr = VALUE_IS_STRING(ARGV[0]);
    if (!isStr && !VALUE_IS_OBJECT(ARGV[0])) {
    	THROW_TYPE_ERR("argument 1 (mem) must be either a binary string or an object of type MemBlock");
    }
    if (isStr) {
    	size_t bytes = 0;
    	uint8* ptr = (uint8*) DecodeBinary(ARGV[0], &bytes);
		self->serialize_mem(ptr, bytes);
	} else {
    	REQUIRE_NATIVE_OBJECT_ARG(1, memBlock, MemBlock);
    	self->serialize_mem(memBlock->ptr, memBlock->bytes);
    }
	NO_RETURN;
	END
METHOD_IMPL(Serializer, GetDataSize)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
 	uint32 dataSize = self->getDataSize();
	RETURN_UINT32(dataSize);
	END
METHOD_IMPL(Serializer, GetDataPtr)
	METHOD_SIGNATURE("", [object MemBlock], 0, ());
    REQUIRE_ARG_COUNT(0);
    // mem block doesn't own this memory, it belongs to the Serializer
 	MemBlock* memBlock = new MemBlock((char*)self->getDataPtr(), self->getDataSize(), false);
	RETURN_NATIVE_OBJECT(memBlock, MemBlock);
	END

CLEANUP_IMPL(Serializer)

NATIVE_CONSTRUCTOR_IMPL(Serializer)
	return new Serializer();
	END


// ========================================================================================
//MARK: Deserializer
// ========================================================================================

//DECLARE_SYMBOL(getSerializedSize);
//DECLARE_SYMBOL(serialize);
//DECLARE_SYMBOL(deserialize);
//DECLARE_SYMBOL(getMyClassTag);
//DECLARE_SYMBOL(constructor);

BINDING_INITIALIZER_IMPL(Deserializer) 
    EXPORT_CLASS_SYMBOLS("Deserializer", Deserializer, , ,
    	// method section
	  %#ifndef PDG_NO_64BIT CR
		HAS_METHOD(Deserializer, "deserialize_8", Deserialize_8)
		HAS_METHOD(Deserializer, "deserialize_8u", Deserialize_8u)
	  %#endif CR
		HAS_METHOD(Deserializer, "deserialize_d", Deserialize_d)
		HAS_METHOD(Deserializer, "deserialize_f", Deserialize_f)
		HAS_METHOD(Deserializer, "deserialize_4", Deserialize_4)
		HAS_METHOD(Deserializer, "deserialize_4u", Deserialize_4u)
		HAS_METHOD(Deserializer, "deserialize_3u", Deserialize_3u)
		HAS_METHOD(Deserializer, "deserialize_2", Deserialize_2)
		HAS_METHOD(Deserializer, "deserialize_2u", Deserialize_2u)
		HAS_METHOD(Deserializer, "deserialize_1", Deserialize_1)
		HAS_METHOD(Deserializer, "deserialize_1u", Deserialize_1u)
		HAS_METHOD(Deserializer, "deserialize_bool", Deserialize_bool)
		HAS_METHOD(Deserializer, "deserialize_uint", Deserialize_uint)
 		HAS_METHOD(Deserializer, "deserialize_color", Deserialize_color)
 		HAS_METHOD(Deserializer, "deserialize_offset", Deserialize_offset)
 		HAS_METHOD(Deserializer, "deserialize_point", Deserialize_point)
 		HAS_METHOD(Deserializer, "deserialize_vector", Deserialize_vector)
 		HAS_METHOD(Deserializer, "deserialize_rect", Deserialize_rect)
 		HAS_METHOD(Deserializer, "deserialize_rotr", Deserialize_rotr)
 		HAS_METHOD(Deserializer, "deserialize_quad", Deserialize_quad)
		HAS_METHOD(Deserializer, "deserialize_str", Deserialize_str)
	//	HAS_METHOD(Deserializer, "deserialize_strGetLen", Deserialize_strGetLen)  // returns utf-8 length, useless in JavaScript
		HAS_METHOD(Deserializer, "deserialize_mem", Deserialize_mem)
		HAS_METHOD(Deserializer, "deserialize_memGetLen", Deserialize_memGetLen)
		HAS_METHOD(Deserializer, "deserialize_obj", Deserialize_obj)
		HAS_METHOD(Deserializer, "setDataPtr", SetDataPtr)
    );
	END
METHOD_IMPL(Deserializer, Deserialize_d)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
    try {
		double val = self->deserialize_d();
		RETURN_NUMBER(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_f)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		float val = self->deserialize_f();
		RETURN_NUMBER(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
%#ifndef PDG_NO_64BIT
METHOD_IMPL(Deserializer, Deserialize_8)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		int64 val = self->deserialize_8();
		RETURN_NUMBER(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_8u)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint64 val = self->deserialize_8u();
		RETURN_NUMBER(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
%#endif
METHOD_IMPL(Deserializer, Deserialize_4)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		int32 val = self->deserialize_4();
		RETURN_INT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_4u)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint32 val = self->deserialize_4u();
		RETURN_UINT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_3u)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint32 val = self->deserialize_3u();
		RETURN_UINT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_2)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		int16 val = self->deserialize_2();
		RETURN_INT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_2u)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint16 val = self->deserialize_2u();
		RETURN_UINT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_1)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		int8 val = self->deserialize_1();
		RETURN_INT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_1u)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint8 val = self->deserialize_1u();
		RETURN_UINT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_bool)
	METHOD_SIGNATURE("", boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		bool val = self->deserialize_bool();
		RETURN_BOOL(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_uint)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint32 val = self->deserialize_uint();
		RETURN_UINT32(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_color)
	METHOD_SIGNATURE("", [object Color], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Color val = self->deserialize_color();
		RETURN_COLOR(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_offset)
	METHOD_SIGNATURE("", [object Offset], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Offset val = self->deserialize_offset();
		RETURN_OFFSET(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_point)
	METHOD_SIGNATURE("", [object Point], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Point val = self->deserialize_point();
		RETURN_POINT(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_vector)
	METHOD_SIGNATURE("", [object Vector], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Vector val = self->deserialize_vector();
		RETURN_VECTOR(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_rect)
	METHOD_SIGNATURE("", [object Rect], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Rect val = self->deserialize_rect();
		RETURN_RECT(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_rotr)
	METHOD_SIGNATURE("", [object RotatedRect], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		RotatedRect val = self->deserialize_rotr();
		RETURN_ROTATED_RECT(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_quad)
	METHOD_SIGNATURE("", [object Quad], 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		Quad val = self->deserialize_quad();
		RETURN_QUAD(val);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_str)
	METHOD_SIGNATURE("", string, 0, ());
    REQUIRE_ARG_COUNT(0);
    try {
		std::string mystr;
		self->deserialize_string(mystr);
		RETURN_STRING(mystr.c_str());
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	} catch(bad_tag& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_strGetLen)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	try {
		uint32 len = self->deserialize_strGetLen();
		RETURN_UINT32(len);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	} catch(bad_tag& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, Deserialize_mem)
	METHOD_SIGNATURE("", [object MemBlock], 0, ());
    REQUIRE_ARG_COUNT(0);
    size_t len, len2;
    try {
		len = self->deserialize_memGetLen();
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	}
    char* mem = (char*) std::malloc(len);
    try {
		len2 = self->deserialize_mem(mem, len);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	} catch(bad_tag& e) {
    	THROW_ERR(e.what());
	}
    if (len2 != len) {
    	THROW_ERR("Deserializer internal error, deserialized memory length mismatch " << len 
    		<< " (" << (void*)len << ") != " << len2 << " (" << (void*)len2 << ")");
    }
    // MemBlock owns this memory and will free it when destroyed
 	MemBlock* memBlock = new MemBlock(mem, len, true);
	RETURN_NATIVE_OBJECT(memBlock, MemBlock);
	END
METHOD_IMPL(Deserializer, Deserialize_memGetLen)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
    try {
		uint32 len = self->deserialize_memGetLen();
		RETURN_UINT32(len);
	} catch(out_of_data& e) {
    	THROW_ERR(e.what());
	} catch(bad_tag& e) {
    	THROW_ERR(e.what());
	}
	END
METHOD_IMPL(Deserializer, SetDataPtr)
	METHOD_SIGNATURE("", undefined, 1, ({[string Binary]|[object MemBlock]} data));
    REQUIRE_ARG_COUNT(1);
    if (!VALUE_IS_STRING(ARGV[0]) && !VALUE_IS_OBJECT(ARGV[0])) {
    	THROW_TYPE_ERR("argument 1 (mem) must be either a binary string or an object of type MemBlock");
    }
    if (VALUE_IS_STRING(ARGV[0])) {
    	size_t bytes = 0;
    	uint8* ptr = (uint8*) DecodeBinary(ARGV[0], &bytes);
		self->setDataPtr(ptr, bytes);
	} else {
    	REQUIRE_NATIVE_OBJECT_ARG(1, memBlock, MemBlock);
    	self->setDataPtr(memBlock->ptr, memBlock->bytes);
    }
	NO_RETURN;
	END

CLEANUP_IMPL(Deserializer)

NATIVE_CONSTRUCTOR_IMPL(Deserializer)
	return new Deserializer();
	END


// ========================================================================================
//MARK: Image
// ========================================================================================

#define HAS_IMAGE_METHODS(klass) \
    HAS_GETTER(klass, Width)                  \
    HAS_GETTER(klass, Height)                 \
    HAS_GETTER(klass, ImageBounds)            \
    HAS_GETTER(klass, Subsection)             \
    HAS_SETTER(klass, TransparentColor)       \
    HAS_PROPERTY(klass, Opacity)              \
    HAS_SETTER(klass, EdgeClamping)           \
    HAS_METHOD(klass, "getTransparentColor", GetTransparentColor)  \
    HAS_METHOD(klass, "retainData", RetainData)                     \
    HAS_METHOD(klass, "retainAlpha", RetainAlpha)                   \
    HAS_METHOD(klass, "prepareToRasterize", PrepareToRasterize)     \
    HAS_GETTER(klass, AlphaValue)             \
    HAS_METHOD(klass, "getPixel", GetPixel)  \

#define IMAGE_BASE_CLASS_IMPL(klass) CR \
  PROPERTY_IMPL(klass, TransparentColor, COLOR) CR \
  GETTER_IMPL(klass, Width, INTEGER) CR \
  GETTER_IMPL(klass, Height, INTEGER) CR \
  METHOD_IMPL(klass, GetImageBounds) CR \
	METHOD_SIGNATURE("get image boundary rect, optionally with top left at given point", CR \
		[object Rect], 1, ([object Point] at)); CR \
    REQUIRE_ARG_MIN_COUNT(0); CR \
    OPTIONAL_POINT_ARG(1, at, pdg::Point(0,0)); CR \
    Rect r = self->getImageBounds(at); CR \
	RETURN(RECT2VAL(r)); CR \
	END CR \
  METHOD_IMPL(klass, GetSubsection) CR \
	METHOD_SIGNATURE("get image that is an arbitrary subsection of this image", CR \
		[object Image], 1, ({[object Quad]|[object Rect]} quad)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_QUAD_ARG(1, quad); CR \
	Image* image = self->getSubsection(quad); CR \
	RETURN_NATIVE_OBJECT(image, Image); CR \
	END CR \
  METHOD_IMPL(klass, GetOpacity) CR \
	METHOD_SIGNATURE("get opacity of this image: 0.0 - completely transparent to 1.0 - completely solid", CR \
		number, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	uint8 opacity = self->getOpacity(); CR \
	double opacityFloat = (float)opacity / 255.0f; CR \
	RETURN_NUMBER(opacityFloat); CR \
	END CR \
  METHOD_IMPL(klass, SetOpacity) CR \
	METHOD_SIGNATURE("set opacity of this image as range from either (0-255) or (0.0 to 1.0)", CR \
		undefined, 1, (number opacity)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	OPTIONAL_UINT32_ARG(1, opacity, 0xffffffff); CR \
	if (opacity == 0xffffffff) { CR \
		REQUIRE_NUMBER_ARG(1, opacityFloat); CR \
		opacity = std::floor(255.0f * opacityFloat); CR \
	} CR \
	if (opacity > 255) opacity = 255; CR \
	self->setOpacity(opacity); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, SetEdgeClamping) CR \
	METHOD_SIGNATURE("set whether image uses edge clamping or not", CR \
		undefined, 1, (boolean inUseEdgeClamp)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_BOOL_ARG(1, inUseEdgeClamp); CR \
	self->setEdgeClamping(inUseEdgeClamp); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, RetainData) CR \
	METHOD_SIGNATURE("retain pixel data for use by Image.getPixel()", CR \
		undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->retainData(); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, RetainAlpha) CR \
	METHOD_SIGNATURE("retain alpha data for use by Image.getAlphaValue() or per-pixel sprite collisions", CR \
		undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->retainAlpha(); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, PrepareToRasterize) CR \
	METHOD_SIGNATURE("bind the image into an OpenGL texture and free image data from main memory", CR \
		number, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->prepareToRasterize(); CR \
	NO_RETURN; CR \
	END CR \
  METHOD_IMPL(klass, GetAlphaValue) CR \
	METHOD_SIGNATURE("", number, 2, ({ ([object Point] p) | ([number int] x, [number int] y) }));  CR \
    REQUIRE_ARG_MIN_COUNT(1);  CR \
    OPTIONAL_INT32_ARG(2, y, -1); CR \
    uint8 a; CR \
    if (y != -1) { CR \
    	REQUIRE_INT32_ARG(1, x); CR \
    	a = self->getAlphaValue(x, y); CR \
    } else { CR \
    	REQUIRE_POINT_ARG(1, p); CR \
    	a = self->getAlphaValue(p.x, p.y); CR \
    } CR \
	RETURN_UNSIGNED(a); CR \
	END CR \
  METHOD_IMPL(klass, GetPixel) CR \
	METHOD_SIGNATURE("", [object Color], 2, ({ ([object Point] p) | ([number int] x, [number int] y) }));  CR \
    REQUIRE_ARG_MIN_COUNT(1);  CR \
    OPTIONAL_INT32_ARG(2, y, -1); CR \
    pdg::Color c; CR \
    if (y != -1) { CR \
    	REQUIRE_INT32_ARG(1, x); CR \
    	c = self->getPixel(x, y); CR \
    } else { CR \
    	REQUIRE_POINT_ARG(1, p); CR \
    	c = self->getPixel(p.x, p.y); CR \
    } CR \
	RETURN(COLOR2VAL(c)); CR \
	END


BINDING_INITIALIZER_IMPL(Image)
    EXPORT_CLASS_SYMBOLS("Image", Image, , ,
    	// method section
    	HAS_IMAGE_METHODS(Image)
    );
	END

IMAGE_BASE_CLASS_IMPL(Image)

CLEANUP_IMPL(Image)

NATIVE_CONSTRUCTOR_IMPL(Image)
	SETUP_NON_SCRIPT_CALL;
	if (ARGC < 1) {
		return 0;
	} else if (!VALUE_IS_STRING(ARGV[0])) {
		SAVE_SYNTAX_ERR("argument 1 must be a string (filename)");
		return 0;
	} else {
		VALUE_TO_CSTRING(filename, ARGV[0]);
		return Image::createImageFromFile(filename);
	}
	END

// ========================================================================================
//MARK: ImageStrip
// ========================================================================================

BINDING_INITIALIZER_IMPL(ImageStrip)
    EXPORT_CLASS_SYMBOLS("ImageStrip", ImageStrip, , ,
    	// method section
		HAS_IMAGE_METHODS(ImageStrip)
		HAS_GETTER(ImageStrip, Frame)
		HAS_PROPERTY(ImageStrip, FrameWidth)
		HAS_PROPERTY(ImageStrip, NumFrames)
    );
	END
IMAGE_BASE_CLASS_IMPL(ImageStrip)
PROPERTY_IMPL(ImageStrip, NumFrames, INT32)
PROPERTY_IMPL(ImageStrip, FrameWidth, INT32)
METHOD_IMPL(ImageStrip, GetFrame)
	METHOD_SIGNATURE("get image that is a single frame of this multi-frame image",
		[object Image], 1, ([number int] frameNum));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, frameNum);
	Image* image = self->getFrame(frameNum);
	RETURN_NATIVE_OBJECT(image, Image);
	END

CLEANUP_IMPL(ImageStrip)


NATIVE_CONSTRUCTOR_IMPL(ImageStrip)
	SETUP_NON_SCRIPT_CALL;
	if (ARGC < 1) {
		return 0;
	} else if (!VALUE_IS_STRING(ARGV[0])) {
		SAVE_SYNTAX_ERR("argument 1 must be a string (filename)");
		return 0;
	} else {
		VALUE_TO_CSTRING(filename, ARGV[0]);
		return ImageStrip::createImageStripFromFile(filename);
	}
	END

%#ifndef PDG_NO_GUI
// ========================================================================================
//MARK: Graphics Manager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(GraphicsManager)
    EXPORT_CLASS_SYMBOLS("GraphicsManager", GraphicsManager, , ,
    	// method section
		HAS_GETTER(GraphicsManager, NumScreens)
		HAS_GETTER(GraphicsManager, FPS)
		HAS_PROPERTY(GraphicsManager, TargetFPS)
		HAS_GETTER(GraphicsManager, Mouse)
		HAS_METHOD(GraphicsManager, "getCurrentScreenMode", GetCurrentScreenMode)
		HAS_METHOD(GraphicsManager, "getNumSupportedScreenModes", GetNumSupportedScreenModes)
		HAS_METHOD(GraphicsManager, "getNthSupportedScreenMode", GetNthSupportedScreenMode)
		HAS_METHOD(GraphicsManager, "setScreenMode", SetScreenMode)
		HAS_METHOD(GraphicsManager, "createWindowPort", CreateWindowPort)
		HAS_METHOD(GraphicsManager, "createFullScreenPort", CreateFullScreenPort)
		HAS_METHOD(GraphicsManager, "closeGraphicsPort", CloseGraphicsPort)
		HAS_METHOD(GraphicsManager, "createFont", CreateFont)
		HAS_METHOD(GraphicsManager, "getMainPort", GetMainPort)
		HAS_METHOD(GraphicsManager, "switchToFullScreenMode", SwitchToFullScreenMode)
		HAS_METHOD(GraphicsManager, "switchToWindowMode", SwitchToWindowMode)
		HAS_METHOD(GraphicsManager, "inFullScreenMode", InFullScreenMode)
    );
	END
GETTER_IMPL(GraphicsManager, NumScreens, INTEGER)
GETTER_IMPL(GraphicsManager, FPS, NUMBER)
PROPERTY_IMPL(GraphicsManager, TargetFPS, NUMBER)
CUSTOM_GETTER_IMPL(GraphicsManager, Mouse, POINT, 1, MIN_,
	OPTIONAL_INT32_ARG(1, mouseNumber, 0); CR , 
    pdg::Point theMouse = self->getMouse(mouseNumber), ([number int] mouseNumber = 0) )
CUSTOM_GETTER_IMPL(GraphicsManager, NumSupportedScreenModes, INTEGER, 0, ,
	OPTIONAL_INT32_ARG(1, screenNum, GraphicsManager::screenNum_PrimaryScreen); CR ,
    int32 theNumSupportedScreenModes = self->getNumSupportedScreenModes(screenNum), ([number int] screen = PRIMARY_SCREEN) )
METHOD_IMPL(GraphicsManager, SetScreenMode);
 	METHOD_SIGNATURE("changes specified screen to closest matching mode", 
 		undefined, 4, ([number int] width, [number int] height, [number int] screenNum = PRIMARY_SCREEN, [number int] bpp = 0));
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_INT32_ARG(1, width);
	REQUIRE_INT32_ARG(2, height);
	OPTIONAL_INT32_ARG(3, screenNum, GraphicsManager::screenNum_PrimaryScreen);
	OPTIONAL_INT32_ARG(4, bpp, 0);
	self->setScreenMode(width, height, screenNum, bpp);
	NO_RETURN;
	END
METHOD_IMPL(GraphicsManager, CreateWindowPort)
	METHOD_SIGNATURE("create windowed drawing port with given dimensions, title and depth", 
		[object Port], 3, ([object Rect] rect, string windName = "", [number int] bpp = 0));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_RECT_ARG(1, rect);
	OPTIONAL_STRING_ARG(2, windName, "");
	OPTIONAL_INT32_ARG(3, bpp, 0);
	Port* port = self->createWindowPort(rect, windName, bpp);
	RETURN_NATIVE_OBJECT(port, Port);
	END
METHOD_IMPL(GraphicsManager, CreateFullScreenPort)
	METHOD_SIGNATURE("create full screen drawing port with given dimensions on given screen, optionally changing depth", 
		[object Port], 3, ([object Rect] rect, [number int] screenNum = PRIMARY_SCREEN, boolean allowResChange = true, [number int] bpp = 0));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_RECT_ARG(1, rect);
	OPTIONAL_INT32_ARG(2, screenNum, GraphicsManager::screenNum_PrimaryScreen);
	OPTIONAL_BOOL_ARG(3, allowResChange, true);
	OPTIONAL_INT32_ARG(4, bpp, 0);
	Port* port = self->createFullScreenPort(rect, screenNum, allowResChange, bpp);
	RETURN_NATIVE_OBJECT(port, Port);
	END
METHOD_IMPL(GraphicsManager, CloseGraphicsPort)
	METHOD_SIGNATURE("close given port, along with its window and restore screen mode if changed", 
		undefined, 1, ([object Port] port = MAIN_PORT));
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_NATIVE_OBJECT_ARG(1, port, Port, 0);
	self->closeGraphicsPort(port);
	NO_RETURN;
	END
METHOD_IMPL(GraphicsManager, CreateFont)
	METHOD_SIGNATURE("get a font with optional scaling adjust", 
		[object Font], 2, (string fontName, number scalingFactor = 1.0));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, fontName);
	OPTIONAL_NUMBER_ARG(2, scalingFactor, 1.0f);
	Font* font = self->createFont(fontName, scalingFactor);
	RETURN_NATIVE_OBJECT(font, Font);
	END
METHOD_IMPL(GraphicsManager, GetMainPort)
	METHOD_SIGNATURE("return the primary graphics port", 
		[object Port], 0, ());
    REQUIRE_ARG_COUNT(0);
    Port* port = self->getMainPort();
	RETURN_NATIVE_OBJECT(port, Port);
	END
METHOD_IMPL(GraphicsManager, SwitchToFullScreenMode)
	METHOD_SIGNATURE("change a port to fullscreen mode, return true on success", 
		boolean, 0, (boolean allowResChange = false, [object Port] port = MAIN_PORT));
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_BOOL_ARG(1, allowResChange, 0);
	OPTIONAL_NATIVE_OBJECT_ARG(2, port, Port, 0);
	bool result = self->switchToFullScreenMode(allowResChange, port);
	RETURN_BOOL(result);
	END
METHOD_IMPL(GraphicsManager, SwitchToWindowMode)
	METHOD_SIGNATURE("change a port to window mode, return true on success", 
		boolean, 0, ([object Port] port = MAIN_PORT, string windName = ""));
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_NATIVE_OBJECT_ARG(1, port, Port, 0);
	OPTIONAL_STRING_ARG(2, windName, "");
	bool result = self->switchToWindowMode(port, windName);
	RETURN_BOOL(result);
	END
METHOD_IMPL(GraphicsManager, InFullScreenMode)
	METHOD_SIGNATURE("return whether primary graphics port is fullscreen or not", 
		boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
	bool fullscreen = self->inFullScreenMode();
	RETURN_BOOL(fullscreen);
	END
	
NATIVE_CONSTRUCTOR_IMPL(GraphicsManager)
	return GraphicsManager::getSingletonInstance();
	END


// ========================================================================================
//MARK: Font
// ========================================================================================

#define FONT_GETTER_IMPL(prop) \
CUSTOM_GETTER_IMPL(Font, prop, NUMBER, 1, MIN_,                		\
	REQUIRE_INT32_ARG(1, size); CR                             		\
    OPTIONAL_UINT32_ARG(2, style, Graphics::textStyle_Plain); CR ,	\
    float the##prop = self->get##prop(size, style),                 \
    (number size, [number int] style = textStyle_Plain) )

WRAPPER_INITIALIZER_IMPL(Font)
    EXPORT_CLASS_SYMBOLS("Font", Font, , ,
    	// method section
		HAS_GETTER(Font, FontName)
		HAS_GETTER(Font, FontHeight)
		HAS_GETTER(Font, FontLeading)
		HAS_GETTER(Font, FontAscent)
		HAS_GETTER(Font, FontDescent)
    );
	END
GETTER_IMPL(Font, FontName, STRING)
FONT_GETTER_IMPL(FontHeight)
FONT_GETTER_IMPL(FontLeading)
FONT_GETTER_IMPL(FontAscent)
FONT_GETTER_IMPL(FontDescent)

CLEANUP_IMPL(Font)

// ========================================================================================
//MARK: Port
// ========================================================================================

WRAPPER_INITIALIZER_IMPL_CUSTOM(Port,
  nativeObj->mPortScriptObj = obj )
    EXPORT_CLASS_SYMBOLS("Port", Port, , ,
    	// method section
		HAS_PROPERTY(Port, ClipRect)
		HAS_PROPERTY(Port, Cursor)
		HAS_GETTER(Port, DrawingArea)
		HAS_METHOD(Port, "fillRect", FillRect)
		HAS_METHOD(Port, "frameRect", FrameRect)
		HAS_METHOD(Port, "drawLine", DrawLine)
		HAS_METHOD(Port, "frameOval", FrameOval)
		HAS_METHOD(Port, "fillOval", FillOval)
		HAS_METHOD(Port, "frameCircle", FrameCircle)
		HAS_METHOD(Port, "fillCircle", FillCircle)
		HAS_METHOD(Port, "frameRoundRect", FrameRoundRect)
		HAS_METHOD(Port, "fillRoundRect", FillRoundRect)
		HAS_METHOD(Port, "fillRectEx", FillRectEx)
		HAS_METHOD(Port, "frameRectEx", FrameRectEx)
		HAS_METHOD(Port, "drawLineEx", DrawLineEx)
		HAS_METHOD(Port, "fillRectWithGradient", FillRectWithGradient)
		HAS_METHOD(Port, "drawText", DrawText)
		HAS_METHOD(Port, "drawImage", DrawImage)
		HAS_METHOD(Port, "drawTexture", DrawTexture)
		HAS_METHOD(Port, "drawTexturedSphere", DrawTexturedSphere)
		HAS_METHOD(Port, "getTextWidth", GetTextWidth)
		HAS_METHOD(Port, "getCurrentFont", GetCurrentFont)
		HAS_METHOD(Port, "setFont", SetFont)
		HAS_METHOD(Port, "setFontForStyle", SetFontForStyle)
		HAS_METHOD(Port, "setFontScalingFactor", SetFontScalingFactor)
		HAS_METHOD(Port, "startTrackingMouse", StartTrackingMouse)
		HAS_METHOD(Port, "stopTrackingMouse", StopTrackingMouse)
		HAS_METHOD(Port, "resetCursor", ResetCursor)
    );
	END
GETTER_IMPL(Port, DrawingArea, RECT)
PROPERTY_IMPL(Port, ClipRect, RECT)
METHOD_IMPL(Port, FillRect)
	METHOD_SIGNATURE("", undefined, 2, ({[object Quad]|[object Rect]} quad, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_QUAD_ARG(1, quad);
	OPTIONAL_COLOR_ARG(2, rgba, PDG_BLACK_COLOR);
	self->fillRect(quad, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FrameRect)
	METHOD_SIGNATURE("", undefined, 2, ({[object Quad]|[object Rect]} quad, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_QUAD_ARG(1, quad);
	OPTIONAL_COLOR_ARG(2, rgba, PDG_BLACK_COLOR);
	self->frameRect(quad, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawLine)
	METHOD_SIGNATURE("", undefined, 2, ([object Point] from, [object Point] to, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_POINT_ARG(1, from);
	REQUIRE_POINT_ARG(2, to);
	OPTIONAL_COLOR_ARG(3, rgba, PDG_BLACK_COLOR);
	self->drawLine(from, to, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FrameOval)
	METHOD_SIGNATURE("", undefined, 4, ([object Point] centerPt, number xRadius, number yRadius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(3);
	REQUIRE_POINT_ARG(1, centerPt);
	REQUIRE_NUMBER_ARG(2, xRadius);
	REQUIRE_NUMBER_ARG(3, yRadius);
	OPTIONAL_COLOR_ARG(4, rgba, PDG_BLACK_COLOR);
	self->frameOval(centerPt, xRadius, yRadius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FillOval)
	METHOD_SIGNATURE("", undefined, 4, ([object Point] centerPt, number xRadius, number yRadius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(3);
	REQUIRE_POINT_ARG(1, centerPt);
	REQUIRE_NUMBER_ARG(2, xRadius);
	REQUIRE_NUMBER_ARG(3, yRadius);	
	OPTIONAL_COLOR_ARG(4, rgba, PDG_BLACK_COLOR);
	self->fillOval(centerPt, xRadius, yRadius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FrameCircle)
	METHOD_SIGNATURE("", undefined, 3, ([object Point] centerPt, number radius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_POINT_ARG(1, centerPt);
	REQUIRE_NUMBER_ARG(2, radius);
	OPTIONAL_COLOR_ARG(3, rgba, PDG_BLACK_COLOR);
	self->frameCircle(centerPt, radius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FillCircle)
	METHOD_SIGNATURE("", undefined, 3, ([object Point] centerPt, number radius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_POINT_ARG(1, centerPt);
	REQUIRE_NUMBER_ARG(2, radius);
	OPTIONAL_COLOR_ARG(3, rgba, PDG_BLACK_COLOR);
	self->fillCircle(centerPt, radius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FrameRoundRect)
	METHOD_SIGNATURE("", undefined, 3, ([object Rect] rect, number radius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_RECT_ARG(1, rect);
	REQUIRE_NUMBER_ARG(2, radius);
	OPTIONAL_COLOR_ARG(3, rgba, PDG_BLACK_COLOR);
	self->frameRoundRect(rect, radius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FillRoundRect)
	METHOD_SIGNATURE("", undefined, 3, ([object Rect] rect, number radius, [object Color] rgba = "black")); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_RECT_ARG(1, rect);
	REQUIRE_NUMBER_ARG(2, radius);
	OPTIONAL_COLOR_ARG(3, rgba, PDG_BLACK_COLOR);
	self->fillRoundRect(rect, radius, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FillRectEx)
	METHOD_SIGNATURE("", undefined, 4, ({[object Quad]|[object Rect]} quad, [number uint] pattern, [number uint] patternShift, [object Color] rgba)); 
    REQUIRE_ARG_COUNT(4);
	REQUIRE_QUAD_ARG(1, quad);
	REQUIRE_UINT32_ARG(2, pattern);
	REQUIRE_UINT32_ARG(3, patternShift);
	REQUIRE_COLOR_ARG(4, rgba);
	self->fillRectEx(quad, pattern, patternShift, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FrameRectEx)
	METHOD_SIGNATURE("", undefined, 5, ({[object Quad]|[object Rect]} quad, [number uint] thickness, [number uint] pattern, [number uint] patternShift, [object Color] rgba)); 
    REQUIRE_ARG_COUNT(5);
	REQUIRE_QUAD_ARG(1, quad);
	REQUIRE_UINT32_ARG(2, thickness);
	REQUIRE_UINT32_ARG(3, pattern);
	REQUIRE_UINT32_ARG(4, patternShift);
	REQUIRE_COLOR_ARG(5, rgba);
	self->frameRectEx(quad, thickness, pattern, patternShift, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawLineEx)
	METHOD_SIGNATURE("", undefined, 6, ([object Point] from, [object Point] to, [number uint] thickness, [number uint] pattern, [number uint] patternShift, [object Color] rgba)); 
    REQUIRE_ARG_COUNT(6);
	REQUIRE_POINT_ARG(1, from);
	REQUIRE_POINT_ARG(2, to);
	REQUIRE_UINT32_ARG(3, thickness);
	REQUIRE_UINT32_ARG(4, pattern);
	REQUIRE_UINT32_ARG(5, patternShift);
	REQUIRE_COLOR_ARG(6, rgba);
	self->drawLineEx(from, to, thickness, pattern, patternShift, rgba);
	NO_RETURN;
	END
METHOD_IMPL(Port, FillRectWithGradient)
	METHOD_SIGNATURE("", undefined, 3, ({[object Quad]|[object Rect]} quad, [object Color] startColor, [object Color] endColor)); 
    REQUIRE_ARG_COUNT(3);
	REQUIRE_QUAD_ARG(1, quad);
	REQUIRE_COLOR_ARG(2, startColor);
	REQUIRE_COLOR_ARG(3, endColor);
	self->fillRectWithGradient(quad, startColor, endColor);
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawText)
	METHOD_SIGNATURE("", undefined, 5, (string text, {[object Point] location|[object Quad] quad|[object Rect] rect}, [number int] size, [number uint] style = textStyle_Plain, [object Color] rgba = 'black')); 
    REQUIRE_ARG_MIN_COUNT(3);
	REQUIRE_STRING_ARG(1, text);
	// Arg 2 handled below
	REQUIRE_INT32_ARG(3, size); 
	OPTIONAL_UINT32_ARG(4, style, Graphics::textStyle_Plain); 
	OPTIONAL_COLOR_ARG(5, rgba, PDG_BLACK_COLOR);
	if (VALUE_IS_POINT(ARGV[1])) { 
		// do this if 2nd argument is a point
		pdg::Point loc = VAL2POINT(ARGV[1]);
		self->drawText(text, loc, size, style, rgba);
    } else {
		// otherwise require a quad for 2nd argument
    	REQUIRE_QUAD_ARG(2, quad);
		self->drawText(text, quad, size, style, rgba);
    }
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawImage)
	METHOD_SIGNATURE("", undefined, 4, ({ ([object Image] img, [object Point] loc) | ([object Image] img, [object Quad] quad) | ([object Image] img, [object Rect] rect, [number int] fitType = fit_Fill, boolean clipOverflow = false) })); 
    REQUIRE_NATIVE_OBJECT_ARG(1, img, Image);
    ImageStrip* imgStrip = dynamic_cast<ImageStrip*>(img);
    int argAdd = (imgStrip->frames) ? 1 : 0;
    REQUIRE_ARG_MIN_COUNT( 2 + argAdd );
    int frameNum = 0;
    if (VALUE_IS_NUMBER(ARGV[1])) {
        frameNum = VAL2INT(ARGV[1]);
    }
    if (VALUE_IS_POINT(ARGV[1 + argAdd])) { 
        pdg::Point loc = VAL2POINT(ARGV[1 + argAdd]);
        // do this if 2nd argument is a point
        if (imgStrip->frames) {
            self->drawImage(imgStrip, frameNum, loc);
        } else {
            self->drawImage(img, loc);
        }
    } else if (ARGC > 2 + argAdd) {
        REQUIRE_RECT_ARG(2 + argAdd, rect);
        OPTIONAL_INT32_ARG(3 + argAdd, fitType, Image::fit_Fill);
        OPTIONAL_BOOL_ARG(4 + argAdd, clipOverflow, false);  	
        if (imgStrip->frames) {
            self->drawImage(imgStrip, frameNum, rect, (Image::FitType)fitType, clipOverflow);
        } else {
            self->drawImage(img, rect, (Image::FitType)fitType, clipOverflow);
        }
    } else {
        // otherwise require a quad for 2nd argument
        REQUIRE_QUAD_ARG(2 + argAdd, quad);
        if (imgStrip->frames) {
            self->drawImage(imgStrip, frameNum, quad);
        } else {
            self->drawImage(img, quad);
        }
    }
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawTexture)
	METHOD_SIGNATURE("", undefined, 2, ([object Image] img, [object Rect] r)); 
    REQUIRE_NATIVE_OBJECT_ARG(1, img, Image);
    ImageStrip* imgStrip = dynamic_cast<ImageStrip*>(img);
    int argAdd = (imgStrip->frames) ? 1 : 0;
    REQUIRE_ARG_MIN_COUNT( 2 + argAdd );
    REQUIRE_RECT_ARG(2 + argAdd, r);
    int frameNum = 0;
    if (VALUE_IS_NUMBER(ARGV[1])) {
        frameNum = VAL2INT(ARGV[1]);
    }
    if (imgStrip->frames) {
        self->drawTexture(imgStrip, frameNum, r);
    } else {
        self->drawTexture(img, r);
    }
	NO_RETURN;
	END
METHOD_IMPL(Port, DrawTexturedSphere)
    METHOD_SIGNATURE("", undefined, 2, ({[object Image] img|[object ImageStrip] imgStrip, [number int] frameNum}, [object Point] loc, number radius, number rotation = 0, [object Offset] polarOffsetRadians = Offset(0,0), [object Offset] lightOffsetRadians = Offset(0,0)));
    REQUIRE_NATIVE_OBJECT_ARG(1, img, Image);
    ImageStrip* imgStrip = dynamic_cast<ImageStrip*>(img);
    int argAdd = (imgStrip->frames) ? 1 : 0;
    REQUIRE_ARG_MIN_COUNT( 3 + argAdd );
    REQUIRE_POINT_ARG(2 + argAdd, loc);
    REQUIRE_NUMBER_ARG(3 + argAdd, radius);
    OPTIONAL_NUMBER_ARG(4 + argAdd, rotation, 0.0f);
    OPTIONAL_OFFSET_ARG(5 + argAdd, polarOffsetRadians, Offset());
    OPTIONAL_OFFSET_ARG(6 + argAdd, lightOffsetRadians, Offset());
    int frameNum = 0;
    if (VALUE_IS_NUMBER(ARGV[1])) {
        frameNum = VAL2INT(ARGV[1]);
    }
    if (imgStrip->frames) {
        self->drawTexturedSphere(imgStrip, frameNum, loc, radius, rotation, polarOffsetRadians, lightOffsetRadians);
    } else {
        self->drawTexturedSphere(img, loc, radius, rotation, polarOffsetRadians, lightOffsetRadians);
    }
    NO_RETURN;
    END
METHOD_IMPL(Port, GetTextWidth)
	METHOD_SIGNATURE("", number, 4, (string text, [number int] size, [number uint] style = textStyle_Plain, [number int] len = -1)); 
    REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_STRING_ARG(1, text);
	REQUIRE_INT32_ARG(2, size); 
	OPTIONAL_UINT32_ARG(3, style, Graphics::textStyle_Plain); 
	OPTIONAL_INT32_ARG(4, len, -1); 
	int width = self->getTextWidth(text, size, style, len);
	RETURN_INTEGER(width);
	END
METHOD_IMPL(Port, GetCurrentFont)
	METHOD_SIGNATURE("", [object Font], 1, ([number uint] style = textStyle_Plain)); 
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_UINT32_ARG(1, style, Graphics::textStyle_Plain);
	Font* font = self->getCurrentFont(style);
	RETURN_NATIVE_OBJECT(font, Font);
	END
METHOD_IMPL(Port, SetFont)
	METHOD_SIGNATURE("", undefined, 1, ([object Font] font = DEFAULT_FONT)); 
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_NATIVE_OBJECT_ARG(1, font, Font, 0);
	self->setFont(font);
	NO_RETURN;
	END
METHOD_IMPL(Port, SetFontForStyle)
	METHOD_SIGNATURE("", undefined, 2, ([number uint] style, [object Font] font = DEFAULT_FONT)); 
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_UINT32_ARG(1, style); 
	OPTIONAL_NATIVE_OBJECT_ARG(2, font, Font, 0);
	self->setFontForStyle(font, style);
	NO_RETURN;
	END
METHOD_IMPL(Port, SetFontScalingFactor)
	METHOD_SIGNATURE("", undefined, 1, (number scaleBy));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, scaleBy); 
	self->setFontScalingFactor(scaleBy);
	NO_RETURN;
	END
METHOD_IMPL(Port, StartTrackingMouse)
	METHOD_SIGNATURE("NOT IMPLEMENTED", number, 1, ([object Rect] rect));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_RECT_ARG(1, rect);
	int trackingRef = self->startTrackingMouse(rect);
	RETURN_INTEGER(trackingRef);
	END
METHOD_IMPL(Port, StopTrackingMouse)
	METHOD_SIGNATURE("NOT IMPLEMENTED", undefined, 1, ([number int] trackingRef));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, trackingRef); 
	self->stopTrackingMouse(trackingRef);
	NO_RETURN;
	END
METHOD_IMPL(Port, SetCursor)
	METHOD_SIGNATURE("NOT IMPLEMENTED", undefined, 1, ([object Image] cursorImage, [object Point] hotSpot));
    REQUIRE_ARG_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, cursorImage, Image); 
	REQUIRE_POINT_ARG(2, hotSpot); 
	self->setCursor(cursorImage, hotSpot);
	NO_RETURN;
	END
METHOD_IMPL(Port, GetCursor)
	METHOD_SIGNATURE("NOT IMPLEMENTED: get the Image that is being used as the cursor",
		[object Image], 0, ());
    REQUIRE_ARG_COUNT(0);
    Image* cursorImage = self->getCursor();
    RETURN_NATIVE_OBJECT(cursorImage, Image);
	END
METHOD_IMPL(Port, ResetCursor)
	METHOD_SIGNATURE("NOT IMPLEMENTED: restore the default system cursor",
		undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->resetCursor();
	NO_RETURN;
	END

CLEANUP_IMPL(Port)

%#endif //!PDG_NO_GUI


%#ifndef PDG_NO_SOUND

// ========================================================================================
//MARK: Sound Manager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(SoundManager)
    EXPORT_CLASS_SYMBOLS("SoundManager", SoundManager, , ,
    	// method section
		HAS_METHOD(SoundManager, "setVolume", SetVolume)
		HAS_METHOD(SoundManager, "setMute", SetMute)
    );
	END
METHOD_IMPL(SoundManager, SetVolume)
	METHOD_SIGNATURE("0.0 - silent to 1.0 - full volume", undefined, 1, (number level));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NUMBER_ARG(1, level);
	self->setVolume(level);
	NO_RETURN;
	END
METHOD_IMPL(SoundManager, SetMute)
	METHOD_SIGNATURE("", undefined, 1, (boolean muted));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_BOOL_ARG(1, muted);
	self->setMute(muted);
	NO_RETURN;
	END

// FUNCTION_IMPL(GetSoundManager)
// 	METHOD_SIGNATURE("", [object SoundManager], 0, ()); 
//     RETURN( SoundManager_getScriptSingletonInstance() );
//     END

NATIVE_CONSTRUCTOR_IMPL(SoundManager)
	return SoundManager::getSingletonInstance();
	END

// ========================================================================================
//MARK: Sound
// ========================================================================================

WRAPPER_INITIALIZER_IMPL_CUSTOM(Sound, 
  nativeObj->mEventEmitterScriptObj = obj; 
  nativeObj->mSoundScriptObj = obj;
  nativeObj->addRef() )
    EXPORT_CLASS_SYMBOLS("Sound", Sound, , ,
    	// method section
		HAS_EMITTER_METHODS(Sound)
		HAS_PROPERTY(Sound, Volume)
		HAS_METHOD(Sound, "play", Play)
		HAS_METHOD(Sound, "start", Start)
		HAS_METHOD(Sound, "stop", Stop)
		HAS_METHOD(Sound, "pause", Pause)
		HAS_METHOD(Sound, "resume", Resume)
		HAS_METHOD(Sound, "isPaused", IsPaused)
		HAS_METHOD(Sound, "setLooping", SetLooping)
		HAS_METHOD(Sound, "isLooping", IsLooping)
  		HAS_METHOD(Sound, "setPitch", SetPitch)
  		HAS_METHOD(Sound, "changePitch", ChangePitch)
  		HAS_METHOD(Sound, "setOffsetX", SetOffsetX)
  		HAS_METHOD(Sound, "changeOffsetX", ChangeOffsetX)
		HAS_METHOD(Sound, "fadeOut", FadeOut)
		HAS_METHOD(Sound, "fadeIn", FadeIn)
		HAS_METHOD(Sound, "changeVolume", ChangeVolume)
		HAS_METHOD(Sound, "skip", Skip)
		HAS_METHOD(Sound, "skipTo", SkipTo)
    );
	END
EMITTER_BASE_CLASS_IMPL(Sound)
PROPERTY_IMPL(Sound, Volume, NUMBER)
METHOD_IMPL(Sound, Play)
	METHOD_SIGNATURE("", undefined, 0, (number vol = 1.0, [number int] offsetX = 0, number pitch = 0, [number uint] fromMs = 0, [number int] lenMs = ENTIRE_LENGTH));
    REQUIRE_ARG_MIN_COUNT(0);
	OPTIONAL_NUMBER_ARG(1, vol, 1.0); 
	OPTIONAL_INT32_ARG(2, offsetX, 0); 
	OPTIONAL_NUMBER_ARG(3, pitch, 0.0); 
	OPTIONAL_UINT32_ARG(4, fromMs, 0); 
	OPTIONAL_INT32_ARG(5, lenMs, -1); 
	self->play(vol, offsetX, pitch, fromMs, lenMs);
	NO_RETURN;
	END
METHOD_IMPL(Sound, Start)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->start();
	NO_RETURN;
	END
METHOD_IMPL(Sound, Stop)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->stop();
	NO_RETURN;
	END
METHOD_IMPL(Sound, Pause)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->pause();
	NO_RETURN;
	END
METHOD_IMPL(Sound, Resume)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->resume();
	NO_RETURN;
	END
METHOD_IMPL(Sound, IsPaused)
	METHOD_SIGNATURE("", boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
	bool result = self->isPaused();
	RETURN_BOOL(result);
	END
METHOD_IMPL(Sound, SetLooping)
	METHOD_SIGNATURE("", Sound, 1, (boolean loopingOn));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_BOOL_ARG(1, loopingOn);
	self->setLooping(loopingOn);
	RETURN_THIS;
	END
METHOD_IMPL(Sound, IsLooping)
	METHOD_SIGNATURE("", boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
	bool result = self->isLooping();
	RETURN_BOOL(result);
	END
METHOD_IMPL(Sound, SetPitch)
	METHOD_SIGNATURE("", Sound, 0, (number pitchOffset));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NUMBER_ARG(1, pitchOffset);
	self->setPitch(pitchOffset);
	RETURN_THIS;
	END
METHOD_IMPL(Sound, ChangePitch)
	METHOD_SIGNATURE("", undefined, 0, (number targetOffset, [number int] msDuration, [number int] easing = easeInOutQuad));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_NUMBER_ARG(1, targetOffset);
    REQUIRE_INT32_ARG(2, msDuration);
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad);
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
		self->changePitch(targetOffset, msDuration, gEasingFunctions[easing]);
	} else {
		self->changePitch(targetOffset, msDuration);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sound, SetOffsetX)
	METHOD_SIGNATURE("", Sound, 0, ([number int] offsetX));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, offsetX);
	self->setOffsetX(offsetX);
	RETURN_THIS;
	END
METHOD_IMPL(Sound, ChangeOffsetX)
	METHOD_SIGNATURE("", undefined, 0, ([number int] targetOffset, [number int] msDuration, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, targetOffset);
    REQUIRE_INT32_ARG(2, msDuration);
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::linearTween);
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
		self->changePitch(targetOffset, msDuration, gEasingFunctions[easing]);
	} else {
		self->changePitch(targetOffset, msDuration);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sound, FadeOut)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] fadeMs, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(1);
    REQUIRE_UINT32_ARG(1, fadeMs);
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween);
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
		self->fadeOut(fadeMs, gEasingFunctions[easing]);
	} else {
		self->fadeOut(fadeMs);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sound, FadeIn)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] fadeMs, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(1);
    REQUIRE_UINT32_ARG(1, fadeMs);
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween);
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
		self->fadeIn(fadeMs, gEasingFunctions[easing]);
	} else {
		self->fadeIn(fadeMs);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sound, ChangeVolume)
	METHOD_SIGNATURE("", undefined, 2, (number level, [number uint] fadeMs, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_NUMBER_ARG(1, level);
    REQUIRE_UINT32_ARG(2, fadeMs);
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::linearTween);
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
		self->changeVolume(level, fadeMs, gEasingFunctions[easing]);
	} else {
		self->changeVolume(level, fadeMs);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sound, Skip)
	METHOD_SIGNATURE("", Sound, 1, ([number int] skipMilliseconds));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, skipMilliseconds);
	self->skip(skipMilliseconds);
	RETURN_THIS;
	END
METHOD_IMPL(Sound, SkipTo)
	METHOD_SIGNATURE("", Sound, 1, ([number uint] timeMs));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_UINT32_ARG(1, timeMs);
	self->skipTo(timeMs);
	RETURN_THIS;
	END

CLEANUP_IMPL(Sound)

NATIVE_CONSTRUCTOR_IMPL(Sound)
	SETUP_NON_SCRIPT_CALL;
	if (ARGC < 1) {
		return 0;
	} else if (!VALUE_IS_STRING(ARGV[0])) {
		SAVE_SYNTAX_ERR("argument 1 must be a string (filename)");
		return 0;
	} else {
		VALUE_TO_CSTRING(filename, ARGV[0]);
		return Sound::createSoundFromFile(filename);
	}
	END



%#endif //! PDG_NO_SOUND



// ========================================================================================
//MARK: FileManager
// ========================================================================================

DECLARE_SYMBOL(nodeName);
DECLARE_SYMBOL(isDirectory);
DECLARE_SYMBOL(found);

FACADE_INITIALIZER_IMPL(FileManager)
    EXPORT_CLASS_SYMBOLS("FileManager", FileManager, , ,
    	// method section
		HAS_METHOD(FileManager, "findFirst", FindFirst)
		HAS_METHOD(FileManager, "findNext", FindNext)
		HAS_METHOD(FileManager, "findClose", FindClose)
		HAS_GETTER(FileManager, ApplicationDataDirectory)
		HAS_GETTER(FileManager, ApplicationDirectory)
		HAS_GETTER(FileManager, ApplicationResourceDirectory)
    );
	END

SCRIPT_METHOD_IMPL(FileManager, FindFirst)
	METHOD_SIGNATURE("", object, 1, (string inFindName));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_STRING_ARG(1, inFindName);
    FindDataT* ioFindDataPtr = new FindDataT;
	bool found = OS::findFirst(inFindName, *ioFindDataPtr);
	OBJECT_REF jsFindData = OBJECT_CREATE_EMPTY(ioFindDataPtr);
	VALUE nodeName = STR2VAL(ioFindDataPtr->nodeName);
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(nodeName), nodeName);
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(isDirectory), BOOL2VAL(ioFindDataPtr->isDirectory));
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(found), BOOL2VAL(found));
	RETURN_OBJECT(jsFindData);
	END
SCRIPT_METHOD_IMPL(FileManager, FindNext)
	METHOD_SIGNATURE("", object, 1, (object inFindData));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_OBJECT_ARG(1, jsFindData);
	FindDataT* ioFindDataPtr = static_cast<FindDataT*>(OBJECT_PRIVATE_DATA(jsFindData));
	if (!ioFindDataPtr) {
		THROW_TYPE_ERR("findNext must only be called with the object returned by findFirst");
	}
	bool found = OS::findNext(*ioFindDataPtr);
	VALUE nodeName = STR2VAL(ioFindDataPtr->nodeName);
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(nodeName), nodeName);
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(isDirectory), BOOL2VAL(ioFindDataPtr->isDirectory));
	OBJECT_SET_PROPERTY_VALUE(jsFindData, SYMBOL(found), BOOL2VAL(found));
	RETURN_BOOL(found);
	END
SCRIPT_METHOD_IMPL(FileManager, FindClose)
	METHOD_SIGNATURE("", undefined, 1, (object inFindData));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_OBJECT_ARG(1, jsFindData);
	FindDataT* ioFindDataPtr = static_cast<FindDataT*>(OBJECT_PRIVATE_DATA(jsFindData));
	if (!ioFindDataPtr) {
		THROW_TYPE_ERR("findClose must only be called with the object returned by findFirst");
	}
	OS::findClose(*ioFindDataPtr);
	delete ioFindDataPtr;
	NO_RETURN;
	END
SCRIPT_METHOD_IMPL(FileManager, GetApplicationDataDirectory)
	METHOD_SIGNATURE("", string, 0, ());
    REQUIRE_ARG_COUNT(0);
	RETURN_STRING( OS::getApplicationDataDirectory() );
	END
SCRIPT_METHOD_IMPL(FileManager, GetApplicationDirectory)
	METHOD_SIGNATURE("", string, 0, ());
    REQUIRE_ARG_COUNT(0);
	RETURN_STRING( OS::getApplicationDirectory() );
	END
SCRIPT_METHOD_IMPL(FileManager, GetApplicationResourceDirectory)
	METHOD_SIGNATURE("", string, 0, ());
    REQUIRE_ARG_COUNT(0);
	RETURN_STRING( OS::getApplicationResourceDirectory() );
	END


// ========================================================================================
//MARK: TimerManager
// ========================================================================================

SINGLETON_INITIALIZER_IMPL(TimerManager)
    EXPORT_CLASS_SYMBOLS("TimerManager", TimerManager, , ,
    	// method section
		HAS_EMITTER_METHODS(TimerManager)
		HAS_METHOD(TimerManager, "startTimer", StartTimer)
		HAS_METHOD(TimerManager, "cancelTimer", CancelTimer)
		HAS_METHOD(TimerManager, "cancelAllTimers", CancelAllTimers)
		HAS_METHOD(TimerManager, "delayTimer", DelayTimer)
		HAS_METHOD(TimerManager, "delayTimerUntil", DelayTimerUntil)
		HAS_METHOD(TimerManager, "pause", Pause)
		HAS_METHOD(TimerManager, "unpause", Unpause)
		HAS_METHOD(TimerManager, "isPaused", IsPaused)
		HAS_METHOD(TimerManager, "pauseTimer", PauseTimer)
		HAS_METHOD(TimerManager, "unpauseTimer", UnpauseTimer)
		HAS_METHOD(TimerManager, "isTimerPaused", IsTimerPaused)
		HAS_METHOD(TimerManager, "getWhenTimerFiresNext", GetWhenTimerFiresNext)
		HAS_METHOD(TimerManager, "getMilliseconds", GetMilliseconds)
    );
	END
EMITTER_BASE_CLASS_IMPL(TimerManager)
METHOD_IMPL(TimerManager, StartTimer)
	METHOD_SIGNATURE("", undefined, 3, ([number int] id, [number uint] delay, boolean oneShot = true));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, id);
    REQUIRE_UINT32_ARG(2, delay);
    OPTIONAL_BOOL_ARG(3, oneShot, true);
    self->startTimer(id, delay, oneShot);  // userData
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, CancelTimer)
	METHOD_SIGNATURE("", undefined, 1, ([number int] id));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, id);
    self->cancelTimer(id);
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, CancelAllTimers)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
    self->cancelAllTimers();
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, DelayTimer)
	METHOD_SIGNATURE("", undefined, 2, ([number int] id, [number uint] delay));
    REQUIRE_ARG_COUNT(2);
    REQUIRE_INT32_ARG(1, id);
    REQUIRE_UINT32_ARG(2, delay);
    self->delayTimer(id, delay);
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, DelayTimerUntil)
	METHOD_SIGNATURE("", undefined, 2, ([number int] id, [number uint] msTime));
    REQUIRE_ARG_COUNT(2);
    REQUIRE_INT32_ARG(1, id);
    REQUIRE_UINT32_ARG(2, msTime);
    self->delayTimerUntil(id, msTime);
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, Pause)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
    self->pause();
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, Unpause)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
    self->unpause();
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, IsPaused)
	METHOD_SIGNATURE("", boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
    bool isPaused = self->isPaused();
    RETURN_BOOL(isPaused);
	END
METHOD_IMPL(TimerManager, PauseTimer)
	METHOD_SIGNATURE("", undefined, 1, ([number int] id));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, id);
    self->pauseTimer(id);
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, UnpauseTimer)
	METHOD_SIGNATURE("", undefined, 1, ([number int] id));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, id);
    self->unpauseTimer(id);
	NO_RETURN;
	END
METHOD_IMPL(TimerManager, IsTimerPaused)
	METHOD_SIGNATURE("", boolean, 1, ([number int] id));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, id);
    bool isPaused = self->isTimerPaused(id);
    RETURN_BOOL(isPaused);
	END
METHOD_IMPL(TimerManager, GetWhenTimerFiresNext)
	METHOD_SIGNATURE("", number, 1, ([number int] id));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, id);
    uint32 when = self->getWhenTimerFiresNext(id);
    RETURN_UNSIGNED(when);
	END
STATIC_METHOD_IMPL(TimerManager, GetMilliseconds)
	METHOD_SIGNATURE("", number, 0, ());
	RETURN_UNSIGNED( OS::getMilliseconds() );
	END

NATIVE_CONSTRUCTOR_IMPL(TimerManager)
    return TimerManager::getSingletonInstance();
	END


// ========================================================================================
//MARK: Custom Easing Functions
// ========================================================================================

#define SETUP_CUSTOM_EASING(n) CR \
  float customEasing##n(uint32 ut, float b, float c, uint32 ud) {	CR \
	return CallScriptEasingFunc(n, ut, b, c, ud);					CR \
  }

SETUP_CUSTOM_EASING(0)
SETUP_CUSTOM_EASING(1)
SETUP_CUSTOM_EASING(2)
SETUP_CUSTOM_EASING(3)
SETUP_CUSTOM_EASING(4)
SETUP_CUSTOM_EASING(5)
SETUP_CUSTOM_EASING(6)
SETUP_CUSTOM_EASING(7)
SETUP_CUSTOM_EASING(8)
SETUP_CUSTOM_EASING(9)

// EasingFunc gEasingFunctions[NUM_EASING_FUNCTIONS] = {
// 	EASING_FUNC_LIST
// };
// 
// int gNumCustomEasings = 0;

// ========================================================================================
//MARK: IAnimationHelper
// ========================================================================================

BINDING_INITIALIZER_IMPL(IAnimationHelper) 
    EXPORT_CLASS_SYMBOLS("IAnimationHelper", IAnimationHelper, , , );
	END

CLEANUP_IMPL(IAnimationHelper)


// ========================================================================================
//MARK: Animated
// ========================================================================================

#define HAS_ANIMATED_METHODS(klass) \
	HAS_GETTER(klass, BoundingBox)  \
	HAS_GETTER(klass, RotatedBounds)  \
	HAS_PROPERTY(klass, Location)  \
	HAS_PROPERTY(klass, Speed)  \
	HAS_PROPERTY(klass, Velocity)  \
	HAS_PROPERTY(klass, Width)  \
	HAS_PROPERTY(klass, Height)  \
	HAS_PROPERTY(klass, Rotation)  \
	HAS_PROPERTY(klass, CenterOffset)  \
	HAS_PROPERTY(klass, Spin)  \
	HAS_PROPERTY(klass, Mass)  \
	HAS_PROPERTY(klass, MoveFriction)  \
	HAS_PROPERTY(klass, SpinFriction)  \
	HAS_PROPERTY(klass, SizeFriction)  \
	HAS_METHOD(klass, "move", Move)  \
	HAS_METHOD(klass, "moveTo", MoveTo)  \
	HAS_METHOD(klass, "setVelocityInRadians", SetVelocityInRadians)  \
	HAS_METHOD(klass, "getMovementDirectionInRadians", GetMovementDirectionInRadians)  \
	HAS_METHOD(klass, "stopMoving", StopMoving)  \
	HAS_METHOD(klass, "accelerate", Accelerate)  \
	HAS_METHOD(klass, "accelerateTo", AccelerateTo)  \
	HAS_METHOD(klass, "setSize", SetSize)  \
	HAS_METHOD(klass, "grow", Grow)  \
	HAS_METHOD(klass, "stretch", Stretch)  \
	HAS_METHOD(klass, "startGrowing", StartGrowing)  \
	HAS_METHOD(klass, "stopGrowing", StopGrowing)  \
	HAS_METHOD(klass, "startStretching", StartStretching)  \
	HAS_METHOD(klass, "stopStretching", StopStretching)  \
	HAS_METHOD(klass, "resize", Resize)  \
	HAS_METHOD(klass, "resizeTo", ResizeTo)  \
	HAS_METHOD(klass, "rotate", Rotate)  \
	HAS_METHOD(klass, "rotateTo", RotateTo)  \
	HAS_METHOD(klass, "stopSpinning", StopSpinning)  \
	HAS_METHOD(klass, "changeCenter", ChangeCenter)  \
	HAS_METHOD(klass, "changeCenterTo", ChangeCenterTo)  \
	HAS_METHOD(klass, "wait", Wait)  \
	HAS_METHOD(klass, "setFriction", SetFriction)  \
	HAS_METHOD(klass, "applyForce", ApplyForce)  \
	HAS_METHOD(klass, "applyTorque", ApplyTorque)  \
	HAS_METHOD(klass, "stopAllForces", StopAllForces)  \
	HAS_METHOD(klass, "addAnimationHelper", AddAnimationHelper)  \
	HAS_METHOD(klass, "removeAnimationHelper", RemoveAnimationHelper)  \
	HAS_METHOD(klass, "clearAnimationHelpers", ClearAnimationHelpers)


#define ANIMATED_BASE_CLASS_IMPL(klass) CR \
PROPERTY_IMPL(klass, Location, POINT) CR \
PROPERTY_IMPL(klass, Speed, NUMBER) CR \
PROPERTY_IMPL(klass, Width, NUMBER) CR \
PROPERTY_IMPL(klass, Height, NUMBER) CR \
PROPERTY_IMPL(klass, Rotation, NUMBER) CR \
PROPERTY_IMPL(klass, CenterOffset, OFFSET) CR \
PROPERTY_IMPL(klass, Spin, NUMBER) CR \
PROPERTY_IMPL(klass, Mass, NUMBER) CR \
PROPERTY_IMPL(klass, MoveFriction, NUMBER) CR \
PROPERTY_IMPL(klass, SpinFriction, NUMBER) CR \
PROPERTY_IMPL(klass, SizeFriction, NUMBER) CR \
METHOD_IMPL(klass, GetBoundingBox) CR \
 	METHOD_SIGNATURE("", [object Rect], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    pdg::Rect r = self->getBoundingBox(); CR \
	RETURN( RECT2VAL(r) ); CR \
	END CR \
METHOD_IMPL(klass, GetRotatedBounds) CR \
 	METHOD_SIGNATURE("", [object Rect], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    pdg::RotatedRect r = self->getRotatedBounds(); CR \
	RETURN( RECT2VAL(r) ); CR \
	END CR \
METHOD_IMPL(klass, Move) CR \
 	METHOD_SIGNATURE("", undefined, 4, ([object Offset] delta, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	pdg::Offset delta; CR \
	int32 msDuration; CR \
	int easing; CR \
	if (VALUE_IS_OFFSET(ARGV[0])) { CR \
		delta = VAL2OFFSET(ARGV[0]); CR \
		OPTIONAL_INT32_ARG(2, msDuration_2, 0); CR \
		OPTIONAL_INT32_ARG(3, easing_3, EasingFuncRef::easeInOutQuad); CR \
		msDuration = msDuration_2; CR \
		easing = easing_3; CR \
    } else { CR \
		REQUIRE_NUMBER_ARG(1, deltaX); CR \
		REQUIRE_NUMBER_ARG(2, deltaY); CR \
		delta.x = deltaX; CR \
		delta.y = deltaY; CR \
		OPTIONAL_INT32_ARG(3, msDuration_3, 0); CR \
		OPTIONAL_INT32_ARG(4, easing_4, EasingFuncRef::easeInOutQuad); CR \
		msDuration = msDuration_3; CR \
		easing = easing_4; CR \
    } CR \
    if (msDuration || self->mDelayMs) { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->move(delta, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
    		self->move(delta, msDuration); CR \
    	} CR \
    } else { CR \
    	self->move(delta); CR \
    } CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveTo) CR \
 	METHOD_SIGNATURE("", undefined, 4, ([object Point] where, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	pdg::Point where; CR \
	int32 msDuration; CR \
	int easing; CR \
	if (VALUE_IS_POINT(ARGV[0])) {  CR \
		where = VAL2POINT(ARGV[0]); CR \
		OPTIONAL_INT32_ARG(2, msDuration2, 0); CR \
		OPTIONAL_INT32_ARG(3, easing_3, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_3; CR \
		msDuration = msDuration2; CR \
    } else { CR \
		REQUIRE_NUMBER_ARG(1, x); CR \
		REQUIRE_NUMBER_ARG(2, y); CR \
		where.x = x; CR \
		where.y = y; CR \
		OPTIONAL_INT32_ARG(3, msDuration3, 0); CR \
		OPTIONAL_INT32_ARG(4, easing_4, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_4; CR \
		msDuration = msDuration3; CR \
    } CR \
    if (msDuration || self->mDelayMs) { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->moveTo(where, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
    		self->moveTo(where, msDuration); CR \
    	} CR \
    } else { CR \
    	self->moveTo(where); CR \
    } CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetVelocityInRadians) CR \
 	METHOD_SIGNATURE("", [object Animated], 2, (number speed, number direction)); CR \
    REQUIRE_ARG_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, speed); CR \
	REQUIRE_NUMBER_ARG(2, direction); CR \
	self->setVelocityInRadians(speed, direction); CR \
	RETURN_THIS; CR \
	END CR \
METHOD_IMPL(klass, GetMovementDirectionInRadians) CR \
 	METHOD_SIGNATURE("", number, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	float dir = self->getMovementDirectionInRadians(); CR \
	RETURN_NUMBER(dir); CR \
	END CR \
METHOD_IMPL(klass, SetVelocity) CR \
 	METHOD_SIGNATURE("", [object Animated], 2, ({ ([object Vector] deltaPerSec) | (number deltaXPerSec, number deltaYPerSec) })); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	pdg::Vector deltaPerSec; CR \
	if (VALUE_IS_VECTOR(ARGV[0])) { CR \
		deltaPerSec = VAL2VECTOR(ARGV[0]);  CR \
		self->setVelocity(deltaPerSec); CR \
    } else { CR \
		REQUIRE_NUMBER_ARG(1, deltaXPerSec); CR \
		REQUIRE_NUMBER_ARG(2, deltaYPerSec); CR \
		self->setVelocity(Vector(deltaXPerSec, deltaYPerSec)); CR \
    } CR \
	RETURN_THIS; CR \
	END CR \
GETTER_IMPL(klass, Velocity, VECTOR) CR \
METHOD_IMPL(klass, StopMoving) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopMoving(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Accelerate) CR \
 	METHOD_SIGNATURE("", undefined, 3, (number deltaSpeed, [number int] msDuration = duration_Instantaneous, [number int] easing = linearTween)); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, deltaSpeed); CR \
	REQUIRE_INT32_ARG(2, msDuration); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::linearTween); CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->accelerate(deltaSpeed, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
		self->accelerate(deltaSpeed, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, AccelerateTo) CR \
 	METHOD_SIGNATURE("", undefined, 3, (number speed, [number int] msDuration = duration_Instantaneous, [number int] easing = linearTween)); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, speed); CR \
	REQUIRE_INT32_ARG(2, msDuration); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::linearTween); CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->accelerateTo(speed, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
		self->accelerateTo(speed, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetSize) CR \
 	METHOD_SIGNATURE("", [object Animated], 2, (number width, number height)); CR \
    REQUIRE_ARG_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, width); CR \
	REQUIRE_NUMBER_ARG(2, height); CR \
	self->setSize(width, height); CR \
	RETURN_THIS; CR \
	END CR \
METHOD_IMPL(klass, Grow) CR \
 	METHOD_SIGNATURE("", undefined, 3, (number factor, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, factor); CR \
	OPTIONAL_INT32_ARG(2, msDuration, 0); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad); CR \
	if (msDuration == 0) { CR \
		self->grow(factor); CR \
	} else { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->grow(factor, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
			self->grow(factor, msDuration); CR \
		} CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Stretch) CR \
 	METHOD_SIGNATURE("", undefined, 4, (number widthFactor, number heightFactor, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
    REQUIRE_NUMBER_ARG(1, widthFactor); CR \
	REQUIRE_NUMBER_ARG(2, heightFactor); CR \
	OPTIONAL_INT32_ARG(3, msDuration, 0); CR \
	OPTIONAL_INT32_ARG(4, easing, EasingFuncRef::easeInOutQuad); CR \
	if (msDuration == 0) { CR \
		self->stretch(widthFactor, heightFactor); CR \
	} else { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->stretch(widthFactor, heightFactor, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
			self->stretch(widthFactor, heightFactor, msDuration); CR \
		} CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StartGrowing) CR \
	METHOD_SIGNATURE("", undefined, 1, (number amountPerSecond)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, amountPerSecond); CR \
	self->startGrowing(amountPerSecond); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StopGrowing) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopGrowing(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StartStretching) CR \
 	METHOD_SIGNATURE("", undefined, 2, (number widthPerSecond, number heightPerSecond)); CR \
    REQUIRE_ARG_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, widthPerSecond); CR \
	REQUIRE_NUMBER_ARG(2, heightPerSecond); CR \
	self->startStretching(widthPerSecond, heightPerSecond); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StopStretching) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopStretching(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Resize) CR \
 	METHOD_SIGNATURE("", undefined, 4, (number deltaWidth, number deltaHeight, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(3); CR \
	REQUIRE_NUMBER_ARG(1, deltaWidth); CR \
	REQUIRE_NUMBER_ARG(2, deltaHeight); CR \
	REQUIRE_INT32_ARG(3, msDuration); CR \
	OPTIONAL_INT32_ARG(4, easing, EasingFuncRef::easeInOutQuad); CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->resize(deltaWidth, deltaHeight, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
		self->resize(deltaWidth, deltaHeight, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, ResizeTo) CR \
 	METHOD_SIGNATURE("", undefined, 4, (number width, number height, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(3); CR \
	REQUIRE_NUMBER_ARG(1, width); CR \
	REQUIRE_NUMBER_ARG(2, height); CR \
	REQUIRE_INT32_ARG(3, msDuration); CR \
	OPTIONAL_INT32_ARG(4, easing, EasingFuncRef::easeInOutQuad); CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->resizeTo(width, height, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
		self->resizeTo(width, height, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Rotate) CR \
 	METHOD_SIGNATURE("", undefined, 3, (number radians, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, radians); CR \
	OPTIONAL_INT32_ARG(2, msDuration, 0); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad); CR \
	if (msDuration == 0) { CR \
		self->rotate(radians); CR \
	} else { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->rotate(radians, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
			self->rotate(radians, msDuration); CR \
		} CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, RotateTo) CR \
 	METHOD_SIGNATURE("", undefined, 4, (number radiansRotation, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, radiansRotation); CR \
	OPTIONAL_INT32_ARG(2, msDuration, 0); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad); CR \
	if (msDuration == 0) { CR \
		self->rotateTo(radiansRotation); CR \
	} else { CR \
    	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    		self->rotateTo(radiansRotation, msDuration, gEasingFunctions[easing]); CR \
    	} else { CR \
			self->rotateTo(radiansRotation, msDuration); CR \
		} CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StopSpinning) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopSpinning(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, ChangeCenter) CR \
 	METHOD_SIGNATURE("", undefined, 3, ([object Offset] offset, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	pdg::Offset offset; CR \
	int32 msDuration; CR \
	int easing; CR \
	if (VALUE_IS_OFFSET(ARGV[0])) { CR \
		offset = VAL2OFFSET(ARGV[0]);  CR \
		REQUIRE_INT32_ARG(2, msDuration2); CR \
		OPTIONAL_INT32_ARG(3, easing_3, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_3; CR \
		msDuration = msDuration2; CR \
    } else { CR \
		REQUIRE_NUMBER_ARG(1, deltaXOffset); CR \
		REQUIRE_NUMBER_ARG(2, deltaYOffset); CR \
		offset.x = deltaXOffset; CR \
		offset.y = deltaYOffset; CR \
		REQUIRE_INT32_ARG(3, msDuration3); CR \
		OPTIONAL_INT32_ARG(4, easing_4, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_4; CR \
		msDuration = msDuration3; CR \
    } CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->changeCenter(offset, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
    	self->changeCenter(offset, msDuration); CR \
    } CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, ChangeCenterTo) CR \
 	METHOD_SIGNATURE("", undefined, 3, ([object Offset] offset, [number int] msDuration = duration_Instantaneous, [number int] easing = easeInOutQuad)); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	pdg::Offset offset; CR \
	int32 msDuration; CR \
	int easing; CR \
	if (VALUE_IS_OFFSET(ARGV[0])) { CR \
		offset = VAL2OFFSET(ARGV[0]);  CR \
		REQUIRE_INT32_ARG(2, msDuration2); CR \
		OPTIONAL_INT32_ARG(3, easing_3, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_3; CR \
		msDuration = msDuration2; CR \
    } else { CR \
		REQUIRE_NUMBER_ARG(1, deltaXOffset); CR \
		REQUIRE_NUMBER_ARG(2, deltaYOffset); CR \
		offset.x = deltaXOffset; CR \
		offset.y = deltaYOffset; CR \
		REQUIRE_INT32_ARG(3, msDuration3); CR \
		OPTIONAL_INT32_ARG(4, easing_4, EasingFuncRef::easeInOutQuad); CR \
		easing = easing_4; CR \
		msDuration = msDuration3; CR \
    } CR \
	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
		self->changeCenterTo(offset, msDuration, gEasingFunctions[easing]); CR \
	} else { CR \
    	self->changeCenterTo(offset, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Wait) CR \
	METHOD_SIGNATURE("", [object Animated], 1, ([number int] msDuration)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, msDuration); CR \
	self->wait(msDuration); CR \
	RETURN_THIS; CR \
	END CR \
METHOD_IMPL(klass, SetFriction) CR \
 	METHOD_SIGNATURE("", [object Animated], 1, (number frictionCoefficient)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, frictionCoefficient); CR \
	self->setFriction(frictionCoefficient); CR \
	RETURN_THIS; CR \
	END CR \
METHOD_IMPL(klass, ApplyForce) CR \
	METHOD_SIGNATURE("", undefined, 2, ([object Vector] force, [number int] msDuration = duration_Instantaneous)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_VECTOR_ARG(1, force); CR \
	OPTIONAL_INT32_ARG(2, msDuration, Animated::duration_Instantaneous); CR \
	self->applyForce(force, msDuration); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, ApplyTorque) CR \
	METHOD_SIGNATURE("", undefined, 2, (number forceSpin, [number int] msDuration = duration_Instantaneous)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, forceSpin); CR \
	OPTIONAL_INT32_ARG(2, msDuration, Animated::duration_Instantaneous); CR \
	self->applyTorque(forceSpin, msDuration); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StopAllForces) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopAllForces(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, AddAnimationHelper) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object IAnimationHelper] helper)); CR \
	OBJECT_SAVE(self->mAnimatedScriptObj, THIS); CR \
    DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], IAnimationHelper); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_ARG(1, helper, IAnimationHelper); CR \
	self->addAnimationHelper(helper); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, RemoveAnimationHelper) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object IAnimationHelper] helper)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NATIVE_OBJECT_ARG(1, helper, IAnimationHelper); CR \
	self->removeAnimationHelper(helper); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, ClearAnimationHelpers) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->clearAnimationHelpers(); CR \
	NO_RETURN; CR \
	END CR \


WRAPPER_INITIALIZER_IMPL_CUSTOM(Animated,
  nativeObj->mAnimatedScriptObj = obj)
    EXPORT_CLASS_SYMBOLS("Animated", Animated, , , 
		HAS_ANIMATED_METHODS(Animated)
		HAS_METHOD(Animated, "animate", Animate)
    );
	END

ANIMATED_BASE_CLASS_IMPL(Animated)
METHOD_IMPL(Animated, Animate)
	METHOD_SIGNATURE("", boolean, 1, ([number int] msElapsed));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, msElapsed);
	bool result = self->animate(msElapsed);
	RETURN_BOOL(result);
	END

CLEANUP_IMPL(Animated)

NATIVE_CONSTRUCTOR_IMPL(Animated)
    return new Animated();
	END


%#ifdef PDG_USE_CHIPMUNK_PHYSICS
// ========================================================================================
//MARK: cpArbiter
// ========================================================================================

WRAPPER_INITIALIZER_IMPL_CUSTOM(cpArbiter, ) 
    EXPORT_CLASS_SYMBOLS("CpArbiter", cpArbiter, , ,
    	// method section
		HAS_METHOD(cpArbiter, "isFirstContact", IsFirstContact)
		HAS_GETTER(cpArbiter, Count)
		HAS_GETTER(cpArbiter, Normal)
		HAS_GETTER(cpArbiter, Point)
		HAS_GETTER(cpArbiter, Depth)
    );
	END

METHOD_IMPL(cpArbiter, IsFirstContact)
	METHOD_SIGNATURE("true if this is the first moment of contact between the objects, false if part of an ongoing collison", 
		boolean, 0, ());
    REQUIRE_ARG_COUNT(0);
	cpBool isFirst = cpArbiterIsFirstContact(self);
	RETURN_BOOL(isFirst);
	END
CP_GETTER_IMPL(cpArbiter, Count, INTEGER)
CUSTOM_GETTER_IMPL(cpArbiter, Normal, VECTOR, 1, ,
	REQUIRE_INT32_ARG(1, i); CR ,
	cpVect normal = cpArbiterGetNormal(self, i); CR
	pdg::Vector theNormal(normal.x, normal.y), ([number int] i) )
CUSTOM_GETTER_IMPL(cpArbiter, Point, POINT, 1, ,
	REQUIRE_INT32_ARG(1, i); CR ,
	cpVect pt = cpArbiterGetPoint(self, i); CR
	pdg::Point thePoint(pt.x, pt.y), ([number int] i) )
CUSTOM_GETTER_IMPL(cpArbiter, Depth, NUMBER, 1, ,
	REQUIRE_INT32_ARG(1, i); CR ,
	cpFloat theDepth = cpArbiterGetDepth(self, i), ([number int] i) )

// NATIVE_CONSTRUCTOR_IMPL(cpArbiter)
// 	return 0;
//	END


// ========================================================================================
//MARK: cpConstraint
// ========================================================================================

#define IF_CONSTRAINT_TYPE(type) \
	if (strcmp((const char*)cpConstraintGetUserData(self), #type) == 0)

#define REQUIRE_CONSTRAINT_TYPE(prop, type) \
	if (strcmp((const char*)cpConstraintGetUserData(self), #type) != 0) { CR \
		std::ostringstream msg; CR \
        msg << "cpConstraint.set" #prop "() not valid for constraint type "  CR \
                << (const char*)cpConstraintGetUserData(self); CR \
		THROW_TYPE_ERR(msg.str().c_str()) ; CR \
	}

#define UNDEFINED_UNLESS_CONSTRAINT_TYPE(type) \
	if (strcmp((const char*)cpConstraintGetUserData(self), #type) != 0) { CR \
		RETURN_UNDEFINED; CR \
	}

#define CONSTRAINT_CUSTOM_PROPERTY_IMPL(prop, type, contype, cpgetcode, cpsetcode) \
CUSTOM_GETTER_IMPL(cpConstraint, prop, type, 0, , , \
	UNDEFINED_UNLESS_CONSTRAINT_TYPE(contype); CR \
	cpgetcode, () ) \
CUSTOM_SETTER_IMPL(cpConstraint, prop, type, 1, , , \
	REQUIRE_CONSTRAINT_TYPE(prop, contype); CR \
	cpsetcode)

#define CONSTRAINT_PROPERTY_IMPL(prop, contype, cpgetcall, cpsetcall) \
	CONSTRAINT_CUSTOM_PROPERTY_IMPL(prop, NUMBER, contype, cpFloat the##prop = cpgetcall(self), cpsetcall(self, the##prop))

WRAPPER_INITIALIZER_IMPL_CUSTOM(cpConstraint, ) 
    EXPORT_CLASS_SYMBOLS("CpConstraint", cpConstraint, , ,
    	// method section
		HAS_GETTER(cpConstraint, Type)	// return string of "pin, slide, etc..."
		HAS_METHOD(cpConstraint, "activateBodies", ActivateBodies)
		HAS_GETTER(cpConstraint, Impulse)
		HAS_PROPERTY(cpConstraint, MaxForce)
		HAS_PROPERTY(cpConstraint, ErrorBias)
		HAS_PROPERTY(cpConstraint, MaxBias)
		HAS_GETTER(cpConstraint, Sprite)
		HAS_GETTER(cpConstraint, OtherSprite)
		HAS_PROPERTY(cpConstraint, Anchor)  				// Anchr1: pin, slide, pivot, spring, 
		HAS_PROPERTY(cpConstraint, OtherAnchor) 			// Anchr2: pin, slide, pivot, groove, spring
		HAS_PROPERTY(cpConstraint, PinDist)					// Dist: pin
		HAS_PROPERTY(cpConstraint, SpringStiffness) 		// Stiffness: spring, rotary spring
		HAS_PROPERTY(cpConstraint, SpringDamping)			// Damping: spring, rotary spring
		HAS_PROPERTY(cpConstraint, SlideMinDist)			// Min: slide
		HAS_PROPERTY(cpConstraint, SlideMaxDist)			// Max: slide
		HAS_PROPERTY(cpConstraint, GrooveStart)				// GrooveA: groove
		HAS_PROPERTY(cpConstraint, GrooveEnd)				// GrooveB: groove
		HAS_PROPERTY(cpConstraint, SpringRestLength)		// RestLength: spring
		HAS_PROPERTY(cpConstraint, RotarySpringRestAngle)  	// Rest Angle: rotary spring
		HAS_PROPERTY(cpConstraint, MinAngle)				// Min: rotary limit
		HAS_PROPERTY(cpConstraint, MaxAngle)				// Max: rotary limit
		HAS_PROPERTY(cpConstraint, RatchetAngle)			// Angle: ratchet
		HAS_PROPERTY(cpConstraint, RatchetPhase)			// Phase: ratchet
		HAS_PROPERTY(cpConstraint, RatchetInterval)			// Ratchet: ratchet
		HAS_PROPERTY(cpConstraint, GearRatio)				// Ratio: gear
		HAS_PROPERTY(cpConstraint, GearInitialAngle)		// Phase: gear
		HAS_PROPERTY(cpConstraint, MotorSpinRate)			// Rate: motor
    );
	END

CUSTOM_GETTER_IMPL(cpConstraint, Type, STRING, 0, , ,
	const char* theType = (const char*)cpConstraintGetUserData(self), () );
CP_GETTER_IMPL(cpConstraint, Impulse, NUMBER)
CP_PROPERTY_IMPL(cpConstraint, MaxForce, NUMBER)
CP_PROPERTY_IMPL(cpConstraint, ErrorBias, NUMBER)
CP_PROPERTY_IMPL(cpConstraint, MaxBias, NUMBER)
METHOD_IMPL(cpConstraint, ActivateBodies);
	METHOD_SIGNATURE("", undefined, 0, ());
	REQUIRE_ARG_COUNT(0);
	cpConstraintActivateBodies(self);
	NO_RETURN;
	END
METHOD_IMPL(cpConstraint, GetSprite);
	METHOD_SIGNATURE("", [object Sprite], 0, ());
	REQUIRE_ARG_COUNT(0);
	cpBody* body = cpConstraintGetA(self);
	Sprite* sprite = (Sprite*) cpBodyGetUserData(body);
	RETURN_NATIVE_OBJECT(sprite, Sprite);
	END
METHOD_IMPL(cpConstraint, GetOtherSprite);
	METHOD_SIGNATURE("", [object Sprite], 0, ());
	REQUIRE_ARG_COUNT(0);
	cpBody* body = cpConstraintGetB(self);
	Sprite* otherSprite = (Sprite*) cpBodyGetUserData(body);
	RETURN_NATIVE_OBJECT(otherSprite, Sprite);
	END
CUSTOM_GETTER_IMPL(cpConstraint, Anchor, OFFSET, 0, , ,
	cpVect anchor; CR
	IF_CONSTRAINT_TYPE(PinJoint) { CR
		anchor = cpPinJointGetAnchr1(self);
	} else IF_CONSTRAINT_TYPE(SlideJoint) { CR
		anchor = cpSlideJointGetAnchr1(self);
	} else IF_CONSTRAINT_TYPE(PivotJoint) { CR
		anchor = cpPivotJointGetAnchr1(self);
	} else IF_CONSTRAINT_TYPE(SpringJoint) { CR
		anchor = cpDampedSpringGetAnchr1(self);
	} else { CR
		RETURN_UNDEFINED; CR
	} CR
	pdg::Offset theAnchor(anchor.x, anchor.y), () )
CUSTOM_SETTER_IMPL(cpConstraint, Anchor, OFFSET, 1, , ,
	cpVect anchor = cpv(theAnchor.x, theAnchor.y); CR
	IF_CONSTRAINT_TYPE(PinJoint) { CR
		cpPinJointSetAnchr1(self, anchor);
	} else IF_CONSTRAINT_TYPE(SlideJoint) { CR
		cpSlideJointSetAnchr1(self, anchor);
	} else IF_CONSTRAINT_TYPE(PivotJoint) { CR
		cpPivotJointSetAnchr1(self, anchor);
	} else IF_CONSTRAINT_TYPE(SpringJoint) { CR
		cpDampedSpringSetAnchr1(self, anchor);
	} else { CR
		std::ostringstream msg; CR
        msg << "cpConstraint.setAnchor() not valid for constraint type "  CR
                << (const char*)cpConstraintGetUserData(self); CR
		THROW_TYPE_ERR(msg.str().c_str()) ; CR
	})
CUSTOM_GETTER_IMPL(cpConstraint, OtherAnchor, OFFSET, 0, , ,
	cpVect anchor; CR
	IF_CONSTRAINT_TYPE(PinJoint) { CR
		anchor = cpPinJointGetAnchr2(self);
	} else IF_CONSTRAINT_TYPE(SlideJoint) { CR
		anchor = cpSlideJointGetAnchr2(self);
	} else IF_CONSTRAINT_TYPE(PivotJoint) { CR
		anchor = cpPivotJointGetAnchr2(self);
	} else IF_CONSTRAINT_TYPE(GrooveJoint) { CR
		anchor = cpGrooveJointGetAnchr2(self);
	} else IF_CONSTRAINT_TYPE(SpringJoint) { CR
		anchor = cpDampedSpringGetAnchr2(self);
	} else { CR
		RETURN_UNDEFINED; CR
	} CR
	pdg::Offset theOtherAnchor(anchor.x, anchor.y), () )
CUSTOM_SETTER_IMPL(cpConstraint, OtherAnchor, OFFSET, 1, , ,
	cpVect anchor = cpv(theOtherAnchor.x, theOtherAnchor.y); CR
	IF_CONSTRAINT_TYPE(PinJoint) { CR
		cpPinJointSetAnchr2(self, anchor);
	} else IF_CONSTRAINT_TYPE(SlideJoint) { CR
		cpSlideJointSetAnchr2(self, anchor);
	} else IF_CONSTRAINT_TYPE(PivotJoint) { CR
		cpPivotJointSetAnchr2(self, anchor);
	} else IF_CONSTRAINT_TYPE(GrooveJoint) { CR
		cpGrooveJointSetAnchr2(self, anchor);
	} else IF_CONSTRAINT_TYPE(SpringJoint) { CR
		cpDampedSpringSetAnchr2(self, anchor);
	} else { CR
		std::ostringstream msg; CR
        msg << "cpConstraint.setOtherAnchor() not valid for constraint type "  CR
                << (const char*)cpConstraintGetUserData(self); CR
		THROW_TYPE_ERR(msg.str().c_str()) ; CR
	})
CONSTRAINT_PROPERTY_IMPL(PinDist, PinJoint, cpPinJointGetDist, cpPinJointSetDist)
CONSTRAINT_PROPERTY_IMPL(SlideMinDist, SlideJoint, cpSlideJointGetMin, cpSlideJointSetMin)
CONSTRAINT_PROPERTY_IMPL(SlideMaxDist, SlideJoint, cpSlideJointGetMax, cpSlideJointSetMax)
CONSTRAINT_CUSTOM_PROPERTY_IMPL(GrooveStart, OFFSET, GrooveJoint, 
	cpVect v = cpGrooveJointGetGrooveA(self); CR
	pdg::Offset theGrooveStart(v.x, v.y), 
	cpGrooveJointSetGrooveA(self, cpv(theGrooveStart.x, theGrooveStart.y)))
CONSTRAINT_CUSTOM_PROPERTY_IMPL(GrooveEnd, OFFSET, GrooveJoint,
	cpVect v = cpGrooveJointGetGrooveB(self); CR
	pdg::Offset theGrooveEnd(v.x, v.y), 
	cpGrooveJointSetGrooveB(self, cpv(theGrooveEnd.x, theGrooveEnd.y)))
CONSTRAINT_PROPERTY_IMPL(SpringRestLength, SpringJoint, cpDampedSpringGetRestLength, cpDampedSpringSetRestLength)
CONSTRAINT_PROPERTY_IMPL(RotarySpringRestAngle, RotarySpring, cpDampedRotarySpringGetRestAngle, cpDampedRotarySpringSetRestAngle)
CONSTRAINT_PROPERTY_IMPL(MinAngle, RotaryLimit, cpRotaryLimitJointGetMin, cpRotaryLimitJointSetMin)
CONSTRAINT_PROPERTY_IMPL(MaxAngle, RotaryLimit, cpRotaryLimitJointGetMax, cpRotaryLimitJointSetMax)
CONSTRAINT_PROPERTY_IMPL(RatchetAngle, Ratchet, cpRatchetJointGetAngle, cpRatchetJointSetAngle)
CONSTRAINT_PROPERTY_IMPL(RatchetPhase, Ratchet, cpRatchetJointGetPhase, cpRatchetJointSetPhase)
CONSTRAINT_PROPERTY_IMPL(RatchetInterval, Ratchet, cpRatchetJointGetRatchet, cpRatchetJointSetRatchet)
CONSTRAINT_PROPERTY_IMPL(GearRatio, Gear, cpGearJointGetRatio, cpGearJointSetRatio)
CONSTRAINT_PROPERTY_IMPL(GearInitialAngle, Gear, cpGearJointGetPhase, cpGearJointSetPhase)
CONSTRAINT_PROPERTY_IMPL(MotorSpinRate, Motor, cpSimpleMotorGetRate, cpSimpleMotorSetRate)
CUSTOM_GETTER_IMPL(cpConstraint, SpringStiffness, NUMBER, 0, , ,
	cpFloat theSpringStiffness; CR
	IF_CONSTRAINT_TYPE(SpringJoint) { CR
		theSpringStiffness = cpDampedSpringGetStiffness(self);
	} else IF_CONSTRAINT_TYPE(RotarySpring) { CR
		theSpringStiffness = cpDampedRotarySpringGetStiffness(self);
	} else { CR
		RETURN_UNDEFINED; CR
	}, () )
CUSTOM_SETTER_IMPL(cpConstraint, SpringStiffness, NUMBER, 1, , ,
	IF_CONSTRAINT_TYPE(SpringJoint) { CR
		cpDampedSpringSetStiffness(self, theSpringStiffness);
	} else IF_CONSTRAINT_TYPE(RotarySpring) { CR
		cpDampedRotarySpringSetStiffness(self, theSpringStiffness);
	} else { CR
		std::ostringstream msg; CR
        msg << "cpConstraint.setSpringStiffness() not valid for constraint type "  CR
                << (const char*)cpConstraintGetUserData(self); CR
		THROW_TYPE_ERR(msg.str().c_str()) ; CR
	})
CUSTOM_GETTER_IMPL(cpConstraint, SpringDamping, NUMBER, 0, , ,
	cpFloat theSpringDamping; CR
	IF_CONSTRAINT_TYPE(SpringJoint) { CR
		theSpringDamping = cpDampedSpringGetDamping(self);
	} else IF_CONSTRAINT_TYPE(RotarySpring) { CR
		theSpringDamping = cpDampedRotarySpringGetDamping(self);
	} else { CR
		RETURN_UNDEFINED; CR
	}, () )
CUSTOM_SETTER_IMPL(cpConstraint, SpringDamping, NUMBER, 1, , ,
	IF_CONSTRAINT_TYPE(SpringJoint) { CR
		cpDampedSpringSetDamping(self, theSpringDamping);
	} else IF_CONSTRAINT_TYPE(RotarySpring) { CR
		cpDampedRotarySpringSetDamping(self, theSpringDamping);
	} else { CR
		std::ostringstream msg; CR
        msg << "cpConstraint.setSpringDamping() not valid for constraint type "  CR
                << (const char*)cpConstraintGetUserData(self); CR
		THROW_TYPE_ERR(msg.str().c_str()) ; CR
	})

// NATIVE_CONSTRUCTOR_IMPL(cpConstraint)
// 	return 0;
//	END

// ========================================================================================
//MARK: cpSpace
// ========================================================================================

WRAPPER_INITIALIZER_IMPL_CUSTOM(cpSpace, ) 
    EXPORT_CLASS_SYMBOLS("CpSpace", cpSpace, , ,
    	// method section
		HAS_METHOD(cpSpace, "useSpatialHash", UseSpatialHash)
		HAS_METHOD(cpSpace, "reindexStatic", ReindexStatic)
		HAS_METHOD(cpSpace, "step", Step)
		HAS_PROPERTY(cpSpace, IdleSpeedThreshold)
		HAS_PROPERTY(cpSpace, SleepTimeThreshold)
		HAS_PROPERTY(cpSpace, CollisionSlop)
		HAS_PROPERTY(cpSpace, CollisionBias)
		HAS_PROPERTY(cpSpace, CollisionPersistence)
		HAS_PROPERTY(cpSpace, EnableContactGraph)
    );
	END
METHOD_IMPL(cpSpace, UseSpatialHash)
	METHOD_SIGNATURE("", undefined, 2, (number dim, [number int] count));
    REQUIRE_ARG_COUNT(2);
    REQUIRE_NUMBER_ARG(1, dim);
    REQUIRE_INT32_ARG(2, count);
	cpSpaceUseSpatialHash(self, dim, count);
	NO_RETURN;
	END
METHOD_IMPL(cpSpace, ReindexStatic)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	cpSpaceReindexStatic(self);
	NO_RETURN;
	END
METHOD_IMPL(cpSpace, Step)
	METHOD_SIGNATURE("", undefined, 1, (number dt));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NUMBER_ARG(1, dt);
	cpSpaceStep(self, dt);
	NO_RETURN;
	END
CP_PROPERTY_IMPL(cpSpace, IdleSpeedThreshold, NUMBER)
CP_PROPERTY_IMPL(cpSpace, SleepTimeThreshold, NUMBER)
CP_PROPERTY_IMPL(cpSpace, CollisionSlop, NUMBER)
CP_PROPERTY_IMPL(cpSpace, CollisionBias, NUMBER)
CP_PROPERTY_IMPL(cpSpace, CollisionPersistence, NUMBER)
CP_PROPERTY_IMPL(cpSpace, EnableContactGraph, BOOL)

// NATIVE_CONSTRUCTOR_IMPL(cpSpace)
// 	return 0;
//	END

%#endif // PDG_USE_CHIPMUNK_PHYSICS


// ========================================================================================
//MARK: ISpriteCollideHelper
// ========================================================================================

BINDING_INITIALIZER_IMPL(ISpriteCollideHelper) 
    EXPORT_CLASS_SYMBOLS("ISpriteCollideHelper", ISpriteCollideHelper, , , );
	END

CLEANUP_IMPL(ISpriteCollideHelper)


%#ifndef PDG_NO_GUI
// ========================================================================================
//MARK: ISpriteDrawHelper
// ========================================================================================

BINDING_INITIALIZER_IMPL(ISpriteDrawHelper) 
    EXPORT_CLASS_SYMBOLS("ISpriteDrawHelper", ISpriteDrawHelper, , , );
	END

CLEANUP_IMPL(ISpriteDrawHelper)

%#endif // !PDG_NO_GUI


// ========================================================================================
//MARK: Sprite
// ========================================================================================

static const char* sPinJointStr = "PinJoint";
static const char* sSlideJointStr = "SlideJoint";
static const char* sPivotJointStr = "PivotJoint";
static const char* sGrooveJointStr = "GrooveJoint";
static const char* sSpringJointStr = "SpringJoint";
static const char* sRotarySpringStr = "RotarySpring";
static const char* sRotaryLimitStr = "RotaryLimit";
static const char* sRatchetStr = "Ratchet";
static const char* sGearStr = "Gear";
static const char* sMotorStr = "Motor";

WRAPPER_INITIALIZER_IMPL_CUSTOM(Sprite, 
  nativeObj->mEventEmitterScriptObj = obj; 
  nativeObj->mAnimatedScriptObj = obj;
  nativeObj->mSpriteScriptObj = obj;
  nativeObj->mISerializableScriptObj = obj;
  nativeObj->addRef() )
    EXPORT_CLASS_SYMBOLS("Sprite", Sprite, , ,
    	// method section
		HAS_EMITTER_METHODS(Sprite)
		HAS_ANIMATED_METHODS(Sprite)
		HAS_SERIALIZABLE_METHODS(Sprite)
		HAS_METHOD(Sprite, "getFrameRotatedBounds", GetFrameRotatedBounds)
		HAS_METHOD(Sprite, "setFrame", SetFrame)
		HAS_METHOD(Sprite, "getCurrentFrame", GetCurrentFrame)
		HAS_METHOD(Sprite, "getFrameCount", GetFrameCount)
		HAS_METHOD(Sprite, "startFrameAnimation", StartFrameAnimation)
		HAS_METHOD(Sprite, "stopFrameAnimation", StopFrameAnimation)
		HAS_METHOD(Sprite, "setWantsAnimLoopEvents", SetWantsAnimLoopEvents)
		HAS_METHOD(Sprite, "setWantsAnimEndEvents", SetWantsAnimEndEvents)
		HAS_METHOD(Sprite, "setWantsCollideWallEvents", SetWantsCollideWallEvents)
		HAS_METHOD(Sprite, "addFramesImage", AddFramesImage)
	%#ifdef PDG_SCML_SUPPORT CR
		HAS_METHOD(Sprite, "hasAnimation", HasAnimation)
		HAS_METHOD(Sprite, "startAnimation", StartAnimation)
		HAS_METHOD(Sprite, "setEntityScale", SetEntityScale)
	%#endif CR
		HAS_METHOD(Sprite, "changeFramesImage", ChangeFramesImage)
		HAS_METHOD(Sprite, "offsetFrameCenters", OffsetFrameCenters)
		HAS_METHOD(Sprite, "getFrameCenterOffset", GetFrameCenterOffset)
		HAS_METHOD(Sprite, "setOpacity", SetOpacity)
		HAS_METHOD(Sprite, "getOpacity", GetOpacity)
		HAS_METHOD(Sprite, "fadeTo", FadeTo)
		HAS_METHOD(Sprite, "fadeIn", FadeIn)
		HAS_METHOD(Sprite, "fadeOut", FadeOut)
		HAS_METHOD(Sprite, "isBehind", IsBehind)
		HAS_METHOD(Sprite, "getZOrder", GetZOrder)
		HAS_METHOD(Sprite, "moveBehind", MoveBehind)
		HAS_METHOD(Sprite, "moveInFrontOf", MoveInFrontOf)
		HAS_METHOD(Sprite, "moveToFront", MoveToFront)
		HAS_METHOD(Sprite, "moveToBack", MoveToBack)
		HAS_METHOD(Sprite, "enableCollisions", EnableCollisions)
		HAS_METHOD(Sprite, "disableCollisions", DisableCollisions)
		HAS_METHOD(Sprite, "setCollisionRadius", SetCollisionRadius)
		HAS_METHOD(Sprite, "getCollisionRadius", GetCollisionRadius)
		HAS_METHOD(Sprite, "useCollisionMask", UseCollisionMask)
		HAS_METHOD(Sprite, "setCollisionHelper", SetCollisionHelper)
		HAS_METHOD(Sprite, "setElasticity", SetElasticity)
		HAS_METHOD(Sprite, "getElasticity", GetElasticity)
		HAS_METHOD(Sprite, "getLayer", GetLayer)
	%#ifndef PDG_NO_GUI  CR
		HAS_METHOD(Sprite, "setDrawHelper", SetDrawHelper)
		HAS_METHOD(Sprite, "setPostDrawHelper", SetPostDrawHelper)
		HAS_METHOD(Sprite, "setWantsMouseOverEvents", SetWantsMouseOverEvents)
		HAS_METHOD(Sprite, "setWantsClickEvents", SetWantsClickEvents)
		HAS_METHOD(Sprite, "setMouseDetectMode", SetMouseDetectMode)
		HAS_METHOD(Sprite, "setWantsOffscreenEvents", SetWantsOffscreenEvents)
	%#endif  CR // !PDG_NO_GUI
	  %#ifdef PDG_USE_CHIPMUNK_PHYSICS CR
		HAS_METHOD(Sprite, "makeStatic", MakeStatic)
		HAS_METHOD(Sprite, "getFriction", GetFriction)
		HAS_METHOD(Sprite, "setCollideGroup", SetCollideGroup)
		HAS_METHOD(Sprite, "getCollideGroup", GetCollideGroup)
		HAS_METHOD(Sprite, "pinJoint", PinJoint)
		HAS_METHOD(Sprite, "slideJoint", SlideJoint)
		HAS_METHOD(Sprite, "pivotJoint", PivotJoint)
		HAS_METHOD(Sprite, "grooveJoint", GrooveJoint)
		HAS_METHOD(Sprite, "springJoint", SpringJoint)
		HAS_METHOD(Sprite, "rotarySpring", RotarySpring)
		HAS_METHOD(Sprite, "rotaryLimit", RotaryLimit)
		HAS_METHOD(Sprite, "ratchet", Ratchet)
		HAS_METHOD(Sprite, "gear", Gear)
		HAS_METHOD(Sprite, "motor", Motor)
		HAS_METHOD(Sprite, "removeJoint", RemoveJoint)
		HAS_METHOD(Sprite, "disconnect", Disconnect)
		HAS_METHOD(Sprite, "makeJointBreakable", MakeJointBreakable)
		HAS_METHOD(Sprite, "makeJointUnbreakable", MakeJointUnbreakable)
	  %#endif  CR // PDG_USE_CHIPMUNK_PHYSICS
    );
	END
EMITTER_BASE_CLASS_IMPL(Sprite)
ANIMATED_BASE_CLASS_IMPL(Sprite)
SERIALIZABLE_BASE_CLASS_IMPL(Sprite)
METHOD_IMPL(Sprite, GetFrameRotatedBounds)
	METHOD_SIGNATURE("", [object Rect], 1, ([number int] frame));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, frame);
    pdg::RotatedRect r = self->getFrameRotatedBounds(frame);
	RETURN( RECT2VAL(r) );
	END
METHOD_IMPL(Sprite, SetFrame)
	METHOD_SIGNATURE("", [object Sprite], 1, ([number int] frame));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_INT32_ARG(1, frame);
	self->setFrame(frame);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetCurrentFrame)
	METHOD_SIGNATURE("which frame of animation the sprite is currently showing", 
		number, 0, ());
    REQUIRE_ARG_COUNT(0);
	int frame = self->getCurrentFrame();
	RETURN_INTEGER(frame);
	END
METHOD_IMPL(Sprite, GetFrameCount)
	METHOD_SIGNATURE("total number of frames of animation for this sprite", 
		number, 0, ());
    REQUIRE_ARG_COUNT(0);
	int count = self->getFrameCount();
	RETURN_INTEGER(count);
	END
METHOD_IMPL(Sprite, StartFrameAnimation)
	METHOD_SIGNATURE("", undefined, 4, (number fps, [number int] startingFrame = start_FromFirstFrame, [number int] numFrames = all_Frames, [number int] animateFlags = animate_Looping));
    REQUIRE_ARG_MIN_COUNT(1);
    REQUIRE_NUMBER_ARG(1, fps);
    OPTIONAL_INT32_ARG(2, startingFrame, Sprite::start_FromFirstFrame);
    OPTIONAL_INT32_ARG(3, numFrames, Sprite::all_Frames);
    OPTIONAL_INT32_ARG(4, animateFlags, Sprite::animate_Looping);
	self->startFrameAnimation(fps, startingFrame, numFrames, animateFlags);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, StopFrameAnimation)
	METHOD_SIGNATURE("", undefined, 0, ());
    REQUIRE_ARG_COUNT(0);
	self->stopFrameAnimation();
	NO_RETURN;
	END
METHOD_IMPL(Sprite, SetWantsAnimLoopEvents)
	METHOD_SIGNATURE("", boolean, 0, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	bool wanted = self->setWantsAnimLoopEvents(wantsThem);
	RETURN_BOOL(wanted);
	END
METHOD_IMPL(Sprite, SetWantsAnimEndEvents)
	METHOD_SIGNATURE("", boolean, 0, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	bool wanted = self->setWantsAnimEndEvents(wantsThem);
	RETURN_BOOL(wanted);
	END
METHOD_IMPL(Sprite, SetWantsCollideWallEvents)
	METHOD_SIGNATURE("", [object Sprite], 0, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	self->setWantsCollideWallEvents(wantsThem);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, AddFramesImage)
	METHOD_SIGNATURE("", undefined, 3, ([object Image] image, [number int] startingFrame = start_FromFirstFrame, [number int] numFrames = all_Frames));
    REQUIRE_ARG_MIN_COUNT(1);
    REQUIRE_NATIVE_OBJECT_ARG(1, image, Image);
    OPTIONAL_INT32_ARG(2, startingFrame, Sprite::start_FromFirstFrame);
    OPTIONAL_INT32_ARG(3, numFrames, Sprite::all_Frames);
	self->addFramesImage(image, startingFrame, numFrames);
	NO_RETURN;
	END
%#ifdef PDG_SCML_SUPPORT
METHOD_IMPL(Sprite, HasAnimation)
	METHOD_SIGNATURE("", boolean, 1, ({ [number int] animationId | string animationName }) );
	REQUIRE_ARG_COUNT(1);
	bool hasIt = false;
	if (VALUE_IS_STRING(ARGV[0])) {
		REQUIRE_STRING_ARG(1, animationName);
		hasIt = self->hasAnimation(animationName);
	} else {
		REQUIRE_UINT32_ARG(1, animationId);
		hasIt = self->hasAnimation(animationId);
	}
	RETURN_BOOL(hasIt);
	END
METHOD_IMPL(Sprite, StartAnimation)
	METHOD_SIGNATURE("", undefined, 1, ({ [number int] animationId | string animationName }) );
	REQUIRE_ARG_COUNT(1);
	if (VALUE_IS_STRING(ARGV[0])) {
		REQUIRE_STRING_ARG(1, animationName);
		self->startAnimation(animationName);
	} else {
		REQUIRE_UINT32_ARG(1, animationId);
		self->startAnimation(animationId);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sprite, SetEntityScale)
	METHOD_SIGNATURE("", [object Sprite], 1, (number xScale, number yScale) );
	REQUIRE_ARG_COUNT(2);
	REQUIRE_NUMBER_ARG(1, xScale);
	REQUIRE_NUMBER_ARG(2, yScale);
	self->setEntityScale(xScale, yScale);
	RETURN_THIS;
	END
%#endif
%#ifndef PDG_NO_GUI
METHOD_IMPL(Sprite, SetWantsOffscreenEvents)
	METHOD_SIGNATURE("", [object Sprite], 0, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	self->setWantsOffscreenEvents(wantsThem);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, SetDrawHelper)
	METHOD_SIGNATURE("", undefined, 1, ([object ISpriteDrawHelper] helper));
	OBJECT_SAVE(self->mSpriteScriptObj, THIS);
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_OR_NULL_ARG(1, helper, ISpriteDrawHelper);
	DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], ISpriteDrawHelper)	
    self->setDrawHelper(helper);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, SetPostDrawHelper)
	METHOD_SIGNATURE("", undefined, 1, ([object ISpriteDrawHelper] helper));
	OBJECT_SAVE(self->mSpriteScriptObj, THIS);
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_OR_NULL_ARG(1, helper, ISpriteDrawHelper);
	DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], ISpriteDrawHelper)	
    self->setPostDrawHelper(helper);
	NO_RETURN;
	END
%#endif // !PDG_NO_GUI
METHOD_IMPL(Sprite, ChangeFramesImage)
	METHOD_SIGNATURE("", undefined, 2, ([object Image] oldImage, [object Image] newImage));
    REQUIRE_ARG_COUNT(2);
    REQUIRE_NATIVE_OBJECT_ARG(1, oldImage, Image);
    REQUIRE_NATIVE_OBJECT_ARG(2, newImage, Image);
	self->changeFramesImage(oldImage, newImage);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, OffsetFrameCenters)
	METHOD_SIGNATURE("", undefined, 5, ([number int] offsetX, [number int] offsetY, [object Image] image = null, [number int] startingFrame = start_FromFirstFrame, [number int] numFrames = all_Frames));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, offsetX);
    REQUIRE_INT32_ARG(2, offsetY);
    OPTIONAL_NATIVE_OBJECT_ARG(3, image, Image, 0);
    OPTIONAL_INT32_ARG(4, startingFrame, Sprite::start_FromFirstFrame);
    OPTIONAL_INT32_ARG(5, numFrames, Sprite::all_Frames);
	self->offsetFrameCenters(offsetX, offsetY, image, startingFrame, numFrames);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, GetFrameCenterOffset)
	METHOD_SIGNATURE("", Offset, 2, ([object Image] image = null, [number int] frameNum = 0));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_NATIVE_OBJECT_ARG(1, image, Image, 0);
    OPTIONAL_INT32_ARG(2, frameNum, 0);
	int offsetX;
	int offsetY;
	self->getFrameCenterOffset(offsetX, offsetY, image, frameNum);
	pdg::Offset offset(offsetX, offsetY);
	RETURN_OFFSET(offset);
	END
METHOD_IMPL(Sprite, SetOpacity)
	METHOD_SIGNATURE("", [object Sprite], 1, (number opacity));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NUMBER_ARG(1, opacity);
	self->setOpacity(opacity);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetOpacity)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	float opacity = self->getOpacity();
	RETURN_NUMBER(opacity);
	END
METHOD_IMPL(Sprite, FadeTo)
	METHOD_SIGNATURE("", undefined, 3, (number targetOpacity, [number int] msDuration, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_NUMBER_ARG(1, targetOpacity);
	REQUIRE_INT32_ARG(2, msDuration);
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::linearTween);
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
    	self->fadeTo(targetOpacity, msDuration, gEasingFunctions[easing]);
    } else {
		self->fadeTo(targetOpacity, msDuration);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sprite, FadeIn)
	METHOD_SIGNATURE("", undefined, 2, ([number int] msDuration, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_INT32_ARG(1, msDuration);
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween);
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
    	self->fadeIn(msDuration, gEasingFunctions[easing]);
    } else {
		self->fadeIn(msDuration);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sprite, FadeOut)
	METHOD_SIGNATURE("", undefined, 2, ([number int] msDuration, [number int] easing = linearTween));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_INT32_ARG(1, msDuration);
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween);
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) {
    	self->fadeOut(msDuration, gEasingFunctions[easing]);
    } else {
		self->fadeOut(msDuration);
	}
	NO_RETURN;
	END
METHOD_IMPL(Sprite, IsBehind)
	METHOD_SIGNATURE("", boolean, 1, ([object Sprite] sprite));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite);
	bool behind = self->isBehind(sprite);
	RETURN_BOOL(behind);
	END
METHOD_IMPL(Sprite, GetZOrder)
	METHOD_SIGNATURE("", [number int], 0, ());
    REQUIRE_ARG_COUNT(0);
	int zorder = self->getZOrder();
	RETURN_INTEGER(zorder);
	END
METHOD_IMPL(Sprite, MoveBehind)
	METHOD_SIGNATURE("", [object Sprite], 1, ([object Sprite] sprite));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite);
	self->moveBehind(sprite);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, MoveInFrontOf)
	METHOD_SIGNATURE("", [object Sprite], 1, ([object Sprite] sprite));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite);
	self->moveInFrontOf(sprite);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, MoveToFront)
	METHOD_SIGNATURE("put this sprite in front of all others in its layer", [object Sprite], 0, ());
    REQUIRE_ARG_COUNT(0);
	self->moveToFront();
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, MoveToBack)
	METHOD_SIGNATURE("put this sprite behind all others in its layer", [object Sprite], 0, ());
    REQUIRE_ARG_COUNT(0);
	self->moveToBack();
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, EnableCollisions)
	METHOD_SIGNATURE("", [object Sprite], 1, ([number int] collisionType = collide_AlphaChannel));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_INT32_ARG(1, collisionType, Sprite::collide_AlphaChannel);
	self->enableCollisions(collisionType);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, DisableCollisions)
	METHOD_SIGNATURE("", [object Sprite], 0, ());
    REQUIRE_ARG_COUNT(0);
	self->disableCollisions();
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, SetCollisionRadius)
	METHOD_SIGNATURE("", [object Sprite], 1, (number pixelRadius));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NUMBER_ARG(1, pixelRadius);
	self->setCollisionRadius(pixelRadius);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetCollisionRadius)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	float radius = self->getCollisionRadius();
	RETURN_NUMBER(radius);
	END
METHOD_IMPL(Sprite, UseCollisionMask)
	METHOD_SIGNATURE("", undefined, 2, ([object Image] frameImage, [object Image] maskImage));
    REQUIRE_ARG_COUNT(2);
    REQUIRE_NATIVE_OBJECT_ARG(1, frameImage, Image);
    REQUIRE_NATIVE_OBJECT_ARG(2, maskImage, Image);
	self->useCollisionMask(frameImage, maskImage);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, SetCollisionHelper)
	METHOD_SIGNATURE("", undefined, 1, ([object ISpriteCollideHelper] helper));
	OBJECT_SAVE(self->mSpriteScriptObj, THIS);
    DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], ISpriteCollideHelper);
    REQUIRE_ARG_COUNT(1);
	REQUIRE_NATIVE_OBJECT_OR_SUBCLASS_ARG(1, helper, ISpriteCollideHelper);
	self->setCollisionHelper(helper);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, SetElasticity)
	METHOD_SIGNATURE("", [object Sprite], 1, (number elasticity));
    REQUIRE_ARG_COUNT(1);
    REQUIRE_NUMBER_ARG(1, elasticity);
	self->setElasticity(elasticity);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetElasticity)
	METHOD_SIGNATURE("", number, 0, ());
    REQUIRE_ARG_COUNT(0);
	float elasticity = self->getElasticity();
	RETURN_NUMBER(elasticity);
	END
%#ifndef PDG_NO_GUI
METHOD_IMPL(Sprite, SetWantsMouseOverEvents)
	METHOD_SIGNATURE("", boolean, 1, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	bool wanted = self->setWantsMouseOverEvents(wantsThem);
	RETURN_BOOL(wanted);
	END
METHOD_IMPL(Sprite, SetWantsClickEvents)
	METHOD_SIGNATURE("", boolean, 1, (boolean wantsThem = true));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_BOOL_ARG(1, wantsThem, true);
	bool wanted = self->setWantsClickEvents(wantsThem);
	RETURN_BOOL(wanted);
	END
METHOD_IMPL(Sprite, SetMouseDetectMode)
	METHOD_SIGNATURE("", number, 1, ([number int] collisionType = collide_BoundingBox));
    REQUIRE_ARG_MIN_COUNT(0);
    OPTIONAL_INT32_ARG(1, collisionType, Sprite::collide_BoundingBox);
	int oldMode = self->setMouseDetectMode(collisionType);
	RETURN_INTEGER(oldMode);
	END
%#endif // !PDG_NO_GUI
METHOD_IMPL(Sprite, GetLayer)
	METHOD_SIGNATURE("get the layer that contains this sprite", [object SpriteLayer], 0, ());
    REQUIRE_ARG_COUNT(0);
	SpriteLayer* layer = self->getLayer();
	RETURN_NATIVE_OBJECT(layer, SpriteLayer);
	END
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
METHOD_IMPL(Sprite, MakeStatic)
	METHOD_SIGNATURE("", [object Sprite], 0, ());
	REQUIRE_ARG_COUNT(0);
	self->makeStatic();
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetFriction)
	METHOD_SIGNATURE("", number, 0, ());
	REQUIRE_ARG_COUNT(0);
	float friction = self->getFriction();
	RETURN_NUMBER(friction);
	END
METHOD_IMPL(Sprite, SetCollideGroup)
	METHOD_SIGNATURE("", [object Sprite], 1, ([number int] group));
	REQUIRE_ARG_COUNT(1);
	REQUIRE_INT32_ARG(1, group);
	self->setCollideGroup(group);
	RETURN_THIS;
	END
METHOD_IMPL(Sprite, GetCollideGroup)
	METHOD_SIGNATURE("", number, 0, ());
	REQUIRE_ARG_COUNT(0);
	long group = self->getCollideGroup();
	RETURN_INTEGER(group);
	END
METHOD_IMPL(Sprite, PinJoint)
	METHOD_SIGNATURE("", [object CpConstraint], 4, ([object Offset] anchor, [object Sprite] otherSprite, [object Offset] otherAnchor, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(3);
	REQUIRE_OFFSET_ARG(1, anchor);
	REQUIRE_NATIVE_OBJECT_ARG(2, otherSprite, Sprite);
	REQUIRE_OFFSET_ARG(3, otherAnchor);
	OPTIONAL_NUMBER_ARG(4, breakingForce, 0.0f);
	cpConstraint* joint = self->pinJoint(anchor, otherSprite, otherAnchor, breakingForce);
	cpConstraintSetUserData(joint, (void*)sPinJointStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, SlideJoint)
	METHOD_SIGNATURE("", [object CpConstraint], 6, ([object Offset] anchor, [object Sprite] otherSprite, [object Offset] otherAnchor, number minDist, number maxDist, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(5);
	REQUIRE_OFFSET_ARG(1, anchor);
	REQUIRE_NATIVE_OBJECT_ARG(2, otherSprite, Sprite);
	REQUIRE_OFFSET_ARG(3, otherAnchor);
	REQUIRE_NUMBER_ARG(4, minDist);
	REQUIRE_NUMBER_ARG(5, maxDist);
	OPTIONAL_NUMBER_ARG(6, breakingForce, 0.0f);
	cpConstraint* joint = self->slideJoint(anchor, otherSprite, otherAnchor, minDist, maxDist, breakingForce);
	cpConstraintSetUserData(joint, (void*)sSlideJointStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, PivotJoint)
	METHOD_SIGNATURE("", [object CpConstraint], 3, ([object Sprite] otherSprite, [object Point] pivot, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_POINT_ARG(2, pivot);
	OPTIONAL_NUMBER_ARG(3, breakingForce, 0.0f);
	cpConstraint* joint = self->pivotJoint(otherSprite, pivot, breakingForce);
	cpConstraintSetUserData(joint, (void*)sPivotJointStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, GrooveJoint)
	METHOD_SIGNATURE("", [object CpConstraint], 5, ([object Offset] grooveStart, [object Offset] grooveEnd, [object Sprite] otherSprite, [object Offset] otherAnchor, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(4);
	REQUIRE_OFFSET_ARG(1, grooveStart);
	REQUIRE_OFFSET_ARG(2, grooveEnd);
	REQUIRE_NATIVE_OBJECT_ARG(3, otherSprite, Sprite);
	REQUIRE_OFFSET_ARG(4, otherAnchor);
	OPTIONAL_NUMBER_ARG(5, breakingForce, 0.0f);
	cpConstraint* joint = self->grooveJoint(grooveStart, grooveEnd, otherSprite, otherAnchor, breakingForce);
	cpConstraintSetUserData(joint, (void*)sGrooveJointStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, SpringJoint)
	METHOD_SIGNATURE("", [object CpConstraint], 7, ([object Offset] anchor, [object Sprite] otherSprite, [object Offset] otherAnchor, number restLength, number stiffness, number damping, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(6);
	REQUIRE_OFFSET_ARG(1, anchor);
	REQUIRE_NATIVE_OBJECT_ARG(2, otherSprite, Sprite);
	REQUIRE_OFFSET_ARG(3, otherAnchor);
	REQUIRE_NUMBER_ARG(4, restLength);
	REQUIRE_NUMBER_ARG(5, stiffness);
	REQUIRE_NUMBER_ARG(6, damping);
	OPTIONAL_NUMBER_ARG(7, breakingForce, 0.0f);
	cpConstraint* joint = self->springJoint(anchor, otherSprite, otherAnchor, restLength, stiffness, damping, breakingForce);
	cpConstraintSetUserData(joint, (void*)sSpringJointStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, RotarySpring)
	METHOD_SIGNATURE("", [object CpConstraint], 5, ([object Sprite] otherSprite, number restAngle, number stiffness, number damping, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(4);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_NUMBER_ARG(2, restAngle);
	REQUIRE_NUMBER_ARG(3, stiffness);
	REQUIRE_NUMBER_ARG(4, damping);
	OPTIONAL_NUMBER_ARG(5, breakingForce, 0.0f);
	cpConstraint* joint = self->rotarySpring(otherSprite, restAngle, stiffness, damping, breakingForce);
	cpConstraintSetUserData(joint, (void*)sRotarySpringStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, RotaryLimit)
	METHOD_SIGNATURE("", [object CpConstraint], 4, ([object Sprite] otherSprite, number minAngle, number maxAngle, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(3);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_NUMBER_ARG(2, minAngle);
	REQUIRE_NUMBER_ARG(3, maxAngle);
	OPTIONAL_NUMBER_ARG(4, breakingForce, 0.0f);
	cpConstraint* joint = self->rotaryLimit(otherSprite, minAngle, maxAngle, breakingForce);
	cpConstraintSetUserData(joint, (void*)sRotaryLimitStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, Ratchet)
	METHOD_SIGNATURE("", [object CpConstraint], 4, ([object Sprite] otherSprite, number rachetInterval, number phase, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_NUMBER_ARG(2, rachetInterval);
	OPTIONAL_NUMBER_ARG(3, phase, 0.0f);
	OPTIONAL_NUMBER_ARG(4, breakingForce, 0.0f);
	cpConstraint* joint = self->ratchet(otherSprite, rachetInterval, phase, breakingForce);
	cpConstraintSetUserData(joint, (void*)sRatchetStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, Gear)
	METHOD_SIGNATURE("", [object CpConstraint], 4, ([object Sprite] otherSprite, number gearRatio, number initialAngle = 0, number breakingForce = 0));
	REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_NUMBER_ARG(2, gearRatio);
	OPTIONAL_NUMBER_ARG(3, initialAngle, 0.0f);
	OPTIONAL_NUMBER_ARG(4, breakingForce, 0.0f);
	cpConstraint* joint = self->gear(otherSprite, gearRatio, initialAngle, breakingForce);
	cpConstraintSetUserData(joint, (void*)sGearStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, Motor)
	METHOD_SIGNATURE("", [object CpConstraint], 3, ([object Sprite] otherSprite, number spin, number maxTorque = INFINITY));
	REQUIRE_ARG_MIN_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, otherSprite, Sprite);
	REQUIRE_NUMBER_ARG(2, spin);
	OPTIONAL_NUMBER_ARG(3, maxTorque, std::numeric_limits<float>::infinity());
	cpConstraint* joint = self->motor(otherSprite, spin, maxTorque);
	cpConstraintSetUserData(joint, (void*)sMotorStr);
	RETURN_NEW_NATIVE_OBJECT(joint, cpConstraint);  // has no mcpConstraintScriptObj member
	END
METHOD_IMPL(Sprite, RemoveJoint)
	METHOD_SIGNATURE("", undefined, 1, ([object CpConstraint] joint));
	REQUIRE_ARG_COUNT(1);
	REQUIRE_NATIVE_OBJECT_ARG(1, joint, cpConstraint);
	self->removeJoint(joint);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, Disconnect)
	METHOD_SIGNATURE("", undefined, 1, ([object Sprite] otherSprite));
	OPTIONAL_NATIVE_OBJECT_ARG(1, otherSprite, Sprite, 0);
	self->disconnect(otherSprite);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, MakeJointBreakable)
	METHOD_SIGNATURE("", undefined, 2, ([object CpConstraint] joint, number breakingForce));
	REQUIRE_ARG_COUNT(2);
	REQUIRE_NATIVE_OBJECT_ARG(1, joint, cpConstraint);
	REQUIRE_NUMBER_ARG(2, breakingForce);
	self->makeJointBreakable(joint, breakingForce);
	NO_RETURN;
	END
METHOD_IMPL(Sprite, MakeJointUnbreakable)
	METHOD_SIGNATURE("", undefined, 1, ([object CpConstraint] joint));
	REQUIRE_ARG_COUNT(1);
	REQUIRE_NATIVE_OBJECT_ARG(1, joint, cpConstraint);
	self->makeJointUnbreakable(joint);
	NO_RETURN;
	END
%#endif

CLEANUP_IMPL(Sprite)

NATIVE_CONSTRUCTOR_IMPL(Sprite)
	return new Sprite();
	END

// ========================================================================================
//MARK: SpriteLayer
// ========================================================================================

#define HAS_SPRITE_LAYER_METHODS(klass) \
	HAS_METHOD(klass, "setSerializationFlags", SetSerializationFlags) \
	HAS_METHOD(klass, "startAnimations", StartAnimations)  \
	HAS_METHOD(klass, "stopAnimations", StopAnimations)  \
	HAS_METHOD(klass, "hide", Hide)  \
	HAS_METHOD(klass, "show", Show)  \
	HAS_METHOD(klass, "isHidden", IsHidden)  \
	HAS_METHOD(klass, "fadeIn", FadeIn)  \
	HAS_METHOD(klass, "fadeOut", FadeOut)  \
	HAS_METHOD(klass, "moveBehind", MoveBehind)  \
	HAS_METHOD(klass, "moveInFrontOf", MoveInFrontOf)  \
	HAS_METHOD(klass, "moveToFront", MoveToFront)  \
	HAS_METHOD(klass, "moveToBack", MoveToBack)  \
	HAS_METHOD(klass, "getZOrder", GetZOrder)  \
	HAS_METHOD(klass, "moveWith", MoveWith)  \
	HAS_METHOD(klass, "findSprite", FindSprite)  \
	HAS_METHOD(klass, "getNthSprite", GetNthSprite)  \
	HAS_METHOD(klass, "getSpriteZOrder", GetSpriteZOrder)  \
	HAS_METHOD(klass, "isSpriteBehind", IsSpriteBehind)  \
	HAS_METHOD(klass, "hasSprite", HasSprite)  \
	HAS_METHOD(klass, "addSprite", AddSprite)  \
	HAS_METHOD(klass, "removeSprite", RemoveSprite)  \
	HAS_METHOD(klass, "removeAllSprites", RemoveAllSprites)  \
	HAS_METHOD(klass, "enableCollisions", EnableCollisions)  \
	HAS_METHOD(klass, "disableCollisions", DisableCollisions)  \
	HAS_METHOD(klass, "enableCollisionsWithLayer", EnableCollisionsWithLayer)  \
	HAS_METHOD(klass, "disableCollisionsWithLayer", DisableCollisionsWithLayer)  \
	HAS_METHOD(klass, "createSprite", CreateSprite)  \

#define HAS_SPRITE_LAYER_GUI_METHODS(klass) \
	HAS_METHOD(klass, "getSpritePort", GetSpritePort)  \
	HAS_METHOD(klass, "setSpritePort", SetSpritePort)  \
	HAS_METHOD(klass, "setOrigin", SetOrigin)  \
	HAS_METHOD(klass, "getOrigin", GetOrigin)  \
	HAS_METHOD(klass, "setAutoCenter", SetAutoCenter)  \
	HAS_METHOD(klass, "setFixedMoveAxis", SetFixedMoveAxis)  \
	HAS_METHOD(klass, "setZoom", SetZoom)  \
	HAS_METHOD(klass, "getZoom", GetZoom)  \
	HAS_METHOD(klass, "zoomTo", ZoomTo)  \
	HAS_METHOD(klass, "zoom", Zoom)  \
	HAS_METHOD(klass, "layerToPortPoint", LayerToPortPoint)  \
	HAS_METHOD(klass, "layerToPortOffset", LayerToPortOffset)  \
	HAS_METHOD(klass, "layerToPortVector", LayerToPortVector)  \
	HAS_METHOD(klass, "layerToPortRect", LayerToPortRect)  \
	HAS_METHOD(klass, "layerToPortQuad", LayerToPortQuad)  \
	HAS_METHOD(klass, "portToLayerPoint", PortToLayerPoint)  \
	HAS_METHOD(klass, "portToLayerOffset", PortToLayerOffset)  \
	HAS_METHOD(klass, "portToLayerVector", PortToLayerVector)  \
	HAS_METHOD(klass, "portToLayerRect", PortToLayerRect)  \
	HAS_METHOD(klass, "portToLayerQuad", PortToLayerQuad)  \

#define HAS_SPRITE_LAYER_CHIPMUNK_METHODS(klass) \
	HAS_METHOD(klass, "setGravity", SetGravity)  \
	HAS_METHOD(klass, "setUseChipmunkPhysics", SetUseChipmunkPhysics)  \
	HAS_METHOD(klass, "setStaticLayer", SetStaticLayer)  \
	HAS_METHOD(klass, "setKeepGravityDownward", SetKeepGravityDownward)  \
	HAS_METHOD(klass, "setDamping", SetDamping)  \
	HAS_METHOD(klass, "getSpace", GetSpace)  \

#define SPRITE_LAYER_BASE_CLASS_GUI_IMPL(klass) CR \
METHOD_IMPL(klass, GetSpritePort) CR \
	METHOD_SIGNATURE("", [object Port], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	Port* port = self->getSpritePort(); CR \
	RETURN_NATIVE_OBJECT(port, Port); CR \
	END CR \
METHOD_IMPL(klass, SetSpritePort) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object Port] port)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, port, Port); CR \
	self->setSpritePort(port); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetOrigin) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object Point] origin)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_POINT_ARG(1, origin); CR \
	self->setOrigin(origin); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, GetOrigin) CR \
	METHOD_SIGNATURE("get the point in the layer that is drawn at 0,0 in the port", [object Point], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    Point p = self->getOrigin(); CR \
	RETURN( POINT2VAL(p) ); CR \
	END CR \
METHOD_IMPL(klass, SetAutoCenter) CR \
	METHOD_SIGNATURE("", undefined, 1, (boolean autoCenter = true)); CR \
    REQUIRE_ARG_MIN_COUNT(0); CR \
    OPTIONAL_BOOL_ARG(1, autoCenter, true); CR \
	self->setAutoCenter(autoCenter); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetFixedMoveAxis) CR \
	METHOD_SIGNATURE("", undefined, 1, (boolean fixedAxis = true)); CR \
    REQUIRE_ARG_MIN_COUNT(0); CR \
    OPTIONAL_BOOL_ARG(1, fixedAxis, true); CR \
    self->setFixedMoveAxis(fixedAxis); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetZoom) CR \
	METHOD_SIGNATURE("", undefined, 1, (number zoomLevel)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, zoomLevel); CR \
	self->setZoom(zoomLevel); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, GetZoom) CR \
	METHOD_SIGNATURE("get the current zoom factor", number, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    float zoom = self->getZoom(); CR \
	RETURN_NUMBER(zoom); CR \
	END CR \
METHOD_IMPL(klass, ZoomTo) CR \
	METHOD_SIGNATURE("", undefined, 5, (number zoomLevel, [number int] msDuration, [number int] easing = easeInOutQuad, [object Rect] keepInRect = Rect(0,0), [object Point] centerOn = Point(0,0) )); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, zoomLevel); CR \
	REQUIRE_INT32_ARG(2, msDuration); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad); CR \
	OPTIONAL_RECT_ARG(4, keepInRect, pdg::Rect(0,0)); CR \
    OPTIONAL_POINT_ARG(5, centerOn, pdg::Point(0,0)); CR \
    pdg::Point* centerOnPtr = (ARGC >= 5) ? &centerOn : 0; CR \
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    	self->zoomTo(zoomLevel, msDuration, gEasingFunctions[easing], keepInRect, centerOnPtr); CR \
    } else { CR \
		self->zoomTo(zoomLevel, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Zoom) CR \
	METHOD_SIGNATURE("", undefined, 5, (number deltaZoomLevel, [number int] msDuration, [number int] easing = easeInOutQuad, [object Rect] keepInRect = Rect(0,0), [object Point] centerOn = Point(0,0) )); CR \
    REQUIRE_ARG_MIN_COUNT(2); CR \
	REQUIRE_NUMBER_ARG(1, deltaZoomLevel); CR \
	REQUIRE_INT32_ARG(2, msDuration); CR \
	OPTIONAL_INT32_ARG(3, easing, EasingFuncRef::easeInOutQuad); CR \
	OPTIONAL_RECT_ARG(4, keepInRect, pdg::Rect(0,0)); CR \
    OPTIONAL_POINT_ARG(5, centerOn, pdg::Point(0,0)); CR \
    pdg::Point* centerOnPtr = (ARGC >= 5) ? &centerOn : 0; CR \
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    	self->zoom(deltaZoomLevel, msDuration, gEasingFunctions[easing], keepInRect, centerOnPtr); CR \
    } else { CR \
		self->zoom(deltaZoomLevel, msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, LayerToPortPoint) CR \
	METHOD_SIGNATURE("", [object Point], 1, ([object Point] p)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_POINT_ARG(1, p); CR \
    Point out = self->layerToPort(p); CR \
	RETURN( POINT2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, LayerToPortOffset) CR \
	METHOD_SIGNATURE("", [object Offset], 1, ([object Offset] o)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_OFFSET_ARG(1, o); CR \
    Offset out = self->layerToPort(o); CR \
	RETURN( OFFSET2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, LayerToPortVector) CR \
	METHOD_SIGNATURE("", [object Vector], 1, ([object Vector] v)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_VECTOR_ARG(1, v); CR \
    Vector out = self->layerToPort(v); CR \
	RETURN( VECTOR2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, LayerToPortRect) CR \
	METHOD_SIGNATURE("", [object Rect], 1, ([object Rect] r)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_ROTATED_RECT_ARG(1, r); CR \
    RotatedRect out = self->layerToPort(r); CR \
	RETURN( RECT2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, LayerToPortQuad) CR \
	METHOD_SIGNATURE("", [object Quad], 1, ([object Quad] q)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_QUAD_ARG(1, q); CR \
    Quad out = self->layerToPort(q); CR \
	RETURN( QUAD2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, PortToLayerPoint) CR \
	METHOD_SIGNATURE("", [object Point], 1, ([object Point] p)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_POINT_ARG(1, p); CR \
    Point out = self->portToLayer(p); CR \
	RETURN( POINT2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, PortToLayerOffset) CR \
	METHOD_SIGNATURE("", [object Offset], 1, ([object Offset] o)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_OFFSET_ARG(1, o); CR \
    Offset out = self->portToLayer(o); CR \
	RETURN( OFFSET2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, PortToLayerVector) CR \
	METHOD_SIGNATURE("", [object Vector], 1, ([object Vector] v)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_VECTOR_ARG(1, v); CR \
    Vector out = self->portToLayer(v); CR \
	RETURN( VECTOR2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, PortToLayerRect) CR \
	METHOD_SIGNATURE("", [object Rect], 1, ([object Rect] r)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_ROTATED_RECT_ARG(1, r); CR \
    RotatedRect out = self->portToLayer(r); CR \
	RETURN( RECT2VAL(out) ); CR \
	END CR \
METHOD_IMPL(klass, PortToLayerQuad) CR \
	METHOD_SIGNATURE("", [object Quad], 1, ([object Quad] q)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_QUAD_ARG(1, q); CR \
    Quad out = self->portToLayer(q); CR \
	RETURN( QUAD2VAL(out) ); CR \
	END

#define SPRITE_LAYER_BASE_CLASS_IMPL(klass) CR \
METHOD_IMPL(klass, SetSerializationFlags) CR \
	METHOD_SIGNATURE("", [object SpriteLayer], 0, ([number uint] flags)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_UINT32_ARG(1, flags); CR \
	self->setSerializationFlags(flags); CR \
	RETURN_THIS; CR \
	END CR \
METHOD_IMPL(klass, StartAnimations) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->startAnimations(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, StopAnimations) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->stopAnimations(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Hide) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    self->hide(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, Show) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->show(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, IsHidden) CR \
	METHOD_SIGNATURE("", boolean, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
    bool hidden = self->isHidden(); CR \
	RETURN_BOOL(hidden); CR \
	END CR \
METHOD_IMPL(klass, FadeIn) CR \
	METHOD_SIGNATURE("", undefined, 2, ([number int] msDuration, [number int] easing = linearTween)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, msDuration); CR \
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween); CR \
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    	self->fadeIn(msDuration, gEasingFunctions[easing]); CR \
    } else { CR \
		self->fadeIn(msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, FadeOut) CR \
	METHOD_SIGNATURE("", undefined, 2, ([number int] msDuration, [number int] easing = linearTween)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, msDuration); CR \
	OPTIONAL_INT32_ARG(2, easing, EasingFuncRef::linearTween); CR \
   	if (easing >= 0 && easing < NUM_EASING_FUNCTIONS) { CR \
    	self->fadeOut(msDuration, gEasingFunctions[easing]); CR \
    } else { CR \
		self->fadeOut(msDuration); CR \
	} CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveBehind) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object SpriteLayer] layer)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, layer, SpriteLayer); CR \
	self->moveBehind(layer); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveInFrontOf) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object SpriteLayer] layer)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, layer, SpriteLayer); CR \
	self->moveInFrontOf(layer); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveToFront) CR \
	METHOD_SIGNATURE("move this layer in front of all other layers", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->moveToFront(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveToBack) CR \
	METHOD_SIGNATURE("move this layer behind all other layers", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->moveToBack(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, MoveWith) CR \
	METHOD_SIGNATURE("", , 1, ([object SpriteLayer] layer, number moveRatio = 1.0, number zoomRatio = 1.0 )); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, layer, SpriteLayer); CR \
	OPTIONAL_NUMBER_ARG(2, moveRatio, 1.0f); CR \
	OPTIONAL_NUMBER_ARG(3, zoomRatio, 1.0f); CR \
	self->moveWith(layer, moveRatio, zoomRatio); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, IsSpriteBehind) CR \
	METHOD_SIGNATURE("", boolean, 2, ([object Sprite] sprite, [object Sprite] otherSprite)); CR \
    REQUIRE_ARG_COUNT(2); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite); CR \
    REQUIRE_NATIVE_OBJECT_ARG(2, otherSprite, Sprite); CR \
	bool behind = self->isSpriteBehind(sprite, otherSprite); CR \
	RETURN_BOOL(behind); CR \
	END CR \
METHOD_IMPL(klass, GetZOrder) CR \
	METHOD_SIGNATURE("", [number int], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	int zorder = self->getZOrder(); CR \
	RETURN_INTEGER(zorder); CR \
	END CR \
METHOD_IMPL(klass, GetSpriteZOrder) CR \
	METHOD_SIGNATURE("", [number int], 0, ([object Sprite] sprite)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite); CR \
	int zorder = self->getSpriteZOrder(sprite); CR \
	RETURN_INTEGER(zorder); CR \
	END CR \
METHOD_IMPL(klass, FindSprite) CR \
	METHOD_SIGNATURE("", [object Sprite], 1, ([number int] id)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, id); CR \
	Sprite* sprite = self->findSprite(id); CR \
	RETURN_NATIVE_OBJECT(sprite, Sprite); CR \
	END CR \
METHOD_IMPL(klass, GetNthSprite) CR \
	METHOD_SIGNATURE("", [object Sprite], 1, ([number int] index)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_INT32_ARG(1, index); CR \
	Sprite* sprite = self->getNthSprite(index); CR \
	RETURN_NATIVE_OBJECT(sprite, Sprite); CR \
	END CR \
METHOD_IMPL(klass, HasSprite) CR \
	METHOD_SIGNATURE("", boolean, 1, ([object Sprite] sprite)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, sprite, Sprite); CR \
	bool found = self->hasSprite(sprite); CR \
	RETURN_BOOL(found); CR \
	END CR \
METHOD_IMPL(klass, AddSprite) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object Sprite] newSprite)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, newSprite, Sprite); CR \
	self->addSprite(newSprite); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, RemoveSprite) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object Sprite] oldSprite)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, oldSprite, Sprite); CR \
    self->removeSprite(oldSprite); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, RemoveAllSprites) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->removeAllSprites(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, EnableCollisions) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->enableCollisions(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, DisableCollisions) CR \
	METHOD_SIGNATURE("", undefined, 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	self->disableCollisions(); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, EnableCollisionsWithLayer) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object SpriteLayer] otherLayer)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, otherLayer, SpriteLayer); CR \
	self->enableCollisionsWithLayer(otherLayer); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, DisableCollisionsWithLayer) CR \
	METHOD_SIGNATURE("", undefined, 1, ([object SpriteLayer] otherLayer)); CR \
    REQUIRE_ARG_COUNT(1); CR \
    REQUIRE_NATIVE_OBJECT_ARG(1, otherLayer, SpriteLayer); CR \
	self->disableCollisionsWithLayer(otherLayer); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, CreateSprite) CR \
	METHOD_SIGNATURE("", [object Sprite], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	Sprite* sprite = self->createSprite(); CR \
	RETURN_NATIVE_OBJECT(sprite, Sprite); CR \
	END CR \

#define SPRITE_LAYER_CHIPMUNK_IMPL(klass) CR \
METHOD_IMPL(klass, SetKeepGravityDownward) CR \
	METHOD_SIGNATURE("", undefined, 1, (boolean keepItDownward = true)); CR \
    OPTIONAL_BOOL_ARG(1, keepItDownward, true); CR \
    self->setKeepGravityDownward(keepItDownward); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetGravity) CR \
	METHOD_SIGNATURE("", undefined, 2, (number gravity, boolean keepItDownward = true)); CR \
    REQUIRE_ARG_MIN_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, gravity); CR \
    OPTIONAL_BOOL_ARG(2, keepItDownward, true); CR \
    self->setGravity(gravity, keepItDownward); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetDamping) CR \
	METHOD_SIGNATURE("", undefined, 1, (number damping)); CR \
    REQUIRE_ARG_COUNT(1); CR \
	REQUIRE_NUMBER_ARG(1, damping); CR \
	self->setDamping(damping); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetStaticLayer) CR \
	METHOD_SIGNATURE("", undefined, 1, (boolean isStatic = true)); CR \
    REQUIRE_ARG_MIN_COUNT(0); CR \
    OPTIONAL_BOOL_ARG(1, isStatic, true); CR \
    self->setStaticLayer(isStatic); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, SetUseChipmunkPhysics) CR \
	METHOD_SIGNATURE("", undefined, 1, (boolean useIt = true)); CR \
    REQUIRE_ARG_MIN_COUNT(0); CR \
    OPTIONAL_BOOL_ARG(1, useIt, true); CR \
    self->setUseChipmunkPhysics(useIt); CR \
	NO_RETURN; CR \
	END CR \
METHOD_IMPL(klass, GetSpace) CR \
	METHOD_SIGNATURE("", [object CpSpace], 0, ()); CR \
    REQUIRE_ARG_COUNT(0); CR \
	cpSpace* space = self->getSpace(); CR \
	RETURN_NEW_NATIVE_OBJECT(space, cpSpace); CR \
	END CR \


WRAPPER_INITIALIZER_IMPL_CUSTOM(SpriteLayer, 
  nativeObj->mEventEmitterScriptObj = obj; 
  nativeObj->mAnimatedScriptObj = obj;
  nativeObj->mSpriteLayerScriptObj = obj)
    EXPORT_CLASS_SYMBOLS("SpriteLayer", SpriteLayer, , ,
    	// method section
		HAS_EMITTER_METHODS(SpriteLayer)
		HAS_ANIMATED_METHODS(SpriteLayer)
		HAS_SERIALIZABLE_METHODS(SpriteLayer)
		HAS_SPRITE_LAYER_METHODS(SpriteLayer)
	  %#ifndef PDG_NO_GUI  CR
		HAS_SPRITE_LAYER_GUI_METHODS(SpriteLayer)
	  %#endif CR
	  %#ifdef PDG_USE_CHIPMUNK_PHYSICS  CR
		HAS_SPRITE_LAYER_CHIPMUNK_METHODS(SpriteLayer)
	  %#endif  CR
	//	HAS_METHOD(SpriteLayer, "cloneSprite", CloneSprite);
	  %#ifdef PDG_SCML_SUPPORT CR
		HAS_METHOD(SpriteLayer, "createSpriteFromSCML", CreateSpriteFromSCML) 
		HAS_METHOD(SpriteLayer, "createSpriteFromSCMLFile", CreateSpriteFromSCMLFile) 
		HAS_METHOD(SpriteLayer, "createSpriteFromSCMLEntity", CreateSpriteFromSCMLEntity)
	  %#endif CR
    );
	END
	
EMITTER_BASE_CLASS_IMPL(SpriteLayer)
ANIMATED_BASE_CLASS_IMPL(SpriteLayer)
SERIALIZABLE_BASE_CLASS_IMPL(SpriteLayer)
%#ifndef PDG_NO_GUI
SPRITE_LAYER_BASE_CLASS_GUI_IMPL(SpriteLayer)
%#endif
SPRITE_LAYER_BASE_CLASS_IMPL(SpriteLayer)
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
SPRITE_LAYER_CHIPMUNK_IMPL(SpriteLayer)
%#endif

// METHOD_IMPL(SpriteLayer, CloneSprite)
//    REQUIRE_ARG_COUNT(1);
//    REQUIRE_NATIVE_OBJECT_ARG(1, originalSprite, Sprite);
// 	  Sprite* newSprite = self->cloneSprite(originalSprite);
//    RETURN_NATIVE_OBJECT(newSprite, Sprite);
// 	END
METHOD_IMPL(SpriteLayer, CreateSpriteFromSCML)
	METHOD_SIGNATURE("", [object Sprite], 1, (string inSCML, string inEntityName = null));
	REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, inSCML);
	const char* inEntityName = 0;
	if (ARGC >= 2 && VALUE_IS_STRING(ARGV[1])) {
		REQUIRE_STRING_ARG(2, entityName);
		inEntityName = entityName;
	}
	Sprite* sprite = self->createSpriteFromSCML(inSCML, inEntityName);
	RETURN_NEW_NATIVE_OBJECT(sprite, Sprite);
	END
METHOD_IMPL(SpriteLayer, CreateSpriteFromSCMLFile)
	METHOD_SIGNATURE("", [object Sprite], 1, (string inFileName, string inEntityName = null)); 
	REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, inFileName);
	const char* inEntityName = 0;
	if (ARGC >= 2 && VALUE_IS_STRING(ARGV[1])) {
		REQUIRE_STRING_ARG(2, entityName);
		inEntityName = entityName;
	}
	Sprite* sprite = self->createSpriteFromSCMLFile(inFileName, inEntityName);
	RETURN_NEW_NATIVE_OBJECT(sprite, Sprite);
	END
METHOD_IMPL(SpriteLayer, CreateSpriteFromSCMLEntity)
	METHOD_SIGNATURE("", [object Sprite], 1, (string inEntityName)); 
	REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, inEntityName);
	Sprite* sprite = self->createSpriteFromSCMLEntity(inEntityName);
	RETURN_NEW_NATIVE_OBJECT(sprite, Sprite);
	END

CLEANUP_IMPL(SpriteLayer)

NATIVE_CONSTRUCTOR_IMPL(SpriteLayer)
  %#ifndef PDG_NO_GUI
	Port* port = GraphicsManager::getSingletonInstance()->getMainPort();
	return createSpriteLayer(port);
  %#else
 	return createSpriteLayer();
  %#endif
	END

FUNCTION_IMPL(CreateSpriteLayer)
	METHOD_SIGNATURE("", [object SpriteLayer], 1, ([object Port] port = null)); 
    REQUIRE_ARG_MIN_COUNT(0);
  %#ifndef PDG_NO_GUI
	OPTIONAL_NATIVE_OBJECT_ARG(1, port, Port, 0);
 	SpriteLayer* layer = createSpriteLayer(port);
  %#else
 	SpriteLayer* layer = createSpriteLayer();
  %#endif
    RETURN_NATIVE_OBJECT(layer, SpriteLayer);
    END

FUNCTION_IMPL(CreateSpriteLayerFromSCMLFile)
	METHOD_SIGNATURE("", [object SpriteLayer], 1, (string layerSCMLFilename, bool addSprites = true, [object Port] port = null)); 
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_STRING_ARG(1, layerSCMLFilename);
	OPTIONAL_BOOL_ARG(2, addSprites, true);
  %#ifndef PDG_NO_GUI
	OPTIONAL_NATIVE_OBJECT_ARG(3, port, Port, 0);
 	SpriteLayer* layer = createSpriteLayerFromSCMLFile(layerSCMLFilename, addSprites, port);
  %#else
 	SpriteLayer* layer = createSpriteLayerFromSCMLFile(layerSCMLFilename, addSprites);
  %#endif
    RETURN_NATIVE_OBJECT(layer, SpriteLayer);
    END

FUNCTION_IMPL(CleanupSpriteLayer)
	METHOD_SIGNATURE("", undefined, 1, ([object SpriteLayer] layer)); 
	REQUIRE_ARG_COUNT(1);
	REQUIRE_NATIVE_OBJECT_ARG(1, layer, SpriteLayer);
	cleanupSpriteLayer(layer);
	NO_RETURN;
	END

FUNCTION_IMPL(CreateTileLayer)
	METHOD_SIGNATURE("", [object TileLayer], 1, ([object Port] port = null)); 
    REQUIRE_ARG_MIN_COUNT(0);
  %#ifndef PDG_NO_GUI
	OPTIONAL_NATIVE_OBJECT_ARG(1, port, Port, 0);
 	TileLayer* layer = createTileLayer(port);
  %#else
 	TileLayer* layer = createTileLayer();
  %#endif
    RETURN_NATIVE_OBJECT(layer, TileLayer);
    END



// ========================================================================================
//MARK: TileLayer
// ========================================================================================

DECLARE_SYMBOL(tileType);
DECLARE_SYMBOL(facing);

WRAPPER_INITIALIZER_IMPL_CUSTOM(TileLayer, 
  nativeObj->mEventEmitterScriptObj = obj; 
  nativeObj->mAnimatedScriptObj = obj;
  nativeObj->mSpriteLayerScriptObj = obj;
  nativeObj->mTileLayerScriptObj = obj)
    EXPORT_CLASS_SYMBOLS("TileLayer", TileLayer, , ,
    	// method section
		HAS_EMITTER_METHODS(TileLayer)
		HAS_ANIMATED_METHODS(TileLayer)
		HAS_SPRITE_LAYER_METHODS(TileLayer)
	  %#ifndef PDG_NO_GUI  CR
		HAS_SPRITE_LAYER_GUI_METHODS(TileLayer)
	  %#endif CR
	  %#ifdef PDG_USE_CHIPMUNK_PHYSICS CR
		HAS_SPRITE_LAYER_CHIPMUNK_METHODS(TileLayer)
	  %#endif CR
		HAS_METHOD(TileLayer, "setWorldSize", SetWorldSize)
		HAS_METHOD(TileLayer, "getWorldSize", GetWorldSize)
		HAS_METHOD(TileLayer, "getWorldBounds", GetWorldBounds)
		HAS_METHOD(TileLayer, "defineTileSet", DefineTileSet)
		HAS_METHOD(TileLayer, "loadMapData", LoadMapData)
		HAS_METHOD(TileLayer, "getMapData", GetMapData)
		HAS_METHOD(TileLayer, "getTileSetImage", GetTileSetImage)
		HAS_METHOD(TileLayer, "getTileSize", GetTileSize)
		HAS_METHOD(TileLayer, "getTileTypeAt", GetTileTypeAt)
		HAS_METHOD(TileLayer, "getTileTypeAndFacingAt", GetTileTypeAndFacingAt)
		HAS_METHOD(TileLayer, "setTileTypeAt", SetTileTypeAt)
		HAS_METHOD(TileLayer, "checkCollision", CheckCollision)
    );
	END
	
EMITTER_BASE_CLASS_IMPL(TileLayer)
ANIMATED_BASE_CLASS_IMPL(TileLayer)
%#ifndef PDG_NO_GUI
SPRITE_LAYER_BASE_CLASS_GUI_IMPL(TileLayer)
%#endif
SPRITE_LAYER_BASE_CLASS_IMPL(TileLayer)
%#ifdef PDG_USE_CHIPMUNK_PHYSICS
SPRITE_LAYER_CHIPMUNK_IMPL(TileLayer)
%#endif
METHOD_IMPL(TileLayer, SetWorldSize)
	METHOD_SIGNATURE("", undefined, 4, ([number int] width, [number int] height, boolean repeatingX = false, boolean repeatingY = false));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, width);
    REQUIRE_INT32_ARG(2, height);
    OPTIONAL_BOOL_ARG(3, repeatingX, false);
    OPTIONAL_BOOL_ARG(4, repeatingY, false);
	self->setWorldSize(width, height, repeatingX, repeatingY);
	NO_RETURN;
	END
METHOD_IMPL(TileLayer, GetWorldSize)
	METHOD_SIGNATURE("", [object Rect], 0, ());
    REQUIRE_ARG_COUNT(0);
    Rect r = self->getWorldSize();
	RETURN( RECT2VAL(r) );
	END
METHOD_IMPL(TileLayer, GetWorldBounds)
	METHOD_SIGNATURE("", [object Rect], 0, ());
    REQUIRE_ARG_COUNT(0);
    Rect r = self->getWorldBounds();
	RETURN( RECT2VAL(r) );
	END
METHOD_IMPL(TileLayer, GetTileTypeAt)
	METHOD_SIGNATURE("", number, 0, ([number int] x, [number int] y));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, x);
    REQUIRE_INT32_ARG(2, y);
    uint8 tileType;
	tileType = self->getTileTypeAt(x, y);
   	RETURN_UNSIGNED(tileType);
	END
METHOD_IMPL(TileLayer, GetTileTypeAndFacingAt)
	METHOD_SIGNATURE("", object, 0, ([number int] x, [number int] y));
    REQUIRE_ARG_MIN_COUNT(2);
    REQUIRE_INT32_ARG(1, x);
    REQUIRE_INT32_ARG(2, y);
    uint8 tileType;
	TileLayer::TFacing facing;
	tileType = self->getTileTypeAt(x, y, &facing);
	if (self->mUseFacing || self->mUseFlipping) {
		if (self->mUseFlipping && (!self->mFlipHoriz || !self->mFlipVert)) {
			tileType &= 0x7f;
		} else {
			tileType &= 0x3f;
		}
	}
	OBJECT_REF tileInfo = OBJECT_CREATE_EMPTY(0);
	OBJECT_SET_PROPERTY_VALUE(tileInfo, SYMBOL(tileType), UINT2VAL(tileType));
	OBJECT_SET_PROPERTY_VALUE(tileInfo, SYMBOL(facing), UINT2VAL(facing));
	RETURN_OBJECT(tileInfo);		
	END
METHOD_IMPL(TileLayer, DefineTileSet)
	METHOD_SIGNATURE("", undefined, 5, ([number int] tileWidth, [number int] tileHeight, [object Image] tiles, boolean hasTransparency = true, boolean flipTiles = false));
    REQUIRE_ARG_MIN_COUNT(3);
    REQUIRE_INT32_ARG(1, tileWidth);
    REQUIRE_INT32_ARG(2, tileHeight);
    REQUIRE_NATIVE_OBJECT_ARG(3, tiles, Image);
    OPTIONAL_BOOL_ARG(4, hasTransparency, true);
    OPTIONAL_BOOL_ARG(5, flipTiles, false);
	self->defineTileSet(tileWidth, tileHeight, tiles, hasTransparency, flipTiles);
	NO_RETURN;
	END
METHOD_IMPL(TileLayer, LoadMapData)
	METHOD_SIGNATURE("", undefined, 5, ({[string Binary]|[object MemBlock]} data, [number int] mapWidth = 0, [number int] mapHeight = 0, [number int] dstX, [number int] dstY));
    REQUIRE_ARG_MIN_COUNT(1);
    OPTIONAL_INT32_ARG(2, mapWidth, 0);
    OPTIONAL_INT32_ARG(3, mapHeight, 0);
    OPTIONAL_INT32_ARG(4, dstX, 0);
    OPTIONAL_INT32_ARG(5, dstY, 0);
    if (mapWidth > self->mWorldWidth) {
   		THROW_RANGE_ERR("argument 2 (mapWidth) is larger than world width");
    }
    if ((mapWidth + dstX) > self->mWorldWidth) {
   		THROW_RANGE_ERR("mapWidth + dstX is larger than world width");
    }
    if (mapHeight > self->mWorldHeight) {
   		THROW_RANGE_ERR("argument 3 (mapHeight) is larger than world height");
    }
    if ((mapHeight + dstY) > self->mWorldHeight) {
   		THROW_RANGE_ERR("mapHeight + dstY is larger than world height");
    }
    if (!VALUE_IS_STRING(ARGV[0]) && !VALUE_IS_OBJECT_OF_CLASS(ARGV[0], MemBlock)) {
    	THROW_TYPE_ERR("argument 1 (data) must be either a binary string or an object of type MemBlock");
    }
    if (VALUE_IS_STRING(ARGV[0])) {
    	size_t bytes = 0;
    	uint8* ptr = (uint8*) DecodeBinary(ARGV[0], &bytes);
    	if (bytes < ((size_t)mapWidth * (size_t)mapHeight)) {
    		THROW_RANGE_ERR("argument 1 (data) is insufficient, please check mapWidth and mapHeight against data size");
    	}
    	self->loadMapData(ptr, mapWidth, mapHeight, dstX, dstY);
		std::free(ptr);
	} else {
    	REQUIRE_NATIVE_OBJECT_ARG(1, memBlock, MemBlock);
    	if (memBlock->bytes < ((size_t)mapWidth * (size_t)mapHeight)) {
    		THROW_RANGE_ERR("argument 1 (data) is insufficient, please check mapWidth and mapHeight against data size");
    	}
		self->loadMapData((uint8*)memBlock->ptr, mapWidth, mapHeight, dstX, dstY);
    }
	NO_RETURN;
	END
METHOD_IMPL(TileLayer, GetMapData)
	METHOD_SIGNATURE("", [object MemBlock], 4, ([number int] mapWidth = 0, [number int] mapHeight = 0, [number int] srcX, [number int] srcY));
    OPTIONAL_INT32_ARG(1, mapWidth, self->mWorldWidth);
    OPTIONAL_INT32_ARG(2, mapHeight, self->mWorldHeight);
    OPTIONAL_INT32_ARG(3, srcX, 0);
    OPTIONAL_INT32_ARG(4, srcY, 0);
    if (mapWidth > self->mWorldWidth) {
   		THROW_RANGE_ERR("argument 1 (mapWidth) is larger than world width");
    }
    if ((mapWidth + srcX) > self->mWorldWidth) {
   		THROW_RANGE_ERR("mapWidth + srcX is larger than world width");
    }
    if (mapHeight > self->mWorldHeight) {
   		THROW_RANGE_ERR("argument 2 (mapHeight) is larger than world height");
    }
    if ((mapHeight + srcY) > self->mWorldHeight) {
   		THROW_RANGE_ERR("mapHeight + srcY is larger than world height");
    }
	const uint8* dataPtr = self->getMapData(mapWidth, mapHeight, srcX, srcY);
	size_t bufferSize = mapWidth * mapHeight;
	uint8* ptr = (uint8*) std::malloc(bufferSize);
	std::memcpy(ptr, dataPtr, bufferSize);
 	MemBlock* memBlock = new MemBlock((char*)ptr, bufferSize, true);
	RETURN_NATIVE_OBJECT(memBlock, MemBlock);
	END
METHOD_IMPL(TileLayer, GetTileSetImage)
	METHOD_SIGNATURE("", [object Image], 0, ());
    REQUIRE_ARG_COUNT(0);
    Image* tiles = self->getTileSetImage();
    RETURN_NATIVE_OBJECT(tiles, Image);
	END
METHOD_IMPL(TileLayer, GetTileSize)
	METHOD_SIGNATURE("", [object Point], 0, ());
    REQUIRE_ARG_COUNT(0);
    Point size = self->getTileSize();
	RETURN( POINT2VAL(size) );
	END
METHOD_IMPL(TileLayer, SetTileTypeAt)
	METHOD_SIGNATURE("", undefined, 0, ([number int] x, [number int] y, [number uint] t, [number uint] facing = facing_Ignore));
    REQUIRE_ARG_MIN_COUNT(3);
    REQUIRE_INT32_ARG(1, x);
    REQUIRE_INT32_ARG(2, y);
    REQUIRE_UINT32_ARG(3, t);
    OPTIONAL_UINT32_ARG(4, facing, (uint32) TileLayer::facing_Ignore);
	self->setTileTypeAt(x, y, t, (TileLayer::TFacing) facing);
	NO_RETURN;
	END
METHOD_IMPL(TileLayer, CheckCollision)
	METHOD_SIGNATURE("", number, 0, ([object Sprite] movingSprite, [number uint] alphaThreshold = 128, boolean shortCircuit = true));
    REQUIRE_ARG_MIN_COUNT(1);
    REQUIRE_NATIVE_OBJECT_ARG(1, movingSprite, Sprite);
    OPTIONAL_UINT32_ARG(2, alphaThreshold, 128);
    OPTIONAL_BOOL_ARG(3, shortCircuit, true);
    uint32 overlapPx = self->checkCollision(movingSprite, alphaThreshold, shortCircuit);
    RETURN_UNSIGNED(overlapPx);
	END

CLEANUP_IMPL(TileLayer)

NATIVE_CONSTRUCTOR_IMPL(TileLayer)
	return 0; //new TileLayer();
	END

// ========================================================================================
//MARK: Script Serializable
// ========================================================================================

ScriptSerializable::ScriptSerializable() {
}


// ========================================================================================
//MARK: Script Event Handler
// ========================================================================================

ScriptEventHandler::ScriptEventHandler() {
}

// ========================================================================================
//MARK: Script Animation Helper
// ========================================================================================

ScriptAnimationHelper::ScriptAnimationHelper() {
}


// ========================================================================================
//MARK: Script Sprite Collide Helper
// ========================================================================================

ScriptSpriteCollideHelper::ScriptSpriteCollideHelper() {
}


%#ifndef PDG_NO_GUI
// ========================================================================================
//MARK: Script Sprite Draw Helper
// ========================================================================================

ScriptSpriteDrawHelper::ScriptSpriteDrawHelper() {
}


%#endif // !PDG_NO_GUI


// ========================================================================================
//MARK: Utility Functions
// ========================================================================================

FUNCTION_IMPL(Rand)
	METHOD_SIGNATURE("", number, 0, ()); 
	REQUIRE_ARG_COUNT(0);
	RETURN_UNSIGNED( OS::rand() );
	END
FUNCTION_IMPL(GameCriticalRandom)
	METHOD_SIGNATURE("", number, 0, ()); 
	REQUIRE_ARG_COUNT(0);
 	RETURN_UNSIGNED( OS::gameCriticalRandom() );
 	END
FUNCTION_IMPL(Srand)
	METHOD_SIGNATURE("", undefined, 1, ([number uint] seed)); 
	REQUIRE_ARG_COUNT(1);
    REQUIRE_UINT32_ARG(1, seed);
 	OS::srand( seed );
 	NO_RETURN;
 	END
FUNCTION_IMPL(SetSerializationDebugMode)
	METHOD_SIGNATURE("", undefined, 1, (bool debugMode)); 
	REQUIRE_ARG_COUNT(1);
    REQUIRE_BOOL_ARG(1, debugMode);
 	ISerializer::s_DebugMode = debugMode;
 	NO_RETURN;
 	END



// =========================  initializers =============================
bool Initializer::allowHorizontalOrientation() throw() { return true; }
bool Initializer::allowVerticalOrientation() throw() { return true; }
const char* Initializer::getAppName(bool haveMainResourceFile) throw() { return "pdg"; }
const char* Initializer::getMainResourceFileName() throw() {return NULL;}
bool Initializer::installGlobalHandlers() throw() {return true;}
bool Initializer::getGraphicsEnvironmentDimensions(Rect maxWindowDim, Rect maxFullScreenDim,
	long& ioWidth, long& ioHeight, uint8& ioDepth) throw()
{
  ioWidth = 640;
  ioHeight = 480;
  ioDepth = 32;
  return false;
}


FUNCTION_IMPL(Idle)
	METHOD_SIGNATURE("", undefined, 0, ()); 
	REQUIRE_ARG_COUNT(0);
	pdg_LibIdle();
	NO_RETURN;
END

FUNCTION_IMPL(Run)
	METHOD_SIGNATURE("", undefined, 0, ()); 
	REQUIRE_ARG_COUNT(0);
	pdg_LibRun();
	NO_RETURN;
END

FUNCTION_IMPL(Quit)
	METHOD_SIGNATURE("", undefined, 0, ()); 
	REQUIRE_ARG_COUNT(0);
	pdg_LibQuit();
	NO_RETURN;
END

FUNCTION_IMPL(IsQuitting)
	METHOD_SIGNATURE("", boolean, 0, ()); 
	REQUIRE_ARG_COUNT(0);
	bool isQuitting = pdg_LibIsQuitting();
	RETURN_BOOL( isQuitting );
END



} // end pdg namespace
