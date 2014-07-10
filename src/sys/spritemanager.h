// -----------------------------------------------
// spritemanager.h
// 
// sprite functionality implementation
//
// Written by Ed Zavada, 2009-2012
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


#ifndef PDG_SPRITEMANAGER_H_INCLUDED
#define PDG_SPRITEMANAGER_H_INCLUDED

#include "pdg_project.h"

#include <vector>

#include "pdg/sys/core.h"

// serialization macros for lists of floating point values
// used by both sprite and sprite layer

#define SIZE_FLOAT_LIST_START(x, n) { uint32 _size = x;

#define SIZE_NON_ZERO_F(val, flagNum)  if (val != 0.0f) { _size+=4; /* DEBUG_PRINT("4 bytes for "#flagNum " "#val); */ }

#define SIZE_FLOAT_LIST_END(var) var += _size; }
	

#define SERIALIZE_FLOAT_LIST_START(x, n) { uint32 _flags = 0; int _datIdx = 0; float _data[n];

#define SERIALIZE_NON_ZERO_F(val, flagNum) \
	if (val != 0.0f) { _flags |= (1 << flagNum); _data[_datIdx++] = val; /* DEBUG_PRINT("writing "#flagNum " "#val); */ }

#define SERIALIZE_FLOAT_LIST_END(x) \
	serializer->serialize_##x##u(_flags); \
	for (int _i = 0; _i < _datIdx; _i++) { serializer->serialize_f(_data[_i]); } }

#define DESERIALIZE_FLOAT_LIST_START(x, n) { uint32 _flags = deserializer->deserialize_##x##u();

#define DESERIALIZE_NON_ZERO_F(flagNum) \
	((_flags & (1 << flagNum)) != 0) ? deserializer->deserialize_f() : 0.0f; \
 //	if ((_flags & (1 << flagNum)) != 0) { DEBUG_PRINT("read "#flagNum); }

#define DESERIALIZE_FLOAT_LIST_END(x) }



namespace pdg {

class SpriteLayer;
class TileLayer;

#ifndef PDG_NO_GUI
class Port;
#endif
	
// -----------------------------------------------------------------------------------
// Sprite Manager
// Used to track sprite layers, and handle timer events for layers
// -----------------------------------------------------------------------------------

	
class SpriteManager : public IEventHandler, public Singleton<SpriteManager> {
friend class Singleton<SpriteManager>;
public:
    virtual bool handleEvent(EventEmitter* inEmitter, long inEventType, void* inEventData) throw();  // return true if completely handled
	SpriteManager(EventManager* eventMgr, TimerManager* timerMgr);
	virtual ~SpriteManager();
    
	void addLayer(SpriteLayer* layer);
	void removeLayer(SpriteLayer* layer);
    static SpriteLayer* createSpriteLayer();
    static TileLayer* createTileLayer();
  #ifndef PDG_NO_GUI
    static SpriteLayer* createSpriteLayer(Port* port);
    static TileLayer* createTileLayer(Port* port);
  #endif // ! PDG_NO_GUI
    static void cleanupLayer(SpriteLayer* layer);
	EventManager* mEventMgr;
	TimerManager* mTimerMgr;
	SpriteLayer* mFirstLayer;
	SpriteLayer* mLastLayer;
	uint32 mLastCallAt;

	static SpriteManager* createSingletonInstance();
    
#ifdef PDG_USE_CHIPMUNK_PHYSICS
    cpSpace* mSpace; 

    static cpBool  ChipmunkSpriteCollisionBeginFunc(cpArbiter *arb, struct cpSpace *space, void *data);
    static void    ChipmunkSpriteCollisionPostSolveFunc(cpArbiter *arb, cpSpace *space, void *data);
    static cpBool  ChipmunkWallCollisionBeginFunc(cpArbiter *arb, struct cpSpace *space, void *data);
    static void    ChipmunkWallCollisionPostSolveFunc(cpArbiter *arb, cpSpace *space, void *data);
    

#endif
    
};
	
} // end namespace pdg

#endif // PDG_SPRITEMANAGER_H_INCLUDED

