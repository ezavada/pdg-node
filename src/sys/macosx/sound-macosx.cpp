// -----------------------------------------------
// sound-macosx.cpp
// 
// implementation of simple sound functionality
// using Mac OS X /iPhone Core Audio APIs
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


#include "pdg_project.h"

#include "pdg/msvcfix.h" // fixes GCC too

#include "sound-macosx.h"
#include "sound-mgr-macosx.h"
#include "pdg/sys/core.h"
#include "pdg/sys/events.h"
#include "pdg/sys/mutex.h"
#include "pdg/sys/os.h"

#ifndef PDG_NO_GUI
  #include "pdg/sys/graphicsmanager.h"
  #include "pdg/sys/port.h"
#endif

#include "internals.h"

//#define PDG_DEBUG_SOUND
//#define PDG_USE_AUDIO_TOOLBOX_INTERNAL_THREAD   // do this if the RunLoop isn't available

#ifndef PDG_DEBUG_SOUND
  // if the sound layer is not being debugged, we ignore all 
  #define SOUND_DEBUG_ONLY(_expression)
#else
  #define SOUND_DEBUG_ONLY DEBUG_ONLY
#endif

#define READ_PERMISSION 0x1 //fsRdPerm
#define _UINT32
#include <AudioToolbox/AudioToolbox.h>
#include <cstdio>


#define CheckError(error, message, error_action) \
do {																	\
    OSStatus __err = error;												\
    if (__err) {														\
        DEBUG_PRINT("Error %s Mac OS X Error - %s", message, __err, os_getPlatformErrorMessage(__err));	\
        error_action;													\
	 }																	\
} while (0)

namespace pdg {

	// private from os module
	std::string os_makeCanonicalPath(const char* fromPath, bool resolveSimLinks = true);  // assumes relative to application if relative path
	
	static const int kNumberSoundBuffers = 3;

	struct snd_MacAudioData {
		CFURLRef						mFileRef;
		AudioFileID						mAudioFile;
		AudioStreamBasicDescription		mDataFormat;
		AudioQueueRef					mQueue;
		AudioQueueBufferRef				mBuffers[kNumberSoundBuffers];
		SInt64							mCurrentPacket;
		UInt32							mNumPacketsToRead;
		AudioStreamPacketDescription *	mPacketDescs;
		bool							mDone;
		pdg::SoundMac*					mSoundObj;
		UInt32							mBufferByteSize;
		OSStatus						mError;
//		pdg::Mutex						mSoundCallbackMutex;
		snd_MacAudioData() {
			mFileRef = 0;
			mQueue = 0;
			mCurrentPacket = 0;
			mNumPacketsToRead = 0;
			mPacketDescs = 0;
			mSoundObj = 0;
			mBufferByteSize = 0;
			mError = 0;
			for (int i = 0; i<kNumberSoundBuffers; i++) { mBuffers[i] = 0; }
		}
	};
	
