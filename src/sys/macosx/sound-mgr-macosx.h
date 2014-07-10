// -----------------------------------------------
// sound-mgr-macosx.h
// 
// Sound Manager functionality
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


#ifndef PDG_SOUNDMGR_H_INCLUDED
#define PDG_SOUNDMGR_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/sound.h"

#include <vector>

namespace pdg {

// -----------------------------------------------------------------------------------
// SoundManager
// implements application wide sound settings and options on the Mac
// -----------------------------------------------------------------------------------

class SoundManagerMac : public SoundManager {
public:
    SoundManagerMac();
    virtual ~SoundManagerMac();

    // set volume for all sounds. Individual sounds may have separate volumes, but they are 
    // treated as a relative volume with this being max
    // 1.0 is max, 0.0 is silent
    virtual void    setVolume(float level);
    // turn on and off the playing of sounds altogether. setMute(false) restores all sounds 
    // to previous levels
    virtual void    setMute(bool muted);
    // give time to the sound layer to do anything it may need to do, such as refilling buffers
    virtual void    idle();

    void            soundPlaying(Sound* sound); // inform the Sound Manager there is a new sound playing
    void            soundStopped(Sound* sound); // the sound is no longer playing
protected:

    typedef std::vector<Sound*> SoundsList;
    
    SoundsList  mSounds;
    bool        mSoundsItemDeleted;
	uint32 mStartedFadeMs;
	float mTargetVolume;
	float mStartingVolume;
	float mDeltaVolumePerMs;
};

} // end namespace pdg

#endif // PDG_SOUNDMGR_H_INCLUDED

