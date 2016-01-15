// -----------------------------------------------
// This file automatically generated from:
//
//    work/pdg/src/bindings/javascript/v8/pdg_js_classes.cpp
//    work/pdg/src/bindings/javascript/v8/pdg_script_macros.h
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

#include "pdg_script_interface.h"

#include "internals.h"
#include "pdg-lib.h"

#include "node.h"

#include <sstream>
#include <cmath>
#include <cstdlib>

#ifndef PDG_DEBUG_SCRIPTING

#define SCRIPT_DEBUG_ONLY(_expression)
#else
#define SCRIPT_DEBUG_ONLY DEBUG_ONLY
#endif

namespace pdg
{

    extern bool s_IEventHandler_InNewFromCpp;
    extern bool s_ISerializable_InNewFromCpp;
    extern bool s_IAnimationHelper_InNewFromCpp;
    extern bool s_ISpriteCollideHelper_InNewFromCpp;
    extern bool s_ISpriteDrawHelper_InNewFromCpp;

    v8::Persistent<v8::Value> s_SavedError;

    static v8::Persistent<v8::Object> s_BindingTarget;

    void GetConfigManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object ConfigManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( ConfigManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }

    void GetLogManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object LogManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( LogManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }

    IEventHandler* New_IEventHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (s_IEventHandler_InNewFromCpp) return 0;
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 0)
        {
            ScriptEventHandler* handler = new ScriptEventHandler();
            handler->addRef();
            return handler;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            {
                std::ostringstream excpt_;
                excpt_ << "EventHandler must be created with a function argument (handlerFunc)";
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                s_SavedError.Reset(isolate, v8::Exception::SyntaxError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
            };
        }
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
        ScriptEventHandler* handler = new ScriptEventHandler(callback);
        handler->addRef();
        return handler;
    }

    void EventManagerWrap::IsKeyDown(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "boolean" " function" "({string unicodeChar|[number uint] utf16CharCode})" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        if (args[0]->IsString())
        {
            v8::Local<v8::String> keyCode_String = args[0]->ToString();
            uint16 utf16Char = 0;
            keyCode_String->Write(&utf16Char, 0, 1, v8::String::NO_NULL_TERMINATION);
            { args.GetReturnValue().Set( v8::Boolean::New(isolate, OS::isKeyDown(utf16Char)) ); return; };
        }
        else
        {
            if (!args[1 -1]->IsNumber())
                v8_ThrowArgTypeException(isolate, 1, "a number (""utf16CharCode"")");
            unsigned long utf16CharCode = args[1 -1]->Uint32Value();
            { args.GetReturnValue().Set( v8::Boolean::New(isolate, OS::isKeyDown(utf16CharCode)) ); return; };
        }
    }
    void EventManagerWrap::GetDeviceOrientation(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "object" " function" "(boolean absolute = false)" " - " "NOT IMPLEMENTED") ); return; };
        };
        if (args.Length() >= 1 && !args[1 -1]->IsBoolean())
            v8_ThrowArgTypeException(isolate, 1, "a boolean (""absolute"")");
        bool absolute = (args.Length()<1) ? false : args[1 -1]->BooleanValue();;
        float roll, pitch, yaw;
        OS::getDeviceOrientation(roll, pitch, yaw, absolute);
        v8::Local<v8::Object> jsOrientation = v8::Object::New(isolate);
        jsOrientation->Set(v8::String::NewFromUtf8(isolate, "roll"),v8::Number::New(isolate, roll));
        jsOrientation->Set(v8::String::NewFromUtf8(isolate, "pitch"),v8::Number::New(isolate, pitch));
        jsOrientation->Set(v8::String::NewFromUtf8(isolate, "yaw"),v8::Number::New(isolate, yaw));
        { args.GetReturnValue().Set( jsOrientation ); return; };
    }
    void GetEventManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object EventManager]" " function" "()" " - " "") ); return; };
        };
        v8::Local<v8::Object> jsInstance = EventManagerWrap::GetScriptSingletonInstance(isolate);
        EventManager* evtMgr = EventManager::getSingletonInstance();
        evtMgr->mEventEmitterScriptObj.Reset(isolate, jsInstance);
        { args.GetReturnValue().Set( jsInstance ); return; };
    }

    void ResourceManagerWrap::GetImage(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->cppPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object Image]" " function" "(string imageName)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        if (!args[1 -1]->IsString())
            v8_ThrowArgTypeException(isolate, 1, "a string  (""imageName"")");
        v8::String::Utf8Value imageName_Str(args[1 -1]->ToString());
        const char* imageName = *imageName_Str;;
        Image* img = self->getImage(imageName);
        if (img == NULL)
        {
            { args.GetReturnValue().Set( v8::Boolean::New(isolate, false) ); return; };
        }
        v8::Local<v8::Object> obj = ImageWrap::NewFromCpp(isolate, img);

        obj->Set(v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, imageName));
        { args.GetReturnValue().Set( obj ); return; };
    }

    void ResourceManagerWrap::GetImageStrip(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->cppPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object ImageStrip]" " function" "(string imageName)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        if (!args[1 -1]->IsString())
            v8_ThrowArgTypeException(isolate, 1, "a string  (""imageName"")");
        v8::String::Utf8Value imageName_Str(args[1 -1]->ToString());
        const char* imageName = *imageName_Str;;
        ImageStrip* img = self->getImageStrip(imageName);
        if (img == NULL)
        {
            { args.GetReturnValue().Set( v8::Boolean::New(isolate, false) ); return; };
        }
        v8::Local<v8::Object> obj = ImageStripWrap::NewFromCpp(isolate, img);

        obj->Set(v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, imageName));
        { args.GetReturnValue().Set( obj ); return; };
    }
#ifndef PDG_NO_SOUND

    void ResourceManagerWrap::GetSound(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->cppPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object Sound]" " function" "(string soundName)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        if (!args[1 -1]->IsString())
            v8_ThrowArgTypeException(isolate, 1, "a string  (""soundName"")");
        v8::String::Utf8Value soundName_Str(args[1 -1]->ToString());
        const char* soundName = *soundName_Str;;
        Sound* snd = self->getSound(soundName);
        if (snd == NULL)
        {
            { args.GetReturnValue().Set( v8::Boolean::New(isolate, false) ); return; };
        }
        v8::Local<v8::Object> obj = SoundWrap::NewFromCpp(isolate, snd);

        obj->Set(v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, soundName) );
        { args.GetReturnValue().Set( obj ); return; };
    }
