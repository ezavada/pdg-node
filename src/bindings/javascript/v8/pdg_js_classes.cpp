// -----------------------------------------------
// pdg_js_classes.cpp 
//
// wrapper definitions for all the classes 
// the are exposed to Javascript, and some
// utility functions
//
// This file is parsed by "make generate-interfaces"
// to generate the interface file:
//       src/sys/js/pdg_interfaces.cpp
//
// This should only be done when the wrappers need 
// to change.
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

#include "pdg_js_macros.h"

%#include "pdg_project.h"

%#include "pdg_script_interface.h"

%#include "internals.h"
%#include "pdg-lib.h"

%#include "node.h"  // for FatalException()

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

extern bool s_IEventHandler_InNewFromCpp;
extern bool s_ISerializable_InNewFromCpp;
extern bool s_IAnimationHelper_InNewFromCpp;
extern bool s_ISpriteCollideHelper_InNewFromCpp;
extern bool s_ISpriteDrawHelper_InNewFromCpp;

// ========================================================================================
//MARK: Shared Global Declarations
// ========================================================================================

v8::Persistent<v8::Value> s_SavedError;

// ========================================================================================
//MARK: Local Declarations
// ========================================================================================

static v8::Persistent<v8::Object> s_BindingTarget;

// ========================================================================================
//MARK: Config Manager
// ========================================================================================

FUNCTION_IMPL(GetConfigManager)
	METHOD_SIGNATURE("", [object ConfigManager], 0, ()); 
    RETURN( ConfigManagerWrap::GetScriptSingletonInstance(isolate) );
    END

// ========================================================================================
//MARK: Log Manager
// ========================================================================================

FUNCTION_IMPL(GetLogManager)
	METHOD_SIGNATURE("", [object LogManager], 0, ()); 
    RETURN( LogManagerWrap::GetScriptSingletonInstance(isolate) );
END

// ========================================================================================
//MARK: IEventHandler
// ========================================================================================

CPP_CONSTRUCTOR_IMPL(IEventHandler)
	if (args.Length() == 0) {
		ScriptEventHandler* handler = new ScriptEventHandler();
		handler->addRef();
		return handler;
	} else if (args.Length() != 1 || !args[0]->IsFunction()) {
		SAVE_SYNTAX_ERR("EventHandler must be created with a function argument (handlerFunc)");
	}	
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
	ScriptEventHandler* handler = new ScriptEventHandler(callback);
	handler->addRef();
	return handler;
	END



// ========================================================================================
//MARK: Event Manager
// ========================================================================================

STATIC_METHOD_IMPL(EventManager, IsKeyDown)
	METHOD_SIGNATURE("", boolean, 1, ({string unicodeChar|[number uint] utf16CharCode}));
    REQUIRE_ARG_COUNT(1);
	if (args[0]->IsString()) {
		v8::Local<v8::String> keyCode_String = args[0]->ToString();
		uint16 utf16Char = 0;
		keyCode_String->Write(&utf16Char, 0, 1, v8::String::NO_NULL_TERMINATION);
		RETURN_BOOL( OS::isKeyDown(utf16Char) );
	} else {
    	REQUIRE_UINT32_ARG(1, utf16CharCode);
		RETURN_BOOL( OS::isKeyDown(utf16CharCode) );
	}
	END
STATIC_METHOD_IMPL(EventManager, GetDeviceOrientation)
	METHOD_SIGNATURE("NOT IMPLEMENTED", object, 0, (boolean absolute = false));
    OPTIONAL_BOOL_ARG(1, absolute, false);
    float roll, pitch, yaw;
	OS::getDeviceOrientation(roll, pitch, yaw, absolute); // not yet implemented in C++
  	v8::Local<v8::Object> jsOrientation = v8::Object::New(isolate);
	jsOrientation->Set(STR2VAL("roll"),NUM2VAL(roll));
	jsOrientation->Set(STR2VAL("pitch"),NUM2VAL(pitch));
	jsOrientation->Set(STR2VAL("yaw"),NUM2VAL(yaw));
	RETURN(jsOrientation);
	END
FUNCTION_IMPL(GetEventManager)
	METHOD_SIGNATURE("", [object EventManager], 0, ());
    v8::Local<v8::Object> jsInstance = EventManagerWrap::GetScriptSingletonInstance(isolate);
    EventManager* evtMgr = EventManager::getSingletonInstance();
    evtMgr->mEventEmitterScriptObj.Reset(isolate, jsInstance);
    RETURN(jsInstance);
	END

// ========================================================================================
//MARK: Resource Manager
// ========================================================================================

METHOD_IMPL(ResourceManager, GetImage)
	METHOD_SIGNATURE("", [object Image], 1, (string imageName));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, imageName);
	Image* img = self->getImage(imageName);
	if (img == NULL) {
		RETURN_FALSE;
	}
    v8::Local<v8::Object> obj = ImageWrap::NewFromCpp(isolate, img);
	// add a name to the object so it's easier to keep track of
    obj->Set(STR2VAL("name"), STR2VAL(imageName));
	RETURN(obj);
	END
METHOD_IMPL(ResourceManager, GetImageStrip)
	METHOD_SIGNATURE("", [object ImageStrip], 1, (string imageName));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, imageName);
	ImageStrip* img = self->getImageStrip(imageName);
	if (img == NULL) {
		RETURN_FALSE;
	}
    v8::Local<v8::Object> obj = ImageStripWrap::NewFromCpp(isolate, img);
	// add a name to the object so it's easier to keep track of
	// TODO: make read only property
    obj->Set(STR2VAL("name"), STR2VAL(imageName));
	RETURN(obj);
	END
%#ifndef PDG_NO_SOUND
METHOD_IMPL(ResourceManager, GetSound)
	METHOD_SIGNATURE("", [object Sound], 1, (string soundName));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_STRING_ARG(1, soundName);
	Sound* snd = self->getSound(soundName);
	if (snd == NULL) {
		RETURN_FALSE;
	}
    v8::Local<v8::Object> obj = SoundWrap::NewFromCpp(isolate, snd);
	// add a name to the sound so it's easier to keep track of
	// TODO: make read only property
    obj->Set(STR2VAL("name"), STR2VAL(soundName) );
	RETURN(obj);
	END
%#endif // !PDG_NO_SOUND


FUNCTION_IMPL(GetResourceManager)
	METHOD_SIGNATURE("", [object ResourceManager], 0, ());
    RETURN( ResourceManagerWrap::GetScriptSingletonInstance(isolate) );
    END

// ========================================================================================
//MARK: ISerializable
// ========================================================================================

CPP_CONSTRUCTOR_IMPL(ISerializable)
	if (args.Length() == 0) {
		ScriptSerializable* serializable = new ScriptSerializable();
		serializable->addRef();
		return serializable;
	} else {
		if (args.Length() != 4 || !args[0]->IsFunction() || !args[1]->IsFunction()
			|| !args[2]->IsFunction() || !args[3]->IsFunction()) {
			SAVE_SYNTAX_ERR("Serializable must be created with 4 function arguments "
				"(getSerializedSizeFunc, serializeFunc, deserializeFunc, getMyClassTagFunc)");
		}
		v8::Local<v8::Function> getSerializedSizeFunc = v8::Local<v8::Function>::Cast(args[0]);
		v8::Local<v8::Function> serializeFunc = v8::Local<v8::Function>::Cast(args[1]);
		v8::Local<v8::Function> deserializeFunc = v8::Local<v8::Function>::Cast(args[2]);
		v8::Local<v8::Function> getMyClassTagFunc = v8::Local<v8::Function>::Cast(args[3]);
		ScriptSerializable* serializable = 
			new ScriptSerializable(getSerializedSizeFunc, serializeFunc, 
										deserializeFunc, getMyClassTagFunc);
		serializable->addRef();
		return serializable;
	}
	END

// ========================================================================================
//MARK: Serializer
// ========================================================================================

METHOD_IMPL(Serializer, Serialize_obj)
	self->mSerializerScriptObj.Reset(isolate, args.This());  // correct for callbacks
	METHOD_SIGNATURE("", undefined, 1, ([object ISerializable] obj));
    REQUIRE_ARG_COUNT(1);
	REQUIRE_CPP_OBJECT_OR_SUBCLASS_ARG(1, obj, ISerializable);
	DEBUG_DUMP_SCRIPT_OBJECT(ARGV[0], ISerializable);
 	self->serialize_obj(obj);
	NO_RETURN;
	END