	bool	snd_IsFormatVBR(const AudioStreamBasicDescription& au);
	void    snd_CalculateBytesForTime (AudioStreamBasicDescription& inDesc, UInt32 inMaxPacketSize, Float64 inSeconds, UInt32 *outBufferSize, UInt32 *outNumPackets);
	void    snd_CopyFilePropertyIntoQueue(AudioFileID inAudioFile, AudioQueueRef inAudioQueue, AudioFilePropertyID inFilePropertyID,  AudioQueuePropertyID inQueuePropertyID);

	
	static void snd_BufferCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inCompleteAQBuffer);
	static void snd_AudioQueueRunningListenerCallback(void* inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID);


	static void snd_BufferCallback(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inCompleteAQBuffer) {
		snd_MacAudioData * myInfo = (snd_MacAudioData*)inUserData;
//		pdg::AutoMutex(&myInfo->mSoundCallbackMutex);
		if (myInfo->mDone) return;
		myInfo->mError = noErr;
		
		UInt32 numBytes;
		UInt32 nPackets = myInfo->mNumPacketsToRead;
		OSStatus err = AudioFileReadPackets(myInfo->mAudioFile, false, &numBytes, myInfo->mPacketDescs, myInfo->mCurrentPacket, &nPackets, 
											inCompleteAQBuffer->mAudioData);
		if (err) { // if an error happens, stop trying to play the sound. the idle() method will detect this and post the error to the application layer
        	DEBUG_PRINT("AudioFileReadPackets failed Mac OS X Error: %d - %s", err, os_getPlatformErrorMessage(err));
			myInfo->mDone = true; 
			myInfo->mError = err; 
			return; 
		}
		
		if (nPackets > 0) {
			SOUND_DEBUG_ONLY( OS::_DOUT("snd_BufferCallback: %ld packets %ld bytes", nPackets, numBytes); )
			inCompleteAQBuffer->mAudioDataByteSize = numBytes;		
			AudioQueueEnqueueBuffer(inAQ, inCompleteAQBuffer, (myInfo->mPacketDescs ? nPackets : 0), myInfo->mPacketDescs);
			myInfo->mCurrentPacket += nPackets;
		} else {
			// reading nPackets == 0 is our EOF condition
			SOUND_DEBUG_ONLY( OS::_DOUT("snd_BufferCallback nPackets == 0"); )
			AudioQueueStop(myInfo->mQueue, false);
			myInfo->mDone = true;
		}
	}
	
	static void snd_AudioQueueRunningListenerCallback(void* inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID) {
		snd_MacAudioData * myInfo = (snd_MacAudioData*)inUserData;
//		pdg::AutoMutex(&myInfo->mSoundCallbackMutex);
		UInt32 isRunning;
		UInt32 size = sizeof(isRunning);
		OSStatus err = AudioQueueGetProperty(inAQ, kAudioQueueProperty_IsRunning, &isRunning, &size);
		if (err) { 
        	DEBUG_PRINT("AudioQueueGetProperty failed Mac OS X Error: %d - %s", err, os_getPlatformErrorMessage(err));
			myInfo->mError = err;
		}
		SOUND_DEBUG_ONLY( OS::_DOUT("snd_AudioQueueRunningListenerCallback %ld", isRunning); )
		SoundMac* theSound = myInfo->mSoundObj;
		if (theSound && theSound->isPlaying() && !isRunning) {
			// clear the playing flag so we know to do something with it at idle time
			theSound->setPlaying(isRunning);
		}
	}
	
	// we only use time here as a guideline
	// we're really trying to get somewhere between 16K and 64K buffers, but not allocate too much if we don't need it
	void snd_CalculateBytesForTime (AudioStreamBasicDescription& inDesc, UInt32 inMaxPacketSize, Float64 inSeconds, UInt32 *outBufferSize, UInt32 *outNumPackets)
	{
		static const UInt32 maxBufferSize = 0x10000; // limit size to 64K
		static const UInt32 minBufferSize = 0x4000; // limit size to 16K
		
		if (inDesc.mFramesPerPacket) {
			Float64 numPacketsForTime = inDesc.mSampleRate / inDesc.mFramesPerPacket * inSeconds;
			*outBufferSize = numPacketsForTime * inMaxPacketSize;
		} else {
			// if frames per packet is zero, then the codec has no predictable packet == time
			// so we can't tailor this (we don't know how many Packets represent a time period
			// we'll just return a default buffer size
			*outBufferSize = maxBufferSize > inMaxPacketSize ? maxBufferSize : inMaxPacketSize;
		}
		
		// we're going to limit our size to our default
		if (*outBufferSize > maxBufferSize && *outBufferSize > inMaxPacketSize)
			*outBufferSize = maxBufferSize;
		else {
			// also make sure we're not too small - we don't want to go the disk for too small chunks
			if (*outBufferSize < minBufferSize)
				*outBufferSize = minBufferSize;
		}
		*outNumPackets = *outBufferSize / inMaxPacketSize;
	}

	void snd_CopyFilePropertyIntoQueue(AudioFileID inAudioFile, AudioQueueRef inAudioQueue, AudioFilePropertyID inFilePropertyID,  AudioQueuePropertyID inQueuePropertyID)
	{
		UInt32 size = sizeof(UInt32);
		OSStatus result = AudioFileGetPropertyInfo (inAudioFile, inFilePropertyID, &size, NULL);
		if (result == noErr && size > 0) {
			void* data = std::malloc(size);		
			if ((result = AudioFileGetProperty(inAudioFile, inFilePropertyID, &size, data)) == noErr) {
				CheckError (AudioQueueSetProperty(inAudioQueue, inQueuePropertyID, data, size), "set property on queue", {});
			} else {
        		DEBUG_PRINT("AudioFileGetProperty failed Mac OS X Error: %d - %s", result, os_getPlatformErrorMessage(result));
			}
			std::free(data);
		}
    }
	
	// -----------------------------------------------------------------------------------
	// Sound Mac
	// A simple sound that can be played over your speakers
	// -----------------------------------------------------------------------------------

	SoundMac::SoundMac(SoundManager* sndMgr, const char* extension)
	  : mSndMgr(static_cast<SoundManagerMac*>(sndMgr)),
		mMacDataRef(0),
		mDataPtr(0),
		mFilename(),
		mExtension(extension),
		mPlaying(false),
		mStartedFadeMs(0),
		mSkipToMs(0),
		mDieAt(0)
	{
		gSoundStats.cxxObjsAllocated++;
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::ct [%p], sound extension [%s]", this, extension); )
		DEBUG_ASSERT( mExtension == "ref", "Mac OS X sound manager only supports .ref file which has a relative path to an external sound");
	}

	SoundMac::SoundMac(SoundMac* snd)
	  : mSndMgr(snd->mSndMgr),
		mMacDataRef(0),
		mDataPtr(0),
		mFilename(snd->mFilename),
		mExtension(snd->mExtension),
		mPlaying(false),
		mStartedFadeMs(0),
		mSkipToMs(0),
		mDieAt(0)
	{
		gSoundStats.cxxObjsAllocated++;
		snd_MacAudioData* srcSndData = static_cast<snd_MacAudioData*>(snd->mMacDataRef);
		createFromFileUrl(srcSndData->mFileRef);
	}

	SoundMac::~SoundMac()
	{
		gSoundStats.cxxObjsFreed++;
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::dt [%p]", this); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (sndData) {
			stop();
			SOUND_DEBUG_ONLY( OS::_DOUT("  Sound::dt [%p] cleaning up data ref", this); )
			// lets clean up
			for (int i = 0; i < kNumberSoundBuffers; ++i) {
				if (sndData->mBuffers[i] != NULL) {
					CheckError(AudioQueueFreeBuffer(sndData->mQueue, sndData->mBuffers[i]), "AudioQueueFreeBuffer() failed", {});
					gSoundStats.osEntitiesFreed--;			
				}
			}
			CheckError(AudioQueueDispose(sndData->mQueue, true), "AudioQueueDispose(true) failed", {});
			gSoundStats.osEntitiesFreed++;
			CheckError(AudioFileClose(sndData->mAudioFile), "AudioFileClose() failed", {});
			gSoundStats.filesClosed++;
			if (sndData->mPacketDescs) {
				delete [] sndData->mPacketDescs;
				gSoundStats.blocksFreed++;
			}
			if (sndData->mFileRef) {
				CFRelease(sndData->mFileRef);
				gSoundStats.osEntitiesFreed++;
			}
			delete sndData;
			gSoundStats.blocksFreed++;
			mMacDataRef = 0;
		}
		if (mDataPtr) {
			SOUND_DEBUG_ONLY( OS::_DOUT("  Sound::dt [%p] cleaning up sound data", this); )
			delete[] (char*)mDataPtr;
			gSoundStats.blocksFreed++;
		}
	}

	void SoundMac::play(float vol, int32 offsetX, float pitch, uint32 fromMs, int32 lenMs)
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::play [%p]", this); )
		SoundMac* snd = new SoundMac(this);
		snd->setVolume(vol).setOffsetX(offsetX).setPitch(pitch).skipTo(fromMs);
		snd->dieAt( (lenMs > 0) ? OS::getMilliseconds() + fromMs + lenMs : -1 );
		snd->start();
	}

	void SoundMac::start()
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::start [%p]", this); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (!mPlaying) {
			mPlaying = true;
			mSndMgr->soundPlaying(this);  // make sure the sound manager knows we are active so it can idle us
		} else if (sndData) {
			SOUND_DEBUG_ONLY( OS::_DOUT("Sound::play [%p] already playing, not registering with Sound Mgr", this); )
//			pdg::AutoMutex(&sndData->mSoundCallbackMutex);		
			CheckError(AudioQueueStop(sndData->mQueue, true), "AudioQueueStop failed", {});
		}
		if (sndData) {
//			pdg::AutoMutex(&sndData->mSoundCallbackMutex);		
			mStartedPlayingAtMs = OS::getMilliseconds() - mSkipToMs;
			// prime the queue with some data before starting
			sndData->mDone = false;
			if (mSkipToMs) {
				sndData->mCurrentPacket = ((float)mSkipToMs/1000.0f) * (sndData->mDataFormat.mSampleRate / sndData->mDataFormat.mFramesPerPacket);
				mSkipToMs = 0;
			} else {
				sndData->mCurrentPacket = 0;
			}
			for (int i = 0; i < kNumberSoundBuffers; ++i) {
				if (sndData->mBuffers[i] == NULL) {
					CheckError(AudioQueueAllocateBuffer(sndData->mQueue, sndData->mBufferByteSize, &sndData->mBuffers[i]), 
							   "AudioQueueAllocateBuffer failed", sndData->mError = __err; return);
					gSoundStats.osEntitiesAllocated++;
				}
				snd_BufferCallback (sndData, sndData->mQueue, sndData->mBuffers[i]);
				
				if (sndData->mDone) break;
			}
		  #if ( __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000 ) || ( __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060 )
			UInt32 propValue = 1;      
			CheckError (AudioQueueSetProperty(sndData->mQueue, kAudioQueueProperty_EnableTimePitch, &propValue, 
        		sizeof(propValue)), "enable time/pitch processor", {} );
          #endif
			CheckError(AudioQueueStart(sndData->mQueue, NULL), "AudioQueueStart failed", sndData->mDone = true; sndData->mError = __err);
		}
	}

	// this does not send a soundEvent_DonePlaying
	void SoundMac::stop()
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::stop [%p]", this); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (mPlaying) {
			mPlaying = false;
			if (sndData) {
//				pdg::AutoMutex(&sndData->mSoundCallbackMutex);		
				CheckError(AudioQueueStop(sndData->mQueue, true), "AudioQueueStop failed", {});
			}
		}
		mSndMgr->soundStopped(this);
	}

	void SoundMac::pause()
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::pause [%p]", this); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (mPlaying) {
			mPlaying = false;
			mPaused = true;
			if (sndData) {
//				pdg::AutoMutex(&sndData->mSoundCallbackMutex);		
				CheckError (AudioQueuePause(sndData->mQueue), "AudioQueuePause", sndData->mError = __err);
			}
		}
		mPausedAtMs = OS::getMilliseconds();
	}

	void SoundMac::resume()
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::resume [%p]", this); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (!mPlaying) {
			mPlaying = true;
			mPaused = false;
			if (sndData) {
//				pdg::AutoMutex(&sndData->mSoundCallbackMutex);		
				CheckError (AudioQueueStart(sndData->mQueue, NULL), "AudioQueueStart", sndData->mDone = true; sndData->mError = __err);
			}
		}
		mStartedPlayingAtMs += (OS::getMilliseconds() - mPausedAtMs);  // offset for time we have been paused
	}

	// set pitch for this sound only
	// returns this sound for chaining calls
	// +1.0 = one octave higher, -1.0 = one octave lower
	Sound& SoundMac::setPitch(float pitchOffset) {
	  #if ( __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000 ) || ( __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060 )
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::setPitch [%p] pitchOffset [%f]", this, pitchOffset); )
		float pitch = pitchOffset * 1200;
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
        // only if we enabled pitch do we actually set it
		CheckError (AudioQueueSetParameter(sndData->mQueue, kAudioQueueParam_Pitch, pitch), "set pitch", {});
	  #endif
		return *this;
	}

	// change the pitch level over time
	// +1.0 = one octave higher, -1.0 = one octave lower
	void SoundMac::changePitch(float targetOffset, int32 msDuration, EasingFunc easing) {
	}

	// set the X offset from the center of the screen for this sound only
	// returns this sound for chaining calls
	Sound& SoundMac::setOffsetX(int32 offsetX) {
	  #ifndef MAC_OS_X_VERSION_10_7 
	  	#define kAudioQueueParam_Pan 13
	  #endif
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::setOffsetX [%p] offsetX [%ld]", this, offsetX); )
		long width = 1000;
	  #ifndef PDG_NO_GUI
		Port* port = GraphicsManager::instance().getMainPort();
		if (port) {
			width = port->getDrawingArea().width();
		}
	  #endif
		float pan = (float)offsetX/(float)width;
		if (pan < -1.0) pan = -1.0;
		if (pan > 1.0) pan = 1.0;
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::setOffsetX [%p] [%ld] setting pan to [%0.2f]", this, offsetX, pan) );
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		CheckError (AudioQueueSetParameter(sndData->mQueue, kAudioQueueParam_Pan, pan), "set pan", {});
		return *this;
	}

	// change the X offset from the center of the screen over time
	void SoundMac::changeOffsetX(int32 targetOffset, int32 msDuration, EasingFunc easing) {
	}

    // Fade to zero volume and stop over the course of fadeMs milliseconds
	void SoundMac::fadeOut(uint32 fadeMs, EasingFunc easing) {
		if (mPlaying || mPaused) {
			changeVolume(0.0f, fadeMs, easing);
		} else {
			setVolume(0.0f);
		}
	}

	// Fade in to full volume over fadeMs milliseconds. If the sound was not already playing, start it.
	void SoundMac::fadeIn(uint32 fadeMs, EasingFunc easing) {
		setVolume(0.0f);
		changeVolume(1.0f, fadeMs, easing);
	}
	
	// Fade up or down to reach a new volume level over fadeMs milliseconds. If the sound was not already playing, start it.
	void SoundMac::changeVolume(float level, uint32 fadeMs, EasingFunc easing) {
		mTargetVolume = level;
		mStartingVolume = mCurrentVolume;
		mDeltaVolumePerMs = (mTargetVolume - mCurrentVolume) / (float)fadeMs;
		mStartedFadeMs = OS::getMilliseconds();
		if (!mPlaying && !mPaused) {
			start();
		}
	}
	
	// skip forward (positive value) or backward (negative value) by skipMilliseconds
	Sound&   SoundMac::skip(int32 skipMilleconds) {
		uint32 currentTime = OS::getMilliseconds() - mStartedPlayingAtMs;
		uint32 newTime = currentTime + skipMilleconds;
		skipTo(newTime);
		return *this;
	}
	
    // skip to a specific point in the sound
	Sound&   SoundMac::skipTo(uint32 timeMs) {
		mSkipToMs = timeMs;
		if (mPlaying) {
			// if we are already playing, jump immediately to the new location
			stop();
			start();
		}
		return *this;
	}
	
	void SoundMac::createFromData(char* soundData, long soundDataLen)
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::createFromData [%p] data [%p] len [%d]", this, soundData, soundDataLen); )
		snd_MacAudioData* sndData = new snd_MacAudioData();
		mMacDataRef = sndData;
		sndData->mSoundObj = this;
		sndData->mError = noErr;
		
		if (mExtension != "ref") return;
		
		std::string realFilename(soundData, soundDataLen -1);
