// -----------------------------------------------
// pdg_script_impl.h
//
// utility functions and binding helper classes 
// that are used for implementation of script bindings 
// without any specific language dependencies
//
// This file is parsed by tools/gen-script_interface.sh
// to generate an interface file
//
// This should only be done when the wrappers need 
// to change.
//
// Written by Ed Zavada, 2012-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
// All Rights Reserved Worldwide
// -----------------------------------------------

#ifdef PDG_GENERATING_DOCS
#include "../../tools/js_docs_macros.h"
namespace pdg {
#else

#include "pdg_script_macros.h"

%#ifndef PDG_SCRIPT_IMPL_H_INCLUDED
%#define PDG_SCRIPT_IMPL_H_INCLUDED

%#include "pdg_project.h"

%#include "pdg_script.h"

%#ifndef PDG_NO_APP_FRAMEWORK
%#define PDG_NO_APP_FRAMEWORK
%#endif
%#include "pdg/framework.h"

%#include <cstdlib>
%#include <sstream>

namespace pdg {

// ========================================================================================
//MARK: Local Declarations
// ========================================================================================
extern bool s_HaveSavedError;
SAVED_ERROR_STORAGE;  // scripting language specific storage of a saved error

VALUE EncodeBinary(const void *buf, size_t len);
void* DecodeBinary(VALUE_REF val, size_t* outLen = 0);

const bool kNoErrorOnFail = true;

// ========================================================================================
//MARK: Utility functions
// ========================================================================================

// create an instance of all of the singletons
void CreateSingletons();

// initializes the bindings for the pdg module
//void initBindings(JSContextRef ctx, JSObjectRef exports);


// ========================================================================================
// Script Helper classes
// ========================================================================================


class ScriptAnimationHelper : public pdg::IAnimationHelper {
public:
	ScriptAnimationHelper();
	ScriptAnimationHelper(FUNCTION_REF javascriptAnimateFunc);
    bool animate(Animated* what, ms_delta msElapsed) throw();
protected:
	SAVED_FUNCTION mScriptAnimateFunc;
};

class ScriptEventHandler : public pdg::RefCountedImpl< pdg::IEventHandler > {
public:
	ScriptEventHandler();
	ScriptEventHandler(FUNCTION_REF javascriptHandlerFunc);
    bool handleEvent(EventEmitter* emitter, long inEventType, void* inEventData) throw();
protected:
	SAVED_FUNCTION mScriptHandlerFunc;
};

class ScriptSerializable : public pdg::ISerializable {
public:
	ScriptSerializable();
	ScriptSerializable(
		FUNCTION_REF javascriptGetSerializedSizeFunc,
		FUNCTION_REF javascriptSerializeFunc,
		FUNCTION_REF javascriptDeserializeFunc,
		FUNCTION_REF javascriptGetMyClassTagFunc
	);
	virtual uint32 	getSerializedSize(ISerializer* serializer) const;
 	virtual void 	serialize(ISerializer* serializer) const;
 	virtual void 	deserialize(IDeserializer* deserializer);
 	virtual uint32 	getMyClassTag() const;
protected:
	SAVED_FUNCTION mScriptGetSerializedSizeFunc;
	SAVED_FUNCTION mScriptSerializeFunc;
	SAVED_FUNCTION mScriptDeserializeFunc;
	SAVED_FUNCTION mScriptGetMyClassTagFunc;
};

class ScriptSpriteCollideHelper : public pdg::ISpriteCollideHelper {
public:
	ScriptSpriteCollideHelper();
	ScriptSpriteCollideHelper(FUNCTION_REF javascriptDrawFunc);
    bool allowCollision(Sprite* sprite, Sprite* withSprite) throw();
protected:
	SAVED_FUNCTION mScriptAllowCollisionFunc;
};

%#ifndef PDG_NO_GUI

class ScriptSpriteDrawHelper : public pdg::ISpriteDrawHelper {
public:
	ScriptSpriteDrawHelper();
	ScriptSpriteDrawHelper(FUNCTION_REF javascriptDrawFunc);
    bool draw(Sprite* sprite, Port* port) throw();
protected:
	SAVED_FUNCTION mScriptDrawFunc;
};

%#endif

// ========================================================================================
//MARK: Easing Functions
// ========================================================================================

float CallScriptEasingFunc(int which, ms_delta ut, float b, float c, ms_delta ud);

#define DECL_CUSTOM_EASING(n) \
  extern float customEasing##n(ms_delta ut, float b, float c, ms_delta ud);

DECL_CUSTOM_EASING(0)
DECL_CUSTOM_EASING(1)
DECL_CUSTOM_EASING(2)
DECL_CUSTOM_EASING(3)
DECL_CUSTOM_EASING(4)
DECL_CUSTOM_EASING(5)
DECL_CUSTOM_EASING(6)
DECL_CUSTOM_EASING(7)
DECL_CUSTOM_EASING(8)
DECL_CUSTOM_EASING(9)

namespace EasingFuncRef {  // no confusion in the global namespace
	enum {
		EASING_FUNC_LIST
	};
}

} // end pdg namespace

%#endif // PDG_SCRIPT_IMPL_H_INCLUDED

#endif //!PDG_GENERATING_DOCS