METHOD_IMPL(Serializer, SerializedSize)
	METHOD_SIGNATURE("", number, 1, ({string|boolean|[number uint]|[object Color]|[object Offset]|[object Point]|[object Vector]|[object Rect]|[object RotatedRect]|[object Quad]|[object MemBlock]|[object ISerializable]} arg));
    REQUIRE_ARG_COUNT(1);
 	uint32 dataSize = 0;
    if (VALUE_IS_STRING(ARGV[0])) {
    	VALUE_TO_CSTRING(str, ARGV[0]);
		dataSize = self->sizeof_str(str);
    } else if (VALUE_IS_BOOL(ARGV[0])) {
    	bool val = VAL2BOOL(ARGV[0]);
    	dataSize = self->sizeof_bool(val);
    } else if (VALUE_IS_NUMBER(ARGV[0])) {
    	uint32 val = VAL2UINT(ARGV[0]);
    	dataSize = self->sizeof_uint(val);
    } else if (VALUE_IS_COLOR(ARGV[0])) {
    	Color c = VAL2COLOR(ARGV[0]);
    	dataSize = self->sizeof_color(c);
    } else if (VALUE_IS_OFFSET(ARGV[0])) {
    	Offset o = VAL2OFFSET(ARGV[0]);
    	dataSize = self->sizeof_offset(o);
    } else if (VALUE_IS_RECT(ARGV[0])) {
    	Rect r = VAL2RECT(ARGV[0]);
    	dataSize = self->sizeof_rect(r);
    } else if (VALUE_IS_ROTRECT(ARGV[0])) {
    	RotatedRect rr = VAL2ROTRECT(ARGV[0]);
    	dataSize = self->sizeof_rotr(rr);
    } else if (VALUE_IS_QUAD(ARGV[0])) {
    	Quad q = VAL2QUAD(ARGV[0]);
    	dataSize = self->sizeof_quad(q);
    } else if (args[0]->IsObject()) {
		v8::Local<v8::Object> obj = args[0]->ToObject();
		MemBlockWrap* memBlock__ = ObjectWrap::Unwrap<MemBlockWrap>(obj);
    	if (memBlock__) {
    		MemBlock* memBlock = memBlock__->getCppObject();
    		dataSize = self->sizeof_mem(memBlock->ptr, memBlock->bytes);
    	} else {
    		// perhaps it's an ISerializable?
			EXTRACT_CPP_OBJECT_OR_SUBCLASS_ARG(1, serializable, ISerializable);
			if (serializable) {
				DEBUG_DUMP_SCRIPT_OBJECT(self->mSerializerScriptObj, Serializer)
				DEBUG_DUMP_SCRIPT_OBJECT(serializable->mISerializableScriptObj, ISerializable)
    			dataSize = self->sizeof_obj(serializable);
			} else {
				THROW_TYPE_ERR("argument 1 must be either an unsigned integer, "
					"a string, a MemBlock object, an ISerializable object");
			}
    	}
    }
	RETURN_UINT32(dataSize);
	END

// ========================================================================================
//MARK: Deserializer
// ========================================================================================

FUNCTION_IMPL(RegisterSerializableObject)
	METHOD_SIGNATURE("", undefined, 2, (object obj, [number uint] uniqueId));
	REQUIRE_ARG_COUNT(2);
	REQUIRE_OBJECT_ARG(1, obj);
	REQUIRE_UINT32_ARG(2, uniqueId);
	
	// We need to keep a reference to the object, but we need to do it
	// in a way that ensures that the JS engine knows it is still around
	// In V8, that means a persistent reference of some kind
	
	// FIXME: not at all sure this is what we want to do.
//	void* objPtr = static_cast<void*>(obj);
	
//	Deserializer::registerObject(objPtr, uniqueId);
    NO_RETURN;
    END



%#ifndef PDG_NO_GUI

// ========================================================================================
//MARK: Graphics Manager
// ========================================================================================

METHOD_IMPL(GraphicsManager, GetCurrentScreenMode);
	METHOD_SIGNATURE("returns object with width, height, depth and maxWindowRect for specified screen", 
		object, 0, ([number int] screenNum = PRIMARY_SCREEN));
	OPTIONAL_INT32_ARG(1, screenNum, screenNum_PrimaryScreen);
	pdg::Rect maxWindowRect;
	pdg::GraphicsManager::ScreenMode mode;
	mode = self->getCurrentScreenMode(screenNum, &maxWindowRect);
	// make native object with these properties
  	v8::Local<v8::Object> jsScreenMode = v8::Object::New(isolate);
	jsScreenMode->Set(STR2VAL("width"),INT2VAL(mode.width));
	jsScreenMode->Set(STR2VAL("height"),INT2VAL(mode.height));
	jsScreenMode->Set(STR2VAL("depth"),INT2VAL(mode.bpp));
	jsScreenMode->Set(STR2VAL("maxWindowRect"), RECT2VAL(maxWindowRect));
	RETURN(jsScreenMode);
	END

METHOD_IMPL(GraphicsManager, GetNthSupportedScreenMode);
	METHOD_SIGNATURE("returns object with width, height, depth and maxWindowRect for specified screen", 
		object, 1, ([number int] n, [number int] screenNum = PRIMARY_SCREEN));
    REQUIRE_ARG_MIN_COUNT(1);
	REQUIRE_INT32_ARG(1, n);
	OPTIONAL_INT32_ARG(2, screenNum, screenNum_PrimaryScreen);
	pdg::GraphicsManager::ScreenMode mode;
	mode = self->getNthSupportedScreenMode(n, screenNum);
	// make native object with these properties
  	v8::Local<v8::Object> jsScreenMode = v8::Object::New(isolate);
	jsScreenMode->Set(STR2VAL("width"),INT2VAL(mode.width));
	jsScreenMode->Set(STR2VAL("height"),INT2VAL(mode.height));
	jsScreenMode->Set(STR2VAL("depth"),INT2VAL(mode.bpp));
	RETURN(jsScreenMode);
	END

FUNCTION_IMPL(GetGraphicsManager)
	METHOD_SIGNATURE("", [object GraphicsManager], 0, ()); 
    RETURN( GraphicsManagerWrap::GetScriptSingletonInstance(isolate) );
    END


%#endif //!PDG_NO_GUI



%#ifndef PDG_NO_SOUND

// ========================================================================================
//MARK: Sound Manager
// ========================================================================================

FUNCTION_IMPL(GetSoundManager)
	METHOD_SIGNATURE("", [object SoundManager], 0, ()); 
    RETURN( SoundManagerWrap::GetScriptSingletonInstance(isolate) );
    END


%#endif //! PDG_NO_SOUND

// ========================================================================================
//MARK: FileManager
// ========================================================================================

FUNCTION_IMPL(GetFileManager)
	METHOD_SIGNATURE("", [object FileManager], 0, ()); 
    RETURN( FileManagerWrap::GetScriptSingletonInstance(isolate) );
    END


// ========================================================================================
//MARK: TimerManager
// ========================================================================================

FUNCTION_IMPL(GetTimerManager)
	METHOD_SIGNATURE("", [object TimerManager], 0, ()); 
    v8::Local<v8::Object> jsInstance = TimerManagerWrap::GetScriptSingletonInstance(isolate);
    TimerManager* timMgr = TimerManager::getSingletonInstance();
    timMgr->mEventEmitterScriptObj.Reset(isolate, jsInstance);
    RETURN(jsInstance);
    END

// ========================================================================================
//MARK: IAnimationHelper
// ========================================================================================

CPP_CONSTRUCTOR_IMPL(IAnimationHelper)
	if (args.Length() == 0) {
		ScriptAnimationHelper* helper = new ScriptAnimationHelper();
		return helper;
	} else if (args.Length() != 1 || !args[0]->IsFunction()) {
		SAVE_SYNTAX_ERR("AnimationHelper must be created with a function argument (handlerFunc)");
		return 0;
	}
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
	ScriptAnimationHelper* helper = new ScriptAnimationHelper(callback);
	return helper;
	END

// ========================================================================================
//MARK: Easing Functions
// ========================================================================================

static v8::Persistent<v8::Function> s_CustomScriptEasing[MAX_CUSTOM_EASINGS];


