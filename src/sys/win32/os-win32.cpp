// -----------------------------------------------
// os-win32.cpp
//
// Windows implementation of common system functions
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

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/os.h"
#include "pdg/sys/log.h"

//#define PDG_DEBUG_OUT_TO_LOG

#include "ConvertUTF.h"
#include "internals.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <time.h>
#include <assert.h>
#include <io.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif

#define WinAPI

#ifdef PDG_VS_NEED__IMP__VSNPRINTF
extern "C" int _imp__vsnprintf(char * s, size_t n, const char * fmt, va_list arg) { 
	return std::vsnprintf(s, n, fmt, arg); 
}
#endif

namespace pdg {


struct PrivateFindData {
    WinAPI::_finddata_t findData;
    long                findRef;
};

std::string os_makeCanonicalPath(const char* fromPath, bool resolveSimLinks = true);  // assumes relative to application if relative path
bool native2path(const char *inNativeFileName, char* outStdFileName, int len);

const char* os_getPlatformErrorMessage(long err) {
	static char sHresultMsgBuf[1024];
	LPVOID a_pvMsgBuf;
	LPCSTR a_pszMsg = NULL;
	std::sprintf(sHresultMsgBuf, "Windows Error: 0X%.8X", err);
	if (WinAPI::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR) &a_pvMsgBuf, 0, NULL )) {
	   a_pszMsg = (LPSTR)a_pvMsgBuf;
	   // Do something with the error message.
	   std::strcat(sHresultMsgBuf, a_pszMsg );
	   LocalFree(a_pvMsgBuf);
	}
	return sHresultMsgBuf;
}

// return false if illegal characters are found
bool os_path2native(const char *inStdFileName, char* outNativeFileName, int len) {
    DEBUG_ASSERT(len > 0, "PRECONDITION: length must not be zero or negative");
    outNativeFileName[0] = 0;
    if (std::strchr(inStdFileName, '\\')) {
        return false;   // backslash is an illegal character
    }
    if (std::strchr(inStdFileName+2, ':')) {
        return false;   // colon is an illegal character after 2nd position
    }
    char tempname[MAX_PATH];
    std::strncpy(tempname, inStdFileName, MAX_PATH);
    MAKE_STRING_BUFFER_SAFE(tempname, MAX_PATH); // make string buffer safe also alerts us to when inStdFileName is too long and gets truncated
    char * p;
    p = std::strchr(tempname,'/');
    while (p) {
        *p = '\\';
        p = std::strchr(p,'/');
    }
    std::strncpy(outNativeFileName, tempname, len);
    MAKE_STRING_BUFFER_SAFE(outNativeFileName, len);
    return true;
}

// return false if illegal characters are found
bool native2path(const char *inNativeFileName, char* outStdFileName, int len) {
    DEBUG_ASSERT(len > 0, "PRECONDITION: length must not be zero or negative");
    if (std::strchr(inNativeFileName, '/')) {
        return false;   // forward slash is an illegal character
    }
    char tempname[1024];
    std::strncpy(tempname, inNativeFileName, 1024);
    MAKE_STRING_BUFFER_SAFE(tempname, 1024); // make string buffer safe also alerts us to when inStdFileName is too long and gets truncated
	// now that we got the drive spec, there is no other place were colon is legal
	if (std::strchr(tempname+2, ':')) {
        return false;   // colon is an illegal character
    }
	char * p;
    p = std::strchr(tempname,'\\');
    while (p) {
        *p = '/';
        p = std::strchr(p+1,'\\');
    }
    std::strncpy(outStdFileName, tempname, len);
    MAKE_STRING_BUFFER_SAFE(outStdFileName, len);
    return true;
}