#endif

    void GetResourceManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object ResourceManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( ResourceManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }

    ISerializable* New_ISerializable(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (s_ISerializable_InNewFromCpp) return 0;
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 0)
        {
            ScriptSerializable* serializable = new ScriptSerializable();
            serializable->addRef();
            return serializable;
        }
        else
        {
            if (args.Length() != 4 || !args[0]->IsFunction() || !args[1]->IsFunction()
                || !args[2]->IsFunction() || !args[3]->IsFunction())
            {
                s_HaveSavedError = true;
                {
                    std::ostringstream excpt_;
                    excpt_ << "Serializable must be created with 4 function arguments " "(getSerializedSizeFunc, serializeFunc, deserializeFunc, getMyClassTagFunc)";
                    v8::Isolate* isolate = v8::Isolate::GetCurrent();
                    s_SavedError.Reset(isolate, v8::Exception::SyntaxError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
                };

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
    }

    void SerializerWrap::Serialize_obj(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        SerializerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<SerializerWrap>(args.This());
        Serializer* self = dynamic_cast<Serializer*>(objWrapper->cppPtr_);

        self->mSerializerScriptObj.Reset(isolate, args.This());
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "undefined" " function" "([object ISerializable] obj)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        ISerializable* obj = 0;
        if (args[1 -1]->IsObject())
        {
            v8::Local<v8::Object> obj_ = args[1 -1]->ToObject();
            ISerializableWrap* obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>( obj_);
            if (! obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                    obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>( obj_);
                }
            }
            if ( obj__)
            {
                obj = obj__->getCppObject();
                obj->mISerializableScriptObj.Reset(isolate, args[1 -1]->ToObject());
            }
        }
        if (!obj)
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be an object derived from ""ISerializable"" (""obj"")";
            isolate->ThrowException( v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
        };
        SCRIPT_DEBUG_ONLY( if (args[0].IsEmpty())
        {
            std::cerr << __func__<<":"<< 249 << " - NIL JS Object (" "args[0]" "|"<<*((void**)&(args[0]))<<")\n";
        }
        else if (!args[0]->IsObject())
        {
            std::cerr << __func__<<":"<< 249 << " - NOT JS Object (" "args[0]" "|"<<*((void**)&(args[0]))<<") : " << (args[0].IsEmpty() ? "empty" : args[0]->IsArray() ? "array" : args[0]->IsFunction() ? "function" : args[0]->IsStringObject() ? "string (object)" : args[0]->IsString() ? "string" : args[0]->IsNull() ? "null" : args[0]->IsUndefined() ? "undefined" : args[0]->IsNumberObject() ? "number (object)" : args[0]->IsNumber() ? "number" : args[0]->IsBoolean() ? "boolean" : args[0]->IsDate() ? "date" : args[0]->IsRegExp() ? "regexp" : args[0]->IsNativeError() ? "error" : args[0]->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = args[0]->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap< ISerializableWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< ISerializableWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 249 << " - JS Object (""args[0]""|"<<*((void**)&(args[0]))<<"): " << objName << " - is a subclass of C++ ""ISerializable""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 249 << " - JS Object (""args[0]""|"<<*((void**)&(args[0]))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                }
            }
            else
            {
                ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 249 << " - JS Object (""args[0]""|" << *((void**)&(args[0])) << "): " << objName<<" - wraps C++ ""ISerializable"" ("<<(void*)obj<<")\n";
            }
        } );
        self->serialize_obj(obj);
        args.GetReturnValue().SetUndefined();
    }

    void SerializerWrap::SerializedSize(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        SerializerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<SerializerWrap>(args.This());
        Serializer* self = dynamic_cast<Serializer*>(objWrapper->cppPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "number" " function" "({string|boolean|[number uint]|[object Color]|[object Offset]|[object Point]|[object Vector]|[object Rect]|[object RotatedRect]|[object Quad]|[object MemBlock]|[object ISerializable]} arg)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        uint32 dataSize = 0;
        if (args[0]->IsString())
        {
            v8::String::Utf8Value str_Str(args[0]->ToString());
            const char* str = *str_Str;;
            dataSize = self->sizeof_str(str);
        }
        else if (args[0]->IsBoolean())
        {
            bool val = args[0]->BooleanValue();
            dataSize = self->sizeof_bool(val);
        }
        else if (args[0]->IsNumber())
        {
            uint32 val = args[0]->Uint32Value();
            dataSize = self->sizeof_uint(val);
        }
        else if (v8_ValueIsColor(isolate, args[0]))
        {
            Color c = v8_ValueToColor(isolate, args[0]);
            dataSize = self->sizeof_color(c);
        }
        else if (v8_ValueIsOffset(isolate, args[0]))
        {
            Offset o = v8_ValueToOffset(isolate, args[0]);
            dataSize = self->sizeof_offset(o);
        }
        else if (v8_ValueIsRect(isolate, args[0]))
        {
            Rect r = v8_ValueToRect(isolate, args[0]);
            dataSize = self->sizeof_rect(r);
        }
        else if (v8_ValueIsRotatedRect(isolate, args[0]))
        {
            RotatedRect rr = v8_ValueToRotatedRect(isolate, args[0]);
            dataSize = self->sizeof_rotr(rr);
        }
        else if (v8_ValueIsQuad(isolate, args[0]))
        {
            Quad q = v8_ValueToQuad(isolate, args[0]);
            dataSize = self->sizeof_quad(q);
        }
        else if (args[0]->IsObject())
        {
            v8::Local<v8::Object> obj = args[0]->ToObject();
            MemBlockWrap* memBlock__ = ObjectWrap::Unwrap<MemBlockWrap>(obj);
            if (memBlock__)
            {
                MemBlock* memBlock = memBlock__->getCppObject();
                dataSize = self->sizeof_mem(memBlock->ptr, memBlock->bytes);
            }
            else
            {

                ISerializable* serializable = 0;
                if (args[1 -1]->IsObject())
                {
                    v8::Local<v8::Object> serializable_ = args[1 -1]->ToObject();
                    ISerializableWrap* serializable__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>( serializable_);
                    if (! serializable__)
                    {
                        v8::Local<v8::Value> protoVal_ = serializable_->GetPrototype();
                        if (protoVal_->IsObject())
                        {
                            serializable_ = protoVal_->ToObject();
                            serializable__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>( serializable_);
                        }
                    }
                    if ( serializable__)
                    {
                        serializable = serializable__->getCppObject();
                        serializable->mISerializableScriptObj.Reset(isolate, args[1 -1]->ToObject());
                    }
                };
                if (serializable)
                {
                    SCRIPT_DEBUG_ONLY( if (self->mSerializerScriptObj.IsEmpty())
                    {
                        std::cerr << __func__<<":"<< 291 << " - NIL JS Object (" "self->mSerializerScriptObj" "|"<<*((void**)&(self->mSerializerScriptObj))<<")\n";
                    }
                    else if (!self->mSerializerScriptObj->IsObject())
                    {
                        std::cerr << __func__<<":"<< 291 << " - NOT JS Object (" "self->mSerializerScriptObj" "|"<<*((void**)&(self->mSerializerScriptObj))<<") : " << (self->mSerializerScriptObj.IsEmpty() ? "empty" : self->mSerializerScriptObj->IsArray() ? "array" : self->mSerializerScriptObj->IsFunction() ? "function" : self->mSerializerScriptObj->IsStringObject() ? "string (object)" : self->mSerializerScriptObj->IsString() ? "string" : self->mSerializerScriptObj->IsNull() ? "null" : self->mSerializerScriptObj->IsUndefined() ? "undefined" : self->mSerializerScriptObj->IsNumberObject() ? "number (object)" : self->mSerializerScriptObj->IsNumber() ? "number" : self->mSerializerScriptObj->IsBoolean() ? "boolean" : self->mSerializerScriptObj->IsDate() ? "date" : self->mSerializerScriptObj->IsRegExp() ? "regexp" : self->mSerializerScriptObj->IsNativeError() ? "error" : self->mSerializerScriptObj->IsObject() ? "object" : "unknown") << "\n";
                    }
                    else
                    {
                        v8::Local<v8::Object> obj_ = self->mSerializerScriptObj->ToObject();
                            v8::String::Utf8Value objNameStr(obj_->ToString());
                            char* objName = *objNameStr;
                            SerializerWrap* obj__=jswrap::ObjectWrap::Unwrap< SerializerWrap>(obj_);
                            if (!obj__)
                        {
                            v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                                if (protoVal_->IsObject())
                            {
                                obj_ = protoVal_->ToObject();
                                    obj__ = jswrap::ObjectWrap::Unwrap< SerializerWrap>(obj_);
                            }
                            if (obj__)
                            {
                                std::cout << __func__<<":"<< 291 << " - JS Object (""self->mSerializerScriptObj""|"<<*((void**)&(self->mSerializerScriptObj))<<"): " << objName << " - is a subclass of C++ ""Serializer""\n";
                            }
                            else
                            {
                                std::cout << __func__<<":"<< 291 << " - JS Object (""self->mSerializerScriptObj""|"<<*((void**)&(self->mSerializerScriptObj))<<"): " << objName << " - does not wrap ""Serializer""\n";
                            }
                        }
                        else
                        {
                            Serializer* obj = dynamic_cast<Serializer*>(obj__->getCppObject());
                                std::cout << __func__<<":"<< 291 << " - JS Object (""self->mSerializerScriptObj""|" << *((void**)&(self->mSerializerScriptObj)) << "): " << objName<<" - wraps C++ ""Serializer"" ("<<(void*)obj<<")\n";
                        }
                    } )
                        SCRIPT_DEBUG_ONLY( if (serializable->mISerializableScriptObj.IsEmpty())
                    {
                        std::cerr << __func__<<":"<< 292 << " - NIL JS Object (" "serializable->mISerializableScriptObj" "|"<<*((void**)&(serializable->mISerializableScriptObj))<<")\n";
                    }
                    else if (!serializable->mISerializableScriptObj->IsObject())
                    {
                        std::cerr << __func__<<":"<< 292 << " - NOT JS Object (" "serializable->mISerializableScriptObj" "|"<<*((void**)&(serializable->mISerializableScriptObj))<<") : " << (serializable->mISerializableScriptObj.IsEmpty() ? "empty" : serializable->mISerializableScriptObj->IsArray() ? "array" : serializable->mISerializableScriptObj->IsFunction() ? "function" : serializable->mISerializableScriptObj->IsStringObject() ? "string (object)" : serializable->mISerializableScriptObj->IsString() ? "string" : serializable->mISerializableScriptObj->IsNull() ? "null" : serializable->mISerializableScriptObj->IsUndefined() ? "undefined" : serializable->mISerializableScriptObj->IsNumberObject() ? "number (object)" : serializable->mISerializableScriptObj->IsNumber() ? "number" : serializable->mISerializableScriptObj->IsBoolean() ? "boolean" : serializable->mISerializableScriptObj->IsDate() ? "date" : serializable->mISerializableScriptObj->IsRegExp() ? "regexp" : serializable->mISerializableScriptObj->IsNativeError() ? "error" : serializable->mISerializableScriptObj->IsObject() ? "object" : "unknown") << "\n";
                    }
                    else
                    {
                        v8::Local<v8::Object> obj_ = serializable->mISerializableScriptObj->ToObject();
                            v8::String::Utf8Value objNameStr(obj_->ToString());
                            char* objName = *objNameStr;
                            ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap< ISerializableWrap>(obj_);
                            if (!obj__)
                        {
                            v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                                if (protoVal_->IsObject())
                            {
                                obj_ = protoVal_->ToObject();
                                    obj__ = jswrap::ObjectWrap::Unwrap< ISerializableWrap>(obj_);
                            }
                            if (obj__)
                            {
                                std::cout << __func__<<":"<< 292 << " - JS Object (""serializable->mISerializableScriptObj""|"<<*((void**)&(serializable->mISerializableScriptObj))<<"): " << objName << " - is a subclass of C++ ""ISerializable""\n";
                            }
                            else
                            {
                                std::cout << __func__<<":"<< 292 << " - JS Object (""serializable->mISerializableScriptObj""|"<<*((void**)&(serializable->mISerializableScriptObj))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                            }
                        }
                        else
                        {
                            ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getCppObject());
                                std::cout << __func__<<":"<< 292 << " - JS Object (""serializable->mISerializableScriptObj""|" << *((void**)&(serializable->mISerializableScriptObj)) << "): " << objName<<" - wraps C++ ""ISerializable"" ("<<(void*)obj<<")\n";
                        }
                    } )
                        dataSize = self->sizeof_obj(serializable);
                }
                else
                {
                    std::ostringstream excpt_;
                    excpt_ << "argument 1 must be either an unsigned integer, " "a string, a MemBlock object, an ISerializable object";
                    isolate->ThrowException( v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));

                }
            }
        }
        { args.GetReturnValue().Set( v8::Integer::NewFromUnsigned(isolate, dataSize) ); return; };
    }

    void RegisterSerializableObject(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "undefined" " function" "(object obj, [number uint] uniqueId)" " - " "") ); return; };
        };
        if (args.Length() != 2)
            v8_ThrowArgCountException(isolate, args.Length(), 2);
        if (!args[1 -1]->IsObject())
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be an object (""obj"")";
            isolate->ThrowException( v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
        }
        v8::Local<v8::Object> obj = args[1 -1]->ToObject();;
        if (!args[2 -1]->IsNumber())
            v8_ThrowArgTypeException(isolate, 2, "a number (""uniqueId"")");
        unsigned long uniqueId = args[2 -1]->Uint32Value();

        args.GetReturnValue().SetUndefined();
    }