// ========================================================================================
//MARK: ISpriteCollideHelper
// ========================================================================================

CPP_CONSTRUCTOR_IMPL(ISpriteCollideHelper)
	if (args.Length() == 0) {
		ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper();
		return helper;
	} else if (args.Length() != 1 || !args[0]->IsFunction()) {
		SAVE_SYNTAX_ERR("SpriteCollideHelper must be created with a function argument (allowCollisionFunc)");
		return 0;
	}	
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
	ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper(callback);
	return helper;
	END


%#ifndef PDG_NO_GUI
// ========================================================================================
//MARK: ISpriteDrawHelper
// ========================================================================================

CPP_CONSTRUCTOR_IMPL(ISpriteDrawHelper)
	if (args.Length() == 0) {
		ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper();
		return helper;
	} else if (args.Length() != 1 || !args[0]->IsFunction()) {
		SAVE_SYNTAX_ERR("SpriteDrawHelper must be created with a function argument (drawFunc)");
		return 0;
	}	
	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
	ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper(callback);
	return helper;
	END

%#endif // !PDG_NO_GUI


// ========================================================================================
//MARK: Script Serializable
// ========================================================================================

ScriptSerializable::ScriptSerializable(
		v8::Local<v8::Function> javascriptGetSerializedSizeFunc,
		v8::Local<v8::Function> javascriptSerializeFunc,
		v8::Local<v8::Function> javascriptDeserializeFunc,
		v8::Local<v8::Function> javascriptGetMyClassTagFunc) 
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	mScriptGetSerializedSizeFunc.Reset(isolate, javascriptGetSerializedSizeFunc);
	mScriptSerializeFunc.Reset(isolate, javascriptSerializeFunc);
	mScriptDeserializeFunc.Reset(isolate, javascriptDeserializeFunc);
	mScriptGetMyClassTagFunc.Reset(isolate, javascriptGetMyClassTagFunc);
}

uint32 	
ScriptSerializable::getSerializedSize(ISerializer* serializer) const {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[1];
    Serializer* ser = dynamic_cast<Serializer*>(serializer);
    if (!ser) {
	  DEBUG_ONLY(
		std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
		exit(1);
	  )
    }

    argv[0] = v8::Local<v8::Object>::New(isolate, ser->mSerializerScriptObj);
    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
    if (!mScriptGetSerializedSizeFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptGetSerializedSizeFunc);
    } else {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("getSerializedSize")));
	}
    resVal = CALL_SCRIPT(func, obj_, 1, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getSerializedSize Function!!" ); )
		FatalException(try_catch);
		return 0;
    }
	if (!resVal->IsUint32()) {
	  DEBUG_ONLY(
		std::cerr << "result mismatch: return value from getSerializedSize Function must be an unsigned integer ("
                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
		exit(1);
	 )
		return 0;
	}
	return resVal->Uint32Value();
}

void 	
ScriptSerializable::serialize(ISerializer* serializer) const {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[1];
    Serializer* ser = dynamic_cast<Serializer*>(serializer);
    if (!ser) {
	  DEBUG_ONLY(
		std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
		exit(1);
	  )
    }

    argv[0] = v8::Local<v8::Object>::New(isolate, ser->mSerializerScriptObj);
    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
    if (!mScriptSerializeFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptSerializeFunc);
    } else {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("serialize")));
	}
    resVal = CALL_SCRIPT(func, obj_, 1, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling serialize Function!!" ); )
		FatalException(try_catch);
    }
}

void 	
ScriptSerializable::deserialize(IDeserializer* deserializer) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[1];
    Deserializer* deser = dynamic_cast<Deserializer*>(deserializer);
    if (!deser) {
	  DEBUG_ONLY(
		std::cerr << "Internal Error: deserialize Function called with invalid Deserializer\n";
		exit(1);
	  )
    }
    argv[0] = v8::Local<v8::Object>::New(isolate, deser->mDeserializerScriptObj);
    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
    if (!mScriptDeserializeFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptDeserializeFunc);
    } else {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("deserialize")));
	}
    resVal = CALL_SCRIPT(func, obj_, 1, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling deserialize Function!!" ); )
		FatalException(try_catch);
    }
}

uint32 	
ScriptSerializable::getMyClassTag() const {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
    if (!mScriptGetMyClassTagFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptGetMyClassTagFunc);
    } else if (obj_->Has(STR2VAL("getMyClassTag"))) {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("getMyClassTag")));
	} else {
	  DEBUG_ONLY(
		v8::String::Utf8Value objectNameStr(obj_->ToString());
		std::cerr << "fatal: ISerializable object " << *objectNameStr << " missing getMyClassTag() Function!!";
		exit(1);
	  )
		return 0;
	}
    resVal = CALL_SCRIPT(func, obj_, 0, 0);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getMyClassTag Function!!" ); )
		FatalException(try_catch);
		return 0;
    }
	if (!resVal->IsUint32()) {
	  DEBUG_ONLY(
		std::cerr << "result mismatch: return value from getMyClassTag Function must be an unsigned integer ("
                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
		exit(1);
	  )
	    return 0;
	}
	return resVal->Uint32Value();
}

// ========================================================================================
//MARK: Script Event Handler
// ========================================================================================

ScriptEventHandler::ScriptEventHandler(FUNCTION_REF func) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	mScriptHandlerFunc.Reset(isolate, func);
}

bool ScriptEventHandler::handleEvent(EventEmitter* emitter, long inEventType, void* inEventData) throw() {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
  	v8::Local<v8::Object> jsEvent = v8::Object::New(isolate);
    v8::Local<v8::Object> emitter_ = v8::Local<v8::Object>::New(isolate, emitter->mEventEmitterScriptObj);
    v8::Local<v8::Object> obj1_;
    v8::Local<v8::Object> obj2_;
  	jsEvent->Set(STR2VAL("emitter"), emitter_);
	jsEvent->Set(STR2VAL("eventType"),INT2VAL(inEventType));
	switch (inEventType) {
		case pdg::eventType_Startup:
			jsEvent->Set(STR2VAL("startupReason"),INT2VAL(static_cast<StartupInfo*>(inEventData)->startupReason));
// TODO: startup params converted to an array
//			jsEvent->Set(STR2VAL("startupParams"),INT2VAL(static_cast<StartupInfo*>(inEventData)->startupParam));
			break;
		case pdg::eventType_Shutdown:
			jsEvent->Set(STR2VAL("exitReason"),INT2VAL(static_cast<ShutdownInfo*>(inEventData)->exitReason));
			jsEvent->Set(STR2VAL("exitCode"),INT2VAL(static_cast<ShutdownInfo*>(inEventData)->exitCode));
			break;
		case pdg::eventType_Timer:
			if (static_cast<TimerInfo*>(inEventData)->id <= 0) {
				// do not send internal timer events to javascript
				return false;
			}
			jsEvent->Set(STR2VAL("id"),INT2VAL(static_cast<TimerInfo*>(inEventData)->id));
			jsEvent->Set(STR2VAL("millisec"),UINT2VAL(static_cast<TimerInfo*>(inEventData)->millisec));
			jsEvent->Set(STR2VAL("msElapsed"),UINT2VAL(static_cast<TimerInfo*>(inEventData)->msElapsed));
// TODO: user data as an object
//			jsEvent->Set(STR2VAL("userData"),INT2VAL(static_cast<TimerInfo*>(inEventData)->userData));
			break;
%#ifndef PDG_NO_GUI
		case pdg::eventType_KeyDown:
		case pdg::eventType_KeyUp:
			jsEvent->Set(STR2VAL("keyCode"),INT2VAL(static_cast<KeyInfo*>(inEventData)->keyCode));
			break;
		case pdg::eventType_KeyPress:
			jsEvent->Set(STR2VAL("shift"),BOOL2VAL(static_cast<KeyPressInfo*>(inEventData)->shift));
			jsEvent->Set(STR2VAL("ctrl"),BOOL2VAL(static_cast<KeyPressInfo*>(inEventData)->ctrl));
			jsEvent->Set(STR2VAL("alt"),BOOL2VAL(static_cast<KeyPressInfo*>(inEventData)->alt));
			jsEvent->Set(STR2VAL("meta"),BOOL2VAL(static_cast<KeyPressInfo*>(inEventData)->meta));
			jsEvent->Set(STR2VAL("unicode"),INT2VAL(static_cast<KeyPressInfo*>(inEventData)->unicode));
			jsEvent->Set(STR2VAL("isRepeating"),BOOL2VAL(static_cast<KeyPressInfo*>(inEventData)->isRepeating));
			break;
		case pdg::eventType_SpriteTouch:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteTouchInfo*>(inEventData)->touchedSprite->mSpriteScriptObj);
            obj2_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteTouchInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);
			jsEvent->Set(STR2VAL("touchType"),INT2VAL(static_cast<SpriteTouchInfo*>(inEventData)->touchType));
			jsEvent->Set(STR2VAL("touchedSprite"), obj1_);
			jsEvent->Set(STR2VAL("inLayer"), obj2_);
			// break; fall through
		case pdg::eventType_MouseDown:
		case pdg::eventType_MouseUp:
		case pdg::eventType_MouseMove:
			jsEvent->Set(STR2VAL("shift"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->shift));
			jsEvent->Set(STR2VAL("ctrl"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->ctrl));
			jsEvent->Set(STR2VAL("alt"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->alt));
			jsEvent->Set(STR2VAL("meta"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->meta));
			jsEvent->Set(STR2VAL("mousePos"),POINT2VAL(static_cast<MouseInfo*>(inEventData)->mousePos));
			jsEvent->Set(STR2VAL("leftButton"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->leftButton));
			jsEvent->Set(STR2VAL("rightButton"),BOOL2VAL(static_cast<MouseInfo*>(inEventData)->rightButton));
			jsEvent->Set(STR2VAL("buttonNumber"),UINT2VAL(static_cast<MouseInfo*>(inEventData)->buttonNumber));
			jsEvent->Set(STR2VAL("lastClickPos"),POINT2VAL(static_cast<MouseInfo*>(inEventData)->lastClickPos));
			jsEvent->Set(STR2VAL("lastClickElapsed"),UINT2VAL(static_cast<MouseInfo*>(inEventData)->lastClickElapsed));
			break;
		case pdg::eventType_ScrollWheel:
			jsEvent->Set(STR2VAL("shift"),BOOL2VAL(static_cast<ScrollWheelInfo*>(inEventData)->shift));
			jsEvent->Set(STR2VAL("ctrl"),BOOL2VAL(static_cast<ScrollWheelInfo*>(inEventData)->ctrl));
			jsEvent->Set(STR2VAL("alt"),BOOL2VAL(static_cast<ScrollWheelInfo*>(inEventData)->alt));
			jsEvent->Set(STR2VAL("meta"),BOOL2VAL(static_cast<ScrollWheelInfo*>(inEventData)->meta));
			jsEvent->Set(STR2VAL("horizDelta"),INT2VAL(static_cast<ScrollWheelInfo*>(inEventData)->horizDelta));
			jsEvent->Set(STR2VAL("vertDelta"),INT2VAL(static_cast<ScrollWheelInfo*>(inEventData)->vertDelta));
			break;
