// -----------------------------------------------
// pdg_script_macros.h
// 
// Macros used to generate JavaScript bindings for 
// the V8 engine
//
// Written by Ed Zavada, 2012-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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


#ifndef PDG_SCRIPT_MACROS_H_INCLUDED
#define PDG_SCRIPT_MACROS_H_INCLUDED

#ifndef PDG_BUILDING_INTERFACE_FILES

	#define CR

#else

	#define CR @

#endif // PDG_BUILDING_INTERFACE_FILES

#define _V8_STR(s)  v8::String::NewSymbol(s)

#ifdef THIS
	#undef THIS // conflict with Windows.h headers
#endif

// ========================================================================================
//MARK: UTILITY MACROS
// ========================================================================================

// used to hold a local copy of a value, object or function
#define VALUE	 	 v8::Local<v8::Value>
#define OBJECT	 	 v8::Local<v8::Object>
#define FUNCTION	 v8::Local<v8::Function>

// used to hold a reference to a value, object, or function that can be passed around
#define VALUE_REF 	 v8::Handle<v8::Value>
#define OBJECT_REF	 v8::Handle<v8::Object>
#define FUNCTION_REF v8::Handle<v8::Function>

// used to hold a persistent copy of a value, object or function
#define SAVED_VALUE		v8::Persistent<v8::Value>
#define SAVED_OBJECT	v8::Persistent<v8::Object>
#define SAVED_FUNCTION	v8::Persistent<v8::Function>

// used to save a Local VALUE/OBJECT/FUNCTION into SAVED_VALUE/OBJECT/FUNCTION storage space
#define VALUE_SAVE(dst, val)		dst = v8::Persistent<v8::Value>::New(val)
#define OBJECT_SAVE(dst, obj)		dst = v8::Persistent<v8::Object>::New(obj)
#define FUNCTION_SAVE(dst, func)	dst = v8::Persistent<v8::Function>::New(func)

// calling conventions for C++ functions called from javascript
#define SCRIPT_ARGS	 	const v8::Arguments& args
#define ARGC			args.Length()
#define ARGV			args
#define THIS	 		args.This()
#define NULL_VAL		v8::Null()

// static symbols (strings) that are declared once and reused often
// avoids reallocation that comes with using JavaScript Strings
#define DECLARE_SYMBOL(sym)	
#define SYMBOL(sym)			v8::String::NewSymbol(#sym)

// conversion between C++ primitives and the all-encompassing JavaScript Value type
#define STR2VAL(s)			v8::String::New(s)
#define INT2VAL(n)			v8::Integer::New(n)
#define UINT2VAL(n)         v8::Integer::NewFromUnsigned(n)
#define NUM2VAL(n)          v8::Number::New(n)
#define BOOL2VAL(b)         v8::Boolean::New(b)
//#define VAL2STR    // not that simple, use VALUE_TO_CSTRING
#define VAL2INT(val)		val->Int32Value()
#define VAL2UINT(val)		val->Uint32Value()
#define VAL2NUM(val)		val->NumberValue()
#define VAL2BOOL(val)		val->BooleanValue()

// Allocates space for a C String to hold the contents of the JavaScript String Value
// and copies the JavaScript string into it, converting from the internal JavaScript
// string representation (usually UTF-16) to UTF-8 in the process. 
// When the C variable goes out of scope the memory is automatically freed, ie:
// 
//  {  
//      VALUE_TO_CSTRING(myfilename, ARGV[0]); // myfilename declared and allocated here
//		printf("%s", myfilename);              // then freed as soon as it goes out of scope
//  }  

#define VALUE_TO_CSTRING(cVar, valVar) \
	v8::String::Utf8Value cVar##_Str(valVar->ToString()); CR \
	const char* cVar = *cVar##_Str;

// conversion between JavaScript functions and objects and the all-encompassing JavaScript Value type
#define FUNC2VAL(func)		func
#define OBJ2VAL(obj)		obj
#define VAL2FUNC(val)		v8::Handle<v8::Function>::Cast(val)
#define VAL2OBJ(val)		val->ToObject()

#define TIME_T_TO_VALUE(time)	JSC_TimeToValue(ctx,time, exception)

// conversion between PDG coordinate types and Javascript Value type
#define OFFSET2VAL(o)	v8_MakeJavascriptOffset(o)
#define POINT2VAL(p)	v8_MakeJavascriptPoint(p)
#define VECTOR2VAL(v)	v8_MakeJavascriptVector(v)
#define ROTRECT2VAL(rr)	v8_MakeJavascriptRect(rr)
#define RECT2VAL(r)		v8_MakeJavascriptRect(r)
#define QUAD2VAL(q)		v8_MakeJavascriptQuad(q)
#define COLOR2VAL(c)	v8_MakeJavascriptColor(c)

#define VAL2OFFSET(val)		v8_ValueToOffset(val)
#define VAL2POINT(val)		v8_ValueToPoint(val)
#define VAL2VECTOR(val)		v8_ValueToVector(val)
#define VAL2RECT(val)		v8_ValueToRect(val)
#define VAL2ROTRECT(val)	v8_ValueToRotatedRect(val)
#define VAL2QUAD(val)		v8_ValueToQuad(val)
#define VAL2COLOR(val)		v8_ValueToColor(val)

#define VALUE_IS_OFFSET(val)	v8_ValueIsOffset(val)
#define VALUE_IS_POINT(val)		v8_ValueIsPoint(val)
#define VALUE_IS_VECTOR(val)	v8_ValueIsVector(val)
#define VALUE_IS_RECT(val)		v8_ValueIsRect(val)
#define VALUE_IS_ROTRECT(val)	v8_ValueIsRotatedRect(val)
#define VALUE_IS_QUAD(val)		v8_ValueIsQuad(val)
#define VALUE_IS_COLOR(val)		v8_ValueIsColor(val)

// JavaScript Value query functions
#define VALUE_IS_UNDEFINED(val)	val->IsUndefined()
#define VALUE_IS_NULL(val)      val->IsNull()