#ifndef PDG_NO_GUI

    void GraphicsManagerWrap::GetCurrentScreenMode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        GraphicsManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<GraphicsManagerWrap>(args.This());
        GraphicsManager* self = dynamic_cast<GraphicsManager*>(objWrapper->cppPtr_);
        ;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "object" " function" "([number int] screenNum = PRIMARY_SCREEN)" " - " "returns object with width, height, depth and maxWindowRect for specified screen") ); return; };
        };

        if (args.Length() >= 1 && !args[1 -1]->IsNumber())
            v8_ThrowArgTypeException(isolate, 1, "a number (""screenNum"")");
        long screenNum = (args.Length()<1) ? screenNum_PrimaryScreen : args[1 -1]->Int32Value();;
        pdg::Rect maxWindowRect;
        pdg::GraphicsManager::ScreenMode mode;
        mode = self->getCurrentScreenMode(screenNum, &maxWindowRect);

        v8::Local<v8::Object> jsScreenMode = v8::Object::New(isolate);
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "width"),v8::Integer::New(isolate, mode.width));
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "height"),v8::Integer::New(isolate, mode.height));
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "depth"),v8::Integer::New(isolate, mode.bpp));
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "maxWindowRect"), v8_MakeJavascriptRect(isolate, maxWindowRect));
        { args.GetReturnValue().Set( jsScreenMode ); return; };
    }

    void GraphicsManagerWrap::GetNthSupportedScreenMode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        GraphicsManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<GraphicsManagerWrap>(args.This());
        GraphicsManager* self = dynamic_cast<GraphicsManager*>(objWrapper->cppPtr_);
        ;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "object" " function" "([number int] n, [number int] screenNum = PRIMARY_SCREEN)" " - " "returns object with width, height, depth and maxWindowRect for specified screen") ); return; };
        };

        if (args.Length() < 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1, true);
        if (!args[1 -1]->IsNumber())
            v8_ThrowArgTypeException(isolate, 1, "a number (""n"")");
        long n = args[1 -1]->Int32Value();
        if (args.Length() >= 2 && !args[2 -1]->IsNumber())
            v8_ThrowArgTypeException(isolate, 2, "a number (""screenNum"")");
        long screenNum = (args.Length()<2) ? screenNum_PrimaryScreen : args[2 -1]->Int32Value();;
        pdg::GraphicsManager::ScreenMode mode;
        mode = self->getNthSupportedScreenMode(n, screenNum);

        v8::Local<v8::Object> jsScreenMode = v8::Object::New(isolate);
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "width"),v8::Integer::New(isolate, mode.width));
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "height"),v8::Integer::New(isolate, mode.height));
        jsScreenMode->Set(v8::String::NewFromUtf8(isolate, "depth"),v8::Integer::New(isolate, mode.bpp));
        { args.GetReturnValue().Set( jsScreenMode ); return; };
    }

    void GetGraphicsManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object GraphicsManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( GraphicsManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }
#endif

#ifndef PDG_NO_SOUND

    void GetSoundManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object SoundManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( SoundManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }
#endif

    void GetFileManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object FileManager]" " function" "()" " - " "") ); return; };
        };
        { args.GetReturnValue().Set( FileManagerWrap::GetScriptSingletonInstance(isolate) ); return; };
    }

    void GetTimerManager(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "[object TimerManager]" " function" "()" " - " "") ); return; };
        };
        v8::Local<v8::Object> jsInstance = TimerManagerWrap::GetScriptSingletonInstance(isolate);
        TimerManager* timMgr = TimerManager::getSingletonInstance();
        timMgr->mEventEmitterScriptObj.Reset(isolate, jsInstance);
        { args.GetReturnValue().Set( jsInstance ); return; };
    }

    IAnimationHelper* New_IAnimationHelper(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (s_IAnimationHelper_InNewFromCpp) return 0;
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 0)
        {
            ScriptAnimationHelper* helper = new ScriptAnimationHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            {
                std::ostringstream excpt_;
                excpt_ << "AnimationHelper must be created with a function argument (handlerFunc)";
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                s_SavedError.Reset(isolate, v8::Exception::SyntaxError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
            };
            return 0;
        }
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
        ScriptAnimationHelper* helper = new ScriptAnimationHelper(callback);
        return helper;
    }

    static v8::Persistent<v8::Function> s_CustomScriptEasing[MAX_CUSTOM_EASINGS];

    ISpriteCollideHelper* New_ISpriteCollideHelper(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (s_ISpriteCollideHelper_InNewFromCpp) return 0;
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 0)
        {
            ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            {
                std::ostringstream excpt_;
                excpt_ << "SpriteCollideHelper must be created with a function argument (allowCollisionFunc)";
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                s_SavedError.Reset(isolate, v8::Exception::SyntaxError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
            };
            return 0;
        }
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
        ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper(callback);
        return helper;
    }

#ifndef PDG_NO_GUI

    ISpriteDrawHelper* New_ISpriteDrawHelper(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (s_ISpriteDrawHelper_InNewFromCpp) return 0;
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 0)
        {
            ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            {
                std::ostringstream excpt_;
                excpt_ << "SpriteDrawHelper must be created with a function argument (drawFunc)";
                v8::Isolate* isolate = v8::Isolate::GetCurrent();
                s_SavedError.Reset(isolate, v8::Exception::SyntaxError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
            };
            return 0;
        }
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);
        ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper(callback);
        return helper;
    }
#endif

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
        ScriptSerializable::getSerializedSize(ISerializer* serializer) const
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[1];
        Serializer* ser = dynamic_cast<Serializer*>(serializer);
        if (!ser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
                exit(1);
                )
        }

        argv[0] = v8::Local<v8::Object>::New(isolate, ser->mSerializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
        if (!mScriptGetSerializedSizeFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptGetSerializedSizeFunc);
        }
        else
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "getSerializedSize")));
        }
        resVal = func->Call(obj_, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getSerializedSize Function!!" ); )
                FatalException(try_catch);
            return 0;
        }
        if (!resVal->IsUint32())
        {
            DEBUG_ONLY(
                std::cerr << "result mismatch: return value from getSerializedSize Function must be an unsigned integer ("
                << v8_GetFunctionName(func) << " at " << v8_GetFunctionFileAndLine(func) << ")\n";
                exit(1);
                )
                return 0;
        }
        return resVal->Uint32Value();
    }

    void
        ScriptSerializable::serialize(ISerializer* serializer) const
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[1];
        Serializer* ser = dynamic_cast<Serializer*>(serializer);
        if (!ser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
                exit(1);
                )
        }

        argv[0] = v8::Local<v8::Object>::New(isolate, ser->mSerializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
        if (!mScriptSerializeFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptSerializeFunc);
        }
        else
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "serialize")));
        }
        resVal = func->Call(obj_, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling serialize Function!!" ); )
                FatalException(try_catch);
        }
    }

    void
        ScriptSerializable::deserialize(IDeserializer* deserializer)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[1];
        Deserializer* deser = dynamic_cast<Deserializer*>(deserializer);
        if (!deser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: deserialize Function called with invalid Deserializer\n";
                exit(1);
                )
        }
        argv[0] = v8::Local<v8::Object>::New(isolate, deser->mDeserializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
        if (!mScriptDeserializeFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptDeserializeFunc);
        }
        else
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "deserialize")));
        }
        resVal = func->Call(obj_, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling deserialize Function!!" ); )
                FatalException(try_catch);
        }
    }

    uint32
        ScriptSerializable::getMyClassTag() const
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mISerializableScriptObj);
        if (!mScriptGetMyClassTagFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptGetMyClassTagFunc);
        }
        else if (obj_->Has(v8::String::NewFromUtf8(isolate, "getMyClassTag")))
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "getMyClassTag")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(obj_->ToString());
                std::cerr << "fatal: ISerializable object " << *objectNameStr << " missing getMyClassTag() Function!!";
                exit(1);
                )
                return 0;
        }
        resVal = func->Call(obj_, 0, 0);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getMyClassTag Function!!" ); )
                FatalException(try_catch);
            return 0;
        }
        if (!resVal->IsUint32())
        {
            DEBUG_ONLY(
                std::cerr << "result mismatch: return value from getMyClassTag Function must be an unsigned integer ("
                << v8_GetFunctionName(func) << " at " << v8_GetFunctionFileAndLine(func) << ")\n";
                exit(1);
                )
                return 0;
        }
        return resVal->Uint32Value();
    }

    ScriptEventHandler::ScriptEventHandler(v8::Local<v8::Function> func)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        mScriptHandlerFunc.Reset(isolate, func);
    }

    bool ScriptEventHandler::handleEvent(EventEmitter* emitter, long inEventType, void* inEventData) throw()
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> jsEvent = v8::Object::New(isolate);
        v8::Local<v8::Object> emitter_ = v8::Local<v8::Object>::New(isolate, emitter->mEventEmitterScriptObj);
        v8::Local<v8::Object> obj1_;
        v8::Local<v8::Object> obj2_;
        jsEvent->Set(v8::String::NewFromUtf8(isolate, "emitter"), emitter_);
        jsEvent->Set(v8::String::NewFromUtf8(isolate, "eventType"),v8::Integer::New(isolate, inEventType));
        switch (inEventType)
        {
            case pdg::eventType_Startup:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "startupReason"),v8::Integer::New(isolate, static_cast<StartupInfo*>(inEventData)->startupReason));

                break;
            case pdg::eventType_Shutdown:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "exitReason"),v8::Integer::New(isolate, static_cast<ShutdownInfo*>(inEventData)->exitReason));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "exitCode"),v8::Integer::New(isolate, static_cast<ShutdownInfo*>(inEventData)->exitCode));
                break;
            case pdg::eventType_Timer:
                if (static_cast<TimerInfo*>(inEventData)->id <= 0)
                {

                    return false;
                }
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "id"),v8::Integer::New(isolate, static_cast<TimerInfo*>(inEventData)->id));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "millisec"),v8::Integer::NewFromUnsigned(isolate, static_cast<TimerInfo*>(inEventData)->millisec));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "msElapsed"),v8::Integer::NewFromUnsigned(isolate, static_cast<TimerInfo*>(inEventData)->msElapsed));

                break;
