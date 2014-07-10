// -----------------------------------------------
// This file automatically generated from:
//
//    pdg/src/bindings/javascript/v8/pdg_js_classes.cpp
//    pdg/src/bindings/javascript/v8/pdg_script_macros.h
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

    v8::Persistent<v8::Value> s_SavedError;

    static v8::Persistent<v8::Object> s_BindingTarget;

    v8::Handle<v8::Value> GetConfigManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object ConfigManager]" " function" "()" " - " "") );
        };
        return scope.Close( ConfigManagerWrap::GetScriptSingletonInstance() );
    }

    v8::Handle<v8::Value> GetLogManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object LogManager]" " function" "()" " - " "") );
        };
        return scope.Close( LogManagerWrap::GetScriptSingletonInstance() );
    }

    IEventHandler* New_IEventHandler(const v8::Arguments& args)
    {
        if (args.Length() == 0)
        {
            ScriptEventHandler* handler = new ScriptEventHandler();
            handler->addRef();
            return handler;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            std::ostringstream excpt_;
            excpt_ << "EventHandler must be created with a function argument (handlerFunc)";
            s_SavedError = s_SavedError.New(v8::Exception::SyntaxError( v8::String::New(excpt_.str().c_str())));
        }
        v8::Local<v8::Function> cbfunc = v8::Local<v8::Function>::Cast(args[0]);
        v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(cbfunc);
        ScriptEventHandler* handler = new ScriptEventHandler(callback);
        handler->addRef();
        return handler;
    }

    v8::Handle<v8::Value> EventManagerWrap::IsKeyDown(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("boolean" " function" "({string unicodeChar|[number uint] utf16CharCode})" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (args[0]->IsString())
        {
            v8::Local<v8::String> keyCode_String = args[0]->ToString();
            uint16 utf16Char = 0;
            keyCode_String->Write(&utf16Char, 0, 1, v8::String::NO_NULL_TERMINATION);
            return scope.Close( v8::Boolean::New(OS::isKeyDown(utf16Char)) );
        }
        else
        {
            if (!args[1 -1]->IsNumber())
                return v8_ThrowArgTypeException(1, "a number (""utf16CharCode"")");
            unsigned long utf16CharCode = args[1 -1]->Uint32Value();
            return scope.Close( v8::Boolean::New(OS::isKeyDown(utf16CharCode)) );
        }
    }
    v8::Handle<v8::Value> EventManagerWrap::GetDeviceOrientation(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("object" " function" "(boolean absolute = false)" " - " "NOT IMPLEMENTED") );
        };
        if (args.Length() < 0)
            return v8_ThrowArgCountException(args.Length(), 0, true);
        if (args.Length() >= 1 && !args[1 -1]->IsBoolean())
            return v8_ThrowArgTypeException(1, "a boolean (""absolute"")");
        bool absolute = (args.Length()<1) ? false : args[1 -1]->BooleanValue();;
        float roll, pitch, yaw;
        OS::getDeviceOrientation(roll, pitch, yaw, absolute);
        v8::Local<v8::Object> jsOrientation = v8::Object::New();
        jsOrientation->Set(v8::String::New("roll"),v8::Number::New(roll));
        jsOrientation->Set(v8::String::New("pitch"),v8::Number::New(pitch));
        jsOrientation->Set(v8::String::New("yaw"),v8::Number::New(yaw));
        return scope.Close( jsOrientation );
    }
    v8::Handle<v8::Value> GetEventManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object EventManager]" " function" "()" " - " "") );
        };
        v8::Handle<v8::Object> jsInstance = EventManagerWrap::GetScriptSingletonInstance();
        EventManager* evtMgr = EventManager::getSingletonInstance();
        evtMgr->mEventEmitterScriptObj = v8::Persistent<v8::Object>::New(jsInstance);
        return scope.Close( jsInstance );
    }

    v8::Handle<v8::Value> ResourceManagerWrap::GetImage(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->refPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object Image]" " function" "(string imageName)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (!args[1 -1]->IsString())
            return v8_ThrowArgTypeException(1, "a string  (""imageName"")");
        v8::String::Utf8Value imageName_Str(args[1 -1]->ToString());
        const char* imageName = *imageName_Str;;
        Image* img = self->getImage(imageName);
        if (img == NULL)
        {
            return scope.Close( v8::Boolean::New(false) );
        }
        v8::Handle<v8::Value> obj = ImageWrap::NewFromNative(img);

        obj->ToObject()->Set(v8::String::New("name"), v8::String::New(imageName),
            static_cast<v8::PropertyAttribute>(v8::ReadOnly));
        return scope.Close( obj );
    }

    v8::Handle<v8::Value> ResourceManagerWrap::GetImageStrip(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->refPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object ImageStrip]" " function" "(string imageName)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (!args[1 -1]->IsString())
            return v8_ThrowArgTypeException(1, "a string  (""imageName"")");
        v8::String::Utf8Value imageName_Str(args[1 -1]->ToString());
        const char* imageName = *imageName_Str;;
        ImageStrip* img = self->getImageStrip(imageName);
        if (img == NULL)
        {
            return scope.Close( v8::Boolean::New(false) );
        }
        v8::Handle<v8::Value> obj = ImageStripWrap::NewFromNative(img);

        obj->ToObject()->Set(v8::String::New("name"), v8::String::New(imageName),
            static_cast<v8::PropertyAttribute>(v8::ReadOnly));
        return scope.Close( obj );
    }
#ifndef PDG_NO_SOUND

    v8::Handle<v8::Value> ResourceManagerWrap::GetSound(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        ResourceManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<ResourceManagerWrap>(args.This());
        ResourceManager* self = dynamic_cast<ResourceManager*>(objWrapper->refPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object Sound]" " function" "(string soundName)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (!args[1 -1]->IsString())
            return v8_ThrowArgTypeException(1, "a string  (""soundName"")");
        v8::String::Utf8Value soundName_Str(args[1 -1]->ToString());
        const char* soundName = *soundName_Str;;
        Sound* snd = self->getSound(soundName);
        if (snd == NULL)
        {
            return scope.Close( v8::Boolean::New(false) );
        }
        v8::Handle<v8::Value> obj = SoundWrap::NewFromNative(snd);

        obj->ToObject()->Set(v8::String::New("name"), v8::String::New(soundName),
            static_cast<v8::PropertyAttribute>(v8::ReadOnly));
        return scope.Close( obj );
    }