%#endif // PDG_NO_GUI
%#ifndef PDG_NO_NETWORK
		// not implemented at the moment, since Node.js will provide communications
		case pdg::eventType_NetConnect:
			return false;
			break;
		case pdg::eventType_NetDisconnect:
			return false;
			break;
		case pdg::eventType_NetData:
			return false;
			break;
		case pdg::eventType_NetError:
			return false;
			break;
%#endif // PDG_NO_NETWORK
%#ifndef PDG_NO_SOUND
		case pdg::eventType_SoundEvent:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SoundEventInfo*>(inEventData)->sound->mSoundScriptObj);
			jsEvent->Set(STR2VAL("eventCode"),INT2VAL(static_cast<SoundEventInfo*>(inEventData)->eventCode));
			jsEvent->Set(STR2VAL("sound"), obj1_);
			break;
%#endif // PDG_NO_SOUND
%#ifndef PDG_NO_GUI
		case pdg::eventType_PortResized:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<PortResizeInfo*>(inEventData)->port->mPortScriptObj);
			jsEvent->Set(STR2VAL("port"), obj1_);
			jsEvent->Set(STR2VAL("screenPos"), INT2VAL(static_cast<PortResizeInfo*>(inEventData)->screenPos));
			jsEvent->Set(STR2VAL("oldScreenPos"), INT2VAL(static_cast<PortResizeInfo*>(inEventData)->oldScreenPos));
			jsEvent->Set(STR2VAL("oldWidth"), INT2VAL(static_cast<PortResizeInfo*>(inEventData)->oldWidth));
			jsEvent->Set(STR2VAL("oldHeight"), INT2VAL(static_cast<PortResizeInfo*>(inEventData)->oldHeight));
			break;
		case pdg::eventType_PortDraw:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<PortDrawInfo*>(inEventData)->port->mPortScriptObj);
			jsEvent->Set(STR2VAL("port"), obj1_);
			jsEvent->Set(STR2VAL("frameNum"), INT2VAL(static_cast<PortDrawInfo*>(inEventData)->frameNum));
			break;
%#endif // PDG_NO_GUI
		case pdg::eventType_SpriteCollide:
		case pdg::eventType_SpriteBreak:
			if (inEventType == pdg::eventType_SpriteCollide) {
				if (static_cast<SpriteCollideInfo*>(inEventData)->targetSprite) {
                    obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteCollideInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
					jsEvent->Set(STR2VAL("targetSprite"), obj1_);
				}
				jsEvent->Set(STR2VAL("normal"), VECTOR2VAL(static_cast<SpriteCollideInfo*>(inEventData)->normal));
				jsEvent->Set(STR2VAL("impulse"), VECTOR2VAL(static_cast<SpriteCollideInfo*>(inEventData)->impulse));
				jsEvent->Set(STR2VAL("force"),NUM2VAL(static_cast<SpriteCollideInfo*>(inEventData)->force));
				jsEvent->Set(STR2VAL("kineticEnergy"),NUM2VAL(static_cast<SpriteCollideInfo*>(inEventData)->kineticEnergy));
			  %#ifdef PDG_USE_CHIPMUNK_PHYSICS
			    if (static_cast<SpriteCollideInfo*>(inEventData)->arbiter) {
					jsEvent->Set(STR2VAL("arbiter"), cpArbiterWrap::NewFromCpp(isolate, static_cast<SpriteCollideInfo*>(inEventData)->arbiter));
				}
			  %#endif
 			} else {
			  %#ifdef PDG_USE_CHIPMUNK_PHYSICS
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
				jsEvent->Set(STR2VAL("targetSprite"), obj1_);
				jsEvent->Set(STR2VAL("impulse"),NUM2VAL(static_cast<SpriteJointBreakInfo*>(inEventData)->impulse));
				jsEvent->Set(STR2VAL("force"),NUM2VAL(static_cast<SpriteJointBreakInfo*>(inEventData)->force));
				jsEvent->Set(STR2VAL("breakForce"),NUM2VAL(static_cast<SpriteJointBreakInfo*>(inEventData)->breakForce));
				jsEvent->Set(STR2VAL("joint"), cpConstraintWrap::NewFromCpp(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->joint));
			  %#endif
 			}
			// break; fall through
		case pdg::eventType_SpriteAnimate:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteAnimateInfo*>(inEventData)->actingSprite->mSpriteScriptObj);
            obj2_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteAnimateInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);
			jsEvent->Set(STR2VAL("action"),INT2VAL(static_cast<SpriteAnimateInfo*>(inEventData)->action));
			jsEvent->Set(STR2VAL("actingSprite"), obj1_);
			jsEvent->Set(STR2VAL("inLayer"), obj2_);
			break;
		case pdg::eventType_SpriteLayer:
            obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteLayerInfo*>(inEventData)->actingLayer->mSpriteLayerScriptObj);
			jsEvent->Set(STR2VAL("action"),INT2VAL(static_cast<SpriteLayerInfo*>(inEventData)->action));
			jsEvent->Set(STR2VAL("actingLayer"), obj1_);
			jsEvent->Set(STR2VAL("millisec"),UINT2VAL(static_cast<SpriteLayerInfo*>(inEventData)->millisec));
			break;
		default: {
			std::ostringstream msg;
			msg << "unknown event (" << inEventType << ")";
			SAVE_TYPE_ERR(msg.str().c_str());
			return false; // can't handle event we don't know about
			}
			break;
	
	}

    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[1];
    argv[0] = v8::Local<v8::Value>::New(isolate, jsEvent);

    DEBUG_DUMP_SCRIPT_OBJECT(this->mIEventHandlerScriptObj, IEventHandler);

    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mIEventHandlerScriptObj);
    if (!mScriptHandlerFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptHandlerFunc);
    } else if (obj_->Has(STR2VAL("handleEvent"))) {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("handleEvent")));
	} else {
	  DEBUG_ONLY(
		v8::String::Utf8Value objectNameStr(obj_->ToString());
		std::cerr << "fatal: IEventHandler object " << *objectNameStr << " missing handleEvent() Function!!\n";
		exit(1);
	  )
		return false;
	}
    resVal = CALL_SCRIPT(func, obj_, 1, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling event Handler!!" ); )
		FatalException(try_catch);
		return false;
    }
	if (!resVal->IsBoolean()) {
	  %#ifdef DEBUG
		std::cerr << "result mismatch: return value from event handler Function must be a boolean ("
                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
		exit(1);
	  %#else
	    return false;
	  %#endif
	}
	return resVal->BooleanValue();
}

