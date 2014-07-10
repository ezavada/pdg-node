// -----------------------------------------------
// This file automatically generated from:
//
//    pdg/src/bindings/common/pdg_script_impl.h
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



#ifndef PDG_SCRIPT_IMPL_H_INCLUDED
#define PDG_SCRIPT_IMPL_H_INCLUDED

#include "pdg_project.h"

#include "pdg_script.h"

#ifndef PDG_NO_APP_FRAMEWORK
#define PDG_NO_APP_FRAMEWORK
#endif
#include "pdg/framework.h"

#include <cstdlib>
#include <sstream>

namespace pdg
{

    extern bool s_HaveSavedError;
    extern v8::Persistent<v8::Value> s_SavedError;

    v8::Local<v8::Value> EncodeBinary(const void *buf, size_t len);
    void* DecodeBinary(v8::Handle<v8::Value> val, size_t* outLen = 0);

    const bool kNoErrorOnFail = true;

    void CreateSingletons();

    class ScriptAnimationHelper : public pdg::IAnimationHelper
    {
        public:
            ScriptAnimationHelper();
            ScriptAnimationHelper(v8::Persistent<v8::Function> javascriptAnimateFunc);
            bool animate(Animated* what, uint32 msElapsed) throw();
        protected:
            v8::Persistent<v8::Function> mScriptAnimateFunc;
    };

    class ScriptEventHandler : public pdg::RefCountedImpl< pdg::IEventHandler >
    {
        public:
            ScriptEventHandler();
            ScriptEventHandler(v8::Persistent<v8::Function> javascriptHandlerFunc);
            bool handleEvent(EventEmitter* emitter, long inEventType, void* inEventData) throw();
        protected:
            v8::Persistent<v8::Function> mScriptHandlerFunc;
    };

    class ScriptSerializable : public pdg::ISerializable
    {
        public:
            ScriptSerializable();
            ScriptSerializable(
                v8::Persistent<v8::Function> javascriptGetSerializedSizeFunc,
                v8::Persistent<v8::Function> javascriptSerializeFunc,
                v8::Persistent<v8::Function> javascriptDeserializeFunc,
                v8::Persistent<v8::Function> javascriptGetMyClassTagFunc
                );
            virtual uint32 getSerializedSize(ISerializer* serializer) const;
            virtual void serialize(ISerializer* serializer) const;
            virtual void deserialize(IDeserializer* deserializer);
            virtual uint32 getMyClassTag() const;
        protected:
            v8::Persistent<v8::Function> mScriptGetSerializedSizeFunc;
            v8::Persistent<v8::Function> mScriptSerializeFunc;
            v8::Persistent<v8::Function> mScriptDeserializeFunc;
            v8::Persistent<v8::Function> mScriptGetMyClassTagFunc;
    };

    class ScriptSpriteCollideHelper : public pdg::ISpriteCollideHelper
    {
        public:
            ScriptSpriteCollideHelper();
            ScriptSpriteCollideHelper(v8::Persistent<v8::Function> javascriptDrawFunc);
            bool allowCollision(Sprite* sprite, Sprite* withSprite) throw();
        protected:
            v8::Persistent<v8::Function> mScriptAllowCollisionFunc;
    };

#ifndef PDG_NO_GUI

    class ScriptSpriteDrawHelper : public pdg::ISpriteDrawHelper
    {
        public:
            ScriptSpriteDrawHelper();
            ScriptSpriteDrawHelper(v8::Persistent<v8::Function> javascriptDrawFunc);
            bool draw(Sprite* sprite, Port* port) throw();
        protected:
            v8::Persistent<v8::Function> mScriptDrawFunc;
    };
#endif

    float CallScriptEasingFunc(int which, uint32 ut, float b, float c, uint32 ud);

    extern float customEasing0(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing1(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing2(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing3(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing4(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing5(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing6(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing7(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing8(uint32 ut, float b, float c, uint32 ud);
    extern float customEasing9(uint32 ut, float b, float c, uint32 ud);

    namespace EasingFuncRef
    {
        enum
        {
            EASING_FUNC_LIST
        };
    }

}
#endif