#ifndef PDG_NO_GUI
            case pdg::eventType_KeyDown:
            case pdg::eventType_KeyUp:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "keyCode"),v8::Integer::New(isolate, static_cast<KeyInfo*>(inEventData)->keyCode));
                break;
            case pdg::eventType_KeyPress:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "shift"),v8::Boolean::New(isolate, static_cast<KeyPressInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "ctrl"),v8::Boolean::New(isolate, static_cast<KeyPressInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "alt"),v8::Boolean::New(isolate, static_cast<KeyPressInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "meta"),v8::Boolean::New(isolate, static_cast<KeyPressInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "unicode"),v8::Integer::New(isolate, static_cast<KeyPressInfo*>(inEventData)->unicode));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "isRepeating"),v8::Boolean::New(isolate, static_cast<KeyPressInfo*>(inEventData)->isRepeating));
                break;
            case pdg::eventType_SpriteTouch:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteTouchInfo*>(inEventData)->touchedSprite->mSpriteScriptObj);
                obj2_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteTouchInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "touchType"),v8::Integer::New(isolate, static_cast<SpriteTouchInfo*>(inEventData)->touchType));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "touchedSprite"), obj1_);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "inLayer"), obj2_);

            case pdg::eventType_MouseDown:
            case pdg::eventType_MouseUp:
            case pdg::eventType_MouseMove:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "shift"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "ctrl"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "alt"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "meta"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "mousePos"),v8_MakeJavascriptPoint(isolate, static_cast<MouseInfo*>(inEventData)->mousePos));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "leftButton"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->leftButton));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "rightButton"),v8::Boolean::New(isolate, static_cast<MouseInfo*>(inEventData)->rightButton));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "buttonNumber"),v8::Integer::NewFromUnsigned(isolate, static_cast<MouseInfo*>(inEventData)->buttonNumber));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "lastClickPos"),v8_MakeJavascriptPoint(isolate, static_cast<MouseInfo*>(inEventData)->lastClickPos));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "lastClickElapsed"),v8::Integer::NewFromUnsigned(isolate, static_cast<MouseInfo*>(inEventData)->lastClickElapsed));
                break;
            case pdg::eventType_ScrollWheel:
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "shift"),v8::Boolean::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "ctrl"),v8::Boolean::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "alt"),v8::Boolean::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "meta"),v8::Boolean::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "horizDelta"),v8::Integer::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->horizDelta));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "vertDelta"),v8::Integer::New(isolate, static_cast<ScrollWheelInfo*>(inEventData)->vertDelta));
                break;
#endif
#ifndef PDG_NO_NETWORK

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
#endif
#ifndef PDG_NO_SOUND
            case pdg::eventType_SoundEvent:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SoundEventInfo*>(inEventData)->sound->mSoundScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "eventCode"),v8::Integer::New(isolate, static_cast<SoundEventInfo*>(inEventData)->eventCode));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "sound"), obj1_);
                break;
#endif
#ifndef PDG_NO_GUI
            case pdg::eventType_PortResized:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<PortResizeInfo*>(inEventData)->port->mPortScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "port"), obj1_);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "screenPos"), v8::Integer::New(isolate, static_cast<PortResizeInfo*>(inEventData)->screenPos));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "oldScreenPos"), v8::Integer::New(isolate, static_cast<PortResizeInfo*>(inEventData)->oldScreenPos));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "oldWidth"), v8::Integer::New(isolate, static_cast<PortResizeInfo*>(inEventData)->oldWidth));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "oldHeight"), v8::Integer::New(isolate, static_cast<PortResizeInfo*>(inEventData)->oldHeight));
                break;
            case pdg::eventType_PortDraw:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<PortDrawInfo*>(inEventData)->port->mPortScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "port"), obj1_);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "frameNum"), v8::Integer::New(isolate, static_cast<PortDrawInfo*>(inEventData)->frameNum));
                break;
#endif
            case pdg::eventType_SpriteCollide:
            case pdg::eventType_SpriteBreak:
                if (inEventType == pdg::eventType_SpriteCollide)
                {
                    if (static_cast<SpriteCollideInfo*>(inEventData)->targetSprite)
                    {
                        obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteCollideInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
                        jsEvent->Set(v8::String::NewFromUtf8(isolate, "targetSprite"), obj1_);
                    }
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "normal"), v8_MakeJavascriptVector(isolate, static_cast<SpriteCollideInfo*>(inEventData)->normal));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "impulse"), v8_MakeJavascriptVector(isolate, static_cast<SpriteCollideInfo*>(inEventData)->impulse));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "force"),v8::Number::New(isolate, static_cast<SpriteCollideInfo*>(inEventData)->force));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "kineticEnergy"),v8::Number::New(isolate, static_cast<SpriteCollideInfo*>(inEventData)->kineticEnergy));
#ifdef PDG_USE_CHIPMUNK_PHYSICS
                    if (static_cast<SpriteCollideInfo*>(inEventData)->arbiter)
                    {
                        jsEvent->Set(v8::String::NewFromUtf8(isolate, "arbiter"), cpArbiterWrap::NewFromCpp(isolate, static_cast<SpriteCollideInfo*>(inEventData)->arbiter));
                    }
#endif
                }
                else
                {
#ifdef PDG_USE_CHIPMUNK_PHYSICS
                    obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "targetSprite"), obj1_);
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "impulse"),v8::Number::New(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->impulse));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "force"),v8::Number::New(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->force));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "breakForce"),v8::Number::New(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->breakForce));
                    jsEvent->Set(v8::String::NewFromUtf8(isolate, "joint"), cpConstraintWrap::NewFromCpp(isolate, static_cast<SpriteJointBreakInfo*>(inEventData)->joint));
