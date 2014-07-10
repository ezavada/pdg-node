// -----------------------------------------------
// os-unix.cpp
// 
// Implementation of core os specific functionality
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

#include "pdg/sys/os.h"
#include "pdg/sys/log.h"
#include "internals.h"
#include "pdg-main.h"

#include <sys/time.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstdarg>

#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <fnmatch.h>

#define PosixAPI
#define MAX_PATH 4096


namespace pdg {


struct PrivateFindData {
    DIR* findData;
    struct dirent *entry;
    char  searchName[FindDataT::MAX_NODE_NAME_SIZE];  // filename we are comparing to
};

std::string os_makeCanonicalPath(const char* fromPath, bool resolveSimLinks = true);  // assumes relative to application if relative path

// return false if illegal characters are found
bool os_path2native(const char *inStdFileName, char* outNativeFileName, int len) {
    if (std::strchr(inStdFileName, '\\')) {
        return false;   // backslash is an illegal character
    }
    if (std::strchr(inStdFileName+2, ':')) {
        return false;   // colon is an illegal character after 2nd position
    }
    std::strncpy(outNativeFileName, inStdFileName, len);
    MAKE_STRING_BUFFER_SAFE(outNativeFileName, len);
    return true;
}

std::string os_makeCanonicalPath(const char* fromPath, bool resolveSimLinks) {
	char workingBuf[MAX_PATH];
	workingBuf[0] = 0;
	if (fromPath) {
		// make an absolute path
		if (fromPath[0] != '/') {
			std::strncpy(workingBuf, OS::getApplicationDirectory(), MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
			std::strncat(workingBuf, "/", MAX_PATH); 
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
			std::strncat(workingBuf, fromPath, MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
		} else {
			std::strncpy(workingBuf, fromPath, MAX_PATH);
			MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
		}
		// remove double slashes '//', empty path segments '/./', and backtracking '/<dir>/../'
		// also resolve sim links if desired
		char* lastSlash = workingBuf;
		char* p = workingBuf;
		while (*p) {
			if (*p == '/') {
				// found a slash, see what follows it
				if (p[1] == '/') {
					// another slash, remove
					std::strcpy(p, &p[1]);
				} else if (std::strncmp(p, "/./", 3) == 0) {
					// an empty segment, remove
					std::strcpy(p, &p[2]);
				} else if (std::strncmp(p, "/../", 4) == 0) {
					// a backtrack, remove along with the prior directory segment
					std::strcpy(lastSlash, &p[3]);
					// now search backwards for prev segment
					p = lastSlash;
					while (p > workingBuf) {
						p--;
						if (*p == '/') {
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
				if (resolveSimLinks && (*p == '/')) {
					char buf[MAX_PATH];
					*p = 0;
					int len = readlink(workingBuf, buf, MAX_PATH);
					if (len > 0 && len < MAX_PATH) {
						char buf2[MAX_PATH];
						std::strcpy(buf2, &p[1]); // this is safe because p[1] starts a string that is always shorter than MAX_PATH
						std::strcpy(workingBuf, buf); // also safe, buf always shorter than MAX_PATH
						std::strncat(workingBuf, "/", MAX_PATH);
						MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
						std::strncat(workingBuf, buf2, MAX_PATH);
						MAKE_STRING_BUFFER_SAFE(workingBuf, MAX_PATH);
						p = workingBuf; p += len;
					}
					*p = '/'; // restore our separator so we can pick up where we left off
				}
			} else {
				p++;
			}
		}
		if (resolveSimLinks) {
			// final resolution of sim link
			char buf[MAX_PATH];
			int len = readlink(workingBuf, buf, MAX_PATH);
			if (len > 0 && len < MAX_PATH) {
				std::strcpy(workingBuf, buf); // also safe, buf always shorter than MAX_PATH
			}
		}
	}
	return std::string(workingBuf);
}

std::string OS::makeCanonicalPath(const char* fromPath, bool resolveSimLinks ) {
	return os_makeCanonicalPath(fromPath, resolveSimLinks);
}

bool OS::findFirst(const char* inFindName, FindDataT& outFindData) {
  char searchName[MAX_PATH];

  if (!os_path2native(inFindName, searchName, MAX_PATH)) {
     return false;   // illegal characters
  }
  outFindData.privateData = new PrivateFindData;
  PrivateFindData* pData = static_cast<PrivateFindData*>(outFindData.privateData);

// terminate the string at the last path separator to get the directory name
  std::string dirName("");
  std::string fileName("");

  const char* end = std::strrchr(inFindName, '/');  // find the last path separator
  if (end != 0) {
    char* start = (char*)inFindName;

    while(start != end) {
      dirName += *start;
      start++;
    }

    end++; //points to start of filename
    fileName = end;
  } else {
    fileName = (char*)inFindName;
  }
  dirName = os_makeCanonicalPath(dirName.c_str());

  std::strncpy(pData->searchName,fileName.c_str(),FindDataT::MAX_NODE_NAME_SIZE);
  MAKE_STRING_BUFFER_SAFE(pData->searchName, FindDataT::MAX_NODE_NAME_SIZE);

  pData->findData = PosixAPI::opendir(dirName.c_str());

  if (0 == pData->findData) {
    return false;
  }
  
  // search for first occurance of file that matches the find criteria
  pData->entry = PosixAPI::readdir(pData->findData);
  while (pData->entry != 0) {
    if (PosixAPI::fnmatch(fileName.c_str(), pData->entry->d_name, FNM_FILE_NAME )==0) {
      // copy results of find into our platform independent struct
      if (pData->entry->d_type == DT_DIR) {
        outFindData.isDirectory = true;
      } else {
        outFindData.isDirectory = false;
      }

      std::strncpy(const_cast<char*>(outFindData.nodeName), pData->entry->d_name, FindDataT::MAX_NODE_NAME_SIZE);
      MAKE_STRING_BUFFER_SAFE(const_cast<char*>(outFindData.nodeName), FindDataT::MAX_NODE_NAME_SIZE);
      break;
    }
    pData->entry = PosixAPI::readdir(pData->findData);
  }
  return (pData->entry != NULL);
}


bool OS::findNext(FindDataT& ioFindData) {
    PrivateFindData* pData = static_cast<PrivateFindData*>(ioFindData.privateData);

    while( (pData->entry = PosixAPI::readdir(pData->findData)) != 0) {
      if (PosixAPI::fnmatch(pData->searchName, pData->entry->d_name, FNM_FILE_NAME )==0) {
      // copy results of find into our platform independent struct
        if (pData->entry->d_type == DT_DIR) {
          ioFindData.isDirectory = true;
        } else {
          ioFindData.isDirectory = false;
        }

        std::strncpy(const_cast<char*>(ioFindData.nodeName), pData->entry->d_name, FindDataT::MAX_NODE_NAME_SIZE);
        MAKE_STRING_BUFFER_SAFE(const_cast<char*>(ioFindData.nodeName), FindDataT::MAX_NODE_NAME_SIZE);
        break;
     }
  }
   return (pData->entry != NULL);
}

// cleans up after a find, should always be called when done, even if no file was found on find first
void OS::findClose(FindDataT& inFindData) {
    if (inFindData.privateData) {
        PrivateFindData* pData = static_cast<PrivateFindData*>(inFindData.privateData);
        if (pData->findData) {
            PosixAPI::closedir(pData->findData); // stop the find
        }
        delete pData;
    }
    inFindData.privateData = 0;
}

// Deletes a file. Returns true for success, false for failure.
bool
OS::deleteFile(const char* inFileName) {
	return (remove(inFileName) == 0);
}

// Rename a file. Returns true for success, false for failure.
bool
OS::renameFile(const char* inFileName, const char* inNewFileName) {
	return (rename (inFileName, inNewFileName) == 0);
}
	
unsigned long 
OS::getMilliseconds() {
    unsigned long mstime = 0;
    // this is a more accurate way on Unix to get this
    struct timeval currTime;
    gettimeofday( &currTime, NULL );
    mstime = (currTime.tv_usec/1000) + (currTime.tv_sec*1000);
    return mstime;
}

} // end namespace pdg 

#ifdef DEBUG

pdg::log::category category_DOUT("_DOUT");

void pdg::OS::_DEBUGGER(const char* str) {
	// invoke the debugger
    bool debugger_break_for_assert = false;
	// we still want to dump asserts to the console
	std::cerr << str << std::endl;
    assert(debugger_break_for_assert);
}


#if COMPILER_GCC
namespace std {
    using ::vsnprintf;
}
#endif // COMPILER_GCC

void pdg::OS::_DOUT( const char * fmt, ...) {
    static const int bufsize = 1024;
	static char buf[bufsize];
	std::va_list lst;
	va_start(lst, fmt);
	/* int n = */ std::vsnprintf(buf, bufsize-1, fmt, lst);
	va_end(lst);
	buf[bufsize-1] = 0;
#ifndef PDG_NO_DEBUG_TO_CONSOLE
	std::cout << buf << std::endl;
#endif

#ifdef PDG_DEBUG_OUT_TO_LOG
    main_getDebugLog() << pdg::log::verbose << category_DOUT << buf << pdg::endlog;
#endif // PDG_DEBUG_OUT_TO_LOG
	
	return;
}
#endif