#ifdef PLATFORM_IOS
		// for iOS, we can't have sounds stored in subdirectories, so just extract the filename
		int pos = realFilename.rfind('/');
		if (pos != std::string::npos) {
			realFilename.replace(0, pos + 1, "");
		}
#endif
		std::string inputFile = OS::getApplicationDataDirectory() + realFilename;

		createFromFile(inputFile.c_str());
	}

	void SoundMac::createFromFile(const char* filename)
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::createFromFile [%p] file [%s]", this, filename); )

		std::string realPath = os_makeCanonicalPath(filename);  // assumes relative to application if relative path
		mFilename = realPath;
		CFURLRef fileRef = CFURLCreateFromFileSystemRepresentation (NULL, (const UInt8 *)realPath.c_str(), realPath.length(), false);
		createFromFileUrl(fileRef);
	}
 
	void SoundMac::createFromFileUrl(CFURLRef fileRef) 
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::createFromFileUrl(CFURLRef) [%p] file [%s]", this, mFilename.c_str()); )
		snd_MacAudioData* sndData = new snd_MacAudioData();
		mMacDataRef = sndData;
		sndData->mSoundObj = this;
		sndData->mError = noErr;
		sndData->mFileRef = fileRef;
		CheckError (!sndData->mFileRef, "CFURLCreateFromFileSystemRepresentation can't parse file path", sndData->mError = -1; return);
		CFRetain(sndData->mFileRef);
		CheckError (AudioFileOpenURL (sndData->mFileRef, READ_PERMISSION, 0, &sndData->mAudioFile), "AudioFileOpen", sndData->mError = __err; 
			DEBUG_ONLY( OS::_DOUT("Failed to open file [%s]", mFilename.c_str()) ); return);
		
		UInt32 size = sizeof(sndData->mDataFormat);
		CheckError(AudioFileGetProperty(sndData->mAudioFile, kAudioFilePropertyDataFormat, &size, 
										&sndData->mDataFormat), "couldn't get file's data format", sndData->mError = __err; return);
		