#endif
                }

            case pdg::eventType_SpriteAnimate:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteAnimateInfo*>(inEventData)->actingSprite->mSpriteScriptObj);
                obj2_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteAnimateInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "action"),v8::Integer::New(isolate, static_cast<SpriteAnimateInfo*>(inEventData)->action));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "actingSprite"), obj1_);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "inLayer"), obj2_);
                break;
            case pdg::eventType_SpriteLayer:
                obj1_ = v8::Local<v8::Object>::New(isolate, static_cast<SpriteLayerInfo*>(inEventData)->actingLayer->mSpriteLayerScriptObj);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "action"),v8::Integer::New(isolate, static_cast<SpriteLayerInfo*>(inEventData)->action));
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "actingLayer"), obj1_);
                jsEvent->Set(v8::String::NewFromUtf8(isolate, "millisec"),v8::Integer::NewFromUnsigned(isolate, static_cast<SpriteLayerInfo*>(inEventData)->millisec));
                break;
            default:
            {
                std::ostringstream msg;
                msg << "unknown event (" << inEventType << ")";
                s_HaveSavedError = true;
                {
                    std::ostringstream excpt_;
                    excpt_ << msg.str().c_str();
                    v8::Isolate* isolate = v8::Isolate::GetCurrent();
                    s_SavedError.Reset(isolate, v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
                };
                return false;
            }
            break;

        }

        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[1];
        argv[0] = v8::Local<v8::Value>::New(isolate, jsEvent);

        SCRIPT_DEBUG_ONLY( if (this->mIEventHandlerScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 803 << " - NIL JS Object (" "this->mIEventHandlerScriptObj" "|"<<*((void**)&(this->mIEventHandlerScriptObj))<<")\n";
        }
        else if (!this->mIEventHandlerScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 803 << " - NOT JS Object (" "this->mIEventHandlerScriptObj" "|"<<*((void**)&(this->mIEventHandlerScriptObj))<<") : " << (this->mIEventHandlerScriptObj.IsEmpty() ? "empty" : this->mIEventHandlerScriptObj->IsArray() ? "array" : this->mIEventHandlerScriptObj->IsFunction() ? "function" : this->mIEventHandlerScriptObj->IsStringObject() ? "string (object)" : this->mIEventHandlerScriptObj->IsString() ? "string" : this->mIEventHandlerScriptObj->IsNull() ? "null" : this->mIEventHandlerScriptObj->IsUndefined() ? "undefined" : this->mIEventHandlerScriptObj->IsNumberObject() ? "number (object)" : this->mIEventHandlerScriptObj->IsNumber() ? "number" : this->mIEventHandlerScriptObj->IsBoolean() ? "boolean" : this->mIEventHandlerScriptObj->IsDate() ? "date" : this->mIEventHandlerScriptObj->IsRegExp() ? "regexp" : this->mIEventHandlerScriptObj->IsNativeError() ? "error" : this->mIEventHandlerScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = this->mIEventHandlerScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                IEventHandlerWrap* obj__=jswrap::ObjectWrap::Unwrap< IEventHandlerWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< IEventHandlerWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 803 << " - JS Object (""this->mIEventHandlerScriptObj""|"<<*((void**)&(this->mIEventHandlerScriptObj))<<"): " << objName << " - is a subclass of C++ ""IEventHandler""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 803 << " - JS Object (""this->mIEventHandlerScriptObj""|"<<*((void**)&(this->mIEventHandlerScriptObj))<<"): " << objName << " - does not wrap ""IEventHandler""\n";
                }
            }
            else
            {
                IEventHandler* obj = dynamic_cast<IEventHandler*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 803 << " - JS Object (""this->mIEventHandlerScriptObj""|" << *((void**)&(this->mIEventHandlerScriptObj)) << "): " << objName<<" - wraps C++ ""IEventHandler"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, mIEventHandlerScriptObj);
        if (!mScriptHandlerFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptHandlerFunc);
        }
        else if (obj_->Has(v8::String::NewFromUtf8(isolate, "handleEvent")))
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "handleEvent")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(obj_->ToString());
                std::cerr << "fatal: IEventHandler object " << *objectNameStr << " missing handleEvent() Function!!\n";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(obj_, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling event Handler!!" ); )
                FatalException(try_catch);
            return false;
        }
        if (!resVal->IsBoolean())
        {
#ifdef DEBUG
            std::cerr << "result mismatch: return value from event handler Function must be a boolean ("
                << v8_GetFunctionName(func) << " at " << v8_GetFunctionFileAndLine(func) << ")\n";
            exit(1);
#else
            return false;
#endif
        }
        return resVal->BooleanValue();
    }

    ScriptAnimationHelper::ScriptAnimationHelper(v8::Local<v8::Function> func)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        mScriptAnimateFunc.Reset(isolate, func);
    }

    bool ScriptAnimationHelper::animate(Animated* what, ms_delta msElapsed) throw()
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();

        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Object>::New(isolate, what->mAnimatedScriptObj);
        argv[1] = v8::Local<v8::Value>::New(isolate, v8::Integer::NewFromUnsigned(isolate, msElapsed));

        SCRIPT_DEBUG_ONLY( if (what->mAnimatedScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 857 << " - NIL JS Object (" "what->mAnimatedScriptObj" "|"<<*((void**)&(what->mAnimatedScriptObj))<<")\n";
        }
        else if (!what->mAnimatedScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 857 << " - NOT JS Object (" "what->mAnimatedScriptObj" "|"<<*((void**)&(what->mAnimatedScriptObj))<<") : " << (what->mAnimatedScriptObj.IsEmpty() ? "empty" : what->mAnimatedScriptObj->IsArray() ? "array" : what->mAnimatedScriptObj->IsFunction() ? "function" : what->mAnimatedScriptObj->IsStringObject() ? "string (object)" : what->mAnimatedScriptObj->IsString() ? "string" : what->mAnimatedScriptObj->IsNull() ? "null" : what->mAnimatedScriptObj->IsUndefined() ? "undefined" : what->mAnimatedScriptObj->IsNumberObject() ? "number (object)" : what->mAnimatedScriptObj->IsNumber() ? "number" : what->mAnimatedScriptObj->IsBoolean() ? "boolean" : what->mAnimatedScriptObj->IsDate() ? "date" : what->mAnimatedScriptObj->IsRegExp() ? "regexp" : what->mAnimatedScriptObj->IsNativeError() ? "error" : what->mAnimatedScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = what->mAnimatedScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                AnimatedWrap* obj__=jswrap::ObjectWrap::Unwrap< AnimatedWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< AnimatedWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 857 << " - JS Object (""what->mAnimatedScriptObj""|"<<*((void**)&(what->mAnimatedScriptObj))<<"): " << objName << " - is a subclass of C++ ""Animated""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 857 << " - JS Object (""what->mAnimatedScriptObj""|"<<*((void**)&(what->mAnimatedScriptObj))<<"): " << objName << " - does not wrap ""Animated""\n";
                }
            }
            else
            {
                Animated* obj = dynamic_cast<Animated*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 857 << " - JS Object (""what->mAnimatedScriptObj""|" << *((void**)&(what->mAnimatedScriptObj)) << "): " << objName<<" - wraps C++ ""Animated"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mIAnimationHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 858 << " - NIL JS Object (" "this->mIAnimationHelperScriptObj" "|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<")\n";
        }
        else if (!this->mIAnimationHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 858 << " - NOT JS Object (" "this->mIAnimationHelperScriptObj" "|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<") : " << (this->mIAnimationHelperScriptObj.IsEmpty() ? "empty" : this->mIAnimationHelperScriptObj->IsArray() ? "array" : this->mIAnimationHelperScriptObj->IsFunction() ? "function" : this->mIAnimationHelperScriptObj->IsStringObject() ? "string (object)" : this->mIAnimationHelperScriptObj->IsString() ? "string" : this->mIAnimationHelperScriptObj->IsNull() ? "null" : this->mIAnimationHelperScriptObj->IsUndefined() ? "undefined" : this->mIAnimationHelperScriptObj->IsNumberObject() ? "number (object)" : this->mIAnimationHelperScriptObj->IsNumber() ? "number" : this->mIAnimationHelperScriptObj->IsBoolean() ? "boolean" : this->mIAnimationHelperScriptObj->IsDate() ? "date" : this->mIAnimationHelperScriptObj->IsRegExp() ? "regexp" : this->mIAnimationHelperScriptObj->IsNativeError() ? "error" : this->mIAnimationHelperScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = this->mIAnimationHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                IAnimationHelperWrap* obj__=jswrap::ObjectWrap::Unwrap< IAnimationHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< IAnimationHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 858 << " - JS Object (""this->mIAnimationHelperScriptObj""|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<"): " << objName << " - is a subclass of C++ ""IAnimationHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 858 << " - JS Object (""this->mIAnimationHelperScriptObj""|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<"): " << objName << " - does not wrap ""IAnimationHelper""\n";
                }
            }
            else
            {
                IAnimationHelper* obj = dynamic_cast<IAnimationHelper*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 858 << " - JS Object (""this->mIAnimationHelperScriptObj""|" << *((void**)&(this->mIAnimationHelperScriptObj)) << "): " << objName<<" - wraps C++ ""IAnimationHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mIAnimationHelperScriptObj);
        if (!mScriptAnimateFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptAnimateFunc);
        }
        else if (obj_->Has(v8::String::NewFromUtf8(isolate, "animate")))
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "animate")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(obj_->ToString());
                std::cerr << "fatal: IAnimationHelper object " << *objectNameStr << " missing animate() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(obj_, 2, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Animation Helper!!" ); )
                FatalException(try_catch);
            return false;
        }
        if (!resVal->IsBoolean())
        {
#ifdef DEBUG
            std::cerr << "result mismatch: return value from animate helper Function must be a boolean ("
                << v8_GetFunctionName(func) << " at " << v8_GetFunctionFileAndLine(func) << ")\n";
            exit(1);
#else
            return false;
#endif
        }
        return resVal->BooleanValue();
    }

    ScriptSpriteCollideHelper::ScriptSpriteCollideHelper(v8::Local<v8::Function> func)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        mScriptAllowCollisionFunc.Reset(isolate, func);
    }

    bool ScriptSpriteCollideHelper::allowCollision(Sprite* sprite, Sprite* withSprite) throw()
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();

        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Object>::New(isolate, sprite->mSpriteScriptObj);
        argv[1] = v8::Local<v8::Object>::New(isolate, withSprite->mSpriteScriptObj);

        SCRIPT_DEBUG_ONLY( if (sprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 912 << " - NIL JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else if (!sprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 912 << " - NOT JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<") : " << (sprite->mSpriteScriptObj.IsEmpty() ? "empty" : sprite->mSpriteScriptObj->IsArray() ? "array" : sprite->mSpriteScriptObj->IsFunction() ? "function" : sprite->mSpriteScriptObj->IsStringObject() ? "string (object)" : sprite->mSpriteScriptObj->IsString() ? "string" : sprite->mSpriteScriptObj->IsNull() ? "null" : sprite->mSpriteScriptObj->IsUndefined() ? "undefined" : sprite->mSpriteScriptObj->IsNumberObject() ? "number (object)" : sprite->mSpriteScriptObj->IsNumber() ? "number" : sprite->mSpriteScriptObj->IsBoolean() ? "boolean" : sprite->mSpriteScriptObj->IsDate() ? "date" : sprite->mSpriteScriptObj->IsRegExp() ? "regexp" : sprite->mSpriteScriptObj->IsNativeError() ? "error" : sprite->mSpriteScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = sprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 912 << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of C++ ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 912 << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 912 << " - JS Object (""sprite->mSpriteScriptObj""|" << *((void**)&(sprite->mSpriteScriptObj)) << "): " << objName<<" - wraps C++ ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (withSprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 913 << " - NIL JS Object (" "withSprite->mSpriteScriptObj" "|"<<*((void**)&(withSprite->mSpriteScriptObj))<<")\n";
        }
        else if (!withSprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 913 << " - NOT JS Object (" "withSprite->mSpriteScriptObj" "|"<<*((void**)&(withSprite->mSpriteScriptObj))<<") : " << (withSprite->mSpriteScriptObj.IsEmpty() ? "empty" : withSprite->mSpriteScriptObj->IsArray() ? "array" : withSprite->mSpriteScriptObj->IsFunction() ? "function" : withSprite->mSpriteScriptObj->IsStringObject() ? "string (object)" : withSprite->mSpriteScriptObj->IsString() ? "string" : withSprite->mSpriteScriptObj->IsNull() ? "null" : withSprite->mSpriteScriptObj->IsUndefined() ? "undefined" : withSprite->mSpriteScriptObj->IsNumberObject() ? "number (object)" : withSprite->mSpriteScriptObj->IsNumber() ? "number" : withSprite->mSpriteScriptObj->IsBoolean() ? "boolean" : withSprite->mSpriteScriptObj->IsDate() ? "date" : withSprite->mSpriteScriptObj->IsRegExp() ? "regexp" : withSprite->mSpriteScriptObj->IsNativeError() ? "error" : withSprite->mSpriteScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = withSprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 913 << " - JS Object (""withSprite->mSpriteScriptObj""|"<<*((void**)&(withSprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of C++ ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 913 << " - JS Object (""withSprite->mSpriteScriptObj""|"<<*((void**)&(withSprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 913 << " - JS Object (""withSprite->mSpriteScriptObj""|" << *((void**)&(withSprite->mSpriteScriptObj)) << "): " << objName<<" - wraps C++ ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mISpriteCollideHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 914 << " - NIL JS Object (" "this->mISpriteCollideHelperScriptObj" "|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<")\n";
        }
        else if (!this->mISpriteCollideHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 914 << " - NOT JS Object (" "this->mISpriteCollideHelperScriptObj" "|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<") : " << (this->mISpriteCollideHelperScriptObj.IsEmpty() ? "empty" : this->mISpriteCollideHelperScriptObj->IsArray() ? "array" : this->mISpriteCollideHelperScriptObj->IsFunction() ? "function" : this->mISpriteCollideHelperScriptObj->IsStringObject() ? "string (object)" : this->mISpriteCollideHelperScriptObj->IsString() ? "string" : this->mISpriteCollideHelperScriptObj->IsNull() ? "null" : this->mISpriteCollideHelperScriptObj->IsUndefined() ? "undefined" : this->mISpriteCollideHelperScriptObj->IsNumberObject() ? "number (object)" : this->mISpriteCollideHelperScriptObj->IsNumber() ? "number" : this->mISpriteCollideHelperScriptObj->IsBoolean() ? "boolean" : this->mISpriteCollideHelperScriptObj->IsDate() ? "date" : this->mISpriteCollideHelperScriptObj->IsRegExp() ? "regexp" : this->mISpriteCollideHelperScriptObj->IsNativeError() ? "error" : this->mISpriteCollideHelperScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = this->mISpriteCollideHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISpriteCollideHelperWrap* obj__=jswrap::ObjectWrap::Unwrap< ISpriteCollideHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< ISpriteCollideHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 914 << " - JS Object (""this->mISpriteCollideHelperScriptObj""|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<"): " << objName << " - is a subclass of C++ ""ISpriteCollideHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 914 << " - JS Object (""this->mISpriteCollideHelperScriptObj""|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<"): " << objName << " - does not wrap ""ISpriteCollideHelper""\n";
                }
            }
            else
            {
                ISpriteCollideHelper* obj = dynamic_cast<ISpriteCollideHelper*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 914 << " - JS Object (""this->mISpriteCollideHelperScriptObj""|" << *((void**)&(this->mISpriteCollideHelperScriptObj)) << "): " << objName<<" - wraps C++ ""ISpriteCollideHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mISpriteCollideHelperScriptObj);
        if (!mScriptAllowCollisionFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptAllowCollisionFunc);
        }
        else if (obj_->Has(v8::String::NewFromUtf8(isolate, "allowCollision")))
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "allowCollision")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(obj_->ToString());
                std::cerr << "fatal: ISpriteCollideHelper object " << *objectNameStr << " missing allowCollision() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(obj_, 2, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Sprite Collide Helper!!" ); )
                FatalException(try_catch);
            return false;
        }

        return resVal->BooleanValue();
    }

#ifndef PDG_NO_GUI

    ScriptSpriteDrawHelper::ScriptSpriteDrawHelper(v8::Local<v8::Function> func)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        mScriptDrawFunc.Reset(isolate, func);
    }

    bool ScriptSpriteDrawHelper::draw(Sprite* sprite, Port* port) throw()
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Object>::New(isolate, sprite->mSpriteScriptObj);
        argv[1] = v8::Local<v8::Object>::New(isolate, port->mPortScriptObj);

        SCRIPT_DEBUG_ONLY( if (sprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 969 << " - NIL JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else if (!sprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 969 << " - NOT JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<") : " << (sprite->mSpriteScriptObj.IsEmpty() ? "empty" : sprite->mSpriteScriptObj->IsArray() ? "array" : sprite->mSpriteScriptObj->IsFunction() ? "function" : sprite->mSpriteScriptObj->IsStringObject() ? "string (object)" : sprite->mSpriteScriptObj->IsString() ? "string" : sprite->mSpriteScriptObj->IsNull() ? "null" : sprite->mSpriteScriptObj->IsUndefined() ? "undefined" : sprite->mSpriteScriptObj->IsNumberObject() ? "number (object)" : sprite->mSpriteScriptObj->IsNumber() ? "number" : sprite->mSpriteScriptObj->IsBoolean() ? "boolean" : sprite->mSpriteScriptObj->IsDate() ? "date" : sprite->mSpriteScriptObj->IsRegExp() ? "regexp" : sprite->mSpriteScriptObj->IsNativeError() ? "error" : sprite->mSpriteScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = sprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 969 << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of C++ ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 969 << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 969 << " - JS Object (""sprite->mSpriteScriptObj""|" << *((void**)&(sprite->mSpriteScriptObj)) << "): " << objName<<" - wraps C++ ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (port->mPortScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 970 << " - NIL JS Object (" "port->mPortScriptObj" "|"<<*((void**)&(port->mPortScriptObj))<<")\n";
        }
        else if (!port->mPortScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 970 << " - NOT JS Object (" "port->mPortScriptObj" "|"<<*((void**)&(port->mPortScriptObj))<<") : " << (port->mPortScriptObj.IsEmpty() ? "empty" : port->mPortScriptObj->IsArray() ? "array" : port->mPortScriptObj->IsFunction() ? "function" : port->mPortScriptObj->IsStringObject() ? "string (object)" : port->mPortScriptObj->IsString() ? "string" : port->mPortScriptObj->IsNull() ? "null" : port->mPortScriptObj->IsUndefined() ? "undefined" : port->mPortScriptObj->IsNumberObject() ? "number (object)" : port->mPortScriptObj->IsNumber() ? "number" : port->mPortScriptObj->IsBoolean() ? "boolean" : port->mPortScriptObj->IsDate() ? "date" : port->mPortScriptObj->IsRegExp() ? "regexp" : port->mPortScriptObj->IsNativeError() ? "error" : port->mPortScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = port->mPortScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                PortWrap* obj__=jswrap::ObjectWrap::Unwrap< PortWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< PortWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 970 << " - JS Object (""port->mPortScriptObj""|"<<*((void**)&(port->mPortScriptObj))<<"): " << objName << " - is a subclass of C++ ""Port""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 970 << " - JS Object (""port->mPortScriptObj""|"<<*((void**)&(port->mPortScriptObj))<<"): " << objName << " - does not wrap ""Port""\n";
                }
            }
            else
            {
                Port* obj = dynamic_cast<Port*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 970 << " - JS Object (""port->mPortScriptObj""|" << *((void**)&(port->mPortScriptObj)) << "): " << objName<<" - wraps C++ ""Port"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mISpriteDrawHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< 971 << " - NIL JS Object (" "this->mISpriteDrawHelperScriptObj" "|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<")\n";
        }
        else if (!this->mISpriteDrawHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< 971 << " - NOT JS Object (" "this->mISpriteDrawHelperScriptObj" "|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<") : " << (this->mISpriteDrawHelperScriptObj.IsEmpty() ? "empty" : this->mISpriteDrawHelperScriptObj->IsArray() ? "array" : this->mISpriteDrawHelperScriptObj->IsFunction() ? "function" : this->mISpriteDrawHelperScriptObj->IsStringObject() ? "string (object)" : this->mISpriteDrawHelperScriptObj->IsString() ? "string" : this->mISpriteDrawHelperScriptObj->IsNull() ? "null" : this->mISpriteDrawHelperScriptObj->IsUndefined() ? "undefined" : this->mISpriteDrawHelperScriptObj->IsNumberObject() ? "number (object)" : this->mISpriteDrawHelperScriptObj->IsNumber() ? "number" : this->mISpriteDrawHelperScriptObj->IsBoolean() ? "boolean" : this->mISpriteDrawHelperScriptObj->IsDate() ? "date" : this->mISpriteDrawHelperScriptObj->IsRegExp() ? "regexp" : this->mISpriteDrawHelperScriptObj->IsNativeError() ? "error" : this->mISpriteDrawHelperScriptObj->IsObject() ? "object" : "unknown") << "\n";
        }
        else
        {
            v8::Local<v8::Object> obj_ = this->mISpriteDrawHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISpriteDrawHelperWrap* obj__=jswrap::ObjectWrap::Unwrap< ISpriteDrawHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Local<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap< ISpriteDrawHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< 971 << " - JS Object (""this->mISpriteDrawHelperScriptObj""|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<"): " << objName << " - is a subclass of C++ ""ISpriteDrawHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< 971 << " - JS Object (""this->mISpriteDrawHelperScriptObj""|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<"): " << objName << " - does not wrap ""ISpriteDrawHelper""\n";
                }
            }
            else
            {
                ISpriteDrawHelper* obj = dynamic_cast<ISpriteDrawHelper*>(obj__->getCppObject());
                    std::cout << __func__<<":"<< 971 << " - JS Object (""this->mISpriteDrawHelperScriptObj""|" << *((void**)&(this->mISpriteDrawHelperScriptObj)) << "): " << objName<<" - wraps C++ ""ISpriteDrawHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Local<v8::Function> func;
        v8::Local<v8::Object> obj_ = v8::Local<v8::Object>::New(isolate, this->mISpriteDrawHelperScriptObj);
        if (!mScriptDrawFunc.IsEmpty())
        {
            func = v8::Local<v8::Function>::New(isolate, mScriptDrawFunc);
        }
        else if (obj_->Has(v8::String::NewFromUtf8(isolate, "draw")))
        {
            func = v8::Local<v8::Function>::Cast(obj_->Get(v8::String::NewFromUtf8(isolate, "draw")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(obj_->ToString());
                std::cerr << "fatal: IDrawSpriteHelper object " << *objectNameStr << " missing draw() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(obj_, 2, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Sprite Draw Helper!!" ); )
                FatalException(try_catch);
            return false;
        }

        return resVal->BooleanValue();
    }