// ========================================================================================
//MARK: Script Animation Helper
// ========================================================================================

ScriptAnimationHelper::ScriptAnimationHelper(FUNCTION_REF func) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	mScriptAnimateFunc.Reset(isolate, func);
}

bool ScriptAnimationHelper::animate(Animated* what, ms_delta msElapsed) throw() {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[2];
    argv[0] = v8::Local<v8::Object>::New(isolate, what->mAnimatedScriptObj);
    argv[1] = v8::Local<v8::Value>::New(isolate, UINT2VAL(msElapsed));

    DEBUG_DUMP_SCRIPT_OBJECT(what->mAnimatedScriptObj, Animated);
    DEBUG_DUMP_SCRIPT_OBJECT(this->mIAnimationHelperScriptObj, IAnimationHelper);

    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mIAnimationHelperScriptObj);
    if (!mScriptAnimateFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptAnimateFunc);
    } else if (obj_->Has(STR2VAL("animate"))) {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("animate")));
	} else {
	  DEBUG_ONLY(
		v8::String::Utf8Value objectNameStr(obj_->ToString());
		std::cerr << "fatal: IAnimationHelper object " << *objectNameStr << " missing animate() Function!!";
		exit(1);
	  )
		return false;
	}
    resVal = CALL_SCRIPT(func, obj_, 2, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Animation Helper!!" ); )
		FatalException(try_catch);
		return false;
    }
	if (!resVal->IsBoolean()) {
	  %#ifdef DEBUG
		std::cerr << "result mismatch: return value from animate helper Function must be a boolean ("
                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
		exit(1);
	  %#else
	    return false;
	  %#endif
	}
	return resVal->BooleanValue();
}

// ========================================================================================
//MARK: Script Sprite Collide Helper
// ========================================================================================

ScriptSpriteCollideHelper::ScriptSpriteCollideHelper(FUNCTION_REF func) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	mScriptAllowCollisionFunc.Reset(isolate, func);
}

bool ScriptSpriteCollideHelper::allowCollision(Sprite* sprite, Sprite* withSprite) throw() {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[2];
    argv[0] = v8::Local<v8::Object>::New(isolate, sprite->mSpriteScriptObj);
    argv[1] = v8::Local<v8::Object>::New(isolate, withSprite->mSpriteScriptObj);

    DEBUG_DUMP_SCRIPT_OBJECT(sprite->mSpriteScriptObj, Sprite);
    DEBUG_DUMP_SCRIPT_OBJECT(withSprite->mSpriteScriptObj, Sprite);
    DEBUG_DUMP_SCRIPT_OBJECT(this->mISpriteCollideHelperScriptObj, ISpriteCollideHelper);

    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mISpriteCollideHelperScriptObj);
    if (!mScriptAllowCollisionFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptAllowCollisionFunc);
    } else if (obj_->Has(STR2VAL("allowCollision"))) {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("allowCollision")));
	} else {
	  DEBUG_ONLY(
		v8::String::Utf8Value objectNameStr(obj_->ToString());
		std::cerr << "fatal: ISpriteCollideHelper object " << *objectNameStr << " missing allowCollision() Function!!";
		exit(1);
	  )
		return false;
	}
    resVal = CALL_SCRIPT(func, obj_, 2, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Sprite Collide Helper!!" ); )
		FatalException(try_catch);
		return false;
    }
// 	if (!resVal->IsBoolean()) {
// 	  %#ifdef DEBUG
// 		std::cerr << "result mismatch: return value from sprite collide helper Function must be a boolean ("
//                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
// 		exit(1);
// 	  %#else
// 	    return false;
// 	  %#endif
// 	}
	return resVal->BooleanValue();
}


%#ifndef PDG_NO_GUI
// ========================================================================================
//MARK: Script Sprite Draw Helper
// ========================================================================================

ScriptSpriteDrawHelper::ScriptSpriteDrawHelper(FUNCTION_REF func) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	mScriptDrawFunc.Reset(isolate, func);
}

bool ScriptSpriteDrawHelper::draw(Sprite* sprite, Port* port) throw() {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[2];
    argv[0] = v8::Local<v8::Object>::New(isolate, sprite->mSpriteScriptObj);
    argv[1] = v8::Local<v8::Object>::New(isolate, port->mPortScriptObj);

    DEBUG_DUMP_SCRIPT_OBJECT(sprite->mSpriteScriptObj, Sprite);
    DEBUG_DUMP_SCRIPT_OBJECT(port->mPortScriptObj, Port);
    DEBUG_DUMP_SCRIPT_OBJECT(this->mISpriteDrawHelperScriptObj, ISpriteDrawHelper);

    v8::Local<v8::Value> resVal;
    v8::Local<v8::Function> func;
    v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mISpriteDrawHelperScriptObj);
    if (!mScriptDrawFunc.IsEmpty()) {
		func = v8::Local<v8::Function>::New(isolate, mScriptDrawFunc);
    } else if (obj_->Has(STR2VAL("draw"))) {
    	func = v8::Local<v8::Function>::Cast(obj_->Get(STR2VAL("draw")));
	} else {
	  DEBUG_ONLY(
		v8::String::Utf8Value objectNameStr(obj_->ToString());
		std::cerr << "fatal: IDrawSpriteHelper object " << *objectNameStr << " missing draw() Function!!";
		exit(1);
	  )
		return false;
	}
    resVal = CALL_SCRIPT(func, obj_, 2, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Sprite Draw Helper!!" ); )
		FatalException(try_catch);
		return false;
    }
// 	if (!resVal->IsBoolean()) {
// 	  %#ifdef DEBUG
// 		std::cerr << "result mismatch: return value from sprite draw helper Function must be a boolean ("
//                << FUNCTION_GET_NAME(func) << " at " << FUNCTION_GET_FILE_AND_LINE(func) << ")\n";
// 		exit(1);
// 	  %#else
// 	    return false;
// 	  %#endif
// 	}
	return resVal->BooleanValue();
}
%#endif // !PDG_NO_GUI

// ========================================================================================
//MARK: Utility Functions
// ========================================================================================


v8::Local<v8::Value> EncodeBinary(const void *buf, size_t len) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::EscapableHandleScope scope(isolate);
	const uint8 *cbuf = static_cast<const uint8*>(buf);
	uint16* twobytebuf = new uint16[len];
	for (size_t i = 0; i < len; i++) {
	  twobytebuf[i] = cbuf[i];
	}
	v8::Local<v8::String> chunk = v8::String::NewFromTwoByte(isolate, twobytebuf, v8::String::kNormalString, len);
	delete [] twobytebuf;
	return scope.Escape(chunk);
}


// Returns number of bytes written. 
// call free on the pointer returned when you are done with it
void* DecodeBinary(v8::Local<v8::Value> val, size_t* outLen) {
	v8::Local<v8::String> str = val->ToString();
	size_t buflen = str->Length();
	if (outLen) {
		*outLen = buflen;
	}

	uint16_t * twobytebuf = new uint16_t[buflen];
	str->Write(twobytebuf, 0, buflen);

	char* buf = (char*)std::malloc(buflen);
	for (size_t i = 0; i < buflen; i++) {
		unsigned char* bp = reinterpret_cast<unsigned char*>(&twobytebuf[i]);
		buf[i] = *bp;
	}
	delete [] twobytebuf;
	return buf;
}