std::string os_makeCanonicalPath(const char* inFromPath, bool resolveSimLinks) {
	char fromPath[MAX_PATH];
	char workingBuf[MAX_PATH];
	workingBuf[0] = 0;
	if (std::strchr(inFromPath,'/')) {
		if (!os_path2native(inFromPath, fromPath, MAX_PATH)) {
			DEBUG_PRINT("os_path2native failed for [%s]", inFromPath);
		}
//		DEBUG_PRINT("os_makeCanonicalPath convert to native path [%s]", fromPath);
	} else {
		std::strncpy(fromPath, inFromPath, MAX_PATH);
		MAKE_STRING_BUFFER_SAFE(fromPath, MAX_PATH);
//		DEBUG_PRINT("os_makeCanonicalPath given native path [%s]", fromPath);
	}
	if (fromPath[0] != 0) {
		// make an absolute path
		if (!os_isAbsolutePath(fromPath)) {
			std::strncpy(workingBuf, OS::getApplicationDirectory(), MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
			std::strncat(workingBuf, "\\", MAX_PATH); 
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
			std::strncat(workingBuf, fromPath, MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
		} else {
			std::strncpy(workingBuf, fromPath, MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
		}
		// remove double backslashes '\\', empty path segments '\.\', and backtracking '\<dir>\..\'
		// also resolve sim links if desired
		char* lastSlash = workingBuf;
		char* p = workingBuf;
		while (*p) {
			if (*p == '\\') {
				// found a backslash, see what follows it
				if (p[1] == '\\') {
					// another backslash, remove
					std::strcpy(p, &p[1]);
				} else if (std::strncmp(p, "\\.\\", 3) == 0) {
					// an empty segment, remove
					std::strcpy(p, &p[2]);
				} else if (std::strncmp(p, "\\..\\", 4) == 0) {
					// a backtrack, remove along with the prior directory segment
					std::strcpy(lastSlash, &p[3]);
					// now search backwards for prev segment
					p = lastSlash;
					while (p > workingBuf) {
						p--;
						if (*p == '\\') {
							lastSlash = p;
							break;
						}
					}
				} else {
					// something else, so we are starting a new path segment
					// save this as the new last slash
					lastSlash = p;
					p++;
				}
// 				if (resolveSimLinks && (*p == '\\')) {
// 					char buf[MAX_PATH];
// 					*p = 0;
// 					int len = readlink(workingBuf, buf, MAX_PATH);
// 					if (len > 0 && len < MAX_PATH) {
// 						char buf2[MAX_PATH];
// 						std::strcpy(buf2, &p[1]); // this is safe because p[1] starts a string that is always shorter than MAX_PATH
// 						std::strcpy(workingBuf, buf); // also safe, buf always shorter than MAX_PATH
// 						std::strncat(workingBuf, "\\", MAX_PATH);
// 						MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
// 						std::strncat(workingBuf, buf2, MAX_PATH);
// 						MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
// 						p = workingBuf; p += len;
// 					}
// 					*p = '\\'; // restore our separator so we can pick up where we left off
// 				}
			} else {
				p++;
			}
		}
// 		if (resolveSimLinks) {
// 			// final resolution of sim link
// 			char buf[MAX_PATH];
// 			int len = readlink(workingBuf, buf, MAX_PATH);
// 			if (len > 0 && len < MAX_PATH) {
// 				std::strcpy(workingBuf, buf); // also safe, buf always shorter than MAX_PATH
// 			}
// 		}
	}
	return std::string(workingBuf);
}


std::string OS::makeCanonicalPath(const char* fromPath, bool resolveSimLinks ) {
	return os_makeCanonicalPath(fromPath, resolveSimLinks);
}

// returns true if a file was found, false if no file was found
bool
OS::findFirst(const char* inFindName, FindDataT& outFindData) {
	outFindData.privateData = 0;	// make sure this is good for later
    char searchName[MAX_PATH];
    if (!os_path2native(inFindName, searchName, MAX_PATH)) {
        return false;   // illegal characters
    }
    outFindData.privateData = new PrivateFindData;
    DEBUG_ASSERT(outFindData.privateData, "New failed but didn't throw an exception. Turn exceptions on in the compiler");
    PrivateFindData* pData = static_cast<PrivateFindData*>(outFindData.privateData);
    pData->findRef = WinAPI::_findfirst(searchName, &pData->findData);
	if(pData->findRef != -1)
	{
		// copy results of find into our platform independent struct
		outFindData.isDirectory = (pData->findData.attrib == _A_SUBDIR);
		std::strncpy(const_cast<char*>(outFindData.nodeName), pData->findData.name, FindDataT::MAX_NODE_NAME_SIZE);
		MAKE_STRING_BUFFER_SAFE(const_cast<char*>(outFindData.nodeName), FindDataT::MAX_NODE_NAME_SIZE);
	}
    return (pData->findRef != -1);
}

bool
OS::findNext(FindDataT& ioFindData) {
    PrivateFindData* pData = static_cast<PrivateFindData*>(ioFindData.privateData);
    int result = WinAPI::_findnext(pData->findRef, &pData->findData);
	if(result != -1)
	{
		// copy results of find into our platform independent struct
		ioFindData.isDirectory = (pData->findData.attrib == _A_SUBDIR);
		std::strncpy(const_cast<char*>(ioFindData.nodeName), pData->findData.name, FindDataT::MAX_NODE_NAME_SIZE);
		MAKE_STRING_BUFFER_SAFE(const_cast<char*>(ioFindData.nodeName), FindDataT::MAX_NODE_NAME_SIZE);
	}
    return (result != -1);
}

// cleans up after a find, should always be called when done, even if no file was found on find first
void
OS::findClose(FindDataT& inFindData) {
    if (inFindData.privateData) {
        PrivateFindData* pData = static_cast<PrivateFindData*>(inFindData.privateData);
        WinAPI::_findclose(pData->findRef); // stop the _find
        delete pData;
    }
    inFindData.privateData = 0;
}

// Deletes a file. Returns true for success, false for failure.
bool
OS::deleteFile(const char* inFileName)
{
	return (WinAPI::DeleteFileA(inFileName) != 0);
}

// Renames a file. Returns true for success, false for failure.
bool 
OS::renameFile(const char* inFileName, const char* inNewFileName)
{
	return (WinAPI::rename(inFileName, inNewFileName) != 0);
}

unsigned long OS::getMilliseconds() {
    return WinAPI::GetTickCount();
}


#ifdef DEBUG

#ifdef PDG_DEBUG_OUT_TO_LOG
extern pdg::log gDebugLog;

pdg::log::category category_DOUT("_DOUT");

#endif // PDG_DEBUG_OUT_TO_LOG

void OS::_DEBUGGER(const char* str) {
/*	if(str)
	{
		::MessageBoxA(NULL, str, "ASSERT FAILED!", MB_OK | MB_ICONEXCLAMATION);
	}

	// invoke the debugger
    bool debugger_break_for_assert = false;
    assert(debugger_break_for_assert);
*/
}

#ifndef STADIUM_SERVER
void OS::_DOUT( const char * fmt, ...) {
	char buf[1256];
	va_list lst;
	va_start(lst, fmt);
	int n = std::vsnprintf(buf, 1255, fmt, lst);
	buf[1255] = 0;
	va_end(lst);
#ifdef PDG_DEBUG_OUT_TO_LOG

    gDebugLog << pdg::log::verbose << category_DOUT << buf << pdg::endlog;

#endif // PDG_DEBUG_OUT_TO_LOG

#ifdef UNICODE
    std::string src(buf);
    utf16string dst;
    utf8to16(dst, src);
	OutputDebugString( (WCHAR*)dst.c_str() );
#else
	OutputDebugString( buf );
#endif // UNICODE
    OutputDebugStringA("\n");
	// make the date time string
    char dateTimeStr[40];
	time_t lclTime;
	struct tm *now;
	lclTime = time(NULL);
	now = gmtime(&lclTime);
	strftime(dateTimeStr, 40, "%y%m%d %H:%M:%S ", now);
    unsigned long mstime = OS::getMilliseconds();
    char msStr[40];
    std::snprintf(msStr, 40, "%0#10lu\t", mstime);
	std::cout << dateTimeStr << msStr << '\t' << buf << std::endl;

}
#endif // !STADIUM_SERVER

//End of File
#endif // DEBUG

} // end namespace pdg

