// -----------------------------------------------
// platform-macosx.cpp
// 
// Mac OS X implementation of platform specific functions
// Cocoa implementation
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
#include "internals.h"

#include <iostream>
#include <string>
#include <cstdlib>

namespace pdg {

const char* platform_setupDirectories(int argc, const char* argv[]) {
		// make sure our working directory is the directory the app was launched from
    char* buf = (char*) std::malloc(1024);
    getcwd(buf, 1024);
    MAKE_STRING_BUFFER_SAFE(buf, 1024);
	std::string workingDir;
	workingDir = OS::makeCanonicalPath(buf, true);  // get our current working directory
	std::free(buf);	
	std::string appDir;
	std::string appDataDir;
	std::string resourceDir;

	appDir = workingDir;
	appDataDir = workingDir;
	resourceDir = workingDir;

	os_setApplicationDirectory(appDir.c_str());
	os_setApplicationDataDirectory(appDataDir.c_str());
	os_setApplicationResourceDirectory(resourceDir.c_str());

    DEBUG_ONLY(
		std::cerr << "Using Application directory [" << OS::getApplicationDirectory() << "]" << std::endl;
		std::cerr << "Using Data directory [" << OS::getApplicationDataDirectory() << "]" << std::endl;
		std::cerr << "Using Resource directory [" << OS::getApplicationResourceDirectory() << "]" << std::endl;
	)
	
	return buf;
}

} // end namespace pdg