#endif

    v8::Local<v8::Value> EncodeBinary(const void *buf, size_t len)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::EscapableHandleScope scope(isolate);
        const uint8 *cbuf = static_cast<const uint8*>(buf);
        uint16* twobytebuf = new uint16[len];
        for (size_t i = 0; i < len; i++)
        {
            twobytebuf[i] = cbuf[i];
        }
        v8::Local<v8::String> chunk = v8::String::NewFromTwoByte(isolate, twobytebuf, v8::String::kNormalString, len);
        delete [] twobytebuf;
        return scope.Escape(chunk);
    }

    void* DecodeBinary(v8::Local<v8::Value> val, size_t* outLen)
    {
        v8::Local<v8::String> str = val->ToString();
        size_t buflen = str->Length();
        if (outLen)
        {
            *outLen = buflen;
        }

        uint16_t * twobytebuf = new uint16_t[buflen];
        str->Write(twobytebuf, 0, buflen);

        char* buf = (char*)std::malloc(buflen);
        for (size_t i = 0; i < buflen; i++)
        {
            unsigned char* bp = reinterpret_cast<unsigned char*>(&twobytebuf[i]);
            buf[i] = *bp;
        }
        delete [] twobytebuf;
        return buf;
    }

    float CallScriptEasingFunc(int which, ms_delta ut, float b, float c, ms_delta ud)
    {
        if (which > gNumCustomEasings)
        {
#ifdef DEBUG
            std::cerr << "logic error: attempting to call an unregistered easing function #"
                << which << "(only "<< gNumCustomEasings <<" custom easings have been"
                " registered via registerEasingFunction())\n";
            exit(1);
#else
            return 0.0f;
#endif
        }
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::TryCatch try_catch(isolate);

        v8::Local<v8::Value> argv[4];
        argv[0] = v8::Local<v8::Value>::New(isolate, v8::Integer::NewFromUnsigned(isolate, ut));
        argv[1] = v8::Local<v8::Value>::New(isolate, v8::Number::New(isolate, b));
        argv[2] = v8::Local<v8::Value>::New(isolate, v8::Number::New(isolate, c));
        argv[3] = v8::Local<v8::Value>::New(isolate, v8::Integer::NewFromUnsigned(isolate, ud));
        v8::Local<v8::Function> easingfunc_ = v8::Local<v8::Function>::New(isolate, s_CustomScriptEasing[which]);

        v8::Local<v8::Value> resVal = easingfunc_->Call(isolate->GetCurrentContext()->Global(), 4, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Easing Function!!" ); )
                FatalException(try_catch);
            return 0.0f;
        }
        if (!resVal->IsNumber())
        {

#ifdef DEBUG
            std::cerr << "result mismatch: return value from easing Function must be a Number ("
                << v8_GetFunctionName(easingfunc_) << " at "
                << v8_GetFunctionFileAndLine(easingfunc_) << ")\n";
            exit(1);
#else
            return 0.0f;
#endif
        }
        return resVal->NumberValue();
    }

    void RegisterEasingFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.Length() == 1 && args[0]->IsNull())
        {
            { args.GetReturnValue().Set( v8::String::NewFromUtf8(isolate, "undefined" " function" "(function easingFunc)" " - " "") ); return; };
        };
        if (args.Length() != 1)
            v8_ThrowArgCountException(isolate, args.Length(), 1);
        if (!args[1 -1]->IsFunction())
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be a function (""easingFunc"")";
            isolate->ThrowException( v8::Exception::TypeError( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
        }
        v8::Local<v8::Function> easingFunc = v8::Local<v8::Function>::Cast(args[1 -1]);;
        v8::Local<v8::Function> jsEasingFunc = v8::Local<v8::Function>::New(isolate, easingFunc);
        if (gNumCustomEasings >= MAX_CUSTOM_EASINGS)
        {
            std::ostringstream excpt_;
            excpt_ << "Can't register any more custom easing functions!!";
            isolate->ThrowException( v8::Exception::Error( v8::String::NewFromUtf8(isolate, excpt_.str().c_str())));
        }
        else
        {
            s_CustomScriptEasing[gNumCustomEasings].Reset(isolate, jsEasingFunc);
            v8::String::Utf8Value funcNameStr(easingFunc->GetName()->ToString());
            int funcId = NUM_BUILTIN_EASINGS + gNumCustomEasings;
            CallScriptEasingFunc(gNumCustomEasings, 0, 0.0f, 0.0f, 1);
            gNumCustomEasings++;
            v8::Local<v8::Object> bind_ = v8::Local<v8::Object>::New(isolate, s_BindingTarget);
            bind_->Set(v8::String::NewFromUtf8(isolate, "*funcNameStr", v8::String::kInternalizedString), v8::Integer::New(isolate, funcId));
            DEBUG_ONLY( OS::_DOUT( "Registered custom easing Function %d as constant name %s [%d]",
                gNumCustomEasings, *funcNameStr, funcId); )
        }
        args.GetReturnValue().SetUndefined();
    }

    void FinishedScriptSetup(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Isolate* isolate = args.GetIsolate();
        scriptSetupCompleted();
        args.GetReturnValue().SetUndefined();
    }

    SCRIPT_DEBUG_ONLY(
        static size_t sLastHeapUsed = 0;
        static long sIdleLastHeapReport = OS::getMilliseconds();
        )

        void initBindings(v8::Local<v8::Object> target);

    void initBindings(v8::Local<v8::Object> target)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();

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

        s_BindingTarget.Reset(isolate, target);

        MemBlockWrap::Init(isolate, target);;
        FileManagerWrap::Init(isolate, target);;
        LogManagerWrap::Init(isolate, target);;
        ConfigManagerWrap::Init(isolate, target);;
        ResourceManagerWrap::Init(isolate, target);;
        SerializerWrap::Init(isolate, target);;
        DeserializerWrap::Init(isolate, target);;
        ISerializableWrap::Init(isolate, target);;
        IEventHandlerWrap::Init(isolate, target);;
        EventEmitterWrap::Init(isolate, target);;
        EventManagerWrap::Init(isolate, target);;
        TimerManagerWrap::Init(isolate, target);;
        IAnimationHelperWrap::Init(isolate, target);;
        AnimatedWrap::Init(isolate, target);;
