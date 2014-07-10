// -----------------------------------------------
// sound-directx.cpp
//
// implementation of simple sound functionality
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

#include "pdg/msvcfix.h"  // fix non-standard MSVC
#include <windows.h>
#include <dshow.h>
#undef THIS // eliminate warning about Windows duplicate Macro
#include "sound-directx.h"
#include "sound-mgr-directx.h"
#include "pdg/framework.h"
#include <stdio.h>
#include <math.h>

#ifndef PDG_NO_GUI
  #include "pdg/sys/graphicsmanager.h"
  #include "pdg/sys/port.h"
#endif

#include "internals.h"

#if defined( PDG_DONT_USE_DIRECTSHOW_SOUND )
    #error sound-directx.cpp should not be used when PDG_DONT_USE_DIRECTSHOW_SOUND is defined, remove this file from your project
#endif


//#define NO_SOUND           // Uncomment this line to disable sound.
//#define PDG_DEBUG_SOUND   // Uncomment this line to debug sound.

#ifndef PDG_DEBUG_SOUND
  // if the network layer is not being debugged, we ignore all
  #define SOUND_DEBUG_ONLY(_expression)
#else
  #define SOUND_DEBUG_ONLY DEBUG_ONLY
#endif

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define MAXIMUM_ATTENUATION -10000 // in hundredths of a decibel

namespace pdg {

extern std::string os_makeCanonicalPath(const char* fromPath, bool resolveSimLinks = true);

extern HWND gMainHWND;

// -----------------------------------------------------------------------------------
// Sound Wave
// A simple sound that can be played over your speakers
// -----------------------------------------------------------------------------------

SoundDirectX::SoundDirectX(SoundManager* sndMgr, const char* soundName, const char* extension)
  : mSndMgr(static_cast<SoundManagerDirectX*>(sndMgr)),
    mpMediaControl(0),
	mpMediaSeeking(0),
	mpMediaEvent(0),
	mpBasicAudio(0),
	mTempFilename(0),
	mExtension(extension),
	mRealFilename(soundName),
	mLoadedOk(false),
	mPlaying(false),
	mStartedFadeMs(0),
	mDieAt(0)
{
#ifndef NO_SOUND
    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::__ct [%s]", mRealFilename.c_str()); )
	DEBUG_ASSERT(mSndMgr != 0, "mSndMgr is NULL!")
	setupDXStuff();
#endif
}

SoundDirectX::SoundDirectX(SoundDirectX* snd)
  : mSndMgr(snd->mSndMgr),
    mpMediaControl(0),
	mpMediaSeeking(0),
	mpMediaEvent(0),
	mpBasicAudio(0),
	mTempFilename(0),
	mExtension(snd->mExtension),
	mRealFilename(snd->mRealFilename),
	mLoadedOk(false),
	mPlaying(false),
	mStartedFadeMs(0),
	mDieAt(0)
{
#ifndef NO_SOUND
    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::__ct copy [%s]", mRealFilename.c_str()); )
	DEBUG_ASSERT(mSndMgr != 0, "mSndMgr is NULL!")
	setupDXStuff();
	if (snd->mTempFilename) {
		// the original was allocated from data and created a temporary file
		// the original will delete that temporary file, but we do need to addRef 
		// on the original so it won't delete the file until we are done with it
		// the file too.
		snd->addRef();
		createFromFile(snd->mTempFilename);
	} else {
		createFromFile(mRealFilename.c_str());
	}
#endif
}

SoundDirectX::~SoundDirectX()
{
#ifndef NO_SOUND
    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::__dt [%s]", mRealFilename.c_str()); )
    // Stop playback
    stop();

	// Disable event callbacks
    if (mpMediaEvent)
    {
        mpMediaEvent->SetNotifyWindow((OAHWND)NULL, 0, 0);
        SAFE_RELEASE(mpMediaEvent);
    }

    // Release all remaining pointers
	SAFE_RELEASE(mpBasicAudio);
    SAFE_RELEASE(mpMediaSeeking);
    SAFE_RELEASE(mpMediaControl);
    SAFE_RELEASE(mpGraphBuilder);

	// Free filename
    if (mTempFilename) {
		// Unlink Temporary file
		if(!OS::deleteFile(mTempFilename))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to delete temporary file [%s]", mTempFilename); )
		}

    	std::free(mTempFilename);
	    mTempFilename = 0;
    }
#endif
}

