// -----------------------------------------------
// os.h
// 
// os specific functionality
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


#ifndef PDG_OS_H_INCLUDED
#define PDG_OS_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include "pdg/msvcfix.h"
#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"

#include <vector>

#ifdef COMPILER_GCC
#include <cwchar>
#endif // COMPILER_GCC

#include <cstring>
#include <string>

#ifdef _PREFIX_
    // for use with Microsoft's PREfix static code analysis tool
    #include <assert.h>
#endif // _PREFIX_

#define DEBUG_LINE_LEN 500

#ifndef CHECK_NEW
    #define CHECK_NEW(ptr, classname)
#endif

namespace pdg {

struct FindDataT {
    enum { 
        MAX_NODE_NAME_SIZE = 256
    };
	bool isDirectory;
    char nodeName[MAX_NODE_NAME_SIZE];
    void* privateData;
};

typedef std::basic_string<utf16char> utf16string;

class OS {
public:

	// check to see if a particular key is down based on its virtual key code, as would be 
	// returned in a KeyInfo structure for an eventType_KeyDown or eventType_KeyUp
	// On some platforms controller/joystick buttons might be mapped this way too
	static bool isRawKeyDown(long keyCode);

	// check to see if a particular key is down based on its unicode value, as would be returned 
	// in a  KeyInfo structure for an eventType_KeyDown or eventType_KeyUp
	static bool isKeyDown(uint16 utf16Char);

	// check to see if a particular mouse button is down (0 - left, 1 - right, 2 - middle, etc...)
	// On some platforms controller/joystick buttons might be mapped this way too
	static bool isButtonDown(int buttonNumber);
	
	// get a human readable symbol or name for a particular key, given its virtual key code
	static const char* getKeyName(long keyCode);

	// get a human readable symbol or name for a particular button, given its number
	static const char* getButtonName(int buttonNumber);

	// returns the orientation of the device in radians
	// 0 roll means level along the axis best described as tilting left or right toward the user
	// 0 pitch means not tilted toward nor away from the user
	// 0 yaw means not twisted right nor left from the user
	// if absolute is true, then yaw is compass direction from north around vertical axis, 
	// pitch is rotation around E -> W axis, and roll is rotation around N -> S axis 
	static void getDeviceOrientation(float& roll, float& pitch, float& yaw, bool absolute = false);

    // start a new file search
    // returns true if a file was found, false if no file was found 
    // inFindName can contain wildcards (ie: *.txt, or f1?.dat)
    // if inFindName is a full path, it must be in unix format ("/" must must be path separator);
    // colon and backslash ":\" are both illegal characters
    static bool findFirst(const char* inFindName, FindDataT& ioFindData);
    
    // continue an established file search
    // returns true if a file was found, false if no file was found 
    static bool findNext(FindDataT& ioFindData);
    
    // cleans up after a file search, should always be called when done, even if no file was found on findFirst
    static void findClose(FindDataT& inFindData); 
    
    // returns the path to the application's data directory, in standard Unix format, ie:
    // "/mycomputer/windows/specialapps"
    // this value could have "/*" appended to find everything in the application's directory using the find
    // methods above
    // this is where applications should save data files lacking override from the user
    static const char* getApplicationDataDirectory();

    // returns the path to the directory in which the application resides, in standard Unix format, ie:
    // "/mycomputer/windows/specialapps"
    // this value could have "/*" appended to find everything in the application's directory using the find
    // methods above
	// on many platforms this directory will not be writable
	// on platforms like Mac OS X that have a "bundle" concept, this will point to the location of the
	// bundle, not the executable file within the bundle
    static const char* getApplicationDirectory();

    // returns the path to the application's resource directory, in standard Unix format, ie:
    // "/mycomputer/windows/specialapps/ResourceFiles"
    // this value could have "/*" appended to find everything in the application's directory using the find
    // methods above
	// if PDG_NO_ZIP flag was used at compile time, this will point inside a "ResourceFiles" directory
	// and the individual resource files can be found there
	// if PDG_NO_ZIP was *not* used, then this points to the directory where the zip files that hold the
	// applications resource files are found: Mac OS X - {appname}.app/Contents/MacOS/Resources/
    static const char* getApplicationResourceDirectory();
	
	// Deletes a file. Returns true for success, false for failure.
	static bool deleteFile(const char* inFileName);

	// Renames a file. Returns true for success, false for failure.
	static bool renameFile(const char* inFileName, const char* inNewFileName);
	
	// Get a millisecond time stamp
    static unsigned long  getMilliseconds();
	
	// Get the position of the mouse. 
	// On a multi-touch interface, mouseNumber specifies which pointer/finger, in order they started touching
	// On a device with multiple pointing devices, mouseNumber represents individual pointers in an arbitrary fixed order
	// If mouseNumber is greater than the number of pointing devices attached (or fingers down), the
	// location of the primary pointing device is returned (ie: same as getMouse(0) )
    static pdg::Point     getMouse(int mouseNumber = 0);

    // this random is used for things like response delay timers and so forth that do not determine flow of gameplay.
    // for things that do determine flow of gameplay, such as the sort of stuff you would want to do repeatably in a
    // tutorial game or scripted sequence, use OS::gameCriticalRandom()
    static unsigned long  rand();

