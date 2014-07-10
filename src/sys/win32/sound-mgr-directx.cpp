// -----------------------------------------------
// sound-mgr-directx.cpp
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


#include "pdg_project.h"

#ifndef PDG_NO_SOUND

#include "pdg/msvcfix.h"
#include "sound-mgr-directx.h"
#include "pdg/framework.h"
#include "sound-directx.h"
#include <tchar.h>

#define PDG_WIN32_MAX_VOLUME 0xFFFFFFFF

#if defined( PDG_DONT_USE_DIRECTSHOW_SOUND )
    #error sound-mgr-directx.cpp should not be used when PDG_DONT_USE_DIRECTSHOW_SOUND is defined, remove this file from your project
#endif

namespace pdg {

MemStats gSoundStats;

SoundManagerDirectX::SoundManagerDirectX(EventManager* theEventManager) //, HWND ourWindow) 
: mEventMgr(theEventManager),
//  mWindow(ourWindow),
  mPreviousWaveVolume(0),
  mMusicSound(0),
  mIsMuted(false)
{
    DEBUG_ONLY( OS::_DOUT("SoundManagerDirectX::__ct"); )
 	setup();
}

SoundManagerDirectX::~SoundManagerDirectX() {
    DEBUG_ONLY( OS::_DOUT("SoundManagerDirectX::__dt"); )
}

// set volume for all sounds. Individual sounds may have separate volumes, but they are 
// treated as a relative volume with this being max
// 1.0 is max, 0.0 is silent
void    
SoundManagerDirectX::setVolume(float level) {
}

// turn on and off the playing of sounds altogether. setMute(false) restores all sounds 
// to previous levels
void    
SoundManagerDirectX::setMute(bool muted) {
}

// give time to the sound layer to do anything it may need to do, such as refilling buffers
void    
SoundManagerDirectX::idle() {
}

// inform the Sound Manager there is a new sound playing
void SoundManagerDirectX::soundPlaying(Sound* sound) {
    sound->addRef();
    mSounds.push_back(sound);   // track the sound

	// Check to see if this sound is an mp3 or a midi and track it as our music
	SoundDirectX* soundWave = static_cast<SoundDirectX*>(sound);
	//OS::_DOUT("SOUND PLAYING: %s", soundWave->getFileName());
	if (soundWave) {
		std::string extension(soundWave->getFileNameExtension());
		if(extension == "MP3" || extension == "mp3" ||
			extension == "MID" || extension == "mid")
		{
			mMusicSound = sound;
		}
	}
}

// inform the Sound Manager there is a sound that has stopped playing
void SoundManagerDirectX::soundStopped(Sound* sound) {
	SoundDirectX* soundWave = static_cast<SoundDirectX*>(sound);
	//OS::_DOUT("SOUND STOPPED: %s", soundWave->getFileName());
	SoundsList::iterator it = mSounds.begin();
    while (it != mSounds.end()) {
        if (*it == sound) {
            mSounds.erase(it);
            sound->release();
            break;
        }
        ++it;
    }
}

// Stop sounds from playing except music
void SoundManagerDirectX::stopAllSoundsExceptMusic() 
{
    SoundsList sounds = mSounds; // Make a copy so we don't stomp ourselves when we stop the sounds.
	SoundsList::iterator itr;
	for(itr=sounds.begin(); itr != sounds.end(); itr++) 
	{
        if(*itr && *itr != mMusicSound) 
		{
			//SoundWave* soundWave = static_cast<SoundWave*>(*itr);
			//OS::_DOUT("STOPPING SOUND: %s", soundWave->getFileName());
			(*itr)->stop();
        }
    }
}

// Stops all sounds including music
void SoundManagerDirectX::stopAllSoundsAndMusic()
{
    SoundsList sounds = mSounds; // Make a copy so we don't stomp ourselves when we stop the sounds.
	SoundsList::iterator itr;
	for(itr=sounds.begin(); itr != sounds.end(); itr++) 
	{
        if(*itr /*&& *itr != mMusicSound */) 
		{
			//SoundWave* soundWave = static_cast<SoundWave*>(*itr);
			//OS::_DOUT("STOPPING SOUND: %s", soundWave->getFileName());
			(*itr)->stop();
        }
    }
}

void SoundManagerDirectX::mute(bool muteOn)
{
	mIsMuted = muteOn;
	if(mIsMuted)
	{
		if(getMusic())
		{
			getMusic()->pause();
		}
		stopAllSoundsExceptMusic();
	}
	else
	{
		if(getMusic() && getMusic()->isPaused())
		{
			getMusic()->resume();
		}
	}
}

void SoundManagerDirectX::setup()
{
  #ifndef PDG_NO_SOUND
    DEBUG_ONLY( OS::_DOUT("SoundManagerDirectX::setup CoInitialize"); )
    // Initialize COM
    CoInitialize(NULL);
  #endif // PDG_NO_SOUND
}



SoundManager* SoundManager::createSingletonInstance() {
	EventManager* evtMgr = EventManager::getSingletonInstance();
	SoundManager* sndMgr =  new SoundManagerDirectX(evtMgr);
	return sndMgr;
}


} // close namespace pdg

#endif // PDG_NO_SOUND

