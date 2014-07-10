// -----------------------------------------------
// This file automatically generated from:
//
//    pdg/src/bindings/common/pdg_script_interface.h
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

    MemBlock* New_MemBlock(const v8::Arguments& args);

    class MemBlockWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            MemBlock* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            MemBlock* refPtr_;

            MemBlockWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_MemBlock(args);
            }

            ~MemBlockWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(MemBlock* nativeObj);
            MemBlockWrap(MemBlock* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetData (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDataSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetByte (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBytes (const v8::Arguments& args);
    };

    ConfigManager* New_ConfigManager(const v8::Arguments& args);

    class ConfigManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ConfigManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ConfigManager* refPtr_;

            ConfigManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ConfigManager(args);
            }

            ~ConfigManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static ConfigManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> UseConfig (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetConfigString (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetConfigLong (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetConfigFloat (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetConfigBool (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetConfigString (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetConfigLong (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetConfigFloat (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetConfigBool (const v8::Arguments& args);
    };

    LogManager* New_LogManager(const v8::Arguments& args);

    class LogManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            LogManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            LogManager* refPtr_;

            LogManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_LogManager(args);
            }

            ~LogManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static LogManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> GetLogLevel (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLogLevel (const v8::Arguments& args);
            static v8::Handle<v8::Value> Initialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> WriteLogEntry (const v8::Arguments& args);
            static v8::Handle<v8::Value> BinaryDump (const v8::Arguments& args);
    };

    IEventHandler* New_IEventHandler(const v8::Arguments& args);

    class IEventHandlerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            IEventHandler* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            IEventHandler* refPtr_;

            IEventHandlerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_IEventHandler(args);
            }

            ~IEventHandlerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(IEventHandler* nativeObj);
            IEventHandlerWrap(IEventHandler* obj) : refPtr_(obj) {}

    };

    EventEmitter* New_EventEmitter(const v8::Arguments& args);

    class EventEmitterWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            EventEmitter* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            EventEmitter* refPtr_;

            EventEmitterWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_EventEmitter(args);
            }

            ~EventEmitterWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(EventEmitter* nativeObj);
            EventEmitterWrap(EventEmitter* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);

    };

    EventManager* New_EventManager(const v8::Arguments& args);

    class EventManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            EventManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            EventManager* refPtr_;

            EventManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_EventManager(args);
            }

            ~EventManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static EventManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsKeyDown (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsRawKeyDown (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsButtonDown (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDeviceOrientation (const v8::Arguments& args);
    };

    ResourceManager* New_ResourceManager(const v8::Arguments& args);

    class ResourceManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ResourceManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ResourceManager* refPtr_;

            ResourceManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ResourceManager(args);
            }

            ~ResourceManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static ResourceManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> GetLanguage (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLanguage (const v8::Arguments& args);
            static v8::Handle<v8::Value> OpenResourceFile (const v8::Arguments& args);
            static v8::Handle<v8::Value> CloseResourceFile (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetImage (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetImageStrip (const v8::Arguments& args);
#ifndef PDG_NO_SOUND
            static v8::Handle<v8::Value> GetSound (const v8::Arguments& args);
#endif
            static v8::Handle<v8::Value> GetString (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetResourceSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetResource (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetResourcePaths (const v8::Arguments& args);
    };

    Serializer* New_Serializer(const v8::Arguments& args);

    class SerializerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Serializer* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Serializer* refPtr_;

            SerializerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Serializer(args);
            }

            ~SerializerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Serializer* nativeObj);
            SerializerWrap(Serializer* obj) : refPtr_(obj) {}

#ifndef PDG_NO_64BIT
            static v8::Handle<v8::Value> Serialize_8 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_8u (const v8::Arguments& args);
#endif
            static v8::Handle<v8::Value> Serialize_d (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_f (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_4 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_4u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_3u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_2 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_2u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_1 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_1u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_bool (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_uint (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_color (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_offset (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_point (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_vector (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_rect (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_rotr (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_quad (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_str (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_mem (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize_obj (const v8::Arguments& args);
            static v8::Handle<v8::Value> SerializedSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDataSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDataPtr (const v8::Arguments& args);
    };

    Deserializer* New_Deserializer(const v8::Arguments& args);

    class DeserializerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Deserializer* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Deserializer* refPtr_;

            DeserializerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Deserializer(args);
            }

            ~DeserializerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Deserializer* nativeObj);
            DeserializerWrap(Deserializer* obj) : refPtr_(obj) {}

#ifndef PDG_NO_64BIT
            static v8::Handle<v8::Value> Deserialize_8 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_8u (const v8::Arguments& args);
#endif
            static v8::Handle<v8::Value> Deserialize_d (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_f (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_4 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_4u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_3u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_2 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_2u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_1 (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_1u (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_bool (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_uint (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_color (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_offset (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_point (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_vector (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_rect (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_rotr (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_quad (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_str (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_strGetLen (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_mem (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_memGetLen (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize_obj (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetDataPtr (const v8::Arguments& args);

    };

    ISerializable* New_ISerializable(const v8::Arguments& args);

    class ISerializableWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISerializable* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ISerializable* refPtr_;

            ISerializableWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ISerializable(args);
            }

            ~ISerializableWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(ISerializable* nativeObj);
            ISerializableWrap(ISerializable* obj) : refPtr_(obj) {}

    };

    Image* New_Image(const v8::Arguments& args);

    class ImageWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Image* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Image* refPtr_;

            ImageWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Image(args);
            }

            ~ImageWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Image* nativeObj);
            ImageWrap(Image* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetTransparentColor (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetTransparentColor (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetImageBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSubsection (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetEdgeClamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> RetainData (const v8::Arguments& args);
            static v8::Handle<v8::Value> RetainAlpha (const v8::Arguments& args);
            static v8::Handle<v8::Value> PrepareToRasterize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetAlphaValue (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetPixel (const v8::Arguments& args);
    };

    ImageStrip* New_ImageStrip(const v8::Arguments& args);

    class ImageStripWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ImageStrip* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ImageStrip* refPtr_;

            ImageStripWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ImageStrip(args);
            }

            ~ImageStripWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(ImageStrip* nativeObj);
            ImageStripWrap(ImageStrip* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetTransparentColor (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetTransparentColor (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetImageBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSubsection (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetEdgeClamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> RetainData (const v8::Arguments& args);
            static v8::Handle<v8::Value> RetainAlpha (const v8::Arguments& args);
            static v8::Handle<v8::Value> PrepareToRasterize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetAlphaValue (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetPixel (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFrameWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFrameWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNumFrames (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetNumFrames (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFrame (const v8::Arguments& args);
    };

#ifndef PDG_NO_GUI

    GraphicsManager* New_GraphicsManager(const v8::Arguments& args);

    class GraphicsManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            GraphicsManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            GraphicsManager* refPtr_;

            GraphicsManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_GraphicsManager(args);
            }

            ~GraphicsManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static GraphicsManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> GetNumScreens (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNumSupportedScreenModes (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNthSupportedScreenMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCurrentScreenMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetScreenMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateWindowPort (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateFullScreenPort (const v8::Arguments& args);
            static v8::Handle<v8::Value> CloseGraphicsPort (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateFont (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMainPort (const v8::Arguments& args);
            static v8::Handle<v8::Value> SwitchToFullScreenMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> SwitchToWindowMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> InFullScreenMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFPS (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTargetFPS (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetTargetFPS (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMouse (const v8::Arguments& args);
    };

    class FontWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Font* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Font* refPtr_;

            FontWrap(const v8::Arguments& args)
            {
                refPtr_ = 0;
            }

            ~FontWrap()
            {
                refPtr_ = 0;
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Font* nativeObj);
            FontWrap(Font* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetFontName (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFontHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFontLeading (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFontAscent (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFontDescent (const v8::Arguments& args);
    };

    class PortWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Port* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Port* refPtr_;

            PortWrap(const v8::Arguments& args)
            {
                refPtr_ = 0;
            }

            ~PortWrap()
            {
                refPtr_ = 0;
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Port* nativeObj);
            PortWrap(Port* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetClipRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetClipRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCursor (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCursor (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDrawingArea (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> FrameRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawLine (const v8::Arguments& args);
            static v8::Handle<v8::Value> FrameOval (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillOval (const v8::Arguments& args);
            static v8::Handle<v8::Value> FrameCircle (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillCircle (const v8::Arguments& args);
            static v8::Handle<v8::Value> FrameRoundRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillRoundRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillRectEx (const v8::Arguments& args);
            static v8::Handle<v8::Value> FrameRectEx (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawLineEx (const v8::Arguments& args);
            static v8::Handle<v8::Value> FillRectWithGradient (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawText (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawImage (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawTexture (const v8::Arguments& args);
            static v8::Handle<v8::Value> DrawTexturedSphere (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTextWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCurrentFont (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFont (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFontForStyle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFontScalingFactor (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartTrackingMouse (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopTrackingMouse (const v8::Arguments& args);
            static v8::Handle<v8::Value> ResetCursor (const v8::Arguments& args);
    };
#endif

#ifndef PDG_NO_SOUND
    Sound* New_Sound(const v8::Arguments& args);

    class SoundWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Sound* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Sound* refPtr_;

            SoundWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Sound(args);
            }

            ~SoundWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Sound* nativeObj);
            SoundWrap(Sound* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetVolume (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVolume (const v8::Arguments& args);
            static v8::Handle<v8::Value> Play (const v8::Arguments& args);
            static v8::Handle<v8::Value> Start (const v8::Arguments& args);
            static v8::Handle<v8::Value> Stop (const v8::Arguments& args);
            static v8::Handle<v8::Value> Pause (const v8::Arguments& args);
            static v8::Handle<v8::Value> Resume (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsPaused (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLooping (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsLooping (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetPitch (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangePitch (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOffsetX (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeOffsetX (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeOut (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeIn (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeVolume (const v8::Arguments& args);
            static v8::Handle<v8::Value> Skip (const v8::Arguments& args);
            static v8::Handle<v8::Value> SkipTo (const v8::Arguments& args);
    };

    SoundManager* New_SoundManager(const v8::Arguments& args);

    class SoundManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            SoundManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            SoundManager* refPtr_;

            SoundManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_SoundManager(args);
            }

            ~SoundManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static SoundManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> SetVolume (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMute (const v8::Arguments& args);
            static v8::Handle<v8::Value> Idle (const v8::Arguments& args);
    };
#endif

    TimerManager* New_TimerManager(const v8::Arguments& args);

    class TimerManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            TimerManager* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            TimerManager* refPtr_;

            TimerManagerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_TimerManager(args);
            }

            ~TimerManagerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static TimerManager* getSingletonInstance();
        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartTimer (const v8::Arguments& args);
            static v8::Handle<v8::Value> CancelTimer (const v8::Arguments& args);
            static v8::Handle<v8::Value> CancelAllTimers (const v8::Arguments& args);
            static v8::Handle<v8::Value> DelayTimer (const v8::Arguments& args);
            static v8::Handle<v8::Value> DelayTimerUntil (const v8::Arguments& args);
            static v8::Handle<v8::Value> Pause (const v8::Arguments& args);
            static v8::Handle<v8::Value> Unpause (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsPaused (const v8::Arguments& args);
            static v8::Handle<v8::Value> PauseTimer (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnpauseTimer (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsTimerPaused (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWhenTimerFiresNext (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMilliseconds (const v8::Arguments& args);
    };

    class FileManagerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;

            FileManagerWrap(const v8::Arguments& args) { }

            ~FileManagerWrap() { }

        public:
            static v8::Handle<v8::Object> GetScriptSingletonInstance();
        protected:
            static v8::Persistent<v8::Object> instance_;
            static bool instanced_;

            static v8::Handle<v8::Value> FindFirst (const v8::Arguments& args);
            static v8::Handle<v8::Value> FindNext (const v8::Arguments& args);
            static v8::Handle<v8::Value> FindClose (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetApplicationDataDirectory (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetApplicationDirectory (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetApplicationResourceDirectory (const v8::Arguments& args);
    };

    IAnimationHelper* New_IAnimationHelper(const v8::Arguments& args);

    class IAnimationHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            IAnimationHelper* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            IAnimationHelper* refPtr_;

            IAnimationHelperWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_IAnimationHelper(args);
            }

            ~IAnimationHelperWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(IAnimationHelper* nativeObj);
            IAnimationHelperWrap(IAnimationHelper* obj) : refPtr_(obj) {}

    };

    Animated* New_Animated(const v8::Arguments& args);

    class AnimatedWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Animated* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Animated* refPtr_;

            AnimatedWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Animated(args);
            }

            ~AnimatedWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Animated* nativeObj);
            AnimatedWrap(Animated* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBoundingBox (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotatedBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> Move (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocityInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMovementDirectionInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopMoving (const v8::Arguments& args);
            static v8::Handle<v8::Value> Accelerate (const v8::Arguments& args);
            static v8::Handle<v8::Value> AccelerateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Grow (const v8::Arguments& args);
            static v8::Handle<v8::Value> Stretch (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> Resize (const v8::Arguments& args);
            static v8::Handle<v8::Value> ResizeTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Rotate (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopSpinning (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenterTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Wait (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyTorque (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAllForces (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> ClearAnimationHelpers (const v8::Arguments& args);
            static v8::Handle<v8::Value> Animate (const v8::Arguments& args);

    };

#ifdef PDG_USE_CHIPMUNK_PHYSICS
    class cpArbiterWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpArbiter* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            cpArbiter* refPtr_;

            cpArbiterWrap(const v8::Arguments& args)
            {
                refPtr_ = 0;
            }

            ~cpArbiterWrap()
            {
                refPtr_ = 0;
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(cpArbiter* nativeObj);
            cpArbiterWrap(cpArbiter* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> IsFirstContact (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCount (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNormal (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetPoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetDepth (const v8::Arguments& args);
    };

    class cpConstraintWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpConstraint* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            cpConstraint* refPtr_;

            cpConstraintWrap(const v8::Arguments& args)
            {
                refPtr_ = 0;
            }

            ~cpConstraintWrap()
            {
                refPtr_ = 0;
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(cpConstraint* nativeObj);
            cpConstraintWrap(cpConstraint* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetMaxForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMaxForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetErrorBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetErrorBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMaxBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMaxBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetAnchor (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetAnchor (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOtherAnchor (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOtherAnchor (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetPinDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetPinDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSlideMinDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSlideMinDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSlideMaxDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSlideMaxDist (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetGrooveStart (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGrooveStart (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetGrooveEnd (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGrooveEnd (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpringRestLength (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpringRestLength (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpringStiffness (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpringStiffness (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpringDamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpringDamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotarySpringRestAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRotarySpringRestAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMinAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMinAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMaxAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMaxAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRatchetAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRatchetAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRatchetPhase (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRatchetPhase (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRatchetInterval (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRatchetInterval (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetGearRatio (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGearRatio (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetGearInitialAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGearInitialAngle (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMotorSpinRate (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMotorSpinRate (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetType (const v8::Arguments& args);
            static v8::Handle<v8::Value> ActivateBodies (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetImpulse (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOtherSprite (const v8::Arguments& args);
    };

    class cpSpaceWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            cpSpace* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            cpSpace* refPtr_;

            cpSpaceWrap(const v8::Arguments& args)
            {
                refPtr_ = 0;
            }

            ~cpSpaceWrap()
            {
                refPtr_ = 0;
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(cpSpace* nativeObj);
            cpSpaceWrap(cpSpace* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> GetIdleSpeedThreshold (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetIdleSpeedThreshold (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSleepTimeThreshold (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSleepTimeThreshold (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCollisionSlop (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollisionSlop (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCollisionBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollisionBias (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCollisionPersistence (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollisionPersistence (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetEnableContactGraph (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetEnableContactGraph (const v8::Arguments& args);
            static v8::Handle<v8::Value> UseSpatialHash (const v8::Arguments& args);
            static v8::Handle<v8::Value> ReindexStatic (const v8::Arguments& args);
            static v8::Handle<v8::Value> Step (const v8::Arguments& args);
    };
#endif

    ISpriteCollideHelper* New_ISpriteCollideHelper(const v8::Arguments& args);

    class ISpriteCollideHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISpriteCollideHelper* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ISpriteCollideHelper* refPtr_;

            ISpriteCollideHelperWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ISpriteCollideHelper(args);
            }

            ~ISpriteCollideHelperWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(ISpriteCollideHelper* nativeObj);
            ISpriteCollideHelperWrap(ISpriteCollideHelper* obj) : refPtr_(obj) {}

    };

#ifndef PDG_NO_GUI
    ISpriteDrawHelper* New_ISpriteDrawHelper(const v8::Arguments& args);

    class ISpriteDrawHelperWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            ISpriteDrawHelper* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            ISpriteDrawHelper* refPtr_;

            ISpriteDrawHelperWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_ISpriteDrawHelper(args);
            }

            ~ISpriteDrawHelperWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(ISpriteDrawHelper* nativeObj);
            ISpriteDrawHelperWrap(ISpriteDrawHelper* obj) : refPtr_(obj) {}

    };
#endif

    Sprite* New_Sprite(const v8::Arguments& args);

    class SpriteWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            Sprite* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            Sprite* refPtr_;

            SpriteWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_Sprite(args);
            }

            ~SpriteWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(Sprite* nativeObj);
            SpriteWrap(Sprite* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBoundingBox (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotatedBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> Move (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocityInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMovementDirectionInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopMoving (const v8::Arguments& args);
            static v8::Handle<v8::Value> Accelerate (const v8::Arguments& args);
            static v8::Handle<v8::Value> AccelerateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Grow (const v8::Arguments& args);
            static v8::Handle<v8::Value> Stretch (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> Resize (const v8::Arguments& args);
            static v8::Handle<v8::Value> ResizeTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Rotate (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopSpinning (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenterTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Wait (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyTorque (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAllForces (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> ClearAnimationHelpers (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSerializedSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMyClassTag (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWantsAnimLoopEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsAnimLoopEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWantsAnimEndEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsAnimEndEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOpacity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCollisionRadius (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollisionRadius (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetElasticity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetElasticity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsCollideWallEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFrameRotatedBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFrame (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCurrentFrame (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFrameCount (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartFrameAnimation (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopFrameAnimation (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddFramesImage (const v8::Arguments& args);
#ifdef PDG_SCML_SUPPORT
            static v8::Handle<v8::Value> HasAnimation (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartAnimation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetEntityScale (const v8::Arguments& args);
#endif
#ifndef PDG_NO_GUI
            static v8::Handle<v8::Value> GetWantsMouseOverEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsMouseOverEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWantsClickEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsClickEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMouseDetectMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMouseDetectMode (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWantsOffscreenEvents (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetDrawHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetPostDrawHelper (const v8::Arguments& args);
#endif
            static v8::Handle<v8::Value> ChangeFramesImage (const v8::Arguments& args);
            static v8::Handle<v8::Value> OffsetFrameCenters (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFrameCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeIn (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeOut (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetZOrder (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveInFrontOf (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToFront (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToBack (const v8::Arguments& args);
            static v8::Handle<v8::Value> EnableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> DisableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> UseCollisionMask (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollisionHelper (const v8::Arguments& args);;
            static v8::Handle<v8::Value> SetUserData (const v8::Arguments& args);
            static v8::Handle<v8::Value> FreeUserData (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetLayer (const v8::Arguments& args);
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static v8::Handle<v8::Value> GetCollideGroup (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCollideGroup (const v8::Arguments& args);
            static v8::Handle<v8::Value> MakeStatic (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> PinJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> SlideJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> PivotJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> GrooveJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> SpringJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotarySpring (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotaryLimit (const v8::Arguments& args);
            static v8::Handle<v8::Value> Ratchet (const v8::Arguments& args);
            static v8::Handle<v8::Value> Gear (const v8::Arguments& args);
            static v8::Handle<v8::Value> Motor (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveJoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> Disconnect (const v8::Arguments& args);
            static v8::Handle<v8::Value> MakeJointBreakable (const v8::Arguments& args);
            static v8::Handle<v8::Value> MakeJointUnbreakable (const v8::Arguments& args);
#endif
    };

#ifndef PDG_NO_GUI
#endif

    SpriteLayer* New_SpriteLayer(const v8::Arguments& args);

    class SpriteLayerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            SpriteLayer* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            SpriteLayer* refPtr_;

            SpriteLayerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_SpriteLayer(args);
            }

            ~SpriteLayerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(SpriteLayer* nativeObj);
            SpriteLayerWrap(SpriteLayer* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBoundingBox (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotatedBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> Move (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocityInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMovementDirectionInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopMoving (const v8::Arguments& args);
            static v8::Handle<v8::Value> Accelerate (const v8::Arguments& args);
            static v8::Handle<v8::Value> AccelerateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Grow (const v8::Arguments& args);
            static v8::Handle<v8::Value> Stretch (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> Resize (const v8::Arguments& args);
            static v8::Handle<v8::Value> ResizeTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Rotate (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopSpinning (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenterTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Wait (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyTorque (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAllForces (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> ClearAnimationHelpers (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSerializedSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMyClassTag (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSerializationFlags (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartAnimations (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAnimations (const v8::Arguments& args);
            static v8::Handle<v8::Value> Hide (const v8::Arguments& args);
            static v8::Handle<v8::Value> Show (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsHidden (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeIn (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeOut (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveInFrontOf (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToFront (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToBack (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetZOrder (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveWith (const v8::Arguments& args);
            static v8::Handle<v8::Value> FindSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNthSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpriteZOrder (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsSpriteBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> HasSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAllSprites (const v8::Arguments& args);
            static v8::Handle<v8::Value> EnableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> DisableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> EnableCollisionsWithLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> DisableCollisionsWithLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateSprite (const v8::Arguments& args);
#ifndef PDG_NO_GUI
            static v8::Handle<v8::Value> GetSpritePort (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpritePort (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOrigin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOrigin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetZoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetZoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> Zoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> ZoomTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetAutoCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFixedMoveAxis (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortPoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortVector (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortQuad (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerPoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerVector (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerQuad (const v8::Arguments& args);
#endif
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static v8::Handle<v8::Value> SetUseChipmunkPhysics (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetStaticLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGravity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetKeepGravityDownward (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetDamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpace (const v8::Arguments& args);
#endif
#ifdef PDG_SCML_SUPPORT
            static v8::Handle<v8::Value> CreateSpriteFromSCML (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateSpriteFromSCMLFile (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateSpriteFromSCMLEntity (const v8::Arguments& args);
#endif
    };

    TileLayer* New_TileLayer(const v8::Arguments& args);

    class TileLayerWrap : public jswrap::ObjectWrap
    {
        public:
            static void Init(v8::Handle<v8::Object> target);
            static v8::Handle<v8::Value> New(const v8::Arguments& args);
        protected:
            static v8::Persistent<v8::FunctionTemplate> constructorTpl_;
        public:
            TileLayer* getNativeObject()
            {
                return refPtr_;
            }
        protected:
            TileLayer* refPtr_;

            TileLayerWrap(const v8::Arguments& args) : refPtr_(NULL)
            {
                refPtr_ = New_TileLayer(args);
            }

            ~TileLayerWrap()
            {
                if (refPtr_)
                {
                    delete refPtr_;
                    refPtr_ = NULL;
                }
            }

        public:
            static v8::Handle<v8::Value> NewFromNative(TileLayer* nativeObj);
            TileLayerWrap(TileLayer* obj) : refPtr_(obj) {}

            static v8::Handle<v8::Value> AddHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveHandler (const v8::Arguments& args);
            static v8::Handle<v8::Value> Clear (const v8::Arguments& args);
            static v8::Handle<v8::Value> BlockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> UnblockEvent (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetLocation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpeed (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocity (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWidth (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetHeight (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetRotation (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetCenterOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMass (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetMoveFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpinFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSizeFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetBoundingBox (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetRotatedBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> Move (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetVelocityInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMovementDirectionInRadians (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopMoving (const v8::Arguments& args);
            static v8::Handle<v8::Value> Accelerate (const v8::Arguments& args);
            static v8::Handle<v8::Value> AccelerateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Grow (const v8::Arguments& args);
            static v8::Handle<v8::Value> Stretch (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopGrowing (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopStretching (const v8::Arguments& args);
            static v8::Handle<v8::Value> Resize (const v8::Arguments& args);
            static v8::Handle<v8::Value> ResizeTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Rotate (const v8::Arguments& args);
            static v8::Handle<v8::Value> RotateTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopSpinning (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> ChangeCenterTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> Wait (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFriction (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyForce (const v8::Arguments& args);
            static v8::Handle<v8::Value> ApplyTorque (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAllForces (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAnimationHelper (const v8::Arguments& args);
            static v8::Handle<v8::Value> ClearAnimationHelpers (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSerializedSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Serialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> Deserialize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMyClassTag (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSerializationFlags (const v8::Arguments& args);
            static v8::Handle<v8::Value> StartAnimations (const v8::Arguments& args);
            static v8::Handle<v8::Value> StopAnimations (const v8::Arguments& args);
            static v8::Handle<v8::Value> Hide (const v8::Arguments& args);
            static v8::Handle<v8::Value> Show (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsHidden (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeIn (const v8::Arguments& args);
            static v8::Handle<v8::Value> FadeOut (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveInFrontOf (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToFront (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveToBack (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetZOrder (const v8::Arguments& args);
            static v8::Handle<v8::Value> MoveWith (const v8::Arguments& args);
            static v8::Handle<v8::Value> FindSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetNthSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpriteZOrder (const v8::Arguments& args);
            static v8::Handle<v8::Value> IsSpriteBehind (const v8::Arguments& args);
            static v8::Handle<v8::Value> HasSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> AddSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveSprite (const v8::Arguments& args);
            static v8::Handle<v8::Value> RemoveAllSprites (const v8::Arguments& args);
            static v8::Handle<v8::Value> EnableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> DisableCollisions (const v8::Arguments& args);
            static v8::Handle<v8::Value> EnableCollisionsWithLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> DisableCollisionsWithLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> CreateSprite (const v8::Arguments& args);
#ifndef PDG_NO_GUI
            static v8::Handle<v8::Value> GetSpritePort (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetSpritePort (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetOrigin (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetOrigin (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetZoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetZoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> Zoom (const v8::Arguments& args);
            static v8::Handle<v8::Value> ZoomTo (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetAutoCenter (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetFixedMoveAxis (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortPoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortVector (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> LayerToPortQuad (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerPoint (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerOffset (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerVector (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerRect (const v8::Arguments& args);
            static v8::Handle<v8::Value> PortToLayerQuad (const v8::Arguments& args);
#endif
#ifdef PDG_USE_CHIPMUNK_PHYSICS
            static v8::Handle<v8::Value> SetUseChipmunkPhysics (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetStaticLayer (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetGravity (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetKeepGravityDownward (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetDamping (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetSpace (const v8::Arguments& args);
#endif
            static v8::Handle<v8::Value> GetWorldSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetWorldSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetWorldBounds (const v8::Arguments& args);
            static v8::Handle<v8::Value> DefineTileSet (const v8::Arguments& args);
            static v8::Handle<v8::Value> LoadMapData (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetMapData (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTileSetImage (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTileSize (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTileTypeAt (const v8::Arguments& args);
            static v8::Handle<v8::Value> GetTileTypeAndFacingAt (const v8::Arguments& args);
            static v8::Handle<v8::Value> SetTileTypeAt (const v8::Arguments& args);
            static v8::Handle<v8::Value> CheckCollision (const v8::Arguments& args);
    };

    extern v8::Handle<v8::Value> GetConfigManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetLogManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetEventManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetResourceManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> SetSerializationDebugMode(const v8::Arguments& args);
    extern v8::Handle<v8::Value> RegisterSerializableClass(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetGraphicsManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetSoundManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetFileManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GetTimerManager(const v8::Arguments& args);
    extern v8::Handle<v8::Value> CreateSpriteLayer(const v8::Arguments& args);
    extern v8::Handle<v8::Value> CreateSpriteLayerFromSCMLFile(const v8::Arguments& args);
    extern v8::Handle<v8::Value> CleanupSpriteLayer(const v8::Arguments& args);
    extern v8::Handle<v8::Value> CreateTileLayer(const v8::Arguments& args);
    extern v8::Handle<v8::Value> Rand(const v8::Arguments& args);
    extern v8::Handle<v8::Value> GameCriticalRandom(const v8::Arguments& args);
    extern v8::Handle<v8::Value> Srand(const v8::Arguments& args);
    extern v8::Handle<v8::Value> RegisterEasingFunction(const v8::Arguments& args);
    extern v8::Handle<v8::Value> Idle(const v8::Arguments& args);
    extern v8::Handle<v8::Value> Run(const v8::Arguments& args);
    extern v8::Handle<v8::Value> Quit(const v8::Arguments& args);
    extern v8::Handle<v8::Value> IsQuitting(const v8::Arguments& args);
    extern v8::Handle<v8::Value> FinishedScriptSetup(const v8::Arguments& args);

}
#endif
