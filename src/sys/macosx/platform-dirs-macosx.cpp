// -----------------------------------------------
// platform-dirs-macosx.cpp
// 
// Mac OS X implementation of platform specific directory setup functions
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
#include "internals-macosx.h"

#include <iostream>
#include <string>

//#define DEBUG_DIRECTORY_SETUP

#ifndef DEBUG_DIRECTORY_SETUP
	#define DIR_SETUP_DEBUG_ONLY( exp )
#else
	#define DIR_SETUP_DEBUG_ONLY( exp ) exp
#endif

namespace pdg {

const char* macosx_setupWorkingDirectory(int argc, const char* argv[], bool& isBundle);

const char* platform_setupDirectories(int argc, const char* argv[]) {
		// make sure our working directory is the directory the app was launched from
	bool isBundle;
	const char* cwd = macosx_setupWorkingDirectory(argc, argv, isBundle);
	std::string workingDir = OS::makeCanonicalPath(cwd, true);
	std::string appDir;
	std::string appDataDir;
	std::string resourceDir;

    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("working directory: [%s]", workingDir.c_str()); )

	// for Mac OS X application bundles, we want the application directory to point to the directory that
	// contains the {myapp}.app folder, and the resource directory to point into {myapp.app}/Contents/Resources
	appDir = workingDir;
	std::string appName;
	if (argc > 0) {
		appName.assign(argv[0]);
		appName.replace(0, workingDir.length(), "");
		size_t pos = appName.find('/');
		if (pos != std::string::npos) {
			appName.replace(pos, std::string::npos, "");
		}
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("found app name: [%s]", appName.c_str()); )
	}
#ifdef PDG_NODE_MODULE 
	// for building a node module, we don't want special directories
	// just have everything in the working dir
	appDataDir = workingDir;
	resourceDir = workingDir;
#else
	char buffer[PATH_MAX];
	if ( macosx_getApplicationSupportDirectory(appName.c_str(), buffer, PATH_MAX) ) {
    	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("got app support dir: [%s]", buffer); )
		appDataDir = OS::makeCanonicalPath(buffer, true);
	} else {
		appDataDir = workingDir;
	}
	if (isBundle) {
		resourceDir = workingDir + appName + "/Contents/Resources/";
	} else {
		resourceDir = workingDir;
	}
#endif

	os_setApplicationDirectory(appDir.c_str());
	os_setApplicationDataDirectory(appDataDir.c_str());
	os_setApplicationResourceDirectory(resourceDir.c_str());

    DEBUG_ONLY(
		std::cerr << "Using Application directory [" << OS::getApplicationDirectory() << "]" << std::endl;
		std::cerr << "Using Data directory [" << OS::getApplicationDataDirectory() << "]" << std::endl;
		std::cerr << "Using Resource directory [" << OS::getApplicationResourceDirectory() << "]" << std::endl;
	)
	
	return cwd;
}

const char* macosx_setupWorkingDirectory(int argc, const char* argv[], bool& isBundle) {
	isBundle = false;
    char* buf = (char*) std::malloc(1024);
    getcwd(buf, 1024);
    MAKE_STRING_BUFFER_SAFE(buf, 1024);
	std::string wdstr;
	wdstr.assign(buf);  // get our current working directory
	std::free(buf);	
	DIR_SETUP_DEBUG_ONLY( OS::_DOUT("cwd: [%s]", wdstr.c_str()); )

	if ( (argc>0) && ( (argv[0][0] != '/') || (std::strcmp(wdstr.c_str(), "/") == 0) )  ) {
	    // the correct path is in the combination of wd with argv[0], then subtracting
	    // everything after and including the last /
	    wdstr += '/';
	    wdstr += argv[0];    // add in the new string
	    std::string::size_type pos = wdstr.find_last_of('/');
	    if (pos != std::string::npos) {
	        wdstr.resize(pos);
	    }
	    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("added argv[0] but removed command name: [%s]", wdstr.c_str()); )
	}
	wdstr += '/'; // add back in the trailing path character
	// check to see if the working directory was inside a bundle (this seems 
	// to happen during normal finder launch but not during launch from XCode)
	std::string::size_type pos2 = wdstr.rfind(".app/Contents/MacOS/");
	if (pos2 != std::string::npos) {
		// found it, remove it
		isBundle = true;
		wdstr.resize(pos2);
		pos2 = wdstr.find_last_of('/');
		if (pos2 != std::string::npos) {
			wdstr.resize(pos2 + 1);
		}
	    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("in bundle, adjusted working dir: [%s]", wdstr.c_str()); )
	}

	// eliminate unnecessary "///" from front if present
	if (std::strncmp(wdstr.c_str(), "///", 3) == 0) {
		wdstr.replace(0, 2, "");  // remove the first 2 slashes, leaving only one slash in front of the path
	    DIR_SETUP_DEBUG_ONLY( OS::_DOUT("removed extra slashes: [%s]", wdstr.c_str()); )
	}

	int len = wdstr.size() + 1;
    char* resultStr = (char*) std::malloc( len );
	std::strncpy(resultStr, wdstr.c_str(), len);
	MAKE_STRING_BUFFER_SAFE(resultStr, len);
	
	DEBUG_ONLY(
		std::cerr << "Using working directory [" << wdstr.c_str() << "]" << std::endl;
	)
	
    return resultStr;
}

} // end namespace pdg

