// -----------------------------------------------
// internet-macosx.cpp
// 
// Mac OS X implementation of internet functionality
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


#include "pdg_project.h"

#include "pdg/sys/internet.h"

#include <CoreFoundation/CoreFoundation.h>
#define uint32 MacUInt32
#include <Carbon/Carbon.h>
#undef uint32

namespace pdg {


//	These are the network events we are interested in.
//	We set ReadStreamClientCallBack() as the notifier proc to handle these events.
static const CFOptionFlags kNetworkEvents = kCFStreamEventOpenCompleted |
                                            kCFStreamEventHasBytesAvailable |
                                            kCFStreamEventEndEncountered |
                                            kCFStreamEventErrorOccurred;


char* 
Internet::httpGet(const char* url, uint32 timeoutMs) {
	CFStringRef					rawCFString;
	CFStringRef					normalizedCFString;
	CFStringRef					escapedCFString;
	static	CFURLRef			urlRef;

	FIXME("Eliminate dependency on Carbon");
	static	EventLoopTimerUPP	networkTimeoutTimerUPP;

	CFHTTPMessageRef			messageRef		= NULL;
	CFReadStreamRef				readStreamRef	= NULL;
	CFStreamClientContext		ctxt			= { 0, (void*)NULL, NULL, NULL, NULL };

	if ( url != NULL )
	{
		
		if ( strlen( url ) < 12 )   goto Bail;
		if ( urlRef != NULL ) CFRelease( urlRef );
		
		//	We first create a CFString in a standard URL format, for instance spaces, " ", become "%20" within the string
		//	To do this we normalize the URL first, then create the escaped equivalent
		rawCFString		= CFStringCreateWithCString( NULL, url, CFStringGetSystemEncoding() );										
		if ( rawCFString == NULL ) goto Bail;
		normalizedCFString	= CFURLCreateStringByReplacingPercentEscapes( NULL, rawCFString, CFSTR("") );							
		if ( normalizedCFString == NULL ) goto Bail;
		escapedCFString	= CFURLCreateStringByAddingPercentEscapes( NULL, normalizedCFString, NULL, NULL, kCFStringEncodingUTF8 );	
		if ( escapedCFString == NULL ) goto Bail;

		urlRef= CFURLCreateWithString( kCFAllocatorDefault, escapedCFString, NULL );

		CFRelease( rawCFString );
		CFRelease( normalizedCFString );
		CFRelease( escapedCFString );
		if ( urlRef == NULL ) goto Bail;
	}
    
	messageRef = CFHTTPMessageCreateRequest( kCFAllocatorDefault, CFSTR("GET"), urlRef, kCFHTTPVersion1_1 );
	if ( messageRef == NULL ) goto Bail;

	// Create the stream for the request.
	readStreamRef	= CFReadStreamCreateForHTTPRequest( kCFAllocatorDefault, messageRef );
	if ( readStreamRef == NULL ) goto Bail;
   
	//	There are times when a server checks the User-Agent to match a well known browser.  This is what Safari used at the time the sample was written
	//CFHTTPMessageSetHeaderFieldValue( messageRef, CFSTR("User-Agent"), CFSTR("Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en-us) AppleWebKit/125.5.5 (KHTML, like Gecko) Safari/125")); 

    if ( CFReadStreamSetProperty(readStreamRef, kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue) == false )
		goto Bail;

	// Set the client notifier
	if ( CFReadStreamSetClient( readStreamRef, kNetworkEvents, ReadStreamClientCallBack, &ctxt ) == false )
		goto Bail;
    
	// Schedule the stream
	CFReadStreamScheduleWithRunLoop( readStreamRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
    
	// Start the HTTP connection
	if ( CFReadStreamOpen( readStreamRef ) == false )
	    goto Bail;

	//	Set up a watch dog timer to terminate the download after 5 seconds
	if ( networkTimeoutTimerUPP == NULL )	networkTimeoutTimerUPP	= NewEventLoopTimerUPP( NetworkTimeoutTimerProc );
	if ( gNetworkTimeoutTimerRef != NULL ) RemoveEventLoopTimer( gNetworkTimeoutTimerRef );
	InstallEventLoopTimer( GetCurrentEventLoop(), kEventDurationSecond * 5, 0, networkTimeoutTimerUPP, (void*)readStreamRef, &gNetworkTimeoutTimerRef );

	if ( messageRef != NULL ) CFRelease( messageRef );
            return;
	
Bail:
	if ( messageRef != NULL ) CFRelease( messageRef );
	if ( readStreamRef != NULL )
    {
        CFReadStreamSetClient( readStreamRef, kCFStreamEventNone, NULL, NULL );
	    CFReadStreamUnscheduleFromRunLoop( readStreamRef, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
	    CFReadStreamClose( readStreamRef );
        CFRelease( readStreamRef );
    }
	return;
}
    
    
// retrieve the raw contents of a web page from a URL via HTTP POST
// this call blocks until the data is all downloaded or until the timeout milliseconds have passed
// postData is the data to be sent to the server for the POST, must be a nul terminated C string (UTF-8)
// returns a malloc'd block with the result, must be freed when you are done with it
char* 
Internet::httpPost(const char* url, const char* postData, uint32 timeoutMs) {
}


	
// retrieve the raw contents of a web page from a URL via HTTP GET
// this call just starts the download process
// data is downloaded into the buffer specified, up to buffSize length
// callbacks will be generated to the ReceiveFunction specified
// userContext will be passed back to your ReceiveFunction
// timeOut is the time to wait without any communication from the server before timeout.
// maxBps is the maximumBytesPerSecond to consume in download speed
// returns zero on success, or an error code on failure
int 
Internet::httpGetEx(const char* url, char* buffer, size_t buffSize,  
    					ReceiveFunc func, void* userContext,
    					uint32 timeoutMs, uint32 maxBps)
{
	return -1; // not implemented
}
    
    
// retrieve the raw contents of a web page from a URL via HTTP POST
// data is downloaded into the buffer specified, up to buffSize length
// callbacks will be generated to the ReceiveFunction specified
// userContext will be passed back to your ReceiveFunction
// timeOut is the time to wait without any communication from the server before timeout.
// maxBps is the maximumBytesPerSecond to consume in download speed
// returns zero on success, or an error code on failure
int 
Internet::httpPostEx(const char* url, char* buffer, size_t buffSize, 
    					ReceiveFunc func, void* userContext,
    					const char* postData, size_t postDataSize, 
    					uint32 timeoutMs, uint32 maxBps) 
{
	return -1; // not implemented
}


} // end namespace pdg