    // this random is used for things that determine flow of gameplay, such as dice rolls and card shuffles.
    // ie: the sort of stuff you would want to do repeatably in a tutorial game or scripted sequence. For
    // things that use OS::gameCriticalRandom()
    // IMPORTANT: random numbers that are generated in response to things the user might or might not do without
    // concious choice should use the regular OS::rand(). Otherwise even with the same seed the sequence will 
    // not be repeatable because the user can't repeatably do the same actions.
    static unsigned long gameCriticalRandom();
    
    // seeds both the game critical random numbers and the routine random numbers
    static void           srand(unsigned long seed);
    
    // exit the app next time we go through the event loop
    static void           exit(int errCode);

    // fill a string buffer the with hex and ascii representations in a hex dump format of the data given by inBuf and inBufSize
	// outBufSize should be about (4 * inBufSize) + (6 * inBufSize/bytesPerLine) + (4 * bytesPerLine) + 32 to hold all
	// the output, or it will be trucated
    static void           binaryDump(char *outBuf, int outBufSize, const char *inBuf, int inBufSize, int bytesPerLine=20, int hiliteStart = -1, int hiliteCount = 0);

 #ifdef DEBUG
	static void			  _DOUT(const char * fmt, ...);
	static void           _DEBUGGER(const char* str=0);      // invoke the debugger
	
 #ifdef _PREFIX_
    // for use with Microsoft's PREfix static code analysis tool
    #define DEBUG_ASSERT(cond, msg) assert(cond)
	#define DEBUG_BREAK(msg) exit(1)
	#define CHECK_PTR(ptr, block, block_size) assert( !( ((char*)(ptr)<(char*)(block))||((char*)(ptr)>=(char*)((char*)(block)+(block_size))) ) )
 #else
 	// allow DEBUG_ASSERT to be overridden
    #ifndef DEBUG_ASSERT
		#define DEBUG_ASSERT(cond, msg) if (!(cond)) { char str_[DEBUG_LINE_LEN]; std::snprintf(str_, DEBUG_LINE_LEN, "ASSERT FAILED: %s:%d (%s) %s", __FILE__, __LINE__, #cond, #msg); str_[DEBUG_LINE_LEN-1] = 0; ::pdg::OS::_DOUT(str_); ::pdg::OS::_DEBUGGER(str_); }
	#endif
 	// allow DEBUG_BREAK to be overridden
    #ifndef DEBUG_BREAK
		#define DEBUG_BREAK(msg) { char str_[DEBUG_LINE_LEN]; std::snprintf(str_, DEBUG_LINE_LEN, "BREAK: %s:%d %s", __FILE__, __LINE__, #msg); str_[DEBUG_LINE_LEN-1] = 0; ::pdg::OS::_DOUT(str_); ::pdg::OS::_DEBUGGER(str_); }
	#endif
 	// allow CHECK_PTR to be overridden
    #ifndef CHECK_PTR
		#define CHECK_PTR(ptr, block, block_size) if (((char*)(ptr)<(char*)(block))||((char*)(ptr)>=(char*)((char*)(block)+(block_size)))) { ::pdg::OS::_DOUT("PTR ERROR: %s:%d [%p] is outside block [%p] len [%p]", __FILE__, __LINE__, ptr, block, block_size); ::pdg::OS::_DEBUGGER(); }
	#endif
 #endif // _PREFIX_
    #define MAKE_STRING_BUFFER_SAFE(buffer, buffer_size) CHECK_PTR(&buffer[std::strlen(buffer)], buffer, buffer_size); buffer[(buffer_size) - 1] = 0;
	#define DEBUG_PRINT pdg::OS::_DOUT
	#define CHECK_PTR_WRITE(ptr, bytes, block, block_size) CHECK_PTR(ptr, block, block_size); CHECK_PTR(ptr+bytes-1, block, block_size)
 #else
    #define DEBUG_ASSERT(cond, msg) if (!(cond)) {}
    #define DEBUG_BREAK(msg)
    #define CHECK_PTR(ptr, block, block_size)
	#define CHECK_PTR_WRITE(ptr, bytes, block, block_size)
    #define MAKE_STRING_BUFFER_SAFE(buffer, buffer_size) buffer[(buffer_size) - 1] = 0;
	#define DEBUG_PRINT
 #endif // DEBUG

    enum {
        err_NONE,
        err_TRUNCATION,  // a fragment of a multibyte character was found
        err_OVERFLOW,    // not enough space in the target buffer
        err_ILLEGAL      // an illegal UTF-16 character was found
    };

    static void utf8to16(utf16string& dst, const std::string& src);		// Use this for whole string
	static void utf8to16(utf16string& dst, const std::string& src, int cbLen);	// Use this if you want to specify len
    static void utf16to8(std::string& dst, utf16string& src); 

    // you probably won't want to use these directly, they are mainly here in case you need to prevent the memory allocations that
    // would happen if you use the std::string <--> utf16string methods above
    static int utf8to16(utf16char* ioUTF16dst, long &ioDstBytes, const char* inUTF8src, long inSrcLen, long &outCharsConverted);
    static int utf16to8(char* ioUTF8dst, long &ioDstBytes, const utf16char* inUTF16src, long inSrcLen, long &outCharsConverted);

	static std::string makeCanonicalPath(const char* fromPath, bool resolveSimLinks = true);

};

} // end namespace pdg

#endif // PDG_OS_H_INCLUDED