void SoundDirectX::play(float vol, int32 offsetX, float pitch, uint32 fromMs, int32 lenMs) {
#ifndef NO_SOUND
	SOUND_DEBUG_ONLY( OS::_DOUT("Sound::play [%p]", this); )
	SoundDirectX* snd = new SoundDirectX(this);
	snd->setVolume(vol).setOffsetX(offsetX).setPitch(pitch).skipTo(fromMs);
	snd->dieAt( (lenMs > 0) ? OS::getMilliseconds() + fromMs + lenMs : -1 );
	snd->start();
#endif
}

void SoundDirectX::start()
{
#ifndef NO_SOUND
	if(!mLoadedOk) return;

	SOUND_DEBUG_ONLY( OS::_DOUT("Sound::start [%s]", mRealFilename.c_str()); )
	mPaused = false;
	if(mSndMgr && mSndMgr->isMuted())
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("NOT Playing sound: Muted. [%s]", mRealFilename.c_str()); )
		return; // Don't play if we are muted or the sound didn't load
	}

	// Re-seek the graph to the beginning
    if(mpMediaSeeking)
    {
        LONGLONG llPos = 0;
        HRESULT hr = mpMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
                                          &llPos, AM_SEEKING_NoPositioning);
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to seek to beginning. %s", os_getPlatformErrorMessage(hr)); )
		}
    }

	// Start playback
    if(mpMediaControl && mSndMgr)
	{
        HRESULT hr = mpMediaControl->Run();
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to start playback of sound. %s", os_getPlatformErrorMessage(hr)); )
		}
		else
		{
			mSndMgr->soundPlaying(this);  // make sure the sound manager knows we are active so it can idle us
			mPlaying = true;
		}
	}

#endif
}

void SoundDirectX::stop()
{
#ifndef NO_SOUND
	if(!mLoadedOk) return;

    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::stop [%s]", mRealFilename.c_str()); )
	mPaused = false;

    // Stop playback
    if(mpMediaControl && mSndMgr)
	{
        HRESULT hr = mpMediaControl->Stop();
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to stop playback of sound. %s", os_getPlatformErrorMessage(hr)); )
		}
		mSndMgr->soundStopped(this);
		mPlaying = false;
	}
#endif
}

// pauses playback
void    SoundDirectX::pause()
{
#ifndef NO_SOUND
	if(!mLoadedOk) return;

	SOUND_DEBUG_ONLY( OS::_DOUT("Sound::pause [%s]", mRealFilename.c_str()); )
    // Stop playback
	mPaused = true;
    if(mpMediaControl)
	{
        HRESULT hr = mpMediaControl->Stop();
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to stop playback of sound. %s", os_getPlatformErrorMessage(hr)); )
		}
	}
#endif //NO_SOUND
}

// resumes playback after pausing
void    SoundDirectX::resume()
{
#ifndef NO_SOUND
	if(!mLoadedOk) return;

	if(mSndMgr && mSndMgr->isMuted())
	{
		return; // Don't play if we are muted
	}
    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::resume [%s]", mRealFilename.c_str()); )

	// Start playback
    if(mpMediaControl && mSndMgr)
	{
		mPaused = false;
        HRESULT hr = mpMediaControl->Run();
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to resume playback of sound. %s", os_getPlatformErrorMessage(hr)); )
		}
	}
#endif //NO_SOUND
}

// set pitch for this sound only
Sound& SoundDirectX::setPitch(float pitchOffset)
{
	return *this;
}

// change the pitch level over time
void SoundDirectX::changePitch(float targetOffset, int32 msDuration, EasingFunc easing)
{
}

// set the X offset from the center of the screen for this sound only
Sound& SoundDirectX::setOffsetX(int32 offsetX)
{
	long width = 1000;
  #ifndef PDG_NO_GUI
	Port* port = GraphicsManager::instance().getMainPort();
	if (port) {
		width = port->getDrawingArea().width();
	}
  #endif
	long pan = (5000L * offsetX)/width;
	if (pan < -10000L) pan = -10000L;
	if (pan > 10000L) pan = 10000L;
    if (mpBasicAudio)
    {
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::setOffsetX [%p] [%ld] setting pan to [%ld]", this, offsetX, pan) );
        HRESULT hr = mpBasicAudio->put_Balance(pan);
		if(FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Failed to put_Balance for sound. %s", os_getPlatformErrorMessage(hr)); )
		}
    }
	return *this;
}

