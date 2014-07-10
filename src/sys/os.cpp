// -----------------------------------------------
// os.cpp
// 
// OS independent implementation of common system functions
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2012, Dream Rock Studios, LLC
// Portions Copyright (c) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura
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
//
// Source code for Mersenne Twister random number generation: 
//   ------------------------------------------
//   by Takuji Nishimura and Makoto Matsumoto.
//
//   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
//   All rights reserved.                          
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//     1. Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//
//     2. Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//
//     3. The names of its contributors may not be used to endorse or promote 
//        products derived from this software without specific prior written 
//        permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//   Any feedback is very welcome.
//   http://www.math.keio.ac.jp/matumoto/emt.html
//   email: matumoto@math.keio.ac.jp
//   ------------------------------------------


#include "pdg_project.h"

#include "pdg/msvcfix.h"

#include "pdg/sys/os.h"

#include "ConvertUTF.h"
#include "internals.h"
#include "pdg-main.h"

#ifndef STADIUM_SERVER
    #include "pdg/sys/mutex.h"
    #define RAND_MUTEX      pdg::AutoMutex mutex(&sRandFuncMutex)
    static pdg::Mutex sRandFuncMutex; // used to ensure multiple threads cannot cause out of bounds array access
#endif // !STADIUM_SERVER

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

#ifndef COMPILER_MSVC
#include <cxxabi.h>

/* Use this to demangle a symbol name into a human readable name.
   Can be called from straight C, even for C++ symbols. */
extern "C" const char* demangleSymbol(const char* mangled_name) {
  int  status;
  static char realname[256];
  size_t bufSize = 255;
  
  realname[0] = 0;
  abi::__cxa_demangle(mangled_name, (char*)realname, &bufSize, &status);
  realname[255] = 0;
  return realname;
}

#include <execinfo.h>  // used only for the functionPtrToName() call

/* Use this to get a function name from a function Pointer */
extern "C" const char* functionPtrToName(void* ptr, bool nameOnly) {
	// Unix/GCC specific -- to do this on Windows see the following:
	// http://ivbel.blogspot.com/2012/02/how-to-get-functions-name-from.html
  	static char buffer[256];
	void *funptr = ptr;
	char* btsyms = backtrace_symbols(&funptr, 1)[0];
	char* funName = btsyms;
	while (*funName != 0) {
		if (*funName == '_') break;
		funName++;
	}
	char* p = funName;
	while (*p != 0) {
		if (*p == ' ') {
			*p = 0;
			break;
		}
		p++;
	}
	const char* demangledName = demangleSymbol(funName);
	strncpy(buffer, demangledName, 255);
	char* bp = buffer;
	if (nameOnly) {
		// move forward till we find parens, they become end of string
		p = buffer;
		while (*p != 0) {
			if (*p == '(') {
				*p = 0;
				break;
			}
			p++;
		}
		// to eliminate namespaces, move backwards till we find ":" or buffer start 
		while (p > buffer) {
			p--;
			if (*p == ':') {
				bp = p + 1;
				break;
			}
		}
	}
	return bp;
}
#else
extern "C" const char* demangleSymbol(const char* mangled_name) {
	// TODO: implement name demangling for MSVC
	return mangled_name;
}
#endif // !COMPILER_MSVCs

