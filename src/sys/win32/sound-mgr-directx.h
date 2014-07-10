// -----------------------------------------------
// sound-mgr-directx.h
// 
// Sound Manager functionality
//
// Written by Alan Davies and Ed Zavada, 2004-2012
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


#ifndef SOUNDMGR_DIRECTX_H_INCLUDED
#define SOUNDMGR_DIRECTX_H_INCLUDED

#ifndef PDG_NO_SOUND

#include "pdg/sys/sound.h"

#include <vector>
#include <windows.h>
#undef THIS

#define WM_PDG_SOUND_NOTIFY  WM_APP + 1

namespace pdg {

class EventManager;
// -----------------------------------------------------------------------------------
// SoundManager
// implements application wide sound settings and options on Windows
// -----------------------------------------------------------------------------------

class SoundManagerDirectX : public SoundManager {
public:
    SoundManagerDirectX(EventManager* theEventManager);
    virtual ~SoundManagerDirectX();

    // set volume for all sounds. Individual sounds may have separate volumes, but they are 
    // treated as a relative volume with this being max
    // 1.0 is max, 0.0 is silent
    virtual void    setVolume(float level);
    // turn on and off the playing of sounds altogether. setMute(false) restores all sounds 
    // to previous levels
    virtual void    setMute(bool muted);
    // give time to the sound layer to do anything it may need to do, such as refilling buffers
    virtual void    idle();
    EventManager*   getEventManager() { return mEventMgr; }
//	HWND            getWindow() { return mWindow; }

    void soundPlaying(Sound* sound); // inform the Sound Manager there is a new sound playing
    void soundStopped(Sound* sound); // the sound is no longer playing
	void stopAllSoundsExceptMusic(); // Stop sounds except music from playing
	void stopAllSoundsAndMusic(); // Stop sounds including music from playing
	void setMusic(Sound* music) { mMusicSound = music; }
	Sound* getMusic() { return mMusicSound; }
	bool isMuted() { return mIsMuted; }
	void mute(bool muteOn);

protected:
	void setup();

    EventManager* mEventMgr;
//	HWND          mWindow;
    
    typedef std::vector<Sound*> SoundsList;
    SoundsList  mSounds;

	// Direct X Specific
	unsigned long mPreviousWaveVolume;
	Sound* mMusicSound;
	bool   mIsMuted;

};

} // close namespace pdg

#endif // PDG_NO_SOUND

#endif // SOUNDMGR_DIRECTX_H_INCLUDED