// =========================  easing functions =============================

float CallScriptEasingFunc(int which, ms_delta ut, float b, float c, ms_delta ud) {
	if (which > gNumCustomEasings) {
	  %#ifdef DEBUG
		std::cerr << "logic error: attempting to call an unregistered easing function #"
		    << which << "(only "<< gNumCustomEasings <<" custom easings have been"
		    " registered via registerEasingFunction())\n";
		exit(1);
	  %#else
	  	return 0.0f; // don't do anything in release builds
	  %#endif
	}
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::TryCatch try_catch(isolate);

    v8::Local<v8::Value> argv[4];
    argv[0] = v8::Local<v8::Value>::New(isolate, UINT2VAL(ut));
    argv[1] = v8::Local<v8::Value>::New(isolate, NUM2VAL(b));
    argv[2] = v8::Local<v8::Value>::New(isolate, NUM2VAL(c));
    argv[3] = v8::Local<v8::Value>::New(isolate, UINT2VAL(ud));
    v8::Local<v8::Function> easingfunc_ = v8::Local<v8::Function>::New(isolate, s_CustomScriptEasing[which]);


    v8::Local<v8::Value> resVal = CALL_SCRIPT(easingfunc_, isolate->GetCurrentContext()->Global(), 4, argv);

    if (try_catch.HasCaught()) {
		DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Easing Function!!" ); )
		FatalException(try_catch);
		return 0.0f;
    }
	if (!resVal->IsNumber()) {
		// TODO: call FatalError
	  %#ifdef DEBUG
		std::cerr << "result mismatch: return value from easing Function must be a Number ("
                << FUNCTION_GET_NAME(easingfunc_) << " at " 
                << FUNCTION_GET_FILE_AND_LINE(easingfunc_) << ")\n";
		exit(1);
	  %#else
	    return 0.0f;
	  %#endif
	}
	return resVal->NumberValue();	
}

FUNCTION_IMPL(RegisterEasingFunction)
	METHOD_SIGNATURE("", undefined, 1, (function easingFunc)); 
    REQUIRE_ARG_COUNT(1);
	REQUIRE_FUNCTION_ARG(1, easingFunc);
    v8::Local<v8::Function> jsEasingFunc = v8::Local<v8::Function>::New(isolate, easingFunc);
    if (gNumCustomEasings >= MAX_CUSTOM_EASINGS) {
    	THROW_ERR("Can't register any more custom easing functions!!");
    } else {
    	s_CustomScriptEasing[gNumCustomEasings].Reset(isolate, jsEasingFunc);
		v8::String::Utf8Value funcNameStr(easingFunc->GetName()->ToString());
		int funcId = NUM_BUILTIN_EASINGS + gNumCustomEasings;
		CallScriptEasingFunc(gNumCustomEasings, 0, 0.0f, 0.0f, 1); // test calling the function
     	gNumCustomEasings++;
     	v8::Local<v8::Object> bind_ = v8::Local<v8::Object>::New(isolate, s_BindingTarget);
     	bind_->Set(SYMBOL(*funcNameStr), INT2VAL(funcId));
		DEBUG_ONLY( OS::_DOUT( "Registered custom easing Function %d as constant name %s [%d]",
				gNumCustomEasings, *funcNameStr, funcId); )
    }
	NO_RETURN;
END


FUNCTION_IMPL(FinishedScriptSetup)
	scriptSetupCompleted();  // let the application do anything further it needs to
	NO_RETURN;
END


SCRIPT_DEBUG_ONLY(
static size_t sLastHeapUsed = 0;
static long sIdleLastHeapReport = OS::getMilliseconds();
)

void initBindings(v8::Local<v8::Object> target);

void initBindings(v8::Local<v8::Object> target) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    
	// register all our customEasing functions with pdg C++
	easingFuncToId(customEasing0);
	easingFuncToId(customEasing1);
	easingFuncToId(customEasing2);
	easingFuncToId(customEasing3);
	easingFuncToId(customEasing4);
	easingFuncToId(customEasing5);
	easingFuncToId(customEasing6);
	easingFuncToId(customEasing7);
	easingFuncToId(customEasing8);
	easingFuncToId(customEasing9);

    // save our binding target for runtime bindings, such as easing functions
    s_BindingTarget.Reset(isolate, target);

	INIT_CLASS(MemBlock);
	INIT_CLASS(FileManager);
	INIT_CLASS(LogManager);
	INIT_CLASS(ConfigManager);
	INIT_CLASS(ResourceManager);
	INIT_CLASS(Serializer);
	INIT_CLASS(Deserializer);
	INIT_CLASS(ISerializable);
	INIT_CLASS(IEventHandler);
	INIT_CLASS(EventEmitter);
	INIT_CLASS(EventManager);
	INIT_CLASS(TimerManager);
	INIT_CLASS(IAnimationHelper);
	INIT_CLASS(Animated);
  %#ifdef PDG_USE_CHIPMUNK_PHYSICS
	INIT_CLASS(cpArbiter);
	INIT_CLASS(cpConstraint);
	INIT_CLASS(cpSpace);
  %#endif // PDG_USE_CHIPMUNK_PHYSICS
  %#ifndef PDG_NO_GUI
	INIT_CLASS(ISpriteDrawHelper);
  %#endif // !PDG_NO_GUI
	INIT_CLASS(ISpriteCollideHelper);
	INIT_CLASS(Sprite);
	INIT_CLASS(SpriteLayer);
	INIT_CLASS(TileLayer);
	INIT_CLASS(Image);
	INIT_CLASS(ImageStrip);
  %#ifndef PDG_NO_GUI
	INIT_CLASS(Font);
	INIT_CLASS(Port);
	INIT_CLASS(GraphicsManager);
  %#endif
  %#ifndef PDG_NO_SOUND
	INIT_CLASS(Sound);
	INIT_CLASS(SoundManager);
  %#endif

	// methods of this module
	INIT_FUNCTION("_idle", Idle);
	INIT_FUNCTION("_run", Run);
	INIT_FUNCTION("_quit", Quit);
	INIT_FUNCTION("_isQuitting", IsQuitting);
	INIT_FUNCTION("_finishedScriptSetup", FinishedScriptSetup);

//    INIT_FUNCTION("rand", Rand);
//    INIT_FUNCTION("gameCriticalRandom", GameCriticalRandom);
    INIT_FUNCTION("rand", GameCriticalRandom);
    INIT_FUNCTION("srand", Srand);
    
	INIT_FUNCTION("setSerializationDebugMode", SetSerializationDebugMode);

    INIT_FUNCTION("registerEasingFunction", RegisterEasingFunction);
    
    INIT_FUNCTION("getFileManager", GetFileManager);
    INIT_FUNCTION("getLogManager", GetLogManager);
    INIT_FUNCTION("getConfigManager", GetConfigManager);
    INIT_FUNCTION("getResourceManager", GetResourceManager);
    INIT_FUNCTION("getEventManager", GetEventManager);
    INIT_FUNCTION("getTimerManager", GetTimerManager);
    INIT_FUNCTION("registerSerializableClass", RegisterSerializableClass);
    
  %#ifndef PDG_NO_GUI
	INIT_FUNCTION("getGraphicsManager", GetGraphicsManager);
  %#endif
  %#ifndef PDG_NO_SOUND
    INIT_FUNCTION("getSoundManager", GetSoundManager);
  %#endif

	INIT_FUNCTION("createSpriteLayer", CreateSpriteLayer);
%#ifdef PDG_SCML_SUPPORT
	INIT_FUNCTION("createSpriteLayerFromSCMLFile", CreateSpriteLayerFromSCMLFile);
%#endif
	INIT_FUNCTION("cleanupSpriteLayer", CleanupSpriteLayer);
	INIT_FUNCTION("createTileLayer", CreateTileLayer);

	// Constants
    INIT_CONSTANT("all_events", all_events);