#endif

    v8::Handle<v8::Value> GetResourceManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object ResourceManager]" " function" "()" " - " "") );
        };
        return scope.Close( ResourceManagerWrap::GetScriptSingletonInstance() );
    }

    ISerializable* New_ISerializable(const v8::Arguments& args)
    {
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
                std::ostringstream excpt_;
                excpt_ << "Serializable must be created with 4 function arguments " "(getSerializedSizeFunc, serializeFunc, deserializeFunc, getMyClassTagFunc)";
                s_SavedError = s_SavedError.New(v8::Exception::SyntaxError( v8::String::New(excpt_.str().c_str())));

            }
            v8::Local<v8::Function> cbfunc1 = v8::Local<v8::Function>::Cast(args[0]);
            v8::Local<v8::Function> cbfunc2 = v8::Local<v8::Function>::Cast(args[1]);
            v8::Local<v8::Function> cbfunc3 = v8::Local<v8::Function>::Cast(args[2]);
            v8::Local<v8::Function> cbfunc4 = v8::Local<v8::Function>::Cast(args[3]);
            v8::Persistent<v8::Function> getSerializedSizeFunc = v8::Persistent<v8::Function>::New(cbfunc1);
            v8::Persistent<v8::Function> serializeFunc = v8::Persistent<v8::Function>::New(cbfunc2);
            v8::Persistent<v8::Function> deserializeFunc = v8::Persistent<v8::Function>::New(cbfunc3);
            v8::Persistent<v8::Function> getMyClassTagFunc = v8::Persistent<v8::Function>::New(cbfunc4);
            ScriptSerializable* serializable =
                new ScriptSerializable(getSerializedSizeFunc, serializeFunc,
                deserializeFunc, getMyClassTagFunc);
            serializable->addRef();
            return serializable;
        }
    }

    v8::Handle<v8::Value> SerializerWrap::Serialize_obj(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        SerializerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<SerializerWrap>(args.This());
        Serializer* self = dynamic_cast<Serializer*>(objWrapper->refPtr_);

        self->mSerializerScriptObj = v8::Persistent<v8::Object>::New(args.This());
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("undefined" " function" "([object ISerializable] obj)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        ISerializable* obj = 0;
        if (args[1 -1]->IsObject())
        {
            v8::Handle<v8::Object> obj_ = args[1 -1]->ToObject();
            ISerializableWrap* obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
            if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                    obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                }
            }
            if (obj__)
            {
                obj = obj__->getNativeObject();
                obj->mISerializableScriptObj = v8::Persistent<v8::Object>::New(args[1 -1]->ToObject());
            }
        }
        if (!obj)
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be an object derived from ""ISerializable"" (""obj"")";
            return v8::ThrowException( v8::Exception::TypeError( v8::String::New(excpt_.str().c_str())));
        };
        SCRIPT_DEBUG_ONLY( if (args[0].IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "args[0]" "|"<<*((void**)&(args[0]))<<")\n";
        }
        else if (!args[0]->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "args[0]" "|"<<*((void**)&(args[0]))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = args[0]->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""args[0]""|"<<*((void**)&(args[0]))<<"): " << objName << " - is a subclass of native ""ISerializable""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""args[0]""|"<<*((void**)&(args[0]))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                }
            }
            else
            {
                ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""args[0]""|" << *((void**)&(args[0])) << "): " << objName<<" - wraps native ""ISerializable"" ("<<(void*)obj<<")\n";
            }
        } )
            self->serialize_obj(obj);
        return scope.Close( v8::Undefined() );
    }

    v8::Handle<v8::Value> SerializerWrap::SerializedSize(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        SerializerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<SerializerWrap>(args.This());
        Serializer* self = dynamic_cast<Serializer*>(objWrapper->refPtr_);

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("number" " function" "({string|boolean|[number uint]|[object Color]|[object Offset]|[object Point]|[object Vector]|[object Rect]|[object RotatedRect]|[object Quad]|[object MemBlock]|[object ISerializable]} arg)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        uint32 dataSize = 0;
        if (args[0]->IsString())
        {
            v8::String::Utf8Value jsstr(args[0]->ToString());
            const char* str = *jsstr;
            dataSize = self->serializedSize(str);
        }
        else if (args[0]->IsBoolean())
        {
            bool val = args[0]->BooleanValue();
            dataSize = self->serializedSize(val);
        }
        else if (args[0]->IsNumber())
        {
            uint32 val = args[0]->Uint32Value();
            dataSize = self->serializedSize(val);
        }
        else if (v8_ValueIsColor(args[0]))
        {
            Color c = v8_ValueToColor(args[0]);
            dataSize = self->serializedSize(c);
        }
        else if (v8_ValueIsOffset(args[0]))
        {
            Offset o = v8_ValueToOffset(args[0]);
            dataSize = self->serializedSize(o);
        }
        else if (v8_ValueIsRect(args[0]))
        {
            Rect r = v8_ValueToRect(args[0]);
            dataSize = self->serializedSize(r);
        }
        else if (v8_ValueIsRotatedRect(args[0]))
        {
            RotatedRect rr = v8_ValueToRotatedRect(args[0]);
            dataSize = self->serializedSize(rr);
        }
        else if (v8_ValueIsQuad(args[0]))
        {
            Quad q = v8_ValueToQuad(args[0]);
            dataSize = self->serializedSize(q);
        }
        else if (args[0]->IsObject())
        {
            v8::Handle<v8::Object> obj = args[0]->ToObject();
            MemBlockWrap* memBlock__ = ObjectWrap::Unwrap<MemBlockWrap>(obj);
            if (memBlock__)
            {
                MemBlock* memBlock = memBlock__->getNativeObject();
                dataSize = self->serializedSize(memBlock->ptr, memBlock->bytes);
            }
            else
            {

                ISerializable* serializable = 0;
                if (args[1 -1]->IsObject())
                {
                    v8::Handle<v8::Object> serializable_ = args[1 -1]->ToObject();
                    ISerializableWrap* serializable__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(serializable_);
                    if (!serializable__)
                    {
                        v8::Handle<v8::Value> protoVal_ = serializable_->GetPrototype();
                        if (protoVal_->IsObject())
                        {
                            serializable_ = protoVal_->ToObject();
                            serializable__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(serializable_);
                        }
                    }
                    if (serializable__)
                    {
                        serializable = serializable__->getNativeObject();
                        serializable->mISerializableScriptObj = v8::Persistent<v8::Object>::New(args[1 -1]->ToObject());
                    }
                };
                if (serializable)
                {
                    SCRIPT_DEBUG_ONLY( if (self->mSerializerScriptObj.IsEmpty())
                    {
                        std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "self->mSerializerScriptObj" "|"<<*((void**)&(self->mSerializerScriptObj))<<")\n";
                    }
                    else if (!self->mSerializerScriptObj->IsObject())
                    {
                        std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "self->mSerializerScriptObj" "|"<<*((void**)&(self->mSerializerScriptObj))<<")\n";
                    }
                    else
                    {
                        v8::Handle<v8::Object> obj_ = self->mSerializerScriptObj->ToObject();
                            v8::String::Utf8Value objNameStr(obj_->ToString());
                            char* objName = *objNameStr;
                            SerializerWrap* obj__=jswrap::ObjectWrap::Unwrap<SerializerWrap>(obj_);
                            if (!obj__)
                        {
                            v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                                if (protoVal_->IsObject())
                            {
                                obj_ = protoVal_->ToObject();
                                    obj__ = jswrap::ObjectWrap::Unwrap<SerializerWrap>(obj_);
                            }
                            if (obj__)
                            {
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""self->mSerializerScriptObj""|"<<*((void**)&(self->mSerializerScriptObj))<<"): " << objName << " - is a subclass of native ""Serializer""\n";
                            }
                            else
                            {
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""self->mSerializerScriptObj""|"<<*((void**)&(self->mSerializerScriptObj))<<"): " << objName << " - does not wrap ""Serializer""\n";
                            }
                        }
                        else
                        {
                            Serializer* obj = dynamic_cast<Serializer*>(obj__->getNativeObject());
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""self->mSerializerScriptObj""|" << *((void**)&(self->mSerializerScriptObj)) << "): " << objName<<" - wraps native ""Serializer"" ("<<(void*)obj<<")\n";
                        }
                    } )
                        SCRIPT_DEBUG_ONLY( if (serializable->mISerializableScriptObj.IsEmpty())
                    {
                        std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "serializable->mISerializableScriptObj" "|"<<*((void**)&(serializable->mISerializableScriptObj))<<")\n";
                    }
                    else if (!serializable->mISerializableScriptObj->IsObject())
                    {
                        std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "serializable->mISerializableScriptObj" "|"<<*((void**)&(serializable->mISerializableScriptObj))<<")\n";
                    }
                    else
                    {
                        v8::Handle<v8::Object> obj_ = serializable->mISerializableScriptObj->ToObject();
                            v8::String::Utf8Value objNameStr(obj_->ToString());
                            char* objName = *objNameStr;
                            ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                            if (!obj__)
                        {
                            v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                                if (protoVal_->IsObject())
                            {
                                obj_ = protoVal_->ToObject();
                                    obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                            }
                            if (obj__)
                            {
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""serializable->mISerializableScriptObj""|"<<*((void**)&(serializable->mISerializableScriptObj))<<"): " << objName << " - is a subclass of native ""ISerializable""\n";
                            }
                            else
                            {
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""serializable->mISerializableScriptObj""|"<<*((void**)&(serializable->mISerializableScriptObj))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                            }
                        }
                        else
                        {
                            ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getNativeObject());
                                std::cout << __func__<<":"<< __LINE__ << " - JS Object (""serializable->mISerializableScriptObj""|" << *((void**)&(serializable->mISerializableScriptObj)) << "): " << objName<<" - wraps native ""ISerializable"" ("<<(void*)obj<<")\n";
                        }
                    } )
                        dataSize = self->serializedSize(serializable);
                }
                else
                {
                    std::ostringstream excpt_;
                    excpt_ << "argument 1 must be either an unsigned integer, " "a string, a MemBlock object, an ISerializable object";
                    return v8::ThrowException( v8::Exception::TypeError( v8::String::New(excpt_.str().c_str())));

                }
            }
        }
        return scope.Close( v8::Integer::NewFromUnsigned(dataSize) );
    }

    v8::Handle<v8::Value> DeserializerWrap::Deserialize_obj(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        DeserializerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<DeserializerWrap>(args.This());
        Deserializer* self = dynamic_cast<Deserializer*>(objWrapper->refPtr_);

        self->mDeserializerScriptObj = v8::Persistent<v8::Object>::New(args.This());
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object ISerializable]" " function" "()" " - " "") );
        };
        if (args.Length() != 0)
            return v8_ThrowArgCountException(args.Length(), 0);
        try
        {
            ISerializable* obj = self->deserialize_obj();
            SCRIPT_DEBUG_ONLY( if (obj->mISerializableScriptObj.IsEmpty())
            {
                std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "obj->mISerializableScriptObj" "|"<<*((void**)&(obj->mISerializableScriptObj))<<")\n";
            }
            else if (!obj->mISerializableScriptObj->IsObject())
            {
                std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "obj->mISerializableScriptObj" "|"<<*((void**)&(obj->mISerializableScriptObj))<<")\n";
            }
            else
            {
                v8::Handle<v8::Object> obj_ = obj->mISerializableScriptObj->ToObject();
                    v8::String::Utf8Value objNameStr(obj_->ToString());
                    char* objName = *objNameStr;
                    ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                    if (!obj__)
                {
                    v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                        if (protoVal_->IsObject())
                    {
                        obj_ = protoVal_->ToObject();
                            obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                    }
                    if (obj__)
                    {
                        std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj->mISerializableScriptObj""|"<<*((void**)&(obj->mISerializableScriptObj))<<"): " << objName << " - is a subclass of native ""ISerializable""\n";
                    }
                    else
                    {
                        std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj->mISerializableScriptObj""|"<<*((void**)&(obj->mISerializableScriptObj))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                    }
                }
                else
                {
                    ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getNativeObject());
                        std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj->mISerializableScriptObj""|" << *((void**)&(obj->mISerializableScriptObj)) << "): " << objName<<" - wraps native ""ISerializable"" ("<<(void*)obj<<")\n";
                }
            } )
                if (!obj) return scope.Close( v8::Null() );
            if (obj->mISerializableScriptObj.IsEmpty())
            {
                v8::Handle<v8::Value> obj__ = ISerializableWrap::NewFromNative(obj);
                return scope.Close( obj__ );
            }
            else
            {
                return scope.Close( obj->mISerializableScriptObj );
            };
        }
        catch(out_of_data& e)
        {
            std::ostringstream excpt_;
            excpt_ << e.what();
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
        catch(bad_tag& e)
        {
            std::ostringstream excpt_;
            excpt_ << e.what();
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
        catch(sync_error& e)
        {
            std::ostringstream excpt_;
            excpt_ << e.what();
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
        catch(unknown_object& e)
        {
            std::ostringstream excpt_;
            excpt_ << e.what();
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
    }

    v8::Handle<v8::Value> RegisterSerializableClass(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("undefined" " function" "(function klass)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (!args[1 -1]->IsFunction())
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be a function (""constructorFunc"")";
            return v8::ThrowException( v8::Exception::TypeError( v8::String::New(excpt_.str().c_str())));
        }
        v8::Handle<v8::Function> constructorFunc = v8::Handle<v8::Function>::Cast(args[1 -1]);;
        v8::Local<v8::Value> objVal = constructorFunc->CallAsConstructor(0, 0);
        v8::Local<v8::Object> obj = objVal->ToObject();
        SCRIPT_DEBUG_ONLY( if (obj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "obj" "|"<<*((void**)&(obj))<<")\n";
        }
        else if (!obj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "obj" "|"<<*((void**)&(obj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = obj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISerializableWrap* obj__=jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<ISerializableWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj""|"<<*((void**)&(obj))<<"): " << objName << " - is a subclass of native ""ISerializable""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj""|"<<*((void**)&(obj))<<"): " << objName << " - does not wrap ""ISerializable""\n";
                }
            }
            else
            {
                ISerializable* obj = dynamic_cast<ISerializable*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""obj""|" << *((void**)&(obj)) << "): " << objName<<" - wraps native ""ISerializable"" ("<<(void*)obj<<")\n";
            }
        } )
            v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (obj->Has(v8::String::New("getMyClassTag")))
        {
            func = v8::Handle<v8::Function>::Cast(obj->Get(v8::String::New("getMyClassTag")));
        }
        else
        {
            std::ostringstream msg;
            msg << "argument 1: ISerializable subclass " << *obj->ToString() << " missing getMyClassTag() Function!!";
            std::ostringstream excpt_;
            excpt_ << msg.str().c_str();
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
        resVal = func->Call(obj, 0, 0);
        uint32 classTag = resVal->Uint32Value();
        Deserializer::registerScriptClass(classTag, constructorFunc);
        return scope.Close( v8::Undefined() );
    }

#ifndef PDG_NO_GUI

    v8::Handle<v8::Value> GraphicsManagerWrap::GetCurrentScreenMode(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        GraphicsManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<GraphicsManagerWrap>(args.This());
        GraphicsManager* self = dynamic_cast<GraphicsManager*>(objWrapper->refPtr_);
        ;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("object" " function" "([number int] screenNum = PRIMARY_SCREEN)" " - " "returns object with width, height, depth and maxWindowRect for specified screen") );
        };

        if (args.Length() < 0)
            return v8_ThrowArgCountException(args.Length(), 0, true);
        if (args.Length() >= 1 && !args[1 -1]->IsNumber())
            return v8_ThrowArgTypeException(1, "a number (""screenNum"")");
        long screenNum = (args.Length()<1) ? GraphicsManager::screenNum_PrimaryScreen : args[1 -1]->Int32Value();;
        pdg::Rect maxWindowRect;
        pdg::GraphicsManager::ScreenMode mode;
        mode = self->getCurrentScreenMode(screenNum, &maxWindowRect);

        v8::Local<v8::Object> jsScreenMode = v8::Object::New();
        jsScreenMode->Set(v8::String::New("width"),v8::Integer::New(mode.width));
        jsScreenMode->Set(v8::String::New("height"),v8::Integer::New(mode.height));
        jsScreenMode->Set(v8::String::New("depth"),v8::Integer::New(mode.bpp));
        jsScreenMode->Set(v8::String::New("maxWindowRect"), v8_MakeJavascriptRect(maxWindowRect));
        return scope.Close( jsScreenMode );
    }

    v8::Handle<v8::Value> GraphicsManagerWrap::GetNthSupportedScreenMode(const v8::Arguments& args)
    {
        v8::HandleScope scope;
        GraphicsManagerWrap* objWrapper = jswrap::ObjectWrap::Unwrap<GraphicsManagerWrap>(args.This());
        GraphicsManager* self = dynamic_cast<GraphicsManager*>(objWrapper->refPtr_);
        ;
        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("object" " function" "([number int] n, [number int] screenNum = PRIMARY_SCREEN)" " - " "returns object with width, height, depth and maxWindowRect for specified screen") );
        };

        if (args.Length() < 1)
            return v8_ThrowArgCountException(args.Length(), 1, true);
        if (!args[1 -1]->IsNumber())
            return v8_ThrowArgTypeException(1, "a number (""n"")");
        long n = args[1 -1]->Int32Value();
        if (args.Length() >= 2 && !args[2 -1]->IsNumber())
            return v8_ThrowArgTypeException(2, "a number (""screenNum"")");
        long screenNum = (args.Length()<2) ? GraphicsManager::screenNum_PrimaryScreen : args[2 -1]->Int32Value();;
        pdg::GraphicsManager::ScreenMode mode;
        mode = self->getNthSupportedScreenMode(n, screenNum);

        v8::Local<v8::Object> jsScreenMode = v8::Object::New();
        jsScreenMode->Set(v8::String::New("width"),v8::Integer::New(mode.width));
        jsScreenMode->Set(v8::String::New("height"),v8::Integer::New(mode.height));
        jsScreenMode->Set(v8::String::New("depth"),v8::Integer::New(mode.bpp));
        return scope.Close( jsScreenMode );
    }

    v8::Handle<v8::Value> GetGraphicsManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object GraphicsManager]" " function" "()" " - " "") );
        };
        return scope.Close( GraphicsManagerWrap::GetScriptSingletonInstance() );
    }
