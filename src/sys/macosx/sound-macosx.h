// -----------------------------------------------
// sound-macosx.h
// 
// simple sound functionality
//
// Written by Ed Zavada, 2004-2012
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


#ifndef PDG_SOUNDMAC_H_INCLUDED
#define PDG_SOUNDMAC_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/sound.h"

#include <string>
#include <CoreFoundation/CFURL.h>

namespace pdg {

class SoundManagerMac;

// -----------------------------------------------------------------------------------
// Sound Wave
// A simple sound that can be played over your speakers
// -----------------------------------------------------------------------------------

class SoundMac : public Sound {
public:
    SoundMac(SoundManager* sndMgr, const char* extension);
	SoundMac(SoundMac* snd);
    virtual ~SoundMac();

    // plays sound immediately, fire and forget new instance of sound 
    virtual void    play(float vol, int32 offsetX, float pitch, ms_time fromMs, ms_delta lenMs);
	// play this sound
    virtual void    start();
    virtual void    stop();
	// pauses playback
	virtual void    pause();
	// resumes playback after pausing
	virtual void    resume();
    // set volume level for this sound only - 1.0 is max, 0.0 is silent
	// returns this sound for chaining calls
    virtual Sound&  setVolume(float level);

    // set pitch for this sound only
	// returns this sound for chaining calls
    virtual Sound&  setPitch(float pitchOffset);
	// change the pitch level over time
	virtual void	changePitch(float targetOffset, ms_delta msDuration, EasingFunc easing);
    // set the X offset from the center of the screen for this sound only
	// returns this sound for chaining calls
    virtual Sound&  setOffsetX(int32 offsetX);
	// change the X offset from the center of the screen over time
	virtual void	changeOffsetX(int32 targetOffset, ms_delta msDuration, EasingFunc easing);

    // Fade to zero volume and stop over the course of fadeMs milliseconds
	virtual void    fadeOut(ms_delta fadeMs, EasingFunc easing);
	// Fade in to full volume over fadeMs milliseconds. If the sound was not already playing, start it.
	virtual void    fadeIn(ms_delta fadeMs, EasingFunc easing);
	// Fade up or down to reach a new volume level over fadeMs milliseconds. If the sound was not already playing, start it.
	virtual void	changeVolume(float level, ms_delta fadeMs, EasingFunc easing);

	// skip forward (positive value) or backward (negative value) by skipMilliseconds
	// returns this sound for chaining calls
	virtual Sound&  skip(ms_delta skipMilleconds);
    // skip to a specific point in the sound
	// returns this sound for chaining calls
	virtual Sound&  skipTo(ms_time timeMs);

	virtual bool    createFromFile(const char* filename);
	virtual bool    createFromData(char* soundData, long soundDataLen);

    void            idle();

	bool			isPlaying() {return mPlaying;}
	void			setPlaying(bool playing) {mPlaying = playing;}
	
protected:
    SoundManagerMac* mSndMgr;
    void* mMacDataRef;
    void* mDataPtr;
    std::string mFilename;
	std::string mExtension;
	bool mPlaying;
	ms_time mStartedFadeMs;
	float mTargetVolume;
	float mStartingVolume;
	float mDeltaVolumePerMs;
	ms_time mSkipToMs;
	ms_time mStartedPlayingAtMs;
	ms_time mPausedAtMs;
	ms_time mDieAt;
	
	bool 			createFromFileUrl(CFURLRef fileRef);
	void			dieAt(ms_time msTime) { mDieAt = msTime; }

};

} // end namespace pdg

#endif // PDG_SOUNDMAC_H_INCLUDED