//	INIT_CONSTANT("eventType_Startup", eventType_Startup);
	INIT_CONSTANT("eventType_Shutdown", eventType_Shutdown);
	INIT_CONSTANT("eventType_Timer", eventType_Timer);
	INIT_CONSTANT("eventType_KeyDown", eventType_KeyDown);
	INIT_CONSTANT("eventType_KeyUp", eventType_KeyUp);
	INIT_CONSTANT("eventType_KeyPress", eventType_KeyPress);
	INIT_CONSTANT("eventType_MouseDown", eventType_MouseDown);
	INIT_CONSTANT("eventType_MouseUp", eventType_MouseUp);
	INIT_CONSTANT("eventType_MouseMove", eventType_MouseMove);
	INIT_CONSTANT("eventType_MouseEnter", eventType_MouseEnter);
	INIT_CONSTANT("eventType_MouseLeave", eventType_MouseLeave);
	INIT_CONSTANT("eventType_PortResized", eventType_PortResized);
	INIT_CONSTANT("eventType_ScrollWheel", eventType_ScrollWheel);
	INIT_CONSTANT("eventType_SpriteTouch", eventType_SpriteTouch);
	INIT_CONSTANT("eventType_SpriteAnimate", eventType_SpriteAnimate);
	INIT_CONSTANT("eventType_SpriteLayer", eventType_SpriteLayer);
	INIT_CONSTANT("eventType_SpriteCollide", eventType_SpriteCollide);
	INIT_CONSTANT("eventType_SpriteBreak", eventType_SpriteBreak);
	INIT_CONSTANT("eventType_SoundEvent", eventType_SoundEvent);
	INIT_CONSTANT("eventType_PortDraw", eventType_PortDraw);

	INIT_CONSTANT("soundEvent_DonePlaying", soundEvent_DonePlaying);
	INIT_CONSTANT("soundEvent_Looping", soundEvent_Looping);
	INIT_CONSTANT("soundEvent_FailedToPlay", soundEvent_FailedToPlay);

    INIT_CONSTANT("key_Break", key_Break); 
	INIT_CONSTANT("key_Home", key_Home);
	INIT_CONSTANT("key_End", key_End);
    INIT_CONSTANT("key_Clear", key_Clear);
    INIT_CONSTANT("key_Help", key_Help);
    INIT_CONSTANT("key_Pause", key_Pause);
    INIT_CONSTANT("key_Mute", key_Mute);
    INIT_CONSTANT("key_Backspace", key_Backspace);
    INIT_CONSTANT("key_Delete", key_Delete);
    INIT_CONSTANT("key_Tab", key_Tab);
    INIT_CONSTANT("key_PageUp", key_PageUp);
    INIT_CONSTANT("key_PageDown", key_PageDown);
    INIT_CONSTANT("key_Return", key_Return);
    INIT_CONSTANT("key_Enter", key_Enter);
    INIT_CONSTANT("key_F1", key_F1);
    INIT_CONSTANT("key_F2", key_F2);
    INIT_CONSTANT("key_F3", key_F3);
    INIT_CONSTANT("key_F4", key_F4);
    INIT_CONSTANT("key_F5", key_F5);
    INIT_CONSTANT("key_F6", key_F6);
    INIT_CONSTANT("key_F7", key_F7);
    INIT_CONSTANT("key_F8", key_F8);
    INIT_CONSTANT("key_F9", key_F9);
    INIT_CONSTANT("key_F10", key_F10);
    INIT_CONSTANT("key_F11", key_F11);
    INIT_CONSTANT("key_F12", key_F12);
    INIT_CONSTANT("key_FirstF", key_FirstF);
    INIT_CONSTANT("key_LastF", key_LastF);
    INIT_CONSTANT("key_Insert", key_Insert);
    INIT_CONSTANT("key_Escape", key_Escape);
    INIT_CONSTANT("key_LeftArrow", key_LeftArrow);
    INIT_CONSTANT("key_RightArrow", key_RightArrow);
    INIT_CONSTANT("key_UpArrow", key_UpArrow);
    INIT_CONSTANT("key_DownArrow", key_DownArrow);
    INIT_CONSTANT("key_FirstPrintable", key_FirstPrintable);
    
    INIT_CONSTANT("screenPos_Normal", screenPos_Normal);
    INIT_CONSTANT("screenPos_Rotated180", screenPos_Rotated180);
    INIT_CONSTANT("screenPos_Rotated90Clockwise", screenPos_Rotated90Clockwise);
    INIT_CONSTANT("screenPos_Rotated90CounterClockwise", screenPos_Rotated90CounterClockwise);
    INIT_CONSTANT("screenPos_FaceUp", screenPos_FaceUp);
    INIT_CONSTANT("screenPos_FaceDown", screenPos_FaceDown);
    
  %#ifndef PDG_NO_GUI
    INIT_CONSTANT("textStyle_Plain", textStyle_Plain);
	INIT_CONSTANT("textStyle_Bold", textStyle_Bold);
	INIT_CONSTANT("textStyle_Italic", textStyle_Italic);
	INIT_CONSTANT("textStyle_Underline", textStyle_Underline);
	INIT_CONSTANT("textStyle_Centered", textStyle_Centered);
	INIT_CONSTANT("textStyle_LeftJustified", textStyle_LeftJustified);
	INIT_CONSTANT("textStyle_RightJustified", textStyle_RightJustified);
  %#endif

	INIT_CONSTANT("fit_None", fit_None);
    INIT_CONSTANT("fit_Height", fit_Height);
    INIT_CONSTANT("fit_Width", fit_Width);
    INIT_CONSTANT("fit_Inside", fit_Inside);
    INIT_CONSTANT("fit_Fill", fit_Fill);
    INIT_CONSTANT("fit_FillKeepProportions", fit_FillKeepProportions);

    INIT_CONSTANT("init_CreateUniqueNewFile", LogManager::init_CreateUniqueNewFile);
    INIT_CONSTANT("init_OverwriteExisting", LogManager::init_OverwriteExisting);
    INIT_CONSTANT("init_AppendToExisting", LogManager::init_AppendToExisting);
    INIT_CONSTANT("init_StdOut", LogManager::init_StdOut);
    INIT_CONSTANT("init_StdErr", LogManager::init_StdErr);

	INIT_CONSTANT("duration_Constant", duration_Constant);
	INIT_CONSTANT("duration_Instantaneous", duration_Instantaneous);
		
	INIT_CONSTANT("animate_StartToEnd", Sprite::animate_StartToEnd);
	INIT_CONSTANT("animate_EndToStart", Sprite::animate_EndToStart);
	INIT_CONSTANT("animate_Unidirectional", Sprite::animate_Unidirectional);
	INIT_CONSTANT("animate_Bidirectional", Sprite::animate_Bidirectional);
	INIT_CONSTANT("animate_NoLooping", Sprite::animate_NoLooping);
	INIT_CONSTANT("animate_Looping", Sprite::animate_Looping);
		
	INIT_CONSTANT("start_FromFirstFrame", Sprite::start_FromFirstFrame);
	INIT_CONSTANT("start_FromLastFrame", Sprite::start_FromLastFrame);
		
	INIT_CONSTANT("all_Frames", Sprite::all_Frames);

	INIT_CONSTANT("action_CollideSprite", Sprite::action_CollideSprite);
	INIT_CONSTANT("action_CollideWall", Sprite::action_CollideWall);
	INIT_CONSTANT("action_Offscreen", Sprite::action_Offscreen);
	INIT_CONSTANT("action_Onscreen", Sprite::action_Onscreen);
	INIT_CONSTANT("action_ExitLayer", Sprite::action_ExitLayer);
	INIT_CONSTANT("action_AnimationLoop", Sprite::action_AnimationLoop);
	INIT_CONSTANT("action_AnimationEnd", Sprite::action_AnimationEnd);
	INIT_CONSTANT("action_FadeComplete", Sprite::action_FadeComplete);
	INIT_CONSTANT("action_FadeInComplete", Sprite::action_FadeInComplete);
	INIT_CONSTANT("action_FadeOutComplete", Sprite::action_FadeOutComplete);
	INIT_CONSTANT("action_JointBreak", Sprite::action_JointBreak);
	
	INIT_CONSTANT("touch_MouseEnter", Sprite::touch_MouseEnter);
	INIT_CONSTANT("touch_MouseLeave", Sprite::touch_MouseLeave);
	INIT_CONSTANT("touch_MouseDown", Sprite::touch_MouseDown);
	INIT_CONSTANT("touch_MouseUp", Sprite::touch_MouseUp);
	INIT_CONSTANT("touch_MouseClick", Sprite::touch_MouseClick);

	INIT_CONSTANT("collide_None", Sprite::collide_None);
	INIT_CONSTANT("collide_Point", Sprite::collide_Point);
	INIT_CONSTANT("collide_BoundingBox", Sprite::collide_BoundingBox);
	INIT_CONSTANT("collide_CollisionRadius", Sprite::collide_CollisionRadius);
	INIT_CONSTANT("collide_AlphaChannel", Sprite::collide_AlphaChannel);
	INIT_CONSTANT("collide_Last", Sprite::collide_Last);

	INIT_CONSTANT("action_ErasePort", SpriteLayer::action_ErasePort);
	INIT_CONSTANT("action_PreDrawLayer", SpriteLayer::action_PreDrawLayer);
	INIT_CONSTANT("action_PostDrawLayer", SpriteLayer::action_PostDrawLayer);
	INIT_CONSTANT("action_DrawPortComplete", SpriteLayer::action_DrawPortComplete);
	INIT_CONSTANT("action_AnimationStart", SpriteLayer::action_AnimationStart);
	INIT_CONSTANT("action_PreAnimateLayer", SpriteLayer::action_PreAnimateLayer);
	INIT_CONSTANT("action_PostAnimateLayer", SpriteLayer::action_PostAnimateLayer);
	INIT_CONSTANT("action_AnimationComplete", SpriteLayer::action_AnimationComplete);
	INIT_CONSTANT("action_ZoomComplete", SpriteLayer::action_ZoomComplete);
	INIT_CONSTANT("action_LayerFadeInComplete", SpriteLayer::action_FadeInComplete);
	INIT_CONSTANT("action_LayerFadeOutComplete", SpriteLayer::action_FadeOutComplete);

	INIT_CONSTANT("facing_North", TileLayer::facing_North);
	INIT_CONSTANT("facing_East", TileLayer::facing_East);
	INIT_CONSTANT("facing_South", TileLayer::facing_South);
	INIT_CONSTANT("facing_West", TileLayer::facing_West);
	INIT_CONSTANT("facing_Ignore", TileLayer::facing_Ignore);
	INIT_CONSTANT("flipped_None", TileLayer::flipped_None);
	INIT_CONSTANT("flipped_Horizontal", TileLayer::flipped_Horizontal);
	INIT_CONSTANT("flipped_Vertical", TileLayer::flipped_Vertical);
	INIT_CONSTANT("flipped_Both", TileLayer::flipped_Both);
	INIT_CONSTANT("flipped_Ignore", TileLayer::flipped_Ignore);

	INIT_BOOL_CONSTANT("timer_OneShot", timer_OneShot);
	INIT_BOOL_CONSTANT("timer_Repeating", timer_Repeating);
	INIT_CONSTANT("timer_Never", timer_Never);

	// setup easing list, leave out custom easings since
	// we will register those constants when the easing function is
	// defined
	INIT_CONSTANT("linearTween", EasingFuncRef::linearTween);
	INIT_CONSTANT("easeInQuad", EasingFuncRef::easeInQuad);
	INIT_CONSTANT("easeOutQuad", EasingFuncRef::easeOutQuad);
	INIT_CONSTANT("easeInOutQuad", EasingFuncRef::easeInOutQuad);
	INIT_CONSTANT("easeInCubic", EasingFuncRef::easeInCubic);
	INIT_CONSTANT("easeOutCubic", EasingFuncRef::easeOutCubic);
	INIT_CONSTANT("easeInOutCubic", EasingFuncRef::easeInOutCubic);
	INIT_CONSTANT("easeInQuart", EasingFuncRef::easeInQuart);
	INIT_CONSTANT("easeOutQuart", EasingFuncRef::easeOutQuart);
	INIT_CONSTANT("easeInOutCubic", EasingFuncRef::easeInOutCubic);
	INIT_CONSTANT("easeInQuint", EasingFuncRef::easeInQuint);
	INIT_CONSTANT("easeOutQuint", EasingFuncRef::easeOutQuint);
	INIT_CONSTANT("easeInOutQuint", EasingFuncRef::easeInOutQuint);
	INIT_CONSTANT("easeInSine", EasingFuncRef::easeInSine);
	INIT_CONSTANT("easeOutSine", EasingFuncRef::easeOutSine);
	INIT_CONSTANT("easeInOutSine", EasingFuncRef::easeInOutSine);
	INIT_CONSTANT("easeInExpo", EasingFuncRef::easeInExpo);
	INIT_CONSTANT("easeOutExpo", EasingFuncRef::easeOutExpo);
	INIT_CONSTANT("easeInOutExpo", EasingFuncRef::easeInOutExpo);
	INIT_CONSTANT("easeInCirc", EasingFuncRef::easeInCirc);
	INIT_CONSTANT("easeOutCirc", EasingFuncRef::easeOutCirc);
	INIT_CONSTANT("easeInOutCirc", EasingFuncRef::easeInOutCirc);
	INIT_CONSTANT("easeInBounce", EasingFuncRef::easeInBounce);
	INIT_CONSTANT("easeOutBounce", EasingFuncRef::easeOutBounce);
	INIT_CONSTANT("easeInOutBounce", EasingFuncRef::easeInOutBounce);
	INIT_CONSTANT("easeInBack", EasingFuncRef::easeInBack);
	INIT_CONSTANT("easeOutBack", EasingFuncRef::easeOutBack);
	INIT_CONSTANT("easeInOutBack", EasingFuncRef::easeInOutBack);

	INIT_CONSTANT("ser_Positions", ser_Positions);
	INIT_CONSTANT("ser_ZOrder", ser_ZOrder);
	INIT_CONSTANT("ser_Sizes", ser_Sizes);
	INIT_CONSTANT("ser_Animations", ser_Animations);
	INIT_CONSTANT("ser_Motion", ser_Motion);
	INIT_CONSTANT("ser_Forces", ser_Forces);
	INIT_CONSTANT("ser_Physics", ser_Physics);
	INIT_CONSTANT("ser_LayerDraw", ser_LayerDraw);
	INIT_CONSTANT("ser_ImageRefs", ser_ImageRefs);
	INIT_CONSTANT("ser_SCMLRefs", ser_SCMLRefs);
	INIT_CONSTANT("ser_HelperRefs", ser_HelperRefs);
	INIT_CONSTANT("ser_HelperObjs", ser_HelperObjs);
	INIT_CONSTANT("ser_InitialData", ser_InitialData);
	INIT_CONSTANT("ser_Micro", ser_Micro);
	INIT_CONSTANT("ser_Update", ser_Update);
	INIT_CONSTANT("ser_Full", ser_Full);
}