#ifdef PDG_USE_AUDIO_TOOLBOX_INTERNAL_THREAD
		CheckError(AudioQueueNewOutput(&sndData->mDataFormat, snd_BufferCallback, sndData, NULL, 
									   0, 0, &sndData->mQueue), "AudioQueueNew failed", sndData->mError = __err; return);
#else
		CheckError(AudioQueueNewOutput(&sndData->mDataFormat, snd_BufferCallback, sndData, CFRunLoopGetCurrent(), 
									   kCFRunLoopCommonModes, 0, &sndData->mQueue), "AudioQueueNew failed", sndData->mError = __err; return);
#endif
		// we need to calculate how many packets we read at a time, and how big a buffer we need
		// we base this on the size of the packets in the file and an approximate duration for each buffer
		bool isFormatVBR = snd_IsFormatVBR(sndData->mDataFormat);
		// first check to see what the max size of a packet is - if it is bigger
		// than our allocation default size, that needs to become larger
		UInt32 maxPacketSize;
		size = sizeof(maxPacketSize);
		CheckError(AudioFileGetProperty(sndData->mAudioFile, kAudioFilePropertyPacketSizeUpperBound, 
										&size, &maxPacketSize), "AudioFileGetProperty couldn't get file's max packet size", sndData->mError = __err; return);
		// adjust buffer size to represent about a half second of audio based on this format
		snd_CalculateBytesForTime(sndData->mDataFormat, maxPacketSize, 0.5/*seconds*/, &sndData->mBufferByteSize, &sndData->mNumPacketsToRead);
		 // we don't provide packet descriptions for constant bit rate formats (like linear PCM)
		sndData->mPacketDescs = (isFormatVBR) ? new AudioStreamPacketDescription [sndData->mNumPacketsToRead] : NULL;
		
		// If the file has a cookie and/or channel layout, we should get them and set them on the AQ
		snd_CopyFilePropertyIntoQueue(sndData->mAudioFile, sndData->mQueue, kAudioFilePropertyMagicCookieData, kAudioQueueProperty_MagicCookie);
		snd_CopyFilePropertyIntoQueue(sndData->mAudioFile, sndData->mQueue, kAudioFilePropertyChannelLayout, kAudioQueueProperty_ChannelLayout);
		
		// set the volume of the queue
		CheckError (AudioQueueSetParameter(sndData->mQueue, kAudioQueueParam_Volume, 1.0f), "set queue volume", {});
		// add the listener that watches for the sound to complete
		CheckError (AudioQueueAddPropertyListener (sndData->mQueue, kAudioQueueProperty_IsRunning, snd_AudioQueueRunningListenerCallback, sndData), "add listener", {});	
		mCurrentVolume = 1.0f;
	}

	Sound& SoundMac::setVolume(float level)
	{
		SOUND_DEBUG_ONLY( OS::_DOUT("Sound::setVolume [%p] level [%f]", this, level); )
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		CheckError (AudioQueueSetParameter(sndData->mQueue, kAudioQueueParam_Volume, level), "set queue volume", {});
		mCurrentVolume = level;
		return *this;
	}

	void SoundMac::idle() 
	{
		snd_MacAudioData* sndData = static_cast<snd_MacAudioData*>(mMacDataRef);
		if (mPlaying) {
			// do volume fading if needed
			uint32 t = OS::getMilliseconds();
			if (mDieAt && t > mDieAt) {
				stop();  // this unregisters us from the Sound Manager and does a release which deletes this
			}
			if (mStartedFadeMs) {
				uint32 msElapsed = t - mStartedFadeMs;
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
			if (sndData->mDone) {
				if (sndData->mError != noErr) {
					DEBUG_ONLY( OS::_DOUT("Sound::idle [%p] detected Core Audio error [%d], posting soundEvent_FailedToPlay", this, sndData->mError); )
					stop();
					SoundEventInfo soundInfo;
					soundInfo.sound = this;
					soundInfo.eventCode = soundEvent_FailedToPlay;
					postEvent(eventType_SoundEvent, &soundInfo);
				}
				if (mDieAt == 0) {
					// post a done (or looping) event
					SoundEventInfo soundInfo;
					soundInfo.sound = this;
					if (!mLoopSound) {
						SOUND_DEBUG_ONLY( OS::_DOUT("Sound::idle [%p] sound finished, posting soundEvent_DonePlaying", this); )
						stop();
						soundInfo.eventCode = soundEvent_DonePlaying;
					} else {
						SOUND_DEBUG_ONLY( OS::_DOUT("Sound::idle [%p] sound reached end and looping, posting soundEvent_Looping", this); )
						addRef();  // so we won't get deleted when we stop
						stop();
						start();
						release();
						soundInfo.eventCode = soundEvent_Looping;
					}
					postEvent(eventType_SoundEvent, &soundInfo);
				}
			}
		}
	}
		

	bool snd_IsFormatVBR(const AudioStreamBasicDescription& au) // variable bit rate
	{
	    return (au.mBytesPerPacket == 0 || au.mFramesPerPacket == 0);
	}

    Sound* 
    Sound::createSoundFromData(const char* soundName, char* soundData, long soundDataLen) {
		// Find extension of filename
		std::string filename(soundName);
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
			return 0;
		}
		std::string extension = filename.substr(pos+1, filename.length());
		
		// Instantiate Sound based on filename extension.
		Sound *snd = 0;
		// one single kind of sound player is handling all sounds on the
		// mac through quicktime
		SoundMac* sndMac = new SoundMac(SoundManager::getSingletonInstance(), extension.c_str());
		sndMac->createFromData(soundData, soundDataLen);
		snd = sndMac;
		return snd;
    }

    Sound* 
    Sound::createSoundFromFile(const char* soundFileName) {
		// For now we use ".ref" as the extension to get around restriction
		// eventually we need to support reading any type of sound directly from
		// zip file
		SoundMac* sndMac = new SoundMac(SoundManager::getSingletonInstance(), "ref");
		sndMac->createFromFile(soundFileName);
		return sndMac;
    }

} // end namespace pdg

