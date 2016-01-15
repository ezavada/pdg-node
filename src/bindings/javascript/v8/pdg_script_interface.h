// -----------------------------------------------
// This file automatically generated from:
//
//    work/pdg/src/bindings/common/pdg_script_interface.h
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



#ifndef PDG_INTERFACES_H_INCLUDED
#define PDG_INTERFACES_H_INCLUDED

#include "pdg_project.h"

#include "pdg_script_impl.h"
#include "pdg_script.h"

#ifndef PDG_NO_APP_FRAMEWORK
#define PDG_NO_APP_FRAMEWORK
#endif
#include "pdg/framework.h"

#include <cstdlib>

namespace pdg
{

    MemBlock* New_MemBlock(const v8::FunctionCallbackInfo<v8::Value>& args);

    class MemBlockWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            MemBlock* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            MemBlock* cppPtr_;

            MemBlockWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_MemBlock(args);
            }

            ~MemBlockWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, MemBlock* cppObj);
            MemBlockWrap(MemBlock* obj) : cppPtr_(obj) {}

            static void GetData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDataSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetByte (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetBytes (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    ConfigManager* New_ConfigManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ConfigManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ConfigManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ConfigManager* cppPtr_;

            ConfigManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ConfigManager(args);
            }

            ~ConfigManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static ConfigManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void UseConfig (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetConfigString (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetConfigLong (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetConfigFloat (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetConfigBool (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetConfigString (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetConfigLong (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetConfigFloat (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetConfigBool (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    LogManager* New_LogManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class LogManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            LogManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            LogManager* cppPtr_;

            LogManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_LogManager(args);
            }

            ~LogManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static LogManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void GetLogLevel (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLogLevel (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Initialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void WriteLogEntry (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BinaryDump (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    IEventHandler* New_IEventHandler(const v8::FunctionCallbackInfo<v8::Value>& args);

    class IEventHandlerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            IEventHandler* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            IEventHandler* cppPtr_;

            IEventHandlerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_IEventHandler(args);
            }

            ~IEventHandlerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, IEventHandler* cppObj);
            IEventHandlerWrap(IEventHandler* obj) : cppPtr_(obj) {}

    };

    EventEmitter* New_EventEmitter(const v8::FunctionCallbackInfo<v8::Value>& args);

    class EventEmitterWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            EventEmitter* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            EventEmitter* cppPtr_;

            EventEmitterWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_EventEmitter(args);
            }

            ~EventEmitterWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, EventEmitter* cppObj);
            EventEmitterWrap(EventEmitter* obj) : cppPtr_(obj) {}

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);

    };

    EventManager* New_EventManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class EventManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            EventManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            EventManager* cppPtr_;

            EventManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_EventManager(args);
            }

            ~EventManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static EventManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsKeyDown (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsRawKeyDown (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsButtonDown (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDeviceOrientation (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    ResourceManager* New_ResourceManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ResourceManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ResourceManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ResourceManager* cppPtr_;

            ResourceManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ResourceManager(args);
            }

            ~ResourceManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static ResourceManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void GetLanguage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLanguage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void OpenResourceFile (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CloseResourceFile (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetImage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetImageStrip (const v8::FunctionCallbackInfo<v8::Value>& args);
#ifndef PDG_NO_SOUND
            static void GetSound (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
            static void GetString (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetResourceSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetResource (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetResourcePaths (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    Serializer* New_Serializer(const v8::FunctionCallbackInfo<v8::Value>& args);

    class SerializerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Serializer* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Serializer* cppPtr_;

            SerializerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Serializer(args);
            }

            ~SerializerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Serializer* cppObj);
            SerializerWrap(Serializer* obj) : cppPtr_(obj) {}

#ifndef PDG_NO_64BIT
            static void Serialize_8 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_8u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_8 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_8u (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
            static void Serialize_d (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_f (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_4 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_4u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_3u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_2 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_2u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_1 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_1u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_bool (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_uint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_color (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_offset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_point (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_vector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_rect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_rotr (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_quad (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_str (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_mem (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_obj (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize_ref (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SerializedSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_d (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_f (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_4 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_4u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_3u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_2 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_2u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_1 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_1u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_bool (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_uint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_color (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_offset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_point (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_vector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_rect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_rotr (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_quad (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_str (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_mem (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_obj (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Sizeof_ref (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDataSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDataPtr (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    Deserializer* New_Deserializer(const v8::FunctionCallbackInfo<v8::Value>& args);

    class DeserializerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Deserializer* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Deserializer* cppPtr_;

            DeserializerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Deserializer(args);
            }

            ~DeserializerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Deserializer* cppObj);
            DeserializerWrap(Deserializer* obj) : cppPtr_(obj) {}

#ifndef PDG_NO_64BIT
            static void Deserialize_8 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_8u (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
            static void Deserialize_d (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_f (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_4 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_4u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_3u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_2 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_2u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_1 (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_1u (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_bool (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_uint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_color (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_offset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_point (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_vector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_rect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_rotr (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_quad (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_str (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_strGetLen (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_mem (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_memGetLen (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_obj (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize_ref (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetDataPtr (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    ISerializable* New_ISerializable(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ISerializableWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISerializable* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ISerializable* cppPtr_;

            ISerializableWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ISerializable(args);
            }

            ~ISerializableWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, ISerializable* cppObj);
            ISerializableWrap(ISerializable* obj) : cppPtr_(obj) {}

    };

    Image* New_Image(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ImageWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Image* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Image* cppPtr_;

            ImageWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Image(args);
            }

            ~ImageWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Image* cppObj);
            ImageWrap(Image* obj) : cppPtr_(obj) {}

            static void GetTransparentColor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetTransparentColor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetImageBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSubsection (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetEdgeClamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RetainData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RetainAlpha (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PrepareToRasterize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetAlphaValue (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetPixel (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    ImageStrip* New_ImageStrip(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ImageStripWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ImageStrip* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ImageStrip* cppPtr_;

            ImageStripWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ImageStrip(args);
            }

            ~ImageStripWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, ImageStrip* cppObj);
            ImageStripWrap(ImageStrip* obj) : cppPtr_(obj) {}

            static void GetTransparentColor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetTransparentColor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetImageBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSubsection (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetEdgeClamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RetainData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RetainAlpha (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PrepareToRasterize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetAlphaValue (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetPixel (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFrameWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFrameWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNumFrames (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetNumFrames (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFrame (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

#ifndef PDG_NO_GUI

    GraphicsManager* New_GraphicsManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class GraphicsManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            GraphicsManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            GraphicsManager* cppPtr_;

            GraphicsManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_GraphicsManager(args);
            }

            ~GraphicsManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static GraphicsManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void GetNumScreens (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNumSupportedScreenModes (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNthSupportedScreenMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCurrentScreenMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetScreenMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateWindowPort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateFullScreenPort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CloseGraphicsPort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateFont (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMainPort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SwitchToFullScreenMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SwitchToWindowMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void InFullScreenMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFPS (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTargetFPS (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetTargetFPS (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMouse (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    class FontWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Font* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Font* cppPtr_;

            FontWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                cppPtr_ = 0;
            }

            ~FontWrap()
            {
                cppPtr_ = 0;
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Font* cppObj);
            FontWrap(Font* obj) : cppPtr_(obj) {}

            static void GetFontName (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFontHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFontLeading (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFontAscent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFontDescent (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    class PortWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Port* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Port* cppPtr_;

            PortWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                cppPtr_ = 0;
            }

            ~PortWrap()
            {
                cppPtr_ = 0;
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Port* cppObj);
            PortWrap(Port* obj) : cppPtr_(obj) {}

            static void GetClipRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetClipRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCursor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCursor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDrawingArea (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FrameRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawLine (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FrameOval (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillOval (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FrameCircle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillCircle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FrameRoundRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillRoundRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillRectEx (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FrameRectEx (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawLineEx (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FillRectWithGradient (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawText (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawImage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawTexture (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DrawTexturedSphere (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTextWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCurrentFont (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFont (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFontForStyle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFontScalingFactor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartTrackingMouse (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopTrackingMouse (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ResetCursor (const v8::FunctionCallbackInfo<v8::Value>& args);
    };
#endif

#ifndef PDG_NO_SOUND
    Sound* New_Sound(const v8::FunctionCallbackInfo<v8::Value>& args);

    class SoundWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Sound* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Sound* cppPtr_;

            SoundWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Sound(args);
            }

            ~SoundWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Sound* cppObj);
            SoundWrap(Sound* obj) : cppPtr_(obj) {}

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetVolume (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVolume (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Play (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Start (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Stop (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Pause (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Resume (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsPaused (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLooping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsLooping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetPitch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangePitch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOffsetX (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeOffsetX (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeOut (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeIn (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeVolume (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Skip (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SkipTo (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    SoundManager* New_SoundManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class SoundManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            SoundManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            SoundManager* cppPtr_;

            SoundManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_SoundManager(args);
            }

            ~SoundManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static SoundManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void SetVolume (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMute (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Idle (const v8::FunctionCallbackInfo<v8::Value>& args);
    };
#endif

    TimerManager* New_TimerManager(const v8::FunctionCallbackInfo<v8::Value>& args);

    class TimerManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            TimerManager* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            TimerManager* cppPtr_;

            TimerManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_TimerManager(args);
            }

            ~TimerManagerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static TimerManager* getSingletonInstance();
        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartTimer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CancelTimer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CancelAllTimers (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DelayTimer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DelayTimerUntil (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Pause (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Unpause (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsPaused (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PauseTimer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnpauseTimer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsTimerPaused (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWhenTimerFiresNext (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMilliseconds (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    class FileManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;

            FileManagerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) { }

            ~FileManagerWrap() { }

        public:
            static v8::Local<v8::Object> GetScriptSingletonInstance(v8::Isolate* isolate);
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static void FindFirst (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FindNext (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FindClose (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetApplicationDataDirectory (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetApplicationDirectory (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetApplicationResourceDirectory (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    IAnimationHelper* New_IAnimationHelper(const v8::FunctionCallbackInfo<v8::Value>& args);

    class IAnimationHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            IAnimationHelper* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            IAnimationHelper* cppPtr_;

            IAnimationHelperWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_IAnimationHelper(args);
            }

            ~IAnimationHelperWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, IAnimationHelper* cppObj);
            IAnimationHelperWrap(IAnimationHelper* obj) : cppPtr_(obj) {}

    };

    Animated* New_Animated(const v8::FunctionCallbackInfo<v8::Value>& args);

    class AnimatedWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Animated* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Animated* cppPtr_;

            AnimatedWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Animated(args);
            }

            ~AnimatedWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Animated* cppObj);
            AnimatedWrap(Animated* obj) : cppPtr_(obj) {}

            static void GetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetBoundingBox (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotatedBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Move (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocityInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMovementDirectionInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopMoving (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Accelerate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AccelerateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Grow (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Stretch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Resize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ResizeTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Rotate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopSpinning (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenterTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Wait (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyTorque (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAllForces (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ClearAnimationHelpers (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Animate (const v8::FunctionCallbackInfo<v8::Value>& args);

    };

#ifdef PDG_USE_CHIPMUNK_PHYSICS
    class cpArbiterWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpArbiter* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            cpArbiter* cppPtr_;

            cpArbiterWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                cppPtr_ = 0;
            }

            ~cpArbiterWrap()
            {
                cppPtr_ = 0;
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, cpArbiter* cppObj);
            cpArbiterWrap(cpArbiter* obj) : cppPtr_(obj) {}

            static void IsFirstContact (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCount (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNormal (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetPointA (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetPointB (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetDepth (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    class cpConstraintWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpConstraint* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            cpConstraint* cppPtr_;

            cpConstraintWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                cppPtr_ = 0;
            }

            ~cpConstraintWrap()
            {
                cppPtr_ = 0;
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, cpConstraint* cppObj);
            cpConstraintWrap(cpConstraint* obj) : cppPtr_(obj) {}

            static void GetMaxForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMaxForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetErrorBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetErrorBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMaxBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMaxBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetAnchor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetAnchor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOtherAnchor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOtherAnchor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetPinDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetPinDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSlideMinDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSlideMinDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSlideMaxDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSlideMaxDist (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetGrooveStart (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGrooveStart (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetGrooveEnd (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGrooveEnd (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpringRestLength (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpringRestLength (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpringStiffness (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpringStiffness (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpringDamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpringDamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotarySpringRestAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRotarySpringRestAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMinAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMinAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMaxAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMaxAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRatchetAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRatchetAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRatchetPhase (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRatchetPhase (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRatchetInterval (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRatchetInterval (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetGearRatio (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGearRatio (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetGearInitialAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGearInitialAngle (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMotorSpinRate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMotorSpinRate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetType (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ActivateBodies (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetImpulse (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOtherSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    class cpSpaceWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpSpace* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            cpSpace* cppPtr_;

            cpSpaceWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
            {
                cppPtr_ = 0;
            }

            ~cpSpaceWrap()
            {
                cppPtr_ = 0;
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, cpSpace* cppObj);
            cpSpaceWrap(cpSpace* obj) : cppPtr_(obj) {}

            static void GetIdleSpeedThreshold (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetIdleSpeedThreshold (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSleepTimeThreshold (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSleepTimeThreshold (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCollisionSlop (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollisionSlop (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCollisionBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollisionBias (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCollisionPersistence (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollisionPersistence (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UseSpatialHash (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ReindexStatic (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Step (const v8::FunctionCallbackInfo<v8::Value>& args);
    };
#endif

    ISpriteCollideHelper* New_ISpriteCollideHelper(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ISpriteCollideHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISpriteCollideHelper* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ISpriteCollideHelper* cppPtr_;

            ISpriteCollideHelperWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ISpriteCollideHelper(args);
            }

            ~ISpriteCollideHelperWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, ISpriteCollideHelper* cppObj);
            ISpriteCollideHelperWrap(ISpriteCollideHelper* obj) : cppPtr_(obj) {}

    };

#ifndef PDG_NO_GUI
    ISpriteDrawHelper* New_ISpriteDrawHelper(const v8::FunctionCallbackInfo<v8::Value>& args);

    class ISpriteDrawHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISpriteDrawHelper* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            ISpriteDrawHelper* cppPtr_;

            ISpriteDrawHelperWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_ISpriteDrawHelper(args);
            }

            ~ISpriteDrawHelperWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, ISpriteDrawHelper* cppObj);
            ISpriteDrawHelperWrap(ISpriteDrawHelper* obj) : cppPtr_(obj) {}

    };
#endif

    Sprite* New_Sprite(const v8::FunctionCallbackInfo<v8::Value>& args);

    class SpriteWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Sprite* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            Sprite* cppPtr_;

            SpriteWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_Sprite(args);
            }

            ~SpriteWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, Sprite* cppObj);
            SpriteWrap(Sprite* obj) : cppPtr_(obj) {}

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetBoundingBox (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotatedBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Move (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocityInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMovementDirectionInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopMoving (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Accelerate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AccelerateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Grow (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Stretch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Resize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ResizeTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Rotate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopSpinning (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenterTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Wait (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyTorque (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAllForces (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ClearAnimationHelpers (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSerializedSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMyClassTag (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWantsAnimLoopEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsAnimLoopEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWantsAnimEndEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsAnimEndEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOpacity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCollisionRadius (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollisionRadius (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetElasticity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetElasticity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWantsCollideWallEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsCollideWallEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFrameRotatedBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFrame (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCurrentFrame (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFrameCount (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartFrameAnimation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopFrameAnimation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddFramesImage (const v8::FunctionCallbackInfo<v8::Value>& args);
#ifdef PDG_SCML_SUPPORT
            static void HasAnimation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartAnimation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetEntityScale (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
#ifndef PDG_NO_GUI
            static void GetWantsMouseOverEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsMouseOverEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWantsClickEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsClickEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMouseDetectMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMouseDetectMode (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWantsOffscreenEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWantsOffscreenEvents (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetDrawHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetPostDrawHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
            static void ChangeFramesImage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void OffsetFrameCenters (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFrameCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeIn (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeOut (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetZOrder (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveInFrontOf (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToFront (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToBack (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void EnableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DisableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UseCollisionMask (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollisionHelper (const v8::FunctionCallbackInfo<v8::Value>& args);;
            static void SetUserData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FreeUserData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static void GetCollideGroup (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCollideGroup (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MakeStatic (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PinJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SlideJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PivotJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GrooveJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SpringJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotarySpring (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotaryLimit (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Ratchet (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Gear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Motor (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveJoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Disconnect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MakeJointBreakable (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MakeJointUnbreakable (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
    };

#ifndef PDG_NO_GUI
#endif

    SpriteLayer* New_SpriteLayer(const v8::FunctionCallbackInfo<v8::Value>& args);

    class SpriteLayerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            SpriteLayer* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            SpriteLayer* cppPtr_;

            SpriteLayerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_SpriteLayer(args);
            }

            ~SpriteLayerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, SpriteLayer* cppObj);
            SpriteLayerWrap(SpriteLayer* obj) : cppPtr_(obj) {}

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetBoundingBox (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotatedBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Move (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocityInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMovementDirectionInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopMoving (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Accelerate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AccelerateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Grow (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Stretch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Resize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ResizeTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Rotate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopSpinning (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenterTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Wait (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyTorque (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAllForces (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ClearAnimationHelpers (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSerializedSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMyClassTag (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSerializationFlags (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartAnimations (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAnimations (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Hide (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Show (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsHidden (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeIn (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeOut (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveInFrontOf (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToFront (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToBack (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetZOrder (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveWith (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FindSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNthSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpriteZOrder (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsSpriteBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void HasSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAllSprites (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void EnableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DisableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void EnableCollisionsWithLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DisableCollisionsWithLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
#ifndef PDG_NO_GUI
            static void GetSpritePort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpritePort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOrigin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOrigin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetZoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetZoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Zoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ZoomTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetAutoCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFixedMoveAxis (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortPoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortVector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortQuad (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerPoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerVector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerQuad (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static void SetUseChipmunkPhysics (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetStaticLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGravity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetKeepGravityDownward (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetDamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpace (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
#ifdef PDG_SCML_SUPPORT
            static void CreateSpriteFromSCML (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateSpriteFromSCMLFile (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateSpriteFromSCMLEntity (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
    };

    TileLayer* New_TileLayer(const v8::FunctionCallbackInfo<v8::Value>& args);

    class TileLayerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Isolate* isolate, v8::Local<v8::Object> target);
            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            TileLayer* getCppObject()
            {
                return cppPtr_;
            }
        protected:
            TileLayer* cppPtr_;

            TileLayerWrap(const v8::FunctionCallbackInfo<v8::Value>& args) : cppPtr_(NULL)
            {
                cppPtr_ = New_TileLayer(args);
            }

            ~TileLayerWrap()
            {
                if (cppPtr_)
                {
                    delete cppPtr_;
                    cppPtr_ = NULL;
                }
            }

        public:
            static v8::Local<v8::Object> NewFromCpp(v8::Isolate* isolate, TileLayer* cppObj);
            TileLayerWrap(TileLayer* obj) : cppPtr_(obj) {}

            static void AddHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveHandler (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Clear (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void BlockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void UnblockEvent (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetLocation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpeed (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWidth (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetHeight (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetRotation (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetCenterOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMass (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetMoveFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpinFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSizeFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetBoundingBox (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetRotatedBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Move (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetVelocityInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMovementDirectionInRadians (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopMoving (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Accelerate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AccelerateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Grow (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Stretch (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopGrowing (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopStretching (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Resize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ResizeTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Rotate (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RotateTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopSpinning (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ChangeCenterTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Wait (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFriction (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyForce (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ApplyTorque (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAllForces (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAnimationHelper (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ClearAnimationHelpers (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSerializedSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Serialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Deserialize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMyClassTag (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSerializationFlags (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StartAnimations (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void StopAnimations (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Hide (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Show (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsHidden (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeIn (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FadeOut (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveInFrontOf (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToFront (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveToBack (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetZOrder (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void MoveWith (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void FindSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetNthSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpriteZOrder (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void IsSpriteBehind (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void HasSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void AddSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void RemoveAllSprites (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void EnableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DisableCollisions (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void EnableCollisionsWithLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DisableCollisionsWithLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CreateSprite (const v8::FunctionCallbackInfo<v8::Value>& args);
#ifndef PDG_NO_GUI
            static void GetSpritePort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetSpritePort (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetOrigin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetOrigin (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetZoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetZoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void Zoom (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void ZoomTo (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetAutoCenter (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetFixedMoveAxis (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortPoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortVector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LayerToPortQuad (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerPoint (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerOffset (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerVector (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerRect (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void PortToLayerQuad (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static void SetUseChipmunkPhysics (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetStaticLayer (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetGravity (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetKeepGravityDownward (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetDamping (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetSpace (const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
            static void GetWorldSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetWorldSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetWorldBounds (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void DefineTileSet (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void LoadMapData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetMapData (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTileSetImage (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTileSize (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTileTypeAt (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void GetTileTypeAndFacingAt (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void SetTileTypeAt (const v8::FunctionCallbackInfo<v8::Value>& args);
            static void CheckCollision (const v8::FunctionCallbackInfo<v8::Value>& args);
    };

    extern void GetConfigManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetLogManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetEventManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetResourceManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void SetSerializationDebugMode(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void RegisterSerializableClass(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void RegisterSerializableObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetGraphicsManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetSoundManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetFileManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GetTimerManager(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void CreateSpriteLayer(const v8::FunctionCallbackInfo<v8::Value>& args);
#ifdef PDG_SCML_SUPPORT
    extern void CreateSpriteLayerFromSCMLFile(const v8::FunctionCallbackInfo<v8::Value>& args);
#endif
    extern void CleanupSpriteLayer(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void CreateTileLayer(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void Rand(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void GameCriticalRandom(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void Srand(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void RegisterEasingFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void Idle(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void Run(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void IsQuitting(const v8::FunctionCallbackInfo<v8::Value>& args);
    extern void FinishedScriptSetup(const v8::FunctionCallbackInfo<v8::Value>& args);

}
#endif