void CreateSingletons() {
 	// this creation order matches the behavior of the native pdg framework app
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	FileManagerWrap::GetScriptSingletonInstance(isolate);  // there is now C++ singleton, only JavaScript

	LogManagerWrap::getSingletonInstance();
	ConfigManagerWrap::getSingletonInstance();
	ResourceManagerWrap::getSingletonInstance();
	EventManagerWrap::getSingletonInstance();
	TimerManagerWrap::getSingletonInstance();
  %#ifndef PDG_NO_GUI
	GraphicsManagerWrap::getSingletonInstance();
  %#endif
  %#ifndef PDG_NO_SOUND
	SoundManagerWrap::getSingletonInstance();
  %#endif
  %#ifndef PDG_NO_NETWORK
	NetworkManager::getSingletonInstance();
  %#endif
}

} // end pdg namespace

// this is called from pdg::main_run()
extern "C" void pdg_LibContainerDoIdle() {
	
	// give V8 some time to garbage collect
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	isolate->IdleNotificationDeadline(0.01f);  // only give it 1/100th of a second

	// report heap
	SCRIPT_DEBUG_ONLY(
	if ((sIdleLastHeapReport + 1000) < OS::getMilliseconds()) {
		v8::HeapStatistics hs;
		v8::V8::GetHeapStatistics(&hs);
		long deltaUsed = hs.used_heap_size() - sLastHeapUsed;
		sLastHeapUsed = hs.used_heap_size();
		if (deltaUsed != 0) {
			std::cout << "heap: delta ["<<deltaUsed<<"] used ["<<sLastHeapUsed<<"] total ["<<hs.total_heap_size()
				<<"] executable ["<<hs.total_heap_size_executable()
				<<"]  limit ["<<hs.heap_size_limit()<<"]\n";
			sIdleLastHeapReport = OS::getMilliseconds();
		}
	}
	)
}

