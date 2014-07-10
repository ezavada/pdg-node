// -----------------------------------------------
// internet.h
// 
// internet specific functionality
//
// Written by Ed Zavada, 2006
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


#ifndef PDG_INTERNET_H_INCLUDED
#define PDG_INTERNET_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/sys/global_types.h"

#include <cstdio>

namespace pdg {

// context is the userContext passed in when httpGetEx or httpPostEx was called.
typedef bool (*ReceiveFunc) (int status, int httpResultCode, char* dataPtr, size_t dataSize, void* context);

class Internet {
public:

    // retrieve the raw contents of a web page from a URL via HTTP GET
    // this call blocks until the data is all downloaded or until the timeout milliseconds have passed
    // returns a malloc'd block with the result, must be freed when you are done with it
    static char* httpGet(const char* url, uint32 timeoutMs = 60000);
    
    
    // retrieve the raw contents of a web page from a URL via HTTP POST
    // this call blocks until the data is all downloaded or until the timeout milliseconds have passed
    // postData is the data to be sent to the server for the POST, must be a nul terminated C string (UTF-8)
    // returns a malloc'd block with the result, must be freed when you are done with it
    static char* httpPost(const char* url, const char* postData = 0, uint32 timeoutMs = 60000);


	// --------------------------------
	// Advanced functions
	// --------------------------------
	
	// callback status codes for ReceiveFunc
	enum {
		status_Error = -1, 		// some kind of error occured, see httpResultCode in callback for error code
		status_Complete = 0, 	// the download has completed, httpResultCode will should indicate a success (20x)
		status_BufferFull = 1,	// the buffer is full but the download is not complete, copy contents before returning from callback
		status_Timeout = 2		// the buffer wasn't full, but the timeout time has passed without data arriving
	};
	
    // retrieve the raw contents of a web page from a URL via HTTP GET
    // this call just starts the download process
    // data is downloaded into the buffer specified, up to buffSize length
    // callbacks will be generated to the ReceiveFunction specified
    // userContext will be passed back to your ReceiveFunction
    // timeOut is the time to wait without any communication from the server before timeout.
    // maxBps is the maximumBytesPerSecond to consume in download speed
    // returns zero on success, or an error code on failure
    static int httpGetEx(const char* url, char* buffer, size_t buffSize,  
    					ReceiveFunc func, void* userContext = 0,
    					uint32 timeoutMs = 60000, uint32 maxBps = 0xffffffff);
    
    
    // retrieve the raw contents of a web page from a URL via HTTP POST
    // data is downloaded into the buffer specified, up to buffSize length
    // callbacks will be generated to the ReceiveFunction specified
    // userContext will be passed back to your ReceiveFunction
    // timeOut is the time to wait without any communication from the server before timeout.
    // maxBps is the maximumBytesPerSecond to consume in download speed
    // returns zero on success, or an error code on failure
    static int httpPostEx(const char* url, char* buffer, size_t buffSize, 
    					ReceiveFunc func, void* userContext = 0,
    					const char* postData = 0, size_t postDataSize = 0, 
    					uint32 timeoutMs = 60000, uint32 maxBps = 0xffffffff);

};

} // end namespace pdg

#endif // PDG_OS_H_INCLUDED

