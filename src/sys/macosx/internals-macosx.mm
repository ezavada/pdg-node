// -----------------------------------------------
// internals-macosx.mm
// 
// Mac OS X Objective C stuff
//
// Written by Ed Zavada, 2014
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

#include "internals-macosx.h"

#include <cstring>

#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSError.h>
#import <Foundation/NSDictionary.h>

bool macosx_getApplicationSupportDirectory(const char* appName, char* buffer, int size) {
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSArray* possibleDirs = NSSearchPathForDirectoriesInDomains (
	   NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* appSupportDir = nil;
    bool found = false;
    if (buffer && size > 0) {
    	buffer[0] = 0;
    }
 
    if ([possibleDirs count] >= 1) {
        // Use the first directory (if multiple are returned)
        appSupportDir = [possibleDirs objectAtIndex:0];
    }
 
    // If a valid app support directory exists, add the
    // app's bundle ID to it to specify the final directory.
    if (appSupportDir) {
    	NSString* dirPath = nil;
        NSString* appBundleID = [[NSBundle mainBundle] bundleIdentifier];
        if ([appBundleID length] == 0) {
			appBundleID = [[NSString alloc] initWithUTF8String:appName];
        }
        if ([appBundleID length] > 0) {
			dirPath = [appSupportDir stringByAppendingPathComponent:appBundleID];
			if (buffer && size > 0) {
				std::strncpy(buffer, [dirPath UTF8String], size);
				buffer[size-1] = 0;
			}
			found = true;
		}
    }
    [pool drain];
    return found;
}

void macosx_getSystemVersion( int &major, int &minor, int &bugfix ) { // sensible default static int mMajor = 10; static int mMinor = 8; static int mBugfix = 0;
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSString* versionString = [[NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"] objectForKey:@"ProductVersion"];
	NSArray* versions = [versionString componentsSeparatedByString:@"."];
	if ( versions.count >= 1 ) {
		major = [[versions objectAtIndex:0] integerValue];
	} else {
		major = 0;
	}
	if ( versions.count >= 2 ) {
		minor = [[versions objectAtIndex:1] integerValue];
	} else {
		minor = 0;
	}
	if ( versions.count >= 3 ) {
		bugfix = [[versions objectAtIndex:2] integerValue];
	} else {
		bugfix = 0;
	}
    [pool drain];
}

namespace pdg {

const char* os_getPlatformErrorMessage(long err) {
	static char msgBuf[1024];
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSError* nserr = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:NULL];
	NSString* errStr = [nserr localizedDescription];
	std::strncpy(msgBuf, [errStr UTF8String], 1024);
	msgBuf[1023] = 0;
    [pool drain];
    return msgBuf;
}

}