#endif

#ifndef PDG_NO_SOUND

    v8::Handle<v8::Value> GetSoundManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object SoundManager]" " function" "()" " - " "") );
        };
        return scope.Close( SoundManagerWrap::GetScriptSingletonInstance() );
    }
#endif

    v8::Handle<v8::Value> GetFileManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object FileManager]" " function" "()" " - " "") );
        };
        return scope.Close( FileManagerWrap::GetScriptSingletonInstance() );
    }

    v8::Handle<v8::Value> GetTimerManager(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("[object TimerManager]" " function" "()" " - " "") );
        };
        v8::Handle<v8::Object> jsInstance = TimerManagerWrap::GetScriptSingletonInstance();
        TimerManager* timMgr = TimerManager::getSingletonInstance();
        timMgr->mEventEmitterScriptObj = v8::Persistent<v8::Object>::New(jsInstance);
        return scope.Close( jsInstance );
    }

    IAnimationHelper* New_IAnimationHelper(const v8::Arguments& args)
    {
        if (args.Length() == 0)
        {
            ScriptAnimationHelper* helper = new ScriptAnimationHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            std::ostringstream excpt_;
            excpt_ << "AnimationHelper must be created with a function argument (handlerFunc)";
            s_SavedError = s_SavedError.New(v8::Exception::SyntaxError( v8::String::New(excpt_.str().c_str())));
            return 0;
        }
        v8::Local<v8::Function> cbfunc = v8::Local<v8::Function>::Cast(args[0]);
        v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(cbfunc);
        ScriptAnimationHelper* helper = new ScriptAnimationHelper(callback);
        return helper;
    }

    static v8::Persistent<v8::Function> s_CustomScriptEasing[MAX_CUSTOM_EASINGS];

    ISpriteCollideHelper* New_ISpriteCollideHelper(const v8::Arguments& args)
    {
        if (args.Length() == 0)
        {
            ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            std::ostringstream excpt_;
            excpt_ << "SpriteCollideHelper must be created with a function argument (allowCollisionFunc)";
            s_SavedError = s_SavedError.New(v8::Exception::SyntaxError( v8::String::New(excpt_.str().c_str())));
            return 0;
        }
        v8::Local<v8::Function> cbfunc = v8::Local<v8::Function>::Cast(args[0]);
        v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(cbfunc);
        ScriptSpriteCollideHelper* helper = new ScriptSpriteCollideHelper(callback);
        return helper;
    }

#ifndef PDG_NO_GUI

    ISpriteDrawHelper* New_ISpriteDrawHelper(const v8::Arguments& args)
    {
        if (args.Length() == 0)
        {
            ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper();
            return helper;
        }
        else if (args.Length() != 1 || !args[0]->IsFunction())
        {
            s_HaveSavedError = true;
            std::ostringstream excpt_;
            excpt_ << "SpriteDrawHelper must be created with a function argument (drawFunc)";
            s_SavedError = s_SavedError.New(v8::Exception::SyntaxError( v8::String::New(excpt_.str().c_str())));
            return 0;
        }
        v8::Local<v8::Function> cbfunc = v8::Local<v8::Function>::Cast(args[0]);
        v8::Persistent<v8::Function> callback = v8::Persistent<v8::Function>::New(cbfunc);
        ScriptSpriteDrawHelper* helper = new ScriptSpriteDrawHelper(callback);
        return helper;
    }