// change the X offset from the center of the screen over time
void SoundDirectX::changeOffsetX(int32 targetOffset, int32 msDuration, EasingFunc easing)
{
}


 // Fade to zero volume and stop over the course of fadeMs milliseconds
void SoundDirectX::fadeOut(uint32 fadeMs, EasingFunc easing) {
  	if (mPlaying || mPaused) {
		changeVolume(0.0f, fadeMs, easing);
  	} else {
		setVolume(0.0f);
  	}
}

// Fade in to full volume over fadeMs milliseconds. If the sound was not already playing, start it.
void SoundDirectX::fadeIn(uint32 fadeMs, EasingFunc easing) {
	setVolume(0.0f);
	changeVolume(1.0f, fadeMs, easing);
}

// Fade up or down to reach a new volume level over fadeMs milliseconds. If the sound was not already playing, start it.
void SoundDirectX::changeVolume(float level, uint32 fadeMs, EasingFunc easing) {
	mTargetVolume = level;
	mStartingVolume = mCurrentVolume;
	mDeltaVolumePerMs = mCurrentVolume / (float)fadeMs;
	mStartedFadeMs = OS::getMilliseconds();
	if (!mPlaying && !mPaused) {
		start();
	}
}

// skip forward (positive value) or backward (negative value) by skipMilliseconds
Sound& SoundDirectX::skip(int32 skipMilleconds) {
	return *this;
}

// skip to a specific point in the sound
Sound& SoundDirectX::skipTo(uint32 timeMs) {
	return *this;
}
	
void SoundDirectX::createFromData(char* soundData, long soundDataLen)
{
#ifndef NO_SOUND
	FILE* fp;
//	char *tempFilename;
    SOUND_DEBUG_ONLY( OS::_DOUT("Sound::createFromData [%s]", mRealFilename.c_str()); )

	/* Create a temporary filename for the current working directory: */
/*
  #ifdef COMPILER_MSVC
	if( ( tempFilename = _tempnam( "c:\\", "catan" ) ) == NULL )
  #else
	if( ( tempFilename = std::tmpnam(0) ) == NULL )
  #endif
	{
		DEBUG_ONLY( OS::_DOUT("SOUND Error: Cannot create unique filename."); )
		return;
	}
*/
	DWORD dwBufSize=4096;
	char buffer[4096];
	WinAPI::GetTempPathA(dwBufSize, buffer);
	char tempFilename[MAX_PATH];
	WinAPI::GetTempFileNameA(buffer, "pdgSnd", 0, tempFilename);


	int bufferSize = strlen(tempFilename) + mExtension.length() + 2;
	mTempFilename = (char*) std::malloc(bufferSize);
	if(!mTempFilename)
	{
		DEBUG_ONLY( OS::_DOUT("SOUND Error: Cannot allocate memory for unique filename."); )
		return;
	}

	strncpy(mTempFilename, tempFilename, bufferSize);
    CHECK_PTR( &mTempFilename[strlen(tempFilename)], mTempFilename, bufferSize);
	mTempFilename[strlen(tempFilename)] = 0;

/*  #ifdef COMPILER_MSVC
	if(tempFilename)
	{
		std::free(tempFilename); // Must free memory allocated by _tempnam
	}
  WRONG! If you don't use the unique filename given to you by _tempnam you can't be sure it's unique.
	CHECK_PTR( &mTempFilename[strlen(mTempFilename)], mTempFilename, bufferSize);
	strcat(mTempFilename, ".");
	CHECK_PTR( &mTempFilename[strlen(mTempFilename)], mTempFilename, bufferSize);
	strcat(mTempFilename, mExtension.c_str());
 	CHECK_PTR( &mTempFilename[strlen(mTempFilename)], mTempFilename, bufferSize);

 #endif // COMPILER_MSVC
*/
//	mTempFilename[strlen(mTempFilename)] = 0;

	if(!(fp = fopen(mTempFilename, "wb")))
	{
		DEBUG_ONLY( OS::_DOUT("SOUND Error: Could not write SOUND file: [%s]", mTempFilename); )
		return;
	}
	DEBUG_ASSERT(soundData != 0, "Sound data is NULL!")
	DEBUG_ASSERT(soundDataLen > 0, "Sound data length is 0!")
	fwrite(soundData, soundDataLen, 1, fp);

	fclose(fp);
	SOUND_DEBUG_ONLY( OS::_DOUT("SOUND: Successfully wrote file: [%s]", mTempFilename); )
	createFromFile(mTempFilename);
#endif
}