#ifdef PDG_USE_CHIPMUNK_PHYSICS
        cpArbiterWrap::Init(isolate, target);;
        cpConstraintWrap::Init(isolate, target);;
        cpSpaceWrap::Init(isolate, target);;
#endif
#ifndef PDG_NO_GUI
        ISpriteDrawHelperWrap::Init(isolate, target);;
#endif
        ISpriteCollideHelperWrap::Init(isolate, target);;
        SpriteWrap::Init(isolate, target);;
        SpriteLayerWrap::Init(isolate, target);;
        TileLayerWrap::Init(isolate, target);;
        ImageWrap::Init(isolate, target);;
        ImageStripWrap::Init(isolate, target);;
#ifndef PDG_NO_GUI
        FontWrap::Init(isolate, target);;
        PortWrap::Init(isolate, target);;
        GraphicsManagerWrap::Init(isolate, target);;
#endif
#ifndef PDG_NO_SOUND
        SoundWrap::Init(isolate, target);;
        SoundManagerWrap::Init(isolate, target);;
#endif

        target->Set(v8::String::NewFromUtf8(isolate, "_idle", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, Idle)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "_run", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, Run)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "_quit", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, Quit)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "_isQuitting", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, IsQuitting)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "_finishedScriptSetup", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, FinishedScriptSetup)->GetFunction());;

        target->Set(v8::String::NewFromUtf8(isolate, "rand", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GameCriticalRandom)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "srand", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, Srand)->GetFunction());;

        target->Set(v8::String::NewFromUtf8(isolate, "setSerializationDebugMode", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, SetSerializationDebugMode)->GetFunction());;

        target->Set(v8::String::NewFromUtf8(isolate, "registerEasingFunction", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, RegisterEasingFunction)->GetFunction());;

        target->Set(v8::String::NewFromUtf8(isolate, "getFileManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetFileManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "getLogManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetLogManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "getConfigManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetConfigManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "getResourceManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetResourceManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "getEventManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetEventManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "getTimerManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetTimerManager)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "registerSerializableClass", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, RegisterSerializableClass)->GetFunction());;

