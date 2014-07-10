// -----------------------------------------------
// sound.h
// 
// sound functionality
//
// Written by Ed Zavada, 2004-2014
// Copyright (c) 2014, Dream Rock Studios, LLC
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


#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/platform.h"
#include "pdg/sys/refcounted.h"
#include "pdg/sys/singleton.h"
#include "pdg/sys/eventemitter.h"
#include "pdg/sys/easing.h"
#include "pdg/sys/memstats.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

//! \defgroup Sound
//! Collection of classes, types and constants that are used for audio (sound and music)

// -----------------------------------------------------------------------------------
//! Sound
//! A simple sound that can be played over your speakers
//! \ingroup Sound
// -----------------------------------------------------------------------------------

class Sound : public EventEmitter, public RefCountedObj {
public:

    //! plays sound immediately 
    virtual void    play(float vol = 1.0, int32 offsetX = 0, float pitch = 0.0, 
    					uint32 fromMs = 0, int32 lenMs = -1) = 0;

	//! starts playing sound immediately
    virtual void    start() = 0;

	//! stops playing sound immediately
    virtual void    stop() = 0;

	//! pauses playback
	virtual void    pause() = 0;

	//! resumes playback after pausing
	virtual void    resume() = 0;

	//! tells if sound is paused
	virtual bool    isPaused() { return mPaused; }

	//! Turns on/off looping of sound
	//! returns this sound for chaining calls
	virtual Sound&  setLooping(bool loopingOn) { mLoopSound = loopingOn; return *this; }

	//! tells if sound is looping
	virtual bool    isLooping() { return mLoopSound; }

    //! set volume level for this sound only
    //! 1.0 is max, 0.0 is silent
	//! returns this sound for chaining calls
    virtual Sound&  setVolume(float level) = 0;
	
	//! get current volume level for this sound only
	virtual float	getVolume() { return mCurrentVolume; }

    //! set pitch for this sound only
	//! returns this sound for chaining calls
    virtual Sound&  setPitch(float pitchOffset) = 0;
	
	//! change the pitch level over time
	virtual void    changePitch(float targetOffset, int32 msDuration, 
                         EasingFunc easing = easeInOutQuad) = 0;

    //! set the X offset from the center of the screen for this sound only
	//! returns this sound for chaining calls
    virtual Sound&  setOffsetX(int32 offsetX) = 0;
	
	//! change the X offset from the center of the screen over time	//! returns this sound for chaining calls
	virtual void    changeOffsetX(int32 targetOffset, int32 msDuration, 
                         EasingFunc easing = linearTween) = 0;

    //! Fade to zero volume and stop over the course of fadeMs milliseconds
	virtual void    fadeOut(uint32 fadeMs, EasingFunc easing = linearTween) = 0;

	//! Fade in to full volume over fadeMs milliseconds. If the sound was not already playing, start it.
	virtual void    fadeIn(uint32 fadeMs, EasingFunc easing = linearTween) = 0;
	
	//! Fade up or down to reach a new volume level over fadeMs milliseconds. If the sound was not already playing, start it.
	virtual void    changeVolume(float level, uint32 fadeMs, EasingFunc easing = linearTween) = 0;

	//! skip forward (positive value) or backward (negative value) by skipMilliseconds
	//! returns this sound for chaining calls
	virtual Sound&  skip(int32 skipMilliseconds) = 0;

    //! skip to a specific point in the sound
	//! returns this sound for chaining calls
	virtual Sound&  skipTo(uint32 timeMs) = 0;

    static Sound* createSoundFromData(const char* soundName, char* soundData, long soundDataLen);
    static Sound* createSoundFromFile(const char* soundFileName);

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mSoundScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond INTERNAL
    Sound() : mLoopSound(false), mPaused(false), mCurrentVolume(0.0f) {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mSoundScriptObj);
    	#endif
    }
    virtual ~Sound() {
				#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
					CleanupSoundScriptObject(mSoundScriptObj);
				#endif
    }

	bool mLoopSound;
	bool mPaused;
	float mCurrentVolume;
/// @endcond
};


// -----------------------------------------------------------------------------------
//! SoundManager
//! used for application wide sound settings and options
//! \ingroup Managers
//! \ingroup Sound
// -----------------------------------------------------------------------------------

class SoundManager : public Singleton<SoundManager> {
friend class Singleton<SoundManager>;
public:

    // Load sound from the SoundFactory. Returns true if sound loaded ok.
/*    virtual bool    loadSound(int internalID, int resourceID) = 0;

    // Gets a pointer to a sound specified by the internal ID.
    virtual Sound*  getSound(int internalID) = 0;
*/
	// set volume for all sounds. Individual sounds may have separate volumes, but they are 
    // treated as a relative volume with this being max
    // 1.0 is max, 0.0 is silent
    virtual void    setVolume(float level) = 0;
    
    // turn on and off the playing of sounds altogether. setMute(false) restores all sounds 
    // to previous levels
    virtual void    setMute(bool muted) = 0;
    
    // give time to the sound layer to do anything it may need to do, such as refilling buffers
    virtual void    idle() = 0;

// lifecycle
/// @cond C++
    virtual ~SoundManager() {}
/// @endcond

protected:
/// @cond INTERNAL
	static SoundManager* createSingletonInstance();
    SoundManager() {}  // call SoundManager::getSingletonInstance() instead
/// @endcond
};

extern MemStats gSoundStats;

} // end namespace pdg

#endif // SOUND_H_INCLUDED