void SoundDirectX::createFromFile(const char* filename)
{
#ifndef NO_SOUND
	SOUND_DEBUG_ONLY( OS::_DOUT("Sound::createFromFile [%p] file [%s]", this, filename); )
	std::string realPath = os_makeCanonicalPath(filename);  // assumes relative to application if relative path

	if (mpGraphBuilder)
	{
	    // convert to UTF16 for windows
	    utf16string wfilename;
	    OS::utf8to16(wfilename, realPath);
	    HRESULT hr = mpGraphBuilder->RenderFile((WCHAR*)wfilename.c_str(), 0);
		if (FAILED(hr))
		{
			DEBUG_ONLY( OS::_DOUT("SOUND Error: Can not render file [%s]. %s", filename, os_getPlatformErrorMessage(hr)); )
		}

		// Have the graph signal event via window callbacks
		if (mpMediaEvent && mSndMgr)
		{
			mpMediaEvent->SetNotifyWindow((OAHWND)gMainHWND, WM_PDG_SOUND_NOTIFY, (LONG_PTR)this);
		}
		mLoadedOk = true;
	}
#endif
}

// set volume level for this sound only
// 1.0 is max, 0.0 is silent
Sound& SoundDirectX::setVolume(float level)
{
	mCurrentVolume = level;
#ifndef NO_SOUND
	if(!mLoadedOk) return *this;

	if(mpBasicAudio)
	{
		// Change our volume level range from 0.0 -> 1.0 to 0.1 -> 1.0 since log10(.1)=-1 and log10(1)=0
		double newVolumeLevel = 0.1f + (level * (1.0f - 0.1f));
		// Change linear scale to logarithmic decibels
		double hundrethDecibelVolume = 4000.0*log10(newVolumeLevel);
		// Check to see if we are at the lowest volume level...if so make it even lower :-)
		hundrethDecibelVolume = ((long)hundrethDecibelVolume < -3998) ? MAXIMUM_ATTENUATION : hundrethDecibelVolume;
		// Finally set the volume
		mpBasicAudio->put_Volume((long)hundrethDecibelVolume);
	}
#endif
	return *this;
}


void SoundDirectX::idle() 
{
	if (mPlaying && !mPaused) {
		// do volume fading if needed
		if (mStartedFadeMs) {
			uint32 msElapsed = OS::getMilliseconds() - mStartedFadeMs;
			float newVolume = mStartingVolume + ((float)msElapsed * mDeltaVolumePerMs);
			if ( ((mDeltaVolumePerMs < 0.0f) && (newVolume < mTargetVolume)) 
				|| ((mDeltaVolumePerMs > 0.0f) && (newVolume > mTargetVolume)) ) {
				newVolume = mTargetVolume;
				mStartedFadeMs = 0;  // done fading
			}
			if (newVolume > 1.0f) {
				newVolume = 1.0f;
			} else if (newVolume < 0.0f) {
				newVolume = 0.0f;
			}
			setVolume(newVolume);
		}
	}
}