#define VALUE_IS_BOOL(val)		val->IsBoolean()
#define VALUE_IS_TRUE(val)		val->IsTrue()
#define VALUE_IS_FALSE(val)		!(val->IsTrue())
#define VALUE_IS_EXACTLY_TRUE(val)		(VALUE_IS_BOOL(val) && VALUE_IS_TRUE(val))
#define VALUE_IS_EXACTLY_FALSE(val)		(VALUE_IS_BOOL(val) && VALUE_IS_FALSE(val))

#define VALUE_IS_NUMBER(val)	val->IsNumber()
#define VALUE_IS_STRING(val)	val->IsString()
#define VALUE_IS_FUNCTION(val)  val->IsFunction()
 
#define VALUE_IS_OBJECT(val)	val->IsObject()

#define VALUE_IS_OBJECT_OF_CLASS(val, klass)	\
	((!val->IsObject()) ? false : 													CR \
		(jswrap::ObjectWrap::Unwrap<klass##Wrap>(val->ToObject()) != 0) ? true : 	CR \
			(val->ToObject()->GetPrototype().IsEmpty()) ? false :					CR \
				(jswrap::ObjectWrap::Unwrap<klass##Wrap>(val->ToObject()->GetPrototype()->ToObject()) != 0))

#define VALUE_IS_OBJECT_WITH_PROPERTY(val, propSymbol)  val->IsObject() ? val->ToObject()->Has(propSymbol) : false

// shortcut to get an object property directly from a value 
// without converting to an object first
#define VALUE_GET_OBJECT_PROPERTY(objVal, propSymbol) 	val->IsObject() ? val->ToObject()->Get(propSymbol) : v8::Undefined()

// JavaScript object query methods
#define OBJECT_HAS_PROPERTY(obj, propSymbol)            obj->Has(propSymbol)
#define OBJECT_GET_PROPERTY(obj, propSymbol) 			obj->Get(propSymbol)
#define OBJECT_SET_PROPERTY_VALUE(obj, propSymbol, val) obj->Set(propSymbol, val)
#define OBJECT_GET_CLASS_NAME(obj)						*(obj->GetConstructorName())

// creates an empty, anonymous JavaScript object with an optional pointer to C++ private data
// the private data is typically used to associate this with an C++ data structure
#define OBJECT_CREATE_EMPTY(privateDataPtr)		v8_ObjectCreateEmpty(privateDataPtr)

// retrieve the C++ private data pointer from a JavaScript object
#define OBJECT_PRIVATE_DATA(obj)				obj->GetPointerFromInternalField(0)

// call a JavaScript function with "this" set to obj and with the arguments specified
// argv is an array of Values. Returns whatever Value is passed back from the function
// if NULL (0, not JavaScript Null Value) is returned, an exception was thrown from the
// function and is stored in the "exception" variable
#define CALL_SCRIPT(func, obj, argc, argv)		JSObjectCallAsFunction(ctx, func, obj, argc, argv, exception)

#define FUNCTION_GET_NAME(func)					*(func->GetName()->ToString())
#define FUNCTION_GET_FILE_AND_LINE(func)		"unknown_file:0"  // TODO: implement this!!

// used for non-callback functions so they can use the macros above which often assume 
// certain variables are available in the function scope
#define SETUP_NON_SCRIPT_EXCEPTION  \
// 	JSValueRef  _exception = 0;             CR 
// 	JSValueRef* exception = &_exception

#define SETUP_NON_SCRIPT_CALL \
// 	JSContextRef ctx = gMainContext;		CR 
//	SETUP_NON_SCRIPT_EXCEPTION


// Execute arbitrary javascript, with the result put into the JavaScript Value valVar.
// Whatever you pass as obj will be the "this" pointer when the code is evaluated,
// or you can pass NULL to use the global object.
// Script is built using an output stream, so you can use << to combine static strings
// and C++ variables, ie:
//
// JSC_EVAL(jsOrientation, 0, "{roll="<<roll<<";pitch="<<pitch<<";yaw="<<yaw<<";}" );
//
#define JS_EVAL(valVar, obj, script)     \
	std::ostringstream valVar##_;                                  CR \
	valVar##_ << script;                                           CR \
//    valVar = JSEvaluateScript(ctx, JSStringCreateWithUTF8CString(  
//    	        valVar##_.str().c_str()), obj, 0, 1, exception)

// ========================================================================================
//MARK: EXCEPTION AND ERROR MACROS
// ========================================================================================

#define TRY 					v8::TryCatch try_catch;
#define CATCH_EXCEPTION(var)  	if (try_catch.HasCaught())
#define EXCEPTION_DATA	 		try_catch

#define _V8_THROW_ERR(msg, errtype)     \
	std::ostringstream excpt_;                                  CR \
	excpt_ << msg;                          					CR \
    return v8::ThrowException( v8::Exception::errtype( STR2VAL( excpt_.str().c_str() )))

#define THROW_ERR(msg) _V8_THROW_ERR(msg, Error)

#define THROW_TYPE_ERR(msg) _V8_THROW_ERR(msg, TypeError)

#define THROW_RANGE_ERR(msg) _V8_THROW_ERR(msg, RangeError)

#define THROW_REFERENCE_ERR(msg) _V8_THROW_ERR(msg, ReferenceError)

#define THROW_SYNTAX_ERR(msg) _V8_THROW_ERR(msg, SyntaxError)

#define THROW_IF_SAVED_ERR    \
	if (s_HaveSavedError) {                     	CR \
		s_HaveSavedError = false;               	CR \
		return v8::ThrowException(s_SavedError);    CR \
    }

#define UNIMPLEMENTED  THROW_ERR("Not yet implemented!" )

#define _V8_SAVE_ERR(msg, errtype)   \
	s_HaveSavedError = true;                                    CR \
	std::ostringstream excpt_;                                  CR \
	excpt_ << msg;                          					CR \
    s_SavedError = s_SavedError.New(v8::Exception::errtype( STR2VAL( excpt_.str().c_str() )))
	
#define SAVED_ERROR_STORAGE     extern v8::Persistent<v8::Value> s_SavedError

#define SAVE_ERR(msg)  			 _V8_SAVE_ERR(msg, Error)
#define SAVE_TYPE_ERR(msg)       _V8_SAVE_ERR(msg, TypeError)
#define SAVE_RANGE_ERR(msg)      _V8_SAVE_ERR(msg, RangeError)
#define SAVE_REFERENCE_ERR(msg)  _V8_SAVE_ERR(msg, ReferenceError)
#define SAVE_SYNTAX_ERR(msg)     _V8_SAVE_ERR(msg, SyntaxError)

// ========================================================================================
//MARK: CLASS AND METHOD DECLARATION MACROS
// ========================================================================================

#define _V8_HAS_NEW(klass) \
	klass* New_##klass(const v8::Arguments& args);


#define _V8_MANAGED_NATIVE_INSTANCE(klass, wrapper) \
    wrapper(const v8::Arguments& args) : refPtr_(NULL) {            CR \
    		refPtr_ = New_##klass(args);                            CR \
    	}                                                           CR CR \
    ~wrapper() {                                                    CR \
    		if (refPtr_) {                                          CR \
    			delete refPtr_;                                     CR \
    			refPtr_ = NULL;                                     CR \
    		}                                                       CR \
    	}                                                           CR \


#define _V8_UNMANAGED_NATIVE_INSTANCE(klass, wrapper, ops) \
    wrapper(const v8::Arguments& args) { ops }                      CR CR \
    ~wrapper() { ops }                                              CR \


#define _V8_CLASS_IS_SINGLETON(klass)    \
  public:                                                           CR \
    static v8::Handle<v8::Object> GetScriptSingletonInstance();     CR \
  protected:                                                        CR \
    static v8::Persistent<v8::Object> instance_;                    CR \
    static bool instanced_;                                         CR \


#define _V8_CAN_BE_INSTANTIATED_FROM_NATIVE_OBJECT(klass, wrapper)  \
  public:                                                           CR \
    static v8::Handle<v8::Value> NewFromNative(klass* nativeObj);   CR \
    wrapper(klass* obj) : refPtr_(obj) {}                           CR \


#define _V8_DECLARE_SCRIPT_CLASS(klass, superklasses)   \
class klass superklasses {                                          CR \
  public:                                                           CR \
    static void Init(v8::Handle<v8::Object> target);                CR \
    static v8::Handle<v8::Value> New(const v8::Arguments& args);    CR \
  protected:                                                        CR \
    static v8::Persistent<v8::FunctionTemplate> constructorTpl_;


#define _V8_HAS_NATIVE_OBJECT(klass)   \
  public:                                                           CR \
    klass* getNativeObject() {                                      CR \
    		return refPtr_;                                         CR \
    	}                                                           CR \
  protected:                                                        CR \
    klass* refPtr_;                                                 CR \


#define DECL_END };


#define END }


// SINGLETON_CLASS - there is only one Native instance, instantiated first time it is used in
//					Javascript or in C++. Javascript should never call new MySingletonClass(), 
//					instead it should call getMySingletonClass()

#define SINGLETON_CLASS(klass)    \
  _V8_HAS_NEW(klass)    												CR CR \
  _V8_DECLARE_SCRIPT_CLASS(klass##Wrap, : public jswrap::ObjectWrap)  	CR \
  _V8_HAS_NATIVE_OBJECT(klass)                     						CR \
  _V8_MANAGED_NATIVE_INSTANCE(klass, klass##Wrap) 						CR \
  public: CR  static klass* getSingletonInstance();         			CR \
  _V8_CLASS_IS_SINGLETON(klass)                        					CR \


// WRAPPER_CLASS - Native C++ instances must be instantiated through some kind of 
//                  a factory call, often from another object. This is not a singleton,
//					so calling createMySingletonClass() multiple times will create
//					multiple Native C++ instances, however the lifecycle of the native
//					C++ object is not tied to the lifecycle of the Javascript object

#define WRAPPER_CLASS(klass)    \
  _V8_DECLARE_SCRIPT_CLASS(klass##Wrap, : public jswrap::ObjectWrap)    CR \
  _V8_HAS_NATIVE_OBJECT(klass)                         					CR \
  _V8_UNMANAGED_NATIVE_INSTANCE(klass, klass##Wrap, CR refPtr_ = 0;CR ) CR \
  _V8_CAN_BE_INSTANTIATED_FROM_NATIVE_OBJECT(klass, klass##Wrap)    	CR \


// BINDING_CLASS - Native C++ instances can be instantiated by Javascript code.
// 					var a = new MyClass();
//					Lifecycle of the Native C++ instance is tied to lifecycle of
//					the Javascript object

#define BINDING_CLASS(klass)   \
  _V8_HAS_NEW(klass)    												CR CR \
  _V8_DECLARE_SCRIPT_CLASS(klass##Wrap, : public jswrap::ObjectWrap)  	CR \
  _V8_HAS_NATIVE_OBJECT(klass)                         					CR \
  _V8_MANAGED_NATIVE_INSTANCE(klass, klass##Wrap) 						CR \
  _V8_CAN_BE_INSTANTIATED_FROM_NATIVE_OBJECT(klass, klass##Wrap)    	CR \


// FACADE_CLASS	- There is no Native C++ instance. This is just a facade for a bunch of 
//					static function calls that we want grouped together

#define FACADE_CLASS(klass)   \
  _V8_DECLARE_SCRIPT_CLASS(klass##Wrap, : public jswrap::ObjectWrap)  	CR CR \
  _V8_UNMANAGED_NATIVE_INSTANCE(klass, klass##Wrap, ) 					CR \
  _V8_CLASS_IS_SINGLETON(klass)


#define METHOD(klass, name) static v8::Handle<v8::Value> name (const v8::Arguments& args);


#define PROPERTY(klass, name) \
	METHOD(klass, Get##name) CR \
	METHOD(klass, Set##name)


#define CONSTANT(klass, name)


// ========================================================================================
//MARK: CLASS AND METHOD DEFINITION MACROS
// ========================================================================================

// Sets up the New method, which is used to create a wrapper and
// the underlying C++ object that is being wrapped. Both are created
// the same time
//
// Usage (C++): 
//  	v8::Local<v8::Object> obj =
//  		FooWrap::constructorTpl_->GetFunction()->NewInstance();
//
// Usage (Javascript):
//		var obj = new pdg.Foo();
//
#define _V8_WRAPPER_NEW_IMPL(klass, wrapper, extra)    \
v8::Handle<v8::Value> wrapper::New(const v8::Arguments& args) {  CR \
    v8::HandleScope scope;                                       CR \
    wrapper* objWrapper = new wrapper(args);                     CR \
    objWrapper->Wrap(args.This());                               CR \
    extra;                                                       CR \
    THROW_IF_SAVED_ERR;                                          CR \
    return args.This();                                          CR \
}                                                                CR CR \


// Sets up the NewFromNative method, which is used to wrap an 
// already existing C++ object rather than creating it as we create
// the wrapper.
//
// Usage (C++):
//	   Foo* myFoo = instantiateAndLoadFoo();
//     v8::Handle<v8::Value> obj = ImageWrap::NewFromNative(myFoo);
//
// Not callable from Javascript
//
#define _V8_WRAPPER_NEW_FROM_NATIVE_IMPL(klass, wrapper, extra)    \
v8::Handle<v8::Value> wrapper::NewFromNative(klass* nativeObj) {  CR \
	s_##klass##_InNewFromNative = true;							  CR \
    v8::HandleScope scope;                                        CR \
    v8::Local<v8::Object> instance =                              CR \
    		constructorTpl_->GetFunction()->NewInstance();        CR \
    v8::Persistent<v8::Object> obj =                              CR \
			v8::Persistent<v8::Object>::New(instance);            CR \
    wrapper* objWrapper = jswrap::ObjectWrap::Unwrap<wrapper>(obj);       CR \
    extra;                                                        CR \
    DEBUG_ASSERT(objWrapper->refPtr_ == 0, 						  \
    	"NewFromNative() already have native object!"); 		  CR \
    if (objWrapper->refPtr_) delete objWrapper->refPtr_;          CR \
    objWrapper->refPtr_ = nativeObj;                              CR \
    s_##klass##_InNewFromNative = false;						  CR \
    return scope.Close(obj);                                      CR \
}                                                                 CR CR \

// Sets up the methods and fields needed for a singleton 
// Use GetScriptSingletonInstance() to get the Javascript v8::Object
//   or to create it if it doesn't exist
#define _V8_GET_SCRIPT_SINGLETON_IMPL(klass, wrapper)   \
v8::Persistent<v8::Object> wrapper::instance_;                      CR \
bool wrapper::instanced_ = false;                                   CR CR \
v8::Handle<v8::Object> wrapper::GetScriptSingletonInstance() {  	CR \
  	v8::HandleScope scope;                                          CR \
  	if (!instanced_) {                                              CR \
  		v8::Local<v8::Object> instance =                            CR \
  					constructorTpl_->GetFunction()->NewInstance();  CR \
		instance_ = v8::Persistent<v8::Object>::New(instance);      CR \
		instanced_ = true;                                          CR \
	}                                                               CR \
  	return scope.Close(instance_);                                  CR \
}                                                                   CR CR \


// Sets up the methods and fields needed for a wrapped native singleton 
// Use getSingletonInstance() to get the native object 
//   Note: getSingletonInstance() calls GetJavascriptSingletonInstance()
#define _V8_GET_NATIVE_SINGLETON_IMPL(klass, wrapper)   \
klass* wrapper::getSingletonInstance() {                        CR \
	v8::Handle<v8::Object> val =                                CR \
	   GetScriptSingletonInstance()->ToObject();            	CR \
    wrapper* objWrapper = jswrap::ObjectWrap::Unwrap<wrapper>(val);     CR \
    return dynamic_cast<klass*>(objWrapper->refPtr_);           CR \
}                                                               CR CR \


// this sets up the constructor function template that is used to
// create javascript objects with the class signature desired
// the function must be completed with an EXPORT_CLASS_SYMBOLS macro
// that exports the public constants, properties and methods so they 
// can be added to the class signature
#define _V8_CLASS_INIT_IMPL(klass, wrapper)    \
v8::Persistent<v8::FunctionTemplate> wrapper::constructorTpl_;          CR CR \
void wrapper::Init(v8::Handle<v8::Object> target) {                     CR \

	// deliberately left as incomplete function


#define FACADE_INITIALIZER_IMPL(klass)  \
  _V8_WRAPPER_NEW_IMPL(klass, klass##Wrap, )      CR \
  _V8_GET_SCRIPT_SINGLETON_IMPL(klass, klass##Wrap)   CR \
  _V8_CLASS_INIT_IMPL(klass, klass##Wrap)         CR \


#define SINGLETON_INITIALIZER_IMPL(klass)   \
static bool s_##klass##_InNewFromNative = false;		CR CR \
  _V8_WRAPPER_NEW_IMPL(klass, klass##Wrap, )      		CR \
  _V8_GET_SCRIPT_SINGLETON_IMPL(klass, klass##Wrap)   	CR \
  _V8_GET_NATIVE_SINGLETON_IMPL(klass, klass##Wrap)   	CR \
  _V8_CLASS_INIT_IMPL(klass, klass##Wrap)         		CR \


// WRAPPER_CLASS - Native C++ instances must be instantiated through some kind of 
//                  a factory call, often from another object. This is not a singleton,
//					so calling createMySingletonClass() multiple times will create
//					multiple Native C++ instances, however the lifecycle of the native
//					C++ object is not tied to the lifecycle of the Javascript object

#define WRAPPER_INITIALIZER_IMPL(klass)    \
static bool s_##klass##_InNewFromNative = false;		CR CR \
  _V8_WRAPPER_NEW_IMPL(klass, klass##Wrap, )         	CR \
  _V8_WRAPPER_NEW_FROM_NATIVE_IMPL(klass, klass##Wrap, 	CR \
    nativeObj->m##klass##ScriptObj = 					\
    	v8::Persistent<v8::Object>::New(obj) )  		CR \
  _V8_CLASS_INIT_IMPL(klass,klass##Wrap)               	CR \


#define WRAPPER_INITIALIZER_IMPL_CUSTOM(klass, extraNewFromNative)    \
static bool s_##klass##_InNewFromNative = false;		CR CR \
  _V8_WRAPPER_NEW_IMPL(klass, klass##Wrap, )    		CR \
  _V8_WRAPPER_NEW_FROM_NATIVE_IMPL(klass, klass##Wrap,  CR \
    extraNewFromNative )                                CR \
  _V8_CLASS_INIT_IMPL(klass,klass##Wrap)                CR \


// BINDING_CLASS - Native C++ instances can be instantiated by Javascript code.
// 					var a = new MyClass();
//					Lifecycle of the Native C++ instance is tied to lifecycle of
//					the Javascript object

#define BINDING_INITIALIZER_IMPL(klass)   \
static bool s_##klass##_InNewFromNative = false;	   CR CR \
  _V8_WRAPPER_NEW_IMPL(klass, klass##Wrap, \
    klass* nativeObj = objWrapper->getNativeObject();  CR \
	if (nativeObj) nativeObj->m##klass##ScriptObj = v8::Persistent<v8::Object>::New(args.This());  CR \
	objWrapper->Ref() )    		CR \
  _V8_WRAPPER_NEW_FROM_NATIVE_IMPL(klass, klass##Wrap,  CR \
    nativeObj->m##klass##ScriptObj = v8::Persistent<v8::Object>::New(obj);  CR \
	objWrapper->Ref() )                                 CR \
  _V8_CLASS_INIT_IMPL(klass,klass##Wrap)                CR \


#define NATIVE_CONSTRUCTOR_IMPL(klass)	\
klass* New_##klass(SCRIPT_ARGS) {						CR \
	if (s_##klass##_InNewFromNative) return 0;


// Export all the symbols for a particular class under the name given.
// 1st section is for the list of HAS_CONST() definitions
// 2nd section is for the list of HAS_GETTER/SETTER/PROPERTY() definitions
// Last section is for the list of HAS_METHOD() definitions
#define EXPORT_CLASS_SYMBOLS(name, klass, consts_def, props_def, meth_def) \
    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New); CR \
    t->InstanceTemplate()->SetInternalFieldCount(1);                    CR \
    t->SetClassName(_V8_STR(name));                                     CR \
    constructorTpl_ = v8::Persistent<v8::FunctionTemplate>::New(t);     CR \
	consts_def	props_def	meth_def									\
    target->Set(_V8_STR(name), constructorTpl_->GetFunction())


#define HAS_METHOD(klass, name, method)  \
	v8::Local<v8::Signature> method##_Sig = v8::Signature::New(t);  CR \
	v8::Local<v8::FunctionTemplate> method##_Tpl =                  CR \
    	v8::FunctionTemplate::New(method, v8::Handle<v8::Value>(),     \
        						  method##_Sig);                    CR \
	t->PrototypeTemplate()->Set(_V8_STR(name), method##_Tpl); 		CR


#define HAS_CONSTANT(klass, name)     \
	t->InstanceTemplate()->Set(SYMBOL(name), INT2VAL(pdg::klass::name),    \
    	static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete)); CR

#define HAS_GETTER(klass, name)  \
	HAS_METHOD(klass, "get"#name, Get##name)


#define HAS_SETTER(klass, name)  \
	HAS_METHOD(klass, "set"#name, Set##name)


#define HAS_PROPERTY(klass, name)  \
	HAS_GETTER(klass, name) \
	HAS_SETTER(klass, name)


#define FUNCTION_DECL(func)    \
  extern v8::Handle<v8::Value> func(const v8::Arguments& args);


#define FUNCTION_IMPL(func)    \
  v8::Handle<v8::Value> func(const v8::Arguments& args) {  CR \
    v8::HandleScope scope;                                 CR \


#define SCRIPT_METHOD_IMPL(klass, method)   \
  v8::Handle<v8::Value> klass##Wrap::method(const v8::Arguments& args) {  	CR \
    v8::HandleScope scope;


#define METHOD_IMPL(klass, method)   CR CR \
  SCRIPT_METHOD_IMPL(klass, method)                                             	CR \
    klass##Wrap* objWrapper = jswrap::ObjectWrap::Unwrap<klass##Wrap>(args.This()); CR \
    klass* self = dynamic_cast<klass*>(objWrapper->refPtr_);    					CR \


#define STATIC_METHOD_IMPL(klass, method)  SCRIPT_METHOD_IMPL(klass, method)


#define INIT_CLASS(klass)  klass##Wrap::Init(target);


#define INIT_FUNCTION(sym, func)   \
	target->Set(_V8_STR(sym), v8::FunctionTemplate::New(func)->GetFunction());


#define INIT_CONSTANT(name, value) \
	target->Set(_V8_STR(name), INT2VAL(value), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete))

#define INIT_BOOL_CONSTANT(name, value) \
	target->Set(_V8_STR(name), BOOL2VAL(value), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete))

#define CLEANUP_IMPL(klass)   \
  void Cleanup##klass##ScriptObject(v8::Handle<v8::Object> obj) {  	CR \
	DEBUG_DUMP_SCRIPT_OBJECT(obj, klass);                			CR \
    if (!obj.IsEmpty()) {                                    		CR \
		DEBUG_PRINT("Cleanup" #klass "ScriptObject for JSObj [%p][%s]" \
			, *obj, typeid_name(*obj)); 							CR \
	} 																CR \
  }


// ========================================================================================
//MARK: ARGUMENT HANDLING MACROS
// ========================================================================================

#define REQUIRE_ARG_COUNT(n)   \
	if (ARGC != n)                                        		 	CR \
		return v8_ThrowArgCountException(ARGC, n)


#define REQUIRE_ARG_MIN_COUNT(n)   \
	if (ARGC < n)                                        			CR \
		return v8_ThrowArgCountException(ARGC, n, true)


#define REQUIRE_STRING_ARG(n, paramName)   \
	if (!VALUE_IS_STRING(ARGV[n-1]))                         			CR \
		return v8_ThrowArgTypeException(n, "a string  ("#paramName")");	CR \
	VALUE_TO_CSTRING(paramName, ARGV[n-1])


#define REQUIRE_NUMBER_ARG(n, paramName)   \
	if (!VALUE_IS_NUMBER(ARGV[n-1]))                       				CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")"); 	CR \
	double paramName = VAL2NUM(ARGV[n-1])


#define REQUIRE_INT32_ARG(n, paramName) \
	if (!VALUE_IS_NUMBER(ARGV[n-1]))                         			CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")");	CR \
	long paramName = VAL2INT(ARGV[n-1])
	

#define REQUIRE_UINT32_ARG(n, paramName) \
	if (!VALUE_IS_NUMBER(ARGV[n-1]))                         			CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")");	CR \
	unsigned long paramName = VAL2UINT(ARGV[n-1])


#define REQUIRE_BOOL_ARG(n, paramName)   \
	if (!VALUE_IS_BOOL(ARGV[n-1]))                          			CR \
		return v8_ThrowArgTypeException(n, "a boolean ("#paramName")");	CR \
	bool paramName = VAL2BOOL(ARGV[n-1])


#define REQUIRE_OFFSET_ARG(n, paramName)   \
	if (!VALUE_IS_OFFSET(ARGV[n-1]))                        			CR \
		return v8_ThrowArgTypeException(n, "Offset", *ARGV[n-1]); 		CR \
	pdg::Offset paramName = VAL2OFFSET(ARGV[n-1])
	

#define REQUIRE_POINT_ARG(n, paramName)   \
	if (!VALUE_IS_POINT(ARGV[n-1]))                         			CR \
		return v8_ThrowArgTypeException(n, "Point", *ARGV[n-1]); 		CR \
	pdg::Point paramName = VAL2POINT(ARGV[n-1])


#define REQUIRE_VECTOR_ARG(n, paramName)   \
	if (!VALUE_IS_VECTOR(ARGV[n-1]))                        			CR \
		return v8_ThrowArgTypeException(n, "Vector", *ARGV[n-1]); 		CR \
	pdg::Vector paramName = VAL2VECTOR(ARGV[n-1])


#define REQUIRE_RECT_ARG(n, paramName)   \
	if (!VALUE_IS_RECT(ARGV[n-1]))                          			CR \
		return v8_ThrowArgTypeException(n,	"Rect", *ARGV[n-1]); 		CR \
	pdg::Rect paramName = VAL2RECT(ARGV[n-1])


#define REQUIRE_ROTATED_RECT_ARG(n, paramName)   \
	if (!VALUE_IS_ROTRECT(ARGV[n-1]))                       			CR \
		return v8_ThrowArgTypeException(n, "RotatedRect", *ARGV[n-1]); 	CR \
	pdg::RotatedRect paramName = VAL2ROTRECT(ARGV[n-1])


#define REQUIRE_QUAD_ARG(n, paramName)   \
	if (!VALUE_IS_QUAD(ARGV[n-1]))                          			CR \
		return v8_ThrowArgTypeException(n, "Quad", *ARGV[n-1]); 		CR \
	pdg::Quad paramName = VAL2QUAD(ARGV[n-1])


#define REQUIRE_COLOR_ARG(n, paramName)   \
	if (!VALUE_IS_COLOR(ARGV[n-1]))                         			CR \
		return v8_ThrowArgTypeException(n, "Color", *ARGV[n-1]); 		CR \
	pdg::Color paramName = VAL2COLOR(ARGV[n-1])


#define OPTIONAL_STRING_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_STRING(ARGV[n-1]))                   	CR \
		return v8_ThrowArgTypeException(n, "a string  ("#paramName")");	CR \
	v8::String::Utf8Value _##paramName##_String( (ARGC<n) ?         	\
	             _V8_STR("") : args[n-1]->ToString() );         		CR \
	const char* paramName = (args.Length()<n) ? defaultVal : *_##paramName##_String;


#define OPTIONAL_INT32_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !args[n-1]->IsNumber())                    		CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")");	CR \
	long paramName = (args.Length()<n) ? defaultVal : args[n-1]->Int32Value();


#define OPTIONAL_UINT32_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !args[n-1]->IsNumber())                  			CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")");	CR \
	unsigned long paramName = (args.Length()<n) ? defaultVal : args[n-1]->Uint32Value();


#define OPTIONAL_NUMBER_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !args[n-1]->IsNumber())                   			CR \
		return v8_ThrowArgTypeException(n, "a number ("#paramName")");	CR \
	double paramName = (args.Length()<n) ? defaultVal : args[n-1]->NumberValue();


#define OPTIONAL_BOOL_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !args[n-1]->IsBoolean())                   		CR \
		return v8_ThrowArgTypeException(n, "a boolean ("#paramName")");	CR \
	bool paramName = (ARGC<n) ? defaultVal : args[n-1]->BooleanValue();


#define OPTIONAL_NATIVE_OBJECT_ARG(n, paramName, klass, defaultVal) \
	klass* paramName = defaultVal;                                      CR \
	if (ARGC >= n) {                                              		CR \
		if (!args[n-1]->IsObject()) {                                   CR \
			return v8_ThrowArgTypeException(n, "an object of type "#klass" ("#paramName")");  	CR \
		} else {                                                                                CR \
			v8::Handle<v8::Object> paramName##_ = args[n-1]->ToObject();                        CR \
			klass##Wrap* paramName##__ = jswrap::ObjectWrap::Unwrap<klass##Wrap>(paramName##_); CR \
			paramName = paramName##__->getNativeObject();                                       CR \
		}                                                               CR \
	}


#define OPTIONAL_OFFSET_ARG(n, paramName, defaultVal)   \
    if (ARGC >= n && !VALUE_IS_OFFSET(ARGV[n-1]))     					CR \
		return v8_ThrowArgTypeException(n, "Offset", *ARGV[n-1]); 		CR \
    pdg::Offset paramName = (ARGC<n) ? defaultVal : VAL2OFFSET(ARGV[n-1])


#define OPTIONAL_POINT_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_POINT(ARGV[n-1]))      					CR \
		return v8_ThrowArgTypeException(n, "Point", *ARGV[n-1]); 		CR \
	pdg::Point paramName = (ARGC<n) ? defaultVal : VAL2POINT(ARGV[n-1])


#define OPTIONAL_RECT_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_RECT(ARGV[n-1]))     					CR \
		return v8_ThrowArgTypeException(n, "Rect", *ARGV[n-1]); 		CR \
	pdg::Rect paramName = (ARGC<n) ? defaultVal : VAL2RECT(ARGV[n-1])


#define OPTIONAL_ROTATED_RECT_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_ROTRECT(ARGV[n-1]))    					CR \
		return v8_ThrowArgTypeException(n, "RotatedRect", *ARGV[n-1]); 	CR \
	pdg::RotatedRect paramName = (ARGC<n) ? defaultVal : VAL2ROTRECT(ARGV[n-1])


#define OPTIONAL_QUAD_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_QUAD(ARGV[n-1]))     					CR \
		return v8_ThrowArgTypeException(n, "Quad", *ARGV[n-1]); 		CR \
	pdg::Quad paramName = (ARGC<n) ? defaultVal : VAL2QUAD(ARGV[n-1])


#define OPTIONAL_COLOR_ARG(n, paramName, defaultVal)   \
	if (ARGC >= n && !VALUE_IS_COLOR(ARGV[n-1]))     					CR \
		return v8_ThrowArgTypeException(n, "Color", *ARGV[n-1]); 		CR \
	pdg::Color paramName = (ARGC<n) ? defaultVal : VAL2COLOR(ARGV[n-1])


// ========================================================================================
//MARK: RETURN VALUE MACROS
// ========================================================================================

#define RETURN(what) return scope.Close( what )

#define RETURN_TRUE            RETURN( BOOL2VAL(true) )
#define RETURN_FALSE           RETURN( BOOL2VAL(false) )
#define RETURN_NULL            RETURN( NULL_VAL )
#define RETURN_THIS            RETURN( THIS )
#define RETURN_BOOL(what)      RETURN( BOOL2VAL(what) )
#define RETURN_UNSIGNED(what)  RETURN( UINT2VAL(what) )
#define RETURN_INTEGER(what)   RETURN( INT2VAL(what) )
#define RETURN_UINT32(what)    RETURN_UNSIGNED(what)
#define RETURN_INT32(what)     RETURN_INTEGER(what)
#define RETURN_NUMBER(what)    RETURN( NUM2VAL(what) )
#define RETURN_STRING(what)    RETURN( STR2VAL(what) )
#define RETURN_OBJECT(what)	   RETURN( OBJ2VAL(what) )
#define RETURN_FUNCTION(what)  RETURN( FUNC2VAL(what) )

#define RETURN_OFFSET(what)	   RETURN( OFFSET2VAL( what ) )
#define RETURN_VECTOR(what)	   RETURN( VECTOR2VAL( what ) )
#define RETURN_POINT(what)	   RETURN( POINT2VAL(what) )
#define RETURN_RECT(what)	   RETURN( RECT2VAL( what ) )
#define RETURN_ROTATED_RECT(what)   RETURN( ROTRECT2VAL( what ) )
#define RETURN_QUAD(what)	   RETURN( QUAD2VAL( what ) )
#define RETURN_COLOR(what)	   RETURN( COLOR2VAL( what ) )

#define RETURN_NATIVE_OBJECT(what, klass)   \
	if (!what) RETURN_NULL;												CR \
    if (what->m##klass##ScriptObj.IsEmpty()) {                          CR \
    	v8::Handle<v8::Value> obj__ = klass##Wrap::NewFromNative(what); CR \
		return scope.Close( obj__ );                                    CR \
	} else {                                                            CR \
		return scope.Close( what->m##klass##ScriptObj );                CR \
	}

#define RETURN_NEW_NATIVE_OBJECT(what, klass)   \
	if (!what) RETURN_NULL;												CR \
    return scope.Close( klass##Wrap::NewFromNative(what) );

#define RETURN_UNDEFINED       RETURN( v8::Undefined() )
#define NO_RETURN              RETURN_UNDEFINED



// ========================================================================================
//MARK: PROPERTY GETTER/SETTER MACROS
// ========================================================================================

#define C_BOOL		bool
#define C_UNSIGNED	unsigned int
#define C_INTEGER	int
#define C_UINT32	uint32
#define C_INT32		int32
#define C_NUMBER	double
#define C_STRING	const char*
#define C_OFFSET	pdg::Offset
#define C_VECTOR	pdg::Vector
#define C_POINT		pdg::Point
#define C_RECT		pdg::Rect
#define C_ROTATED_RECT	pdg::RotatedRect
#define C_QUAD		pdg::Quad
#define C_COLOR		pdg::Color

#define SIG_BOOL		boolean
#define SIG_UNSIGNED	[number uint]
#define SIG_INTEGER		[number int]
#define SIG_UINT32		[number uint]
#define SIG_INT32		[number int]
#define SIG_NUMBER		number
#define SIG_STRING		string
#define SIG_OFFSET		[object Offset]
#define SIG_VECTOR		[object Vector]
#define SIG_POINT		[object Point]
#define SIG_RECT		[object Rect]
#define SIG_ROTATED_RECT	[object RotatedRect]
#define SIG_QUAD		[object Quad]
#define SIG_COLOR		object
#define SIG_RET_BOOL		boolean
#define SIG_RET_UNSIGNED	number
#define SIG_RET_INTEGER		number
#define SIG_RET_UINT32		number
#define SIG_RET_INT32		number
#define SIG_RET_NUMBER		number
#define SIG_RET_STRING		string
#define SIG_RET_OFFSET		[object Offset]
#define SIG_RET_VECTOR		[object Vector]
#define SIG_RET_POINT		[object Point]
#define SIG_RET_RECT		[object Rect]
#define SIG_RET_ROTATED_RECT	[object RotatedRect]
#define SIG_RET_QUAD		[object Quad]
#define SIG_RET_COLOR		object


#define CUSTOM_GETTER_IMPL(klass, prop, type, xargc, m, xargs, getcode, paramdoc) \
METHOD_IMPL(klass, Get##prop)  		CR \
	CUSTOM_GETTER_SIG(SIG_RET_##type, paramdoc); 	CR \
    REQUIRE_ARG_##m##COUNT(xargc);  CR \
    xargs						    CR \
	getcode; 				        CR \
	RETURN_##type(the##prop);  		CR \
	END


#define CUSTOM_SETTER_IMPL(klass, prop, type, xargc, m, xargs, setcode) \
METHOD_IMPL(klass, Set##prop)  		CR \
	SETTER_SIG(klass, SIG_##type, in##prop); CR \
    REQUIRE_ARG_##m##COUNT(xargc);  CR \
    REQUIRE_##type##_ARG(1, the##prop);  CR \
    xargs							CR \
    setcode;       					CR \
	RETURN_THIS;                    CR \
	END


#define GETTER_IMPL(klass, prop, type) \
CUSTOM_GETTER_IMPL(klass, prop, type, 0, , , C_##type the##prop = self->get##prop(), () )


#define SETTER_IMPL(klass, prop, type) \
CUSTOM_SETTER_IMPL(klass, prop, type, 1, , , self->set##prop(the##prop) )


#define PROPERTY_IMPL(klass, prop, type) \
	GETTER_IMPL(klass, prop, type) CR \
	SETTER_IMPL(klass, prop, type)


#define CP_GETTER_IMPL(klass, prop, type) \
CUSTOM_GETTER_IMPL(klass, prop, type, 0, , , C_##type the##prop = klass##Get##prop(self), () )


#define CP_SETTER_IMPL(klass, prop, type) \
CUSTOM_SETTER_IMPL(klass, prop, type, 1, , , klass##Set##prop(self, the##prop) )


#define CP_PROPERTY_IMPL(klass, prop, type) \
	CP_GETTER_IMPL(klass, prop, type) CR \
	CP_SETTER_IMPL(klass, prop, type)


// ========================================================================================
//MARK: METHOD SIGNATURE MACROS
// ========================================================================================


#define METHOD_SIGNATURE_NO_DOCS(rettype, paramcount, params) \
	if (args.Length() == 1 && args[0]->IsNull()) {                  CR \
		RETURN_STRING(#rettype " function" #params);                CR \
	}


#define METHOD_SIGNATURE(brief, rettype, paramcount, params) \
	if (args.Length() == 1 && args[0]->IsNull()) {                  CR \
		RETURN_STRING(#rettype " function" #params " - " brief);  CR \
	}


#define CUSTOM_GETTER_SIG(rettype, params) METHOD_SIGNATURE_NO_DOCS(rettype, 0, params)
#define GETTER_SIG(rettype) METHOD_SIGNATURE_NO_DOCS(rettype, 0, ())
#define SETTER_SIG(klass, type, name) METHOD_SIGNATURE_NO_DOCS(klass, 1, (type name))


// ========================================================================================
//MARK: DEBUG MACROS
// ========================================================================================

#define VALUE_TYPE_STR(val) \
        (val.IsEmpty() ? "empty" : val->IsArray() ? "array" : val->IsFunction() ? "function" : \
		val->IsStringObject() ? "string (object)" : val->IsString() ? "string" : val->IsNull() ? "null" : \
		val->IsUndefined() ? "undefined" : val->IsNumberObject() ? "number (object)" : \
		val->IsNumber() ? "number" : val->IsBoolean() ? "boolean" : val->IsDate() ? "date" : \
		val->IsRegExp() ? "regexp" : val->IsNativeError() ? "error" : val->IsObject() ? "object" : "unknown")


// for internal debugging use, shouldn't ever be in non-debug builds
#define DUMP_SCRIPT_OBJECT(val, klass) \
	if (val.IsEmpty()) {                                                   CR \
		std::cerr << __func__<<":"<< __line__ <<                           \
			" - NIL JS Object (" #val "|"<<*((void**)&(val))<<")\n";       CR \
	} else if (!val->IsObject()) {                                         CR \
		std::cerr << __func__<<":"<< __line__ <<                           \
			" - NON JS Object (" #val "|"<<*((void**)&(val))<<")\n";       CR \
	} else {                                                               CR \
		v8::Handle<v8::Object> obj_ = val->ToObject();                     CR \
		v8::String::Utf8Value objNameStr(obj_->ToString());                CR \
		char* objName = *objNameStr;                                       CR \
		klass##Wrap* obj__=jswrap::ObjectWrap::Unwrap<klass##Wrap>(obj_);  CR \
		if (!obj__) {                                                      CR \
			v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();        CR \
			if (protoVal_->IsObject()) {								   CR \
				obj_ = protoVal_->ToObject();						       CR \
				obj__ = jswrap::ObjectWrap::Unwrap<klass##Wrap>(obj_);	   CR \
			}															   CR \
		    if (obj__) {												   CR \
			    std::cout << __func__<<":"<< __line__ << 				   \
			    	" - JS Object ("#val"|"<<*((void**)&(val))<<"): " <<   \
			    	objName << " - is a subclass of native "#klass"\n";    CR \
		    } else {			    									   CR \
			    std::cout << __func__<<":"<< __line__ <<                   \
			    	" - JS Object ("#val"|"<<*((void**)&(val))<<"): " <<   \
			    	objName << " - does not wrap "#klass"\n";              CR \
		    }					            							   CR \
		} else {                                                           CR \
			klass* obj = dynamic_cast<klass*>(obj__->getNativeObject());   CR \
			std::cout << __func__<<":"<< __line__ <<                       \
				" - JS Object ("#val"|" << *((void**)&(val)) << "): " <<   \
				objName<<" - wraps native "#klass" ("<<(void*)obj<<")\n";  CR \
		}                                                                  CR \
	}

#define DEBUG_DUMP_SCRIPT_OBJECT(val, klass) SCRIPT_DEBUG_ONLY( DUMP_SCRIPT_OBJECT(val, klass) )


#endif // PDG_SCRIPT_MACROS_H_INCLUDED