#endif

    ScriptSerializable::ScriptSerializable(
        v8::Persistent<v8::Function> javascriptGetSerializedSizeFunc,
        v8::Persistent<v8::Function> javascriptSerializeFunc,
        v8::Persistent<v8::Function> javascriptDeserializeFunc,
        v8::Persistent<v8::Function> javascriptGetMyClassTagFunc)
    {
        mScriptGetSerializedSizeFunc = v8::Persistent<v8::Function>::New(javascriptGetSerializedSizeFunc);
        mScriptSerializeFunc = v8::Persistent<v8::Function>::New(javascriptSerializeFunc);
        mScriptDeserializeFunc = v8::Persistent<v8::Function>::New(javascriptDeserializeFunc);
        mScriptGetMyClassTagFunc = v8::Persistent<v8::Function>::New(javascriptGetMyClassTagFunc);
    }

    uint32
        ScriptSerializable::getSerializedSize(ISerializer* serializer) const
    {
        v8::HandleScope scope;
        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[1];
        Serializer* ser = dynamic_cast<Serializer*>(serializer);
        if (!ser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
                exit(1);
                )
        }

        argv[0] = v8::Local<v8::Value>::New(ser->mSerializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptGetSerializedSizeFunc.IsEmpty() && mScriptGetSerializedSizeFunc->IsFunction())
        {
            func = mScriptGetSerializedSizeFunc;
        }
        else
        {
            func = v8::Handle<v8::Function>::Cast(mISerializableScriptObj->Get(v8::String::New("getSerializedSize")));
        }
        resVal = func->Call(this->mISerializableScriptObj, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getSerializedSize Function!!" ); )
                FatalException(try_catch);
            return 0;
        }
        if (!resVal->IsUint32())
        {
            DEBUG_ONLY(
                v8::String::Utf8Value funcNameStr(func->GetName()->ToString());
                v8::String::Utf8Value resNameStr(func->GetScriptOrigin().ResourceName()->ToString());
                std::cerr << "result mismatch: return value from getSerializedSize Function must be an unsigned integer ("
                << *funcNameStr << " at " << *resNameStr << ":"
                << func->GetScriptLineNumber()+1 << ")\n";
                exit(1);
                )
                return 0;
        }
        return resVal->Uint32Value();
    }

    void
        ScriptSerializable::serialize(ISerializer* serializer) const
    {
        v8::HandleScope scope;
        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[1];
        Serializer* ser = dynamic_cast<Serializer*>(serializer);
        if (!ser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: getSerializedSize Function called with invalid Serializer\n";
                exit(1);
                )
        }

        argv[0] = v8::Local<v8::Value>::New(ser->mSerializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptSerializeFunc.IsEmpty() && mScriptSerializeFunc->IsFunction())
        {
            func = mScriptSerializeFunc;
        }
        else
        {
            func = v8::Handle<v8::Function>::Cast(mISerializableScriptObj->Get(v8::String::New("serialize")));
        }
        resVal = func->Call(this->mISerializableScriptObj, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling serialize Function!!" ); )
                FatalException(try_catch);
        }
    }

    void
        ScriptSerializable::deserialize(IDeserializer* deserializer)
    {
        v8::HandleScope scope;
        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[1];
        Deserializer* deser = dynamic_cast<Deserializer*>(deserializer);
        if (!deser)
        {
            DEBUG_ONLY(
                std::cerr << "Internal Error: deserialize Function called with invalid Deserializer\n";
                exit(1);
                )
        }
        argv[0] = v8::Local<v8::Value>::New(deser->mDeserializerScriptObj);
        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptDeserializeFunc.IsEmpty() && mScriptDeserializeFunc->IsFunction())
        {
            func = mScriptDeserializeFunc;
        }
        else
        {
            func = v8::Handle<v8::Function>::Cast(mISerializableScriptObj->Get(v8::String::New("deserialize")));
        }
        resVal = func->Call(this->mISerializableScriptObj, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling deserialize Function!!" ); )
                FatalException(try_catch);
        }
    }

    uint32
        ScriptSerializable::getMyClassTag() const
    {
        v8::HandleScope scope;
        v8::TryCatch try_catch;

        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptGetMyClassTagFunc.IsEmpty() && mScriptGetMyClassTagFunc->IsFunction())
        {
            func = mScriptGetMyClassTagFunc;
        }
        else if (mISerializableScriptObj->Has(v8::String::New("getMyClassTag")))
        {
            func = v8::Handle<v8::Function>::Cast(mISerializableScriptObj->Get(v8::String::New("getMyClassTag")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(mISerializableScriptObj->ToString());
                std::cerr << "fatal: ISerializable object " << *objectNameStr << " missing getMyClassTag() Function!!";
                exit(1);
                )
                return 0;
        }
        resVal = func->Call(this->mISerializableScriptObj, 0, 0);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling getMyClassTag Function!!" ); )
                FatalException(try_catch);
            return 0;
        }
        if (!resVal->IsUint32())
        {
            DEBUG_ONLY(
                v8::String::Utf8Value funcNameStr(func->GetName()->ToString());
                v8::String::Utf8Value resNameStr(func->GetScriptOrigin().ResourceName()->ToString());
                std::cerr << "result mismatch: return value from getMyClassTag Function must be an unsigned integer ("
                << *funcNameStr << " at " << *resNameStr << ":"
                << func->GetScriptLineNumber()+1 << ")\n";
                exit(1);
                )
                return 0;
        }
        return resVal->Uint32Value();
    }

    ScriptEventHandler::ScriptEventHandler(v8::Persistent<v8::Function> func)
    {
        mScriptHandlerFunc = v8::Persistent<v8::Function>::New(func);
    }

    bool ScriptEventHandler::handleEvent(EventEmitter* emitter, long inEventType, void* inEventData) throw()
    {
        v8::HandleScope scope;
        v8::Local<v8::Object> jsEvent = v8::Object::New();
        jsEvent->Set(v8::String::New("emitter"), emitter->mEventEmitterScriptObj);
        jsEvent->Set(v8::String::New("eventType"),v8::Integer::New(inEventType));
        switch (inEventType)
        {
            case pdg::eventType_Startup:
                jsEvent->Set(v8::String::New("startupReason"),v8::Integer::New(static_cast<StartupInfo*>(inEventData)->startupReason));

                break;
            case pdg::eventType_Shutdown:
                jsEvent->Set(v8::String::New("exitReason"),v8::Integer::New(static_cast<ShutdownInfo*>(inEventData)->exitReason));
                jsEvent->Set(v8::String::New("exitCode"),v8::Integer::New(static_cast<ShutdownInfo*>(inEventData)->exitCode));
                break;
            case pdg::eventType_Timer:
                if (static_cast<TimerInfo*>(inEventData)->id <= 0)
                {

                    return false;
                }
                jsEvent->Set(v8::String::New("id"),v8::Integer::New(static_cast<TimerInfo*>(inEventData)->id));
                jsEvent->Set(v8::String::New("millisec"),v8::Integer::NewFromUnsigned(static_cast<TimerInfo*>(inEventData)->millisec));
                jsEvent->Set(v8::String::New("msElapsed"),v8::Integer::NewFromUnsigned(static_cast<TimerInfo*>(inEventData)->msElapsed));

                break;
#ifndef PDG_NO_GUI
            case pdg::eventType_KeyDown:
            case pdg::eventType_KeyUp:
                jsEvent->Set(v8::String::New("keyCode"),v8::Integer::New(static_cast<KeyInfo*>(inEventData)->keyCode));
                break;
            case pdg::eventType_KeyPress:
                jsEvent->Set(v8::String::New("shift"),v8::Boolean::New(static_cast<KeyPressInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::New("ctrl"),v8::Boolean::New(static_cast<KeyPressInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::New("alt"),v8::Boolean::New(static_cast<KeyPressInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::New("meta"),v8::Boolean::New(static_cast<KeyPressInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::New("unicode"),v8::Integer::New(static_cast<KeyPressInfo*>(inEventData)->unicode));
                jsEvent->Set(v8::String::New("isRepeating"),v8::Boolean::New(static_cast<KeyPressInfo*>(inEventData)->isRepeating));
                break;
            case pdg::eventType_SpriteTouch:
                jsEvent->Set(v8::String::New("touchType"),v8::Integer::New(static_cast<SpriteTouchInfo*>(inEventData)->touchType));
                jsEvent->Set(v8::String::New("touchedSprite"),static_cast<SpriteTouchInfo*>(inEventData)->touchedSprite->mSpriteScriptObj);
                jsEvent->Set(v8::String::New("inLayer"),static_cast<SpriteTouchInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);

            case pdg::eventType_MouseDown:
            case pdg::eventType_MouseUp:
            case pdg::eventType_MouseMove:
                jsEvent->Set(v8::String::New("shift"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::New("ctrl"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::New("alt"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::New("meta"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::New("mousePos"),v8_MakeJavascriptPoint(static_cast<MouseInfo*>(inEventData)->mousePos));
                jsEvent->Set(v8::String::New("leftButton"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->leftButton));
                jsEvent->Set(v8::String::New("rightButton"),v8::Boolean::New(static_cast<MouseInfo*>(inEventData)->rightButton));
                jsEvent->Set(v8::String::New("buttonNumber"),v8::Integer::NewFromUnsigned(static_cast<MouseInfo*>(inEventData)->buttonNumber));
                jsEvent->Set(v8::String::New("lastClickPos"),v8_MakeJavascriptPoint(static_cast<MouseInfo*>(inEventData)->lastClickPos));
                jsEvent->Set(v8::String::New("lastClickElapsed"),v8::Integer::NewFromUnsigned(static_cast<MouseInfo*>(inEventData)->lastClickElapsed));
                break;
            case pdg::eventType_ScrollWheel:
                jsEvent->Set(v8::String::New("shift"),v8::Boolean::New(static_cast<ScrollWheelInfo*>(inEventData)->shift));
                jsEvent->Set(v8::String::New("ctrl"),v8::Boolean::New(static_cast<ScrollWheelInfo*>(inEventData)->ctrl));
                jsEvent->Set(v8::String::New("alt"),v8::Boolean::New(static_cast<ScrollWheelInfo*>(inEventData)->alt));
                jsEvent->Set(v8::String::New("meta"),v8::Boolean::New(static_cast<ScrollWheelInfo*>(inEventData)->meta));
                jsEvent->Set(v8::String::New("horizDelta"),v8::Integer::New(static_cast<ScrollWheelInfo*>(inEventData)->horizDelta));
                jsEvent->Set(v8::String::New("vertDelta"),v8::Integer::New(static_cast<ScrollWheelInfo*>(inEventData)->vertDelta));
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
                jsEvent->Set(v8::String::New("eventCode"),v8::Integer::New(static_cast<SoundEventInfo*>(inEventData)->eventCode));
                jsEvent->Set(v8::String::New("sound"),static_cast<SoundEventInfo*>(inEventData)->sound->mSoundScriptObj);
                break;
#endif
#ifndef PDG_NO_GUI
            case pdg::eventType_PortResized:
                jsEvent->Set(v8::String::New("port"),static_cast<PortResizeInfo*>(inEventData)->port->mPortScriptObj);
                jsEvent->Set(v8::String::New("screenPos"), v8::Integer::New(static_cast<PortResizeInfo*>(inEventData)->screenPos));
                jsEvent->Set(v8::String::New("oldScreenPos"), v8::Integer::New(static_cast<PortResizeInfo*>(inEventData)->oldScreenPos));
                jsEvent->Set(v8::String::New("oldWidth"), v8::Integer::New(static_cast<PortResizeInfo*>(inEventData)->oldWidth));
                jsEvent->Set(v8::String::New("oldHeight"), v8::Integer::New(static_cast<PortResizeInfo*>(inEventData)->oldHeight));
                break;
            case pdg::eventType_PortDraw:
                jsEvent->Set(v8::String::New("port"),static_cast<PortDrawInfo*>(inEventData)->port->mPortScriptObj);
                jsEvent->Set(v8::String::New("frameNum"), v8::Integer::New(static_cast<PortDrawInfo*>(inEventData)->frameNum));
                break;
#endif
            case pdg::eventType_SpriteCollide:
            case pdg::eventType_SpriteBreak:
                if (inEventType == pdg::eventType_SpriteCollide)
                {
                    if (static_cast<SpriteCollideInfo*>(inEventData)->targetSprite)
                    {
                        jsEvent->Set(v8::String::New("targetSprite"),static_cast<SpriteCollideInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
                    }
                    jsEvent->Set(v8::String::New("normal"), v8_MakeJavascriptVector(static_cast<SpriteCollideInfo*>(inEventData)->normal));
                    jsEvent->Set(v8::String::New("impulse"), v8_MakeJavascriptVector(static_cast<SpriteCollideInfo*>(inEventData)->impulse));
                    jsEvent->Set(v8::String::New("force"),v8::Number::New(static_cast<SpriteCollideInfo*>(inEventData)->force));
                    jsEvent->Set(v8::String::New("kineticEnergy"),v8::Number::New(static_cast<SpriteCollideInfo*>(inEventData)->kineticEnergy));
#ifdef PDG_USE_CHIPMUNK_PHYSICS
                    if (static_cast<SpriteCollideInfo*>(inEventData)->arbiter)
                    {
                        jsEvent->Set(v8::String::New("arbiter"), cpArbiterWrap::NewFromNative(static_cast<SpriteCollideInfo*>(inEventData)->arbiter));
                    }
#endif
                }
                else
                {
#ifdef PDG_USE_CHIPMUNK_PHYSICS
                    jsEvent->Set(v8::String::New("targetSprite"),static_cast<SpriteJointBreakInfo*>(inEventData)->targetSprite->mSpriteScriptObj);
                    jsEvent->Set(v8::String::New("impulse"),v8::Number::New(static_cast<SpriteJointBreakInfo*>(inEventData)->impulse));
                    jsEvent->Set(v8::String::New("force"),v8::Number::New(static_cast<SpriteJointBreakInfo*>(inEventData)->force));
                    jsEvent->Set(v8::String::New("breakForce"),v8::Number::New(static_cast<SpriteJointBreakInfo*>(inEventData)->breakForce));
                    jsEvent->Set(v8::String::New("joint"), cpConstraintWrap::NewFromNative(static_cast<SpriteJointBreakInfo*>(inEventData)->joint));
#endif
                }

            case pdg::eventType_SpriteAnimate:
                jsEvent->Set(v8::String::New("action"),v8::Integer::New(static_cast<SpriteAnimateInfo*>(inEventData)->action));
                jsEvent->Set(v8::String::New("actingSprite"),static_cast<SpriteAnimateInfo*>(inEventData)->actingSprite->mSpriteScriptObj);
                jsEvent->Set(v8::String::New("inLayer"),static_cast<SpriteAnimateInfo*>(inEventData)->inLayer->mSpriteLayerScriptObj);
                break;
            case pdg::eventType_SpriteLayer:
                jsEvent->Set(v8::String::New("action"),v8::Integer::New(static_cast<SpriteLayerInfo*>(inEventData)->action));
                jsEvent->Set(v8::String::New("actingLayer"),static_cast<SpriteLayerInfo*>(inEventData)->actingLayer->mSpriteLayerScriptObj);
                jsEvent->Set(v8::String::New("millisec"),v8::Integer::NewFromUnsigned(static_cast<SpriteLayerInfo*>(inEventData)->millisec));
                break;
            default:
            {
                std::ostringstream msg;
                msg << "unknown event (" << inEventType << ")";
                s_HaveSavedError = true;
                std::ostringstream excpt_;
                excpt_ << msg.str().c_str();
                s_SavedError = s_SavedError.New(v8::Exception::TypeError( v8::String::New(excpt_.str().c_str())));
                return false;
            }
            break;

        }

        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[1];
        argv[0] = v8::Local<v8::Value>::New(jsEvent);

        SCRIPT_DEBUG_ONLY( if (this->mIEventHandlerScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "this->mIEventHandlerScriptObj" "|"<<*((void**)&(this->mIEventHandlerScriptObj))<<")\n";
        }
        else if (!this->mIEventHandlerScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "this->mIEventHandlerScriptObj" "|"<<*((void**)&(this->mIEventHandlerScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = this->mIEventHandlerScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                IEventHandlerWrap* obj__=jswrap::ObjectWrap::Unwrap<IEventHandlerWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<IEventHandlerWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIEventHandlerScriptObj""|"<<*((void**)&(this->mIEventHandlerScriptObj))<<"): " << objName << " - is a subclass of native ""IEventHandler""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIEventHandlerScriptObj""|"<<*((void**)&(this->mIEventHandlerScriptObj))<<"): " << objName << " - does not wrap ""IEventHandler""\n";
                }
            }
            else
            {
                IEventHandler* obj = dynamic_cast<IEventHandler*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIEventHandlerScriptObj""|" << *((void**)&(this->mIEventHandlerScriptObj)) << "): " << objName<<" - wraps native ""IEventHandler"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptHandlerFunc.IsEmpty() && mScriptHandlerFunc->IsFunction())
        {
            func = mScriptHandlerFunc;
        }
        else if (mIEventHandlerScriptObj->Has(v8::String::New("handleEvent")))
        {
            func = v8::Handle<v8::Function>::Cast(mIEventHandlerScriptObj->Get(v8::String::New("handleEvent")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(mIEventHandlerScriptObj->ToString());
                std::cerr << "fatal: IEventHandler object " << *objectNameStr << " missing handleEvent() Function!!\n";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(this->mIEventHandlerScriptObj, 1, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling event Handler!!" ); )
                FatalException(try_catch);
            return false;
        }
        if (!resVal->IsBoolean())
        {
#ifdef DEBUG
            v8::String::Utf8Value funcNameStr(func->GetName()->ToString());
            v8::String::Utf8Value resNameStr(func->GetScriptOrigin().ResourceName()->ToString());
            std::cerr << "result mismatch: return value from event handler Function must be a boolean ("
                << *funcNameStr << " at " << *resNameStr << ":"
                << func->GetScriptLineNumber()+1 << ")\n";
            exit(1);
#else
            return false;
#endif
        }
        return resVal->BooleanValue();
    }

    ScriptAnimationHelper::ScriptAnimationHelper(v8::Persistent<v8::Function> func)
    {
        mScriptAnimateFunc = v8::Persistent<v8::Function>::New(func);
    }

    bool ScriptAnimationHelper::animate(Animated* what, uint32 msElapsed) throw()
    {
        v8::HandleScope scope;

        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Value>::New(what->mAnimatedScriptObj);
        argv[1] = v8::Local<v8::Value>::New(v8::Integer::NewFromUnsigned(msElapsed));

        SCRIPT_DEBUG_ONLY( if (what->mAnimatedScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "what->mAnimatedScriptObj" "|"<<*((void**)&(what->mAnimatedScriptObj))<<")\n";
        }
        else if (!what->mAnimatedScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "what->mAnimatedScriptObj" "|"<<*((void**)&(what->mAnimatedScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = what->mAnimatedScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                AnimatedWrap* obj__=jswrap::ObjectWrap::Unwrap<AnimatedWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<AnimatedWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""what->mAnimatedScriptObj""|"<<*((void**)&(what->mAnimatedScriptObj))<<"): " << objName << " - is a subclass of native ""Animated""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""what->mAnimatedScriptObj""|"<<*((void**)&(what->mAnimatedScriptObj))<<"): " << objName << " - does not wrap ""Animated""\n";
                }
            }
            else
            {
                Animated* obj = dynamic_cast<Animated*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""what->mAnimatedScriptObj""|" << *((void**)&(what->mAnimatedScriptObj)) << "): " << objName<<" - wraps native ""Animated"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mIAnimationHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "this->mIAnimationHelperScriptObj" "|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<")\n";
        }
        else if (!this->mIAnimationHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "this->mIAnimationHelperScriptObj" "|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = this->mIAnimationHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                IAnimationHelperWrap* obj__=jswrap::ObjectWrap::Unwrap<IAnimationHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<IAnimationHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIAnimationHelperScriptObj""|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<"): " << objName << " - is a subclass of native ""IAnimationHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIAnimationHelperScriptObj""|"<<*((void**)&(this->mIAnimationHelperScriptObj))<<"): " << objName << " - does not wrap ""IAnimationHelper""\n";
                }
            }
            else
            {
                IAnimationHelper* obj = dynamic_cast<IAnimationHelper*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mIAnimationHelperScriptObj""|" << *((void**)&(this->mIAnimationHelperScriptObj)) << "): " << objName<<" - wraps native ""IAnimationHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptAnimateFunc.IsEmpty() && mScriptAnimateFunc->IsFunction())
        {
            func = mScriptAnimateFunc;
        }
        else if (mIAnimationHelperScriptObj->Has(v8::String::New("animate")))
        {
            func = v8::Handle<v8::Function>::Cast(mIAnimationHelperScriptObj->Get(v8::String::New("animate")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(mIAnimationHelperScriptObj->ToString());
                std::cerr << "fatal: IAnimationHelper object " << *objectNameStr << " missing animate() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(this->mIAnimationHelperScriptObj, 2, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Animation Helper!!" ); )
                FatalException(try_catch);
            return false;
        }
        if (!resVal->IsBoolean())
        {
#ifdef DEBUG
            v8::String::Utf8Value funcNameStr(func->GetName()->ToString());
            v8::String::Utf8Value resNameStr(func->GetScriptOrigin().ResourceName()->ToString());
            std::cerr << "result mismatch: return value from animate helper Function must be a boolean ("
                << *funcNameStr << " at " << *resNameStr << ":"
                << func->GetScriptLineNumber()+1 << ")\n";
            exit(1);
#else
            return false;
#endif
        }
        return resVal->BooleanValue();
    }

    ScriptSpriteCollideHelper::ScriptSpriteCollideHelper(v8::Persistent<v8::Function> func)
    {
        mScriptAllowCollisionFunc = v8::Persistent<v8::Function>::New(func);
    }

    bool ScriptSpriteCollideHelper::allowCollision(Sprite* sprite, Sprite* withSprite) throw()
    {
        v8::HandleScope scope;

        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Value>::New(sprite->mSpriteScriptObj);
        argv[1] = v8::Local<v8::Value>::New(withSprite->mSpriteScriptObj);

        SCRIPT_DEBUG_ONLY( if (sprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else if (!sprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = sprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of native ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|" << *((void**)&(sprite->mSpriteScriptObj)) << "): " << objName<<" - wraps native ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (withSprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "withSprite->mSpriteScriptObj" "|"<<*((void**)&(withSprite->mSpriteScriptObj))<<")\n";
        }
        else if (!withSprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "withSprite->mSpriteScriptObj" "|"<<*((void**)&(withSprite->mSpriteScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = withSprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""withSprite->mSpriteScriptObj""|"<<*((void**)&(withSprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of native ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""withSprite->mSpriteScriptObj""|"<<*((void**)&(withSprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""withSprite->mSpriteScriptObj""|" << *((void**)&(withSprite->mSpriteScriptObj)) << "): " << objName<<" - wraps native ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mISpriteCollideHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "this->mISpriteCollideHelperScriptObj" "|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<")\n";
        }
        else if (!this->mISpriteCollideHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "this->mISpriteCollideHelperScriptObj" "|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = this->mISpriteCollideHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISpriteCollideHelperWrap* obj__=jswrap::ObjectWrap::Unwrap<ISpriteCollideHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<ISpriteCollideHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteCollideHelperScriptObj""|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<"): " << objName << " - is a subclass of native ""ISpriteCollideHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteCollideHelperScriptObj""|"<<*((void**)&(this->mISpriteCollideHelperScriptObj))<<"): " << objName << " - does not wrap ""ISpriteCollideHelper""\n";
                }
            }
            else
            {
                ISpriteCollideHelper* obj = dynamic_cast<ISpriteCollideHelper*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteCollideHelperScriptObj""|" << *((void**)&(this->mISpriteCollideHelperScriptObj)) << "): " << objName<<" - wraps native ""ISpriteCollideHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptAllowCollisionFunc.IsEmpty() && mScriptAllowCollisionFunc->IsFunction())
        {
            func = mScriptAllowCollisionFunc;
        }
        else if (mISpriteCollideHelperScriptObj->Has(v8::String::New("allowCollision")))
        {
            func = v8::Handle<v8::Function>::Cast(mISpriteCollideHelperScriptObj->Get(v8::String::New("allowCollision")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(mISpriteCollideHelperScriptObj->ToString());
                std::cerr << "fatal: ISpriteCollideHelper object " << *objectNameStr << " missing allowCollision() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(this->mISpriteCollideHelperScriptObj, 2, argv);

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Sprite Collide Helper!!" ); )
                FatalException(try_catch);
            return false;
        }

        return resVal->BooleanValue();
    }

#ifndef PDG_NO_GUI

    ScriptSpriteDrawHelper::ScriptSpriteDrawHelper(v8::Persistent<v8::Function> func)
    {
        mScriptDrawFunc = v8::Persistent<v8::Function>::New(func);
    }

    bool ScriptSpriteDrawHelper::draw(Sprite* sprite, Port* port) throw()
    {
        v8::HandleScope scope;

        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[2];
        argv[0] = v8::Local<v8::Value>::New(sprite->mSpriteScriptObj);
        argv[1] = v8::Local<v8::Value>::New(port->mPortScriptObj);

        SCRIPT_DEBUG_ONLY( if (sprite->mSpriteScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else if (!sprite->mSpriteScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "sprite->mSpriteScriptObj" "|"<<*((void**)&(sprite->mSpriteScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = sprite->mSpriteScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                SpriteWrap* obj__=jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<SpriteWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - is a subclass of native ""Sprite""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|"<<*((void**)&(sprite->mSpriteScriptObj))<<"): " << objName << " - does not wrap ""Sprite""\n";
                }
            }
            else
            {
                Sprite* obj = dynamic_cast<Sprite*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""sprite->mSpriteScriptObj""|" << *((void**)&(sprite->mSpriteScriptObj)) << "): " << objName<<" - wraps native ""Sprite"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (port->mPortScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "port->mPortScriptObj" "|"<<*((void**)&(port->mPortScriptObj))<<")\n";
        }
        else if (!port->mPortScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "port->mPortScriptObj" "|"<<*((void**)&(port->mPortScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = port->mPortScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                PortWrap* obj__=jswrap::ObjectWrap::Unwrap<PortWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<PortWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""port->mPortScriptObj""|"<<*((void**)&(port->mPortScriptObj))<<"): " << objName << " - is a subclass of native ""Port""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""port->mPortScriptObj""|"<<*((void**)&(port->mPortScriptObj))<<"): " << objName << " - does not wrap ""Port""\n";
                }
            }
            else
            {
                Port* obj = dynamic_cast<Port*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""port->mPortScriptObj""|" << *((void**)&(port->mPortScriptObj)) << "): " << objName<<" - wraps native ""Port"" ("<<(void*)obj<<")\n";
            }
        } );
        SCRIPT_DEBUG_ONLY( if (this->mISpriteDrawHelperScriptObj.IsEmpty())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NIL JS Object (" "this->mISpriteDrawHelperScriptObj" "|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<")\n";
        }
        else if (!this->mISpriteDrawHelperScriptObj->IsObject())
        {
            std::cerr << __func__<<":"<< __LINE__ << " - NON JS Object (" "this->mISpriteDrawHelperScriptObj" "|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<")\n";
        }
        else
        {
            v8::Handle<v8::Object> obj_ = this->mISpriteDrawHelperScriptObj->ToObject();
                v8::String::Utf8Value objNameStr(obj_->ToString());
                char* objName = *objNameStr;
                ISpriteDrawHelperWrap* obj__=jswrap::ObjectWrap::Unwrap<ISpriteDrawHelperWrap>(obj_);
                if (!obj__)
            {
                v8::Handle<v8::Value> protoVal_ = obj_->GetPrototype();
                    if (protoVal_->IsObject())
                {
                    obj_ = protoVal_->ToObject();
                        obj__ = jswrap::ObjectWrap::Unwrap<ISpriteDrawHelperWrap>(obj_);
                }
                if (obj__)
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteDrawHelperScriptObj""|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<"): " << objName << " - is a subclass of native ""ISpriteDrawHelper""\n";
                }
                else
                {
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteDrawHelperScriptObj""|"<<*((void**)&(this->mISpriteDrawHelperScriptObj))<<"): " << objName << " - does not wrap ""ISpriteDrawHelper""\n";
                }
            }
            else
            {
                ISpriteDrawHelper* obj = dynamic_cast<ISpriteDrawHelper*>(obj__->getNativeObject());
                    std::cout << __func__<<":"<< __LINE__ << " - JS Object (""this->mISpriteDrawHelperScriptObj""|" << *((void**)&(this->mISpriteDrawHelperScriptObj)) << "): " << objName<<" - wraps native ""ISpriteDrawHelper"" ("<<(void*)obj<<")\n";
            }
        } );

        v8::Local<v8::Value> resVal;
        v8::Handle<v8::Function> func;
        if (!mScriptDrawFunc.IsEmpty() && mScriptDrawFunc->IsFunction())
        {
            func = mScriptDrawFunc;
        }
        else if (mISpriteDrawHelperScriptObj->Has(v8::String::New("draw")))
        {
            func = v8::Handle<v8::Function>::Cast(mISpriteDrawHelperScriptObj->Get(v8::String::New("draw")));
        }
        else
        {
            DEBUG_ONLY(
                v8::String::Utf8Value objectNameStr(mISpriteDrawHelperScriptObj->ToString());
                std::cerr << "fatal: IDrawSpriteHelper object " << *objectNameStr << " missing draw() Function!!";
                exit(1);
                )
                return false;
        }
        resVal = func->Call(this->mISpriteDrawHelperScriptObj, 2, argv);

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
        v8::HandleScope scope;
        const uint8 *cbuf = static_cast<const uint8*>(buf);
        uint16* twobytebuf = new uint16[len];
        for (size_t i = 0; i < len; i++)
        {
            twobytebuf[i] = cbuf[i];
        }
        v8::Local<v8::String> chunk = v8::String::New(twobytebuf, len);
        delete [] twobytebuf;
        return scope.Close(chunk);
    }

    void* DecodeBinary(v8::Handle<v8::Value> val, size_t* outLen)
    {
        v8::HandleScope scope;

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

    float CallScriptEasingFunc(int which, uint32 ut, float b, float c, uint32 ud)
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
        v8::HandleScope scope;

        v8::TryCatch try_catch;

        v8::Local<v8::Value> argv[4];
        argv[0] = v8::Local<v8::Value>::New(v8::Integer::NewFromUnsigned(ut));
        argv[1] = v8::Local<v8::Value>::New(v8::Number::New(b));
        argv[2] = v8::Local<v8::Value>::New(v8::Number::New(c));
        argv[3] = v8::Local<v8::Value>::New(v8::Integer::NewFromUnsigned(ud));
        v8::Local<v8::Value> resVal = s_CustomScriptEasing[which]->Call(v8::Context::GetCurrent()->Global(), 4, argv);;

        if (try_catch.HasCaught())
        {
            DEBUG_ONLY( OS::_DOUT( "Script Fatal Exception calling Easing Function!!" ); )
                FatalException(try_catch);
            return 0.0f;
        }
        if (!resVal->IsNumber())
        {

#ifdef DEBUG
            v8::String::Utf8Value funcNameStr(s_CustomScriptEasing[which]->GetName()->ToString());
            v8::String::Utf8Value resNameStr(s_CustomScriptEasing[which]->GetScriptOrigin().ResourceName()->ToString());
            std::cerr << "result mismatch: return value from easing Function must be a Number ("
                << *funcNameStr << " at " << *resNameStr << ":"
                << s_CustomScriptEasing[which]->GetScriptLineNumber()+1 << ")\n";
            exit(1);
#else
            return 0.0f;
#endif
        }
        return resVal->NumberValue();
    }

    v8::Handle<v8::Value> RegisterEasingFunction(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        if (args.Length() == 1 && args[0]->IsNull())
        {
            return scope.Close( v8::String::New("undefined" " function" "(function easingFunc)" " - " "") );
        };
        if (args.Length() != 1)
            return v8_ThrowArgCountException(args.Length(), 1);
        if (!args[1 -1]->IsFunction())
        {
            std::ostringstream excpt_;
            excpt_ << "argument ""1"" must be a function (""easingFunc"")";
            return v8::ThrowException( v8::Exception::TypeError( v8::String::New(excpt_.str().c_str())));
        }
        v8::Handle<v8::Function> easingFunc = v8::Handle<v8::Function>::Cast(args[1 -1]);;
        v8::Persistent<v8::Function> jsEasingFunc = v8::Persistent<v8::Function>::New(easingFunc);
        if (gNumCustomEasings >= MAX_CUSTOM_EASINGS)
        {
            std::ostringstream excpt_;
            excpt_ << "Can't register any more custom easing functions!!";
            return v8::ThrowException( v8::Exception::Error( v8::String::New(excpt_.str().c_str())));
        }
        else
        {
            s_CustomScriptEasing[gNumCustomEasings] = jsEasingFunc;
            v8::String::Utf8Value funcNameStr(jsEasingFunc->GetName()->ToString());
            int funcId = NUM_BUILTIN_EASINGS + gNumCustomEasings;
            CallScriptEasingFunc(gNumCustomEasings, 0, 0.0f, 0.0f, 1);
            gNumCustomEasings++;
            s_BindingTarget->Set(v8::String::NewSymbol("*funcNameStr"), v8::Integer::New(funcId),
                static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
            DEBUG_ONLY( OS::_DOUT( "Registered custom easing Function %d as constant name %s [%d]",
                gNumCustomEasings, *funcNameStr, funcId); )
        }
        return scope.Close( v8::Undefined() );
    }

    v8::Handle<v8::Value> FinishedScriptSetup(const v8::Arguments& args)
    {
        v8::HandleScope scope;

        scriptSetupCompleted();
        return scope.Close( v8::Undefined() );
    }

    SCRIPT_DEBUG_ONLY(
        static size_t sLastHeapUsed = 0;
        static long sIdleLastHeapReport = OS::getMilliseconds();
        )

        void initBindings(v8::Handle<v8::Object> target);

    void initBindings(v8::Handle<v8::Object> target)
    {
        v8::HandleScope scope;

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

        s_BindingTarget = v8::Persistent<v8::Object>::New(target);

        MemBlockWrap::Init(target);;
        FileManagerWrap::Init(target);;
        LogManagerWrap::Init(target);;
        ConfigManagerWrap::Init(target);;
        ResourceManagerWrap::Init(target);;
        SerializerWrap::Init(target);;
        DeserializerWrap::Init(target);;
        ISerializableWrap::Init(target);;
        IEventHandlerWrap::Init(target);;
        EventEmitterWrap::Init(target);;
        EventManagerWrap::Init(target);;
        TimerManagerWrap::Init(target);;
        IAnimationHelperWrap::Init(target);;
        AnimatedWrap::Init(target);;
#ifdef PDG_USE_CHIPMUNK_PHYSICS
        cpArbiterWrap::Init(target);;
        cpConstraintWrap::Init(target);;
        cpSpaceWrap::Init(target);;
#endif
#ifndef PDG_NO_GUI
        ISpriteDrawHelperWrap::Init(target);;
#endif
        ISpriteCollideHelperWrap::Init(target);;
        SpriteWrap::Init(target);;
        SpriteLayerWrap::Init(target);;
        TileLayerWrap::Init(target);;
        ImageWrap::Init(target);;
        ImageStripWrap::Init(target);;
#ifndef PDG_NO_GUI
        FontWrap::Init(target);;
        PortWrap::Init(target);;
        GraphicsManagerWrap::Init(target);;
#endif
#ifndef PDG_NO_SOUND
        SoundWrap::Init(target);;
        SoundManagerWrap::Init(target);;
#endif

        target->Set(v8::String::NewSymbol("_idle"), v8::FunctionTemplate::New(Idle)->GetFunction());;
        target->Set(v8::String::NewSymbol("_run"), v8::FunctionTemplate::New(Run)->GetFunction());;
        target->Set(v8::String::NewSymbol("_quit"), v8::FunctionTemplate::New(Quit)->GetFunction());;
        target->Set(v8::String::NewSymbol("_isQuitting"), v8::FunctionTemplate::New(IsQuitting)->GetFunction());;
        target->Set(v8::String::NewSymbol("_finishedScriptSetup"), v8::FunctionTemplate::New(FinishedScriptSetup)->GetFunction());;

        target->Set(v8::String::NewSymbol("rand"), v8::FunctionTemplate::New(GameCriticalRandom)->GetFunction());;
        target->Set(v8::String::NewSymbol("srand"), v8::FunctionTemplate::New(Srand)->GetFunction());;

        target->Set(v8::String::NewSymbol("setSerializationDebugMode"), v8::FunctionTemplate::New(SetSerializationDebugMode)->GetFunction());;

        target->Set(v8::String::NewSymbol("registerEasingFunction"), v8::FunctionTemplate::New(RegisterEasingFunction)->GetFunction());;

        target->Set(v8::String::NewSymbol("getFileManager"), v8::FunctionTemplate::New(GetFileManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("getLogManager"), v8::FunctionTemplate::New(GetLogManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("getConfigManager"), v8::FunctionTemplate::New(GetConfigManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("getResourceManager"), v8::FunctionTemplate::New(GetResourceManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("getEventManager"), v8::FunctionTemplate::New(GetEventManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("getTimerManager"), v8::FunctionTemplate::New(GetTimerManager)->GetFunction());;
        target->Set(v8::String::NewSymbol("registerSerializableClass"), v8::FunctionTemplate::New(RegisterSerializableClass)->GetFunction());;

#ifndef PDG_NO_GUI
        target->Set(v8::String::NewSymbol("getGraphicsManager"), v8::FunctionTemplate::New(GetGraphicsManager)->GetFunction());;
#endif
#ifndef PDG_NO_SOUND
        target->Set(v8::String::NewSymbol("getSoundManager"), v8::FunctionTemplate::New(GetSoundManager)->GetFunction());;
#endif

        target->Set(v8::String::NewSymbol("createSpriteLayer"), v8::FunctionTemplate::New(CreateSpriteLayer)->GetFunction());;
        target->Set(v8::String::NewSymbol("cleanupSpriteLayer"), v8::FunctionTemplate::New(CleanupSpriteLayer)->GetFunction());;
        target->Set(v8::String::NewSymbol("createTileLayer"), v8::FunctionTemplate::New(CreateTileLayer)->GetFunction());;

        target->Set(v8::String::NewSymbol("all_events"), v8::Integer::New(all_events), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("eventType_Shutdown"), v8::Integer::New(eventType_Shutdown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_Timer"), v8::Integer::New(eventType_Timer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_KeyDown"), v8::Integer::New(eventType_KeyDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_KeyUp"), v8::Integer::New(eventType_KeyUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_KeyPress"), v8::Integer::New(eventType_KeyPress), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_MouseDown"), v8::Integer::New(eventType_MouseDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_MouseUp"), v8::Integer::New(eventType_MouseUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_MouseMove"), v8::Integer::New(eventType_MouseMove), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_MouseEnter"), v8::Integer::New(eventType_MouseEnter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_MouseLeave"), v8::Integer::New(eventType_MouseLeave), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_PortResized"), v8::Integer::New(eventType_PortResized), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_ScrollWheel"), v8::Integer::New(eventType_ScrollWheel), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SpriteTouch"), v8::Integer::New(eventType_SpriteTouch), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SpriteAnimate"), v8::Integer::New(eventType_SpriteAnimate), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SpriteLayer"), v8::Integer::New(eventType_SpriteLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SpriteCollide"), v8::Integer::New(eventType_SpriteCollide), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SpriteBreak"), v8::Integer::New(eventType_SpriteBreak), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_SoundEvent"), v8::Integer::New(eventType_SoundEvent), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("eventType_PortDraw"), v8::Integer::New(eventType_PortDraw), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("soundEvent_DonePlaying"), v8::Integer::New(soundEvent_DonePlaying), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("soundEvent_Looping"), v8::Integer::New(soundEvent_Looping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("soundEvent_FailedToPlay"), v8::Integer::New(soundEvent_FailedToPlay), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("key_Break"), v8::Integer::New(key_Break), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Home"), v8::Integer::New(key_Home), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_End"), v8::Integer::New(key_End), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Clear"), v8::Integer::New(key_Clear), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Help"), v8::Integer::New(key_Help), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Pause"), v8::Integer::New(key_Pause), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Mute"), v8::Integer::New(key_Mute), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Backspace"), v8::Integer::New(key_Backspace), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Delete"), v8::Integer::New(key_Delete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Tab"), v8::Integer::New(key_Tab), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_PageUp"), v8::Integer::New(key_PageUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_PageDown"), v8::Integer::New(key_PageDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Return"), v8::Integer::New(key_Return), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Enter"), v8::Integer::New(key_Enter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F1"), v8::Integer::New(key_F1), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F2"), v8::Integer::New(key_F2), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F3"), v8::Integer::New(key_F3), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F4"), v8::Integer::New(key_F4), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F5"), v8::Integer::New(key_F5), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F6"), v8::Integer::New(key_F6), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F7"), v8::Integer::New(key_F7), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F8"), v8::Integer::New(key_F8), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F9"), v8::Integer::New(key_F9), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F10"), v8::Integer::New(key_F10), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F11"), v8::Integer::New(key_F11), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_F12"), v8::Integer::New(key_F12), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_FirstF"), v8::Integer::New(key_FirstF), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_LastF"), v8::Integer::New(key_LastF), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Insert"), v8::Integer::New(key_Insert), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_Escape"), v8::Integer::New(key_Escape), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_LeftArrow"), v8::Integer::New(key_LeftArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_RightArrow"), v8::Integer::New(key_RightArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_UpArrow"), v8::Integer::New(key_UpArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_DownArrow"), v8::Integer::New(key_DownArrow), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("key_FirstPrintable"), v8::Integer::New(key_FirstPrintable), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("screenPos_Normal"), v8::Integer::New(screenPos_Normal), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("screenPos_Rotated180"), v8::Integer::New(screenPos_Rotated180), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("screenPos_Rotated90Clockwise"), v8::Integer::New(screenPos_Rotated90Clockwise), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("screenPos_Rotated90CounterClockwise"), v8::Integer::New(screenPos_Rotated90CounterClockwise), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("screenPos_FaceUp"), v8::Integer::New(screenPos_FaceUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("screenPos_FaceDown"), v8::Integer::New(screenPos_FaceDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

#ifndef PDG_NO_GUI
        target->Set(v8::String::NewSymbol("textStyle_Plain"), v8::Integer::New(Graphics::textStyle_Plain), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_Bold"), v8::Integer::New(Graphics::textStyle_Bold), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_Italic"), v8::Integer::New(Graphics::textStyle_Italic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_Underline"), v8::Integer::New(Graphics::textStyle_Underline), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_Centered"), v8::Integer::New(Graphics::textStyle_Centered), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_LeftJustified"), v8::Integer::New(Graphics::textStyle_LeftJustified), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("textStyle_RightJustified"), v8::Integer::New(Graphics::textStyle_RightJustified), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
#endif

        target->Set(v8::String::NewSymbol("fit_None"), v8::Integer::New(Image::fit_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("fit_Height"), v8::Integer::New(Image::fit_Height), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("fit_Width"), v8::Integer::New(Image::fit_Width), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("fit_Inside"), v8::Integer::New(Image::fit_Inside), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("fit_Fill"), v8::Integer::New(Image::fit_Fill), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("fit_FillKeepProportions"), v8::Integer::New(Image::fit_FillKeepProportions), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("init_CreateUniqueNewFile"), v8::Integer::New(LogManager::init_CreateUniqueNewFile), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("init_OverwriteExisting"), v8::Integer::New(LogManager::init_OverwriteExisting), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("init_AppendToExisting"), v8::Integer::New(LogManager::init_AppendToExisting), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("init_StdOut"), v8::Integer::New(LogManager::init_StdOut), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("init_StdErr"), v8::Integer::New(LogManager::init_StdErr), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("duration_Constant"), v8::Integer::New(Sprite::duration_Constant), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("duration_Instantaneous"), v8::Integer::New(Sprite::duration_Instantaneous), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("animate_StartToEnd"), v8::Integer::New(Sprite::animate_StartToEnd), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("animate_EndToStart"), v8::Integer::New(Sprite::animate_EndToStart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("animate_Unidirectional"), v8::Integer::New(Sprite::animate_Unidirectional), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("animate_Bidirectional"), v8::Integer::New(Sprite::animate_Bidirectional), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("animate_NoLooping"), v8::Integer::New(Sprite::animate_NoLooping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("animate_Looping"), v8::Integer::New(Sprite::animate_Looping), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("start_FromFirstFrame"), v8::Integer::New(Sprite::start_FromFirstFrame), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("start_FromLastFrame"), v8::Integer::New(Sprite::start_FromLastFrame), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("all_Frames"), v8::Integer::New(Sprite::all_Frames), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("action_CollideSprite"), v8::Integer::New(Sprite::action_CollideSprite), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_CollideWall"), v8::Integer::New(Sprite::action_CollideWall), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_Offscreen"), v8::Integer::New(Sprite::action_Offscreen), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_Onscreen"), v8::Integer::New(Sprite::action_Onscreen), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_ExitLayer"), v8::Integer::New(Sprite::action_ExitLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_AnimationLoop"), v8::Integer::New(Sprite::action_AnimationLoop), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_AnimationEnd"), v8::Integer::New(Sprite::action_AnimationEnd), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_FadeComplete"), v8::Integer::New(Sprite::action_FadeComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_FadeInComplete"), v8::Integer::New(Sprite::action_FadeInComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_FadeOutComplete"), v8::Integer::New(Sprite::action_FadeOutComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_JointBreak"), v8::Integer::New(Sprite::action_JointBreak), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("touch_MouseEnter"), v8::Integer::New(Sprite::touch_MouseEnter), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("touch_MouseLeave"), v8::Integer::New(Sprite::touch_MouseLeave), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("touch_MouseDown"), v8::Integer::New(Sprite::touch_MouseDown), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("touch_MouseUp"), v8::Integer::New(Sprite::touch_MouseUp), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("touch_MouseClick"), v8::Integer::New(Sprite::touch_MouseClick), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("collide_None"), v8::Integer::New(Sprite::collide_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("collide_Point"), v8::Integer::New(Sprite::collide_Point), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("collide_BoundingBox"), v8::Integer::New(Sprite::collide_BoundingBox), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("collide_CollisionRadius"), v8::Integer::New(Sprite::collide_CollisionRadius), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("collide_AlphaChannel"), v8::Integer::New(Sprite::collide_AlphaChannel), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("collide_Last"), v8::Integer::New(Sprite::collide_Last), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("action_ErasePort"), v8::Integer::New(SpriteLayer::action_ErasePort), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_PreDrawLayer"), v8::Integer::New(SpriteLayer::action_PreDrawLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_PostDrawLayer"), v8::Integer::New(SpriteLayer::action_PostDrawLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_DrawPortComplete"), v8::Integer::New(SpriteLayer::action_DrawPortComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_AnimationStart"), v8::Integer::New(SpriteLayer::action_AnimationStart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_PreAnimateLayer"), v8::Integer::New(SpriteLayer::action_PreAnimateLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_PostAnimateLayer"), v8::Integer::New(SpriteLayer::action_PostAnimateLayer), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_AnimationComplete"), v8::Integer::New(SpriteLayer::action_AnimationComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_ZoomComplete"), v8::Integer::New(SpriteLayer::action_ZoomComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_LayerFadeInComplete"), v8::Integer::New(SpriteLayer::action_FadeInComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("action_LayerFadeOutComplete"), v8::Integer::New(SpriteLayer::action_FadeOutComplete), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("facing_North"), v8::Integer::New(TileLayer::facing_North), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("facing_East"), v8::Integer::New(TileLayer::facing_East), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("facing_South"), v8::Integer::New(TileLayer::facing_South), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("facing_West"), v8::Integer::New(TileLayer::facing_West), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("facing_Ignore"), v8::Integer::New(TileLayer::facing_Ignore), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("flipped_None"), v8::Integer::New(TileLayer::flipped_None), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("flipped_Horizontal"), v8::Integer::New(TileLayer::flipped_Horizontal), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("flipped_Vertical"), v8::Integer::New(TileLayer::flipped_Vertical), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("flipped_Both"), v8::Integer::New(TileLayer::flipped_Both), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("flipped_Ignore"), v8::Integer::New(TileLayer::flipped_Ignore), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("timer_OneShot"), v8::Boolean::New(timer_OneShot), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("timer_Repeating"), v8::Boolean::New(timer_Repeating), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("timer_Never"), v8::Integer::New(timer_Never), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("linearTween"), v8::Integer::New(EasingFuncRef::linearTween), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInQuad"), v8::Integer::New(EasingFuncRef::easeInQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutQuad"), v8::Integer::New(EasingFuncRef::easeOutQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutQuad"), v8::Integer::New(EasingFuncRef::easeInOutQuad), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInCubic"), v8::Integer::New(EasingFuncRef::easeInCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutCubic"), v8::Integer::New(EasingFuncRef::easeOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutCubic"), v8::Integer::New(EasingFuncRef::easeInOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInQuart"), v8::Integer::New(EasingFuncRef::easeInQuart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutQuart"), v8::Integer::New(EasingFuncRef::easeOutQuart), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutCubic"), v8::Integer::New(EasingFuncRef::easeInOutCubic), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInQuint"), v8::Integer::New(EasingFuncRef::easeInQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutQuint"), v8::Integer::New(EasingFuncRef::easeOutQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutQuint"), v8::Integer::New(EasingFuncRef::easeInOutQuint), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInSine"), v8::Integer::New(EasingFuncRef::easeInSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutSine"), v8::Integer::New(EasingFuncRef::easeOutSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutSine"), v8::Integer::New(EasingFuncRef::easeInOutSine), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInExpo"), v8::Integer::New(EasingFuncRef::easeInExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutExpo"), v8::Integer::New(EasingFuncRef::easeOutExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutExpo"), v8::Integer::New(EasingFuncRef::easeInOutExpo), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInCirc"), v8::Integer::New(EasingFuncRef::easeInCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutCirc"), v8::Integer::New(EasingFuncRef::easeOutCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutCirc"), v8::Integer::New(EasingFuncRef::easeInOutCirc), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInBounce"), v8::Integer::New(EasingFuncRef::easeInBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutBounce"), v8::Integer::New(EasingFuncRef::easeOutBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutBounce"), v8::Integer::New(EasingFuncRef::easeInOutBounce), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInBack"), v8::Integer::New(EasingFuncRef::easeInBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeOutBack"), v8::Integer::New(EasingFuncRef::easeOutBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("easeInOutBack"), v8::Integer::New(EasingFuncRef::easeInOutBack), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));

        target->Set(v8::String::NewSymbol("ser_Positions"), v8::Integer::New(ser_Positions), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_ZOrder"), v8::Integer::New(ser_ZOrder), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Sizes"), v8::Integer::New(ser_Sizes), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Animations"), v8::Integer::New(ser_Animations), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Motion"), v8::Integer::New(ser_Motion), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Forces"), v8::Integer::New(ser_Forces), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Physics"), v8::Integer::New(ser_Physics), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_LayerDraw"), v8::Integer::New(ser_LayerDraw), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_ImageRefs"), v8::Integer::New(ser_ImageRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_SCMLRefs"), v8::Integer::New(ser_SCMLRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_HelperRefs"), v8::Integer::New(ser_HelperRefs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_HelperObjs"), v8::Integer::New(ser_HelperObjs), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_InitialData"), v8::Integer::New(ser_InitialData), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Micro"), v8::Integer::New(ser_Micro), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Update"), v8::Integer::New(ser_Update), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
        target->Set(v8::String::NewSymbol("ser_Full"), v8::Integer::New(ser_Full), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
    }

    void CreateSingletons()
    {

        FileManagerWrap::GetScriptSingletonInstance();

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

    v8::V8::IdleNotification();

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