void SoundDirectX::handleGraphEvent(void)
{
#ifndef NO_SOUND
    LONG evCode, evParam1, evParam2;

    // Since we may have a scenario where we're shutting down the application,
    // but events are still being generated, make sure that the event
    // interface is still valid before using it.  It's possible that
    // the interface could be freed during shutdown but later referenced in
    // this callback before the app completely exits.
    if (!mpMediaEvent || !mSndMgr)
        return;

    while(SUCCEEDED(mpMediaEvent->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                    (LONG_PTR *) &evParam2, 0)))
    {
        // Spin through the events
        if(EC_COMPLETE == evCode)
        {
			SoundEventInfo soundInfo;

			// If looping, reset to beginning and continue playing
            if (mLoopSound)
            {
				SOUND_DEBUG_ONLY( OS::_DOUT("Sound: [%s] sound reached end and looping, posting soundEvent_Looping", getRealFileName()); )
 				soundInfo.eventCode = soundEvent_Looping;
				soundInfo.sound = this;

                if (mpMediaSeeking)
                {
                    LONGLONG pos=0;
					HRESULT hr;

                    // Reset to first frame of movie
                    hr = mpMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
                                                      NULL, AM_SEEKING_NoPositioning);
                    if (FAILED(hr))
                    {
 						DEBUG_ONLY( OS::_DOUT("SOUND Error: Can not seek to beginning of sound for looping. "
 							"Stopping and re-Playing [%s]. %s", mTempFilename, os_getPlatformErrorMessage(hr)); )
                        // Some custom filters (like the Windows CE MIDI filter)
                        // may not implement seeking interfaces (IMediaSeeking)
                        // to allow seeking to the start.  In that case, just stop
                        // and restart for the same effect.  This should not be
                        // necessary in most cases.
						stop();
                        start();
                    }
					// Reset the volume level. For some reason some machines go back to 100%.
					setVolume(mCurrentVolume);
                }
            }
			else
			{
				SOUND_DEBUG_ONLY( OS::_DOUT("Sound: [%s] sound finished, posting soundEvent_DonePlaying", getRealFileName()); )
                mSndMgr->soundStopped(this);
			    soundInfo.eventCode = soundEvent_DonePlaying;
				soundInfo.sound = this;
            }
            postEvent(eventType_SoundEvent, &soundInfo);
		}

        // Free memory associated with this event
        mpMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
    }
#endif
}

void SoundDirectX::setupDXStuff() {
#ifndef NO_SOUND
	// Create DirectShow Graph
    CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC, IID_IGraphBuilder,
        reinterpret_cast<void **>(&mpGraphBuilder));
	DEBUG_ASSERT(mpGraphBuilder != 0, "mpGraphBuilder is NULL!")

    // Get the IMediaControl Interface
	if(mpGraphBuilder)
	{
		mpGraphBuilder->QueryInterface(IID_IMediaControl,
			reinterpret_cast<void **>(&mpMediaControl));
		DEBUG_ASSERT(mpMediaControl != 0, "mpMediaControl is NULL!")

		// Get the IMediaSeeking Interface
		mpGraphBuilder->QueryInterface(IID_IMediaSeeking,
			reinterpret_cast<void **>(&mpMediaSeeking));
		DEBUG_ASSERT(mpMediaSeeking != 0, "mpMediaSeeking is NULL!")

		// Get the IMediaEventEx Interface
		mpGraphBuilder->QueryInterface(IID_IMediaEventEx,
			reinterpret_cast<void **>(&mpMediaEvent));
		DEBUG_ASSERT(mpMediaEvent != 0, "mpMediaEvent is NULL!")

		// Get the IBasicAudio Interface
		mpGraphBuilder->QueryInterface(IID_IBasicAudio,
			reinterpret_cast<void **>(&mpBasicAudio));
		DEBUG_ASSERT(mpBasicAudio != 0, "mpBasicAudio is NULL!")
	}
#endif
}


static std::string getfileext(std::string filename) {
	// first convert filename to all lowercase
	 for (unsigned int i = 0; i < filename.size(); i++) {
		 if (isupper(filename[i])) {
			 filename[i] = tolower(filename[i]);
		 }
	 }
 
	std::string::size_type pos = filename.rfind(".");
	if(pos == std::string::npos)
	{
		DEBUG_ONLY( OS::_DOUT("ERROR: Couldn't find extension to sound filename."); )
		return std::string("");
	}
	std::string extension = filename.substr(pos+1, filename.length());
	return extension;
}

Sound* 
Sound::createSoundFromData(const char* soundName, char* soundData, long soundDataLen) {
	std::string extension = getfileext(soundName);
	SoundDirectX* sndX = new SoundDirectX(SoundManager::getSingletonInstance(), soundName, extension.c_str());
	sndX->createFromData(soundData, soundDataLen);
	return sndX;
}

Sound* 
Sound::createSoundFromFile(const char* soundFileName) {
	std::string extension = getfileext(soundFileName);
	SoundDirectX* sndX = new SoundDirectX(SoundManager::getSingletonInstance(), soundFileName, extension.c_str());
	sndX->createFromFile(soundFileName);
	return sndX;
}


} // close namespace pdg

#endif PDG_NO_SOUND