namespace pdg {


extern bool  gExit;
extern int   gExitCode;

std::string gApplicationDirectory;
std::string gApplicationDataDirectory;
std::string gApplicationResourceDirectory;

bool 
os_isAbsolutePath(const char* path) {
  #ifdef PLATFORM_WIN32
  	if (path[0] == '"') path++; // ignore leading quotes
    if ((path[0] == 0) || (path[1] == 0) || (path[2] == 0)) return false;
    return (std::isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/'));
  #else
    return (path[0] == '/');
  #endif
    return false;
}

	
void 
os_setApplicationDirectory(const char* dir) {
	gApplicationDirectory = dir;
}

void
os_setApplicationDataDirectory(const char* dir) {
	gApplicationDataDirectory = dir;
}

void
os_setApplicationResourceDirectory(const char* dir) {
	gApplicationResourceDirectory = dir;
}

const char* 
OS::getApplicationDirectory() {
    return gApplicationDirectory.c_str();
}

const char* 
OS::getApplicationDataDirectory() {
    return gApplicationDataDirectory.c_str();
}
	
const char* 
OS::getApplicationResourceDirectory() {
    return gApplicationResourceDirectory.c_str();
}


bool
OS::isButtonDown(int buttonNumber) {
  #ifndef PDG_NO_GUI
	return main_getButtonState(buttonNumber);
  #else
	return false;
  #endif
}

bool 
OS::isRawKeyDown(long keyCode) {
  #ifndef PDG_NO_GUI
	return main_getRawKeyState(keyCode);
  #else
	return false;
  #endif
}

bool 
OS::isKeyDown(uint16 utf16Char) {
  #ifndef PDG_NO_GUI
	return main_getKeyState(utf16Char);
  #else
	return false;
  #endif
}

Point OS::getMouse(int mouseNumber) {
  #ifndef PDG_NO_GUI
    return main_getMousePos(mouseNumber);
  #else
	return Point(-1,-1);
  #endif
}

#ifndef STADIUM_SERVER

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */


void
OS::getDeviceOrientation(float& roll, float& pitch, float& yaw, bool absolute) {
    platform_getDeviceOrientation(&roll, &pitch, &yaw, absolute);
}

// this random is used for things like response delay timers and so forth that do not determine flow of gameplay.
// for things that do determine flow of gameplay, such as the sort of stuff you would want to do repeatably in a
// tutorial game or scripted sequence, use OS::gameCriticalRandom()
unsigned long  
OS::rand() {
    return (unsigned long) std::rand();
}

// this random is used for things that determine flow of gameplay, such as dice rolls and card shuffles.
// ie: the sort of stuff you would want to do repeatably in a tutorial game or scripted sequence. For
// things that use OS::gameCriticalRandom()
// IMPORTANT: random numbers that are generated in response to things the user might or might not do without
// concious choice should use the regular OS::rand(). Otherwise even with the same seed the sequence will 
// not be repeatable because the user can't repeatably do the same actions.
unsigned long
OS::gameCriticalRandom() {
  #ifdef PDG_NO_MERSENNE_TWISTER
    return (unsigned long) std::rand();
  #else
    RAND_MUTEX;
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if srand() has not been called, */
            srand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];
    
    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    
    return y;
  #endif // !PDG_NO_MERSENNE_TWISTER
}

void
OS::srand(unsigned long seed) {
    std::srand(seed);   // this seeds the non-critical random number generation
  #ifndef PDG_NO_MERSENNE_TWISTER
    // following seeds critical random number generation
    RAND_MUTEX;
    mt[0]= seed & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
        (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
  #endif  // ! PDG_NO_MERSENNE_TWISTER
}
#endif // !STADIUM_SERVER

void
OS::exit(int errCode) {
    gExit = true;
    gExitCode = errCode;
}

void 
OS::binaryDump(char *outBuf, int outBufSize, const char *inBuf, int inBufSize, int bytesPerLine, int hiliteStart, int hiliteCount) {
// do a nicely formatted binary dump of buf to string
	int hexBufLen = 3*bytesPerLine+3;  // allow 2 extra bytes for hilite
    char* hexbuf = (char*) std::malloc(hexBufLen);
    if (!hexbuf) return;
    int ascBufLen = bytesPerLine+1;
    char* ascbuf = (char*) std::malloc(ascBufLen);
    if (!ascbuf) {
        std::free(hexbuf);
        return;
    }
    unsigned char *p;
    int n, total;
    int ch;
    int hiliteEnd = (hiliteCount > 0) ? hiliteStart + hiliteCount : 0;
    char hiliteChar = (hiliteEnd) ? '[' : '>';

    total = 0;
    p = (unsigned char *)inBuf;
    outBuf[0] = 0;  // start with a clean buffer

	const char truncMsg[22] = "<HEX DUMP TRUNCATED>";
	int truncMsgLen = std::strlen(truncMsg);
	int offset = 0;

	int outPos = 0;
	
    while (total < inBufSize)
    {
		int ex = 0;
        for (n = 0; total < inBufSize && n < bytesPerLine; total++, n++)
        {
        	if ((hiliteStart != -1) && (total == hiliteStart)) {
            	hexbuf[n*3 + ex] = hiliteChar;
            	ex += 1;
            	if (hiliteEnd == 0) {
            		hexbuf[n*3 + ex] = hiliteChar;
            		ex += 1;
            	}
        	}
            ch = *p++;
			CHECK_PTR_WRITE(hexbuf + n*3 + ex, 4, hexbuf, hexBufLen); // 3 + NUL
			if (hiliteEnd && (total == (hiliteEnd -1))) {
            	std::sprintf(hexbuf + n*3 + ex, "%02X] ", ch);
            	ex += 1;
            } else {
            	std::sprintf(hexbuf + n*3 + ex, "%02X ", ch);
            }
			CHECK_PTR(ascbuf + n, ascbuf, ascBufLen);
            ascbuf[n] = std::isprint(ch) ? ch : '.';
        }
		CHECK_PTR(ascbuf + n, ascbuf, ascBufLen);
        ascbuf[n] = '\0';

		int lineLen = std::strlen(hexbuf) + std::strlen(ascbuf);
		if (outPos + lineLen + 5 > outBufSize) {
			if (outPos + truncMsgLen > outBufSize) {
				offset = outBufSize - truncMsgLen - 1;
			} else {
				offset = outPos;
			}
			CHECK_PTR_WRITE(outBuf + offset, truncMsgLen + 1, outBuf, outBufSize);
			std::strcpy(&outBuf[offset], truncMsg);
			break; // jump out of the loop
		}
		if ( (n < bytesPerLine) || (total == inBufSize) ) {
			for( ch = n; ch < bytesPerLine; ch++ ) {
			    CHECK_PTR_WRITE(hexbuf + ex + ch*3, 4, hexbuf, hexBufLen); // 3 + NUL
				std::strcpy( hexbuf + ex + ch*3, "   " );
			}
			CHECK_PTR_WRITE(&outBuf[outPos], lineLen + 4, outBuf, outBufSize) // " | " + NUL
	        std::sprintf(&outBuf[outPos], "%s | %s", hexbuf, ascbuf);
	        outPos += lineLen + 3;
		} else {
			CHECK_PTR_WRITE(&outBuf[outPos], lineLen + 5, outBuf, outBufSize) // " | " + "\n" + NUL
			std::sprintf(&outBuf[outPos], "%s | %s\n", hexbuf, ascbuf);
	        outPos += lineLen + 4;
		}

    }
    std::free(hexbuf);
    std::free(ascbuf);
}


void 
OS::utf8to16(utf16string& dst, const std::string& src)
{
	int cbLen = src.length();
	utf8to16( dst, src, cbLen);
}


void 
OS::utf8to16(utf16string& dst, const std::string& src, int cbLen)
{
    long bufsize = (cbLen+1) * sizeof(utf16char) * 2;    // bigger buffer than necessary for oddball UTF16 that takes multiple chars
	utf16char* theText = (utf16char*) std::malloc(bufsize); 
	if (!theText) return;
    long dstBytes = bufsize - sizeof(utf16char); // subtract space for nul terminator
    long converted;
	long srcBytes = (cbLen > 0) ? cbLen : std::strlen(src.c_str());
    OS::utf8to16(theText, dstBytes, src.c_str(), srcBytes, converted);
	//OS::_DOUT( "$$$ bufsize[%d] dstBytes[%d] converted[%d], buf[%lx]", bufsize, dstBytes, converted, theText );
    CHECK_PTR(&theText[converted], theText, bufsize);
    theText[converted] = 0; // must nul terminate
    dst.assign(theText);
	std::free(theText);
}

void 
OS::utf16to8(std::string& dst, utf16string& src)
{
    long bufsize = (src.length()+1) * sizeof(char) * 6;    // buffer must accomodate up to 6 bytes of utf8 for each char 
	char* theText = (char*) std::malloc(bufsize); 
	if (!theText) return;
    long dstBytes = bufsize - sizeof(char); // subtract space for nul terminator
    long converted;
    CHECK_PTR(&theText[dstBytes], theText, bufsize);
    OS::utf16to8(theText, dstBytes, src.c_str(), src.length(), converted);
    theText[dstBytes] = 0; // must nul terminate, use dstBytes because converted is utf8 char count, and a utf8 char can be multiple c chars
    dst.assign(theText);
	std::free(theText);
}

int 
OS::utf8to16(utf16char* ioUTF16dst, long &ioDstBytes, const char* inUTF8src, long inSrcLen, long &outCharsConverted)
{
    UTF16* targetStart = (UTF16*)ioUTF16dst;
    UTF16* targetEnd = (UTF16*)((char*)ioUTF16dst + ioDstBytes);
    const UTF8* sourceStart = (UTF8*)inUTF8src;
    UTF8* sourceEnd = (UTF8*)inUTF8src + inSrcLen;
    ConversionResult result = ConvertUTF8toUTF16 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
    ioDstBytes = (char*)targetStart - (char*)ioUTF16dst;  // targetStart will be advanced, return the number of bytes added to buffer
    outCharsConverted = ioDstBytes/sizeof(UTF16);
	return result;
}

int 
OS::utf16to8(char* ioUTF8dst, long &ioDstBytes, const utf16char* inUTF16src, long inSrcLen, long &outCharsConverted) 
{
    UTF8* targetStart = (UTF8*)ioUTF8dst;
    UTF8* targetEnd = (UTF8*)ioUTF8dst + ioDstBytes;
    const UTF16* sourceStart = (UTF16*)inUTF16src;
    UTF16* sourceEnd = (UTF16*)inUTF16src + inSrcLen;
    ConversionResult result = ConvertUTF16toUTF8 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
    ioDstBytes = targetStart - (UTF8*)ioUTF8dst;  // targetStart will be advanced, return the number of bytes added to buffer
    outCharsConverted = sourceStart - (UTF16*)inUTF16src;
	return result;
}


} // end namespace pdg

