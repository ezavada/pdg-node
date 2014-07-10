// -----------------------------------------------
// resource.h
// 
// resource manager functionality
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


#ifndef PDG_RESOURCE_H_INCLUDED
#define PDG_RESOURCE_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"
#include "pdg/sys/singleton.h"

#include <string>
#include <vector>


#define STRINGS_FILENAME	"strings.txt"

#ifdef PDG_NO_ZIP
  #define MAX_CACHED_STRING_FILES 1 // when using no zip option we only have one strings.txt file
#else
  #define MAX_CACHED_STRING_FILES 10
#endif // !PDG_NO_ZIP
#define MAX_CACHED_IMAGES 1024


namespace pdg {

class Image;
class ImageStrip;
class Sound;
class SoundManager;
class GraphicsManager;

// -----------------------------------------------------------------------------------
//! ResourceManager
//! used for loading resources
//! \ingroup Managers
//! \ingroup Sound
//! \ingroup Graphics
// -----------------------------------------------------------------------------------

class ResourceManager : public Singleton<ResourceManager> {
friend class Singleton<ResourceManager>;
public:

	// set the language we are currently working in
	// when a language is set, we always look for files
	// with a -{langCode} ending first.
	void	    setLanguage(const char* langCode);

	// get the language we are currently working in
	const char* getLanguage();

    // add a new resource file to the list of files to be searched for resources
    // files are searched in order from most recently added to first added
    // returns refNum for file. This can actually be a directory name too.
    int     openResourceFile(const char* filename);

    // optionally remove a resource file from the list. this is not necessary
    // for cleanup, the cleanup is done automatically
    void     closeResourceFile(int refNum);
    
    // load an image from the first resource file it can be found in
    Image*       getImage(const char* imageName);
    
    // load a multi-frame image from the first resource file it can be found in
    ImageStrip*  getImageStrip(const char* imageName);

    // load a sound from the first resource file it can be found in
    Sound*       getSound(const char* soundName);
    
    // load a string (or substring from a multipart string)
    const char*  getString(std::string& ioStr, short id, short substring = -1);

    // find out how big a resource is
    unsigned long  getResourceSize(const char* resourceName);
    
    // load a resource into a preallocated buffer
    // use getResourceSize to determine the size of the buffer required for the whole
    // resource if you don't already know it
    // returns true if resource was found, false if not found
    bool    getResource(const char* resourceName, void* buffer, unsigned long bufferSize);

	// return a semicolon separated list of paths (directories and zip file names) that
	// are currently open for loading resources
	std::string	getResourcePaths();

	// TODO: eliminate these
	// process the specialized format string from the res file and snprintf() the args in correct order
	enum	spLocErr { SUCCESS, BUF_OVERRUN, UNDEF_ERROR };
	API_DEPRECATED int snprintfLoc(char* dst, int maxlen, const char * fmt, ...);

    
// lifecycle
/// @cond C++
    virtual ~ResourceManager();
/// @endcond

protected:

/// @cond INTERNAL
    // WARNING! alters string passed in
	static char* getNthSubstring(int n, char* s, int bufflen);
	static void getStringFromDataBlock( char* block, int blocksize, std::string& outStr, short id, short substring);

	static ResourceManager* createSingletonInstance();
    ResourceManager(); // call ResourceManager::getSingletonInstance() instead
    
    std::vector<void*>      mFiles;
    std::string				mLanguage;
    char*                   mImagesNamesCache[MAX_CACHED_IMAGES];
	bool					mImagesCacheHasLang[MAX_CACHED_IMAGES];
    Image*                  mImagesCache[MAX_CACHED_IMAGES];
	int						mNumCachedImages;
/// @endcond
};



} // end namespace pdg



#endif // PDG_RESOURCE_H_INCLUDED