#ifndef PDG_NO_GUI
        target->Set(v8::String::NewFromUtf8(isolate, "getGraphicsManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetGraphicsManager)->GetFunction());;
#endif
#ifndef PDG_NO_SOUND
        target->Set(v8::String::NewFromUtf8(isolate, "getSoundManager", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, GetSoundManager)->GetFunction());;
#endif

        target->Set(v8::String::NewFromUtf8(isolate, "createSpriteLayer", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, CreateSpriteLayer)->GetFunction());;
#ifdef PDG_SCML_SUPPORT
        target->Set(v8::String::NewFromUtf8(isolate, "createSpriteLayerFromSCMLFile", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, CreateSpriteLayerFromSCMLFile)->GetFunction());;
#endif
        target->Set(v8::String::NewFromUtf8(isolate, "cleanupSpriteLayer", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, CleanupSpriteLayer)->GetFunction());;
        target->Set(v8::String::NewFromUtf8(isolate, "createTileLayer", v8::String::kInternalizedString), v8::FunctionTemplate::New(isolate, CreateTileLayer)->GetFunction());;

        target->ForceSet(v8::String::NewFromUtf8(isolate, "all_events", v8::String::kInternalizedString), v8::Integer::New(isolate, all_events), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_Shutdown", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_Shutdown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_Timer", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_Timer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_KeyDown", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_KeyDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_KeyUp", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_KeyUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_KeyPress", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_KeyPress), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_MouseDown", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_MouseDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_MouseUp", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_MouseUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_MouseMove", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_MouseMove), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_MouseEnter", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_MouseEnter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_MouseLeave", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_MouseLeave), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_PortResized", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_PortResized), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_ScrollWheel", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_ScrollWheel), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SpriteTouch", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SpriteTouch), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SpriteAnimate", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SpriteAnimate), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SpriteLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SpriteLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SpriteCollide", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SpriteCollide), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SpriteBreak", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SpriteBreak), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_SoundEvent", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_SoundEvent), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "eventType_PortDraw", v8::String::kInternalizedString), v8::Integer::New(isolate, eventType_PortDraw), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "soundEvent_DonePlaying", v8::String::kInternalizedString), v8::Integer::New(isolate, soundEvent_DonePlaying), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "soundEvent_Looping", v8::String::kInternalizedString), v8::Integer::New(isolate, soundEvent_Looping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "soundEvent_FailedToPlay", v8::String::kInternalizedString), v8::Integer::New(isolate, soundEvent_FailedToPlay), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Break", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Break), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Home", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Home), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_End", v8::String::kInternalizedString), v8::Integer::New(isolate, key_End), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Clear", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Clear), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Help", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Help), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Pause", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Pause), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Mute", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Mute), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Backspace", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Backspace), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Delete", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Delete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Tab", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Tab), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_PageUp", v8::String::kInternalizedString), v8::Integer::New(isolate, key_PageUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_PageDown", v8::String::kInternalizedString), v8::Integer::New(isolate, key_PageDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Return", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Return), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Enter", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Enter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F1", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F1), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F2", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F2), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F3", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F3), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F4", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F4), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F5", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F5), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F6", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F6), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F7", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F7), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F8", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F8), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F9", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F9), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F10", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F10), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F11", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F11), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_F12", v8::String::kInternalizedString), v8::Integer::New(isolate, key_F12), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_FirstF", v8::String::kInternalizedString), v8::Integer::New(isolate, key_FirstF), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_LastF", v8::String::kInternalizedString), v8::Integer::New(isolate, key_LastF), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Insert", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Insert), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_Escape", v8::String::kInternalizedString), v8::Integer::New(isolate, key_Escape), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_LeftArrow", v8::String::kInternalizedString), v8::Integer::New(isolate, key_LeftArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_RightArrow", v8::String::kInternalizedString), v8::Integer::New(isolate, key_RightArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_UpArrow", v8::String::kInternalizedString), v8::Integer::New(isolate, key_UpArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_DownArrow", v8::String::kInternalizedString), v8::Integer::New(isolate, key_DownArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "key_FirstPrintable", v8::String::kInternalizedString), v8::Integer::New(isolate, key_FirstPrintable), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_Normal", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_Normal), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_Rotated180", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_Rotated180), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_Rotated90Clockwise", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_Rotated90Clockwise), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_Rotated90CounterClockwise", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_Rotated90CounterClockwise), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_FaceUp", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_FaceUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "screenPos_FaceDown", v8::String::kInternalizedString), v8::Integer::New(isolate, screenPos_FaceDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

#ifndef PDG_NO_GUI
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_Plain", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_Plain), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_Bold", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_Bold), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_Italic", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_Italic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_Underline", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_Underline), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_Centered", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_Centered), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_LeftJustified", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_LeftJustified), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "textStyle_RightJustified", v8::String::kInternalizedString), v8::Integer::New(isolate, textStyle_RightJustified), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
#endif

        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_None", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_Height", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_Height), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_Width", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_Width), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_Inside", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_Inside), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_Fill", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_Fill), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "fit_FillKeepProportions", v8::String::kInternalizedString), v8::Integer::New(isolate, fit_FillKeepProportions), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "init_CreateUniqueNewFile", v8::String::kInternalizedString), v8::Integer::New(isolate, LogManager::init_CreateUniqueNewFile), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "init_OverwriteExisting", v8::String::kInternalizedString), v8::Integer::New(isolate, LogManager::init_OverwriteExisting), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "init_AppendToExisting", v8::String::kInternalizedString), v8::Integer::New(isolate, LogManager::init_AppendToExisting), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "init_StdOut", v8::String::kInternalizedString), v8::Integer::New(isolate, LogManager::init_StdOut), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "init_StdErr", v8::String::kInternalizedString), v8::Integer::New(isolate, LogManager::init_StdErr), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "duration_Constant", v8::String::kInternalizedString), v8::Integer::New(isolate, duration_Constant), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "duration_Instantaneous", v8::String::kInternalizedString), v8::Integer::New(isolate, duration_Instantaneous), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_StartToEnd", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_StartToEnd), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_EndToStart", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_EndToStart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_Unidirectional", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_Unidirectional), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_Bidirectional", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_Bidirectional), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_NoLooping", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_NoLooping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "animate_Looping", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::animate_Looping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "start_FromFirstFrame", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::start_FromFirstFrame), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "start_FromLastFrame", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::start_FromLastFrame), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "all_Frames", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::all_Frames), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_CollideSprite", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_CollideSprite), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_CollideWall", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_CollideWall), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_Offscreen", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_Offscreen), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_Onscreen", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_Onscreen), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_ExitLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_ExitLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_AnimationLoop", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_AnimationLoop), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_AnimationEnd", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_AnimationEnd), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_FadeComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_FadeComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_FadeInComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_FadeInComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_FadeOutComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_FadeOutComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_JointBreak", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::action_JointBreak), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "touch_MouseEnter", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::touch_MouseEnter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "touch_MouseLeave", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::touch_MouseLeave), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "touch_MouseDown", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::touch_MouseDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "touch_MouseUp", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::touch_MouseUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "touch_MouseClick", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::touch_MouseClick), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_None", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_Point", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_Point), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_BoundingBox", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_BoundingBox), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_CollisionRadius", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_CollisionRadius), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_AlphaChannel", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_AlphaChannel), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "collide_Last", v8::String::kInternalizedString), v8::Integer::New(isolate, Sprite::collide_Last), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_ErasePort", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_ErasePort), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_PreDrawLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_PreDrawLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_PostDrawLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_PostDrawLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_DrawPortComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_DrawPortComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_AnimationStart", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_AnimationStart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_PreAnimateLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_PreAnimateLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_PostAnimateLayer", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_PostAnimateLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_AnimationComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_AnimationComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_ZoomComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_ZoomComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_LayerFadeInComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_FadeInComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "action_LayerFadeOutComplete", v8::String::kInternalizedString), v8::Integer::New(isolate, SpriteLayer::action_FadeOutComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "facing_North", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::facing_North), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "facing_East", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::facing_East), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "facing_South", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::facing_South), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "facing_West", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::facing_West), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "facing_Ignore", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::facing_Ignore), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "flipped_None", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::flipped_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "flipped_Horizontal", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::flipped_Horizontal), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "flipped_Vertical", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::flipped_Vertical), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "flipped_Both", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::flipped_Both), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "flipped_Ignore", v8::String::kInternalizedString), v8::Integer::New(isolate, TileLayer::flipped_Ignore), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "timer_OneShot", v8::String::kInternalizedString), v8::Boolean::New(isolate, timer_OneShot), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "timer_Repeating", v8::String::kInternalizedString), v8::Boolean::New(isolate, timer_Repeating), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "timer_Never", v8::String::kInternalizedString), v8::Integer::New(isolate, timer_Never), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "linearTween", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::linearTween), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInQuad", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutQuad", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutQuad", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInCubic", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutCubic", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutCubic", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInQuart", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInQuart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutQuart", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutQuart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutCubic", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInQuint", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutQuint", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutQuint", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInSine", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutSine", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutSine", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInExpo", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutExpo", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutExpo", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInCirc", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutCirc", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutCirc", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInBounce", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutBounce", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutBounce", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInBack", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeOutBack", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeOutBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "easeInOutBack", v8::String::kInternalizedString), v8::Integer::New(isolate, EasingFuncRef::easeInOutBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Positions", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Positions), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_ZOrder", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_ZOrder), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Sizes", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Sizes), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Animations", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Animations), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Motion", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Motion), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Forces", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Forces), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Physics", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Physics), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_LayerDraw", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_LayerDraw), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_ImageRefs", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_ImageRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_SCMLRefs", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_SCMLRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_HelperRefs", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_HelperRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_HelperObjs", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_HelperObjs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_InitialData", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_InitialData), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Micro", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Micro), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Update", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Update), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->ForceSet(v8::String::NewFromUtf8(isolate, "ser_Full", v8::String::kInternalizedString), v8::Integer::New(isolate, ser_Full), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
    }

    void CreateSingletons()
    {

        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        FileManagerWrap::GetScriptSingletonInstance(isolate);

        LogManagerWrap::getSingletonInstance();
        ConfigManagerWrap::getSingletonInstance();
        ResourceManagerWrap::getSingletonInstance();
        EventManagerWrap::getSingletonInstance();
        TimerManagerWrap::getSingletonInstance();
#ifndef PDG_NO_GUI
        GraphicsManagerWrap::getSingletonInstance();
#endif
#ifndef PDG_NO_SOUND
        SoundManagerWrap::getSingletonInstance();
#endif
#ifndef PDG_NO_NETWORK
        NetworkManager::getSingletonInstance();
#endif
    }

}


extern "C" void pdg_LibContainerDoIdle()
{

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    isolate->IdleNotificationDeadline(0.01f);

    SCRIPT_DEBUG_ONLY(
        if ((sIdleLastHeapReport + 1000) < OS::getMilliseconds())
    {
        v8::HeapStatistics hs;
            v8::V8::GetHeapStatistics(&hs);
            long deltaUsed = hs.used_heap_size() - sLastHeapUsed;
            sLastHeapUsed = hs.used_heap_size();
            if (deltaUsed != 0)
        {
            std::cout << "heap: delta ["<<deltaUsed<<"] used ["<<sLastHeapUsed<<"] total ["<<hs.total_heap_size()
                <<"] executable ["<<hs.total_heap_size_executable()
                <<"]  limit ["<<hs.heap_size_limit()<<"]\n";
                sIdleLastHeapReport = OS::getMilliseconds();
        }
    }
    )
}
