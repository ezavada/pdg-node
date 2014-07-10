// -----------------------------------------------
// resource.cpp
// 
// resource manager functionality
//
// Written by Ed Zavada and Ariel Butler, 2004-2012
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

#include "pdg/sys/resource.h"
#include "pdg/sys/os.h"
#include "pdg/sys/global_types.h"
#include "pdg/sys/image.h"
#include "pdg/sys/imagestrip.h"
#ifndef PDG_NO_GUI
#include "pdg/sys/sound.h"
#endif // !PDG_NO_GUI

#include <cctype>
#include <cstdarg>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "internals.h"

#ifndef PDG_NO_ZIP
#include "unzip.h"
zlib_filefunc_def* gZLibFileFuncs = 0;
#endif // PDG_NO_ZIP


#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif


namespace pdg {

long gLastRefNum = 0;

struct FileInfo {
	long ref;
	bool isDir;
	std::string name;
	std::string fullpath;
	const char* stringCache;
	const char* stringLangCache;
	size_t	stringCacheSize;
	size_t	stringLangCacheSize;
#ifndef PDG_NO_ZIP
	unzFile file;
#endif // PDG_NO_ZIP
};

bool fileExists(const char* path);
size_t fileSize(const char* path);
const char* localizedName(const char* filename, const char* lang);

bool fileExists(const char* path) {
	std::ifstream file;
	file.open(path, std::ios::binary);
	file.seekg(0, std::ios::end);
	int len = file.tellg();
	file.close();
	return (len != -1);
}
size_t fileSize(const char* path) {
	std::ifstream file;
	file.open(path, std::ios::binary);
	file.seekg(0, std::ios::end);
	int len = file.tellg();
	file.close();
	return (len != -1) ? (size_t)len : 0;
}

const char* localizedName(const char* filename, const char* lang) {
	static char outName[256];
	std::strcpy(outName, filename);
	char* p = std::strrchr(outName, '.');
	if (p) *p = 0;
	std::strcat(outName, "-");
	std::strcat(outName, lang);
	if (p) {
		size_t idx = (size_t) p - (size_t)&outName;
		std::strcat(outName, &filename[idx]);
	}
	return outName;
}

// finds a string by id (or substring from a multipart string) in a data block
// returns empty string if there is no such string or string & substring
// input string data block must be nul terminated
void 
ResourceManager::getStringFromDataBlock( char* stringData, int len, std::string& outStr, short id, short substring) {
    // read the data, now find the string in question
    char* p = stringData;
    char* start = 0;
    enum ScanModes { seekID, seekEOL };
    ScanModes mode = seekID;
    long currID = 0;
    while (*p != 0) {
        char c = *p;
        if (mode == seekID) {
            if (c == '#') {
                // comment, switch to scanning for end of line
                mode = seekEOL;
            } else if (c >= '0' && c <= '9') {
                currID *= 10;
                currID += (long) (c - '0');
            } else if (c == '\t') { // this is the tab character at the end of the ID
                if (currID == id) {
                    start = p + 1;  // id match, save start of our string
                }
                mode = seekEOL;
            } else {
                // bad line, just skip it
                mode = seekEOL;
            }
        }
        // deliberately not doing an else here, so we check for EOL immediately
        // if we switched to that mode
        if (mode == seekEOL) {
            if ((c == 0x0D) || (c == 0x0A)) {
                if (start) {    // we were looking for end of our matched string
                    outStr.assign(start, p - start);
                    start = 0;  // we aren't still looking for the string
                    break;  // we are now done looking for the string
                }
                ++p;
                if ((*p != 0x0D) && (*p != 0x0A)) {  // check for 2nd char of DOS EOL
                    --p;    // not second char of DOS EOL, put it back
                }
                mode = seekID; 
                currID = 0; // reset so we can start accumulating new digits
            }
        }
        ++p;
    }
    if (start && (mode == seekEOL)) {
        // string ends at end of file
        outStr.assign(start, p - start);
    }
    // find a substring
    if ((outStr.length()>0) && (substring > -1)) {
        std::size_t sublen = outStr.length()+1;
        char* temp = (char*)std::malloc(sublen);
        std::strncpy(temp, outStr.c_str(), sublen);
        MAKE_STRING_BUFFER_SAFE(temp, sublen);
        outStr.assign(ResourceManager::getNthSubstring(substring, temp, sublen));
        std::free(temp);
    }
}


// find Nth (zero based) substring in a string
// substring elements are separated by vertical bars "|"
// no vertical bar at begining or end, must be nul terminated
// WARNING! alters string passed in
char* ResourceManager::getNthSubstring(int n, char* s, int bufflen)
{
    char* p = s;
    char* ep = s;
    do {
        CHECK_PTR(ep, s, bufflen);
        if ( (*ep == '\0') || (*ep == '|') ) {
            if (n == 0) {
                *ep = '\0';  // null terminate current segment and return it
                return p;
            } else {
                --n;
                if (*ep == '\0') {  // at end, we couldn't find the string
                    return ep;  // return an empty string
                } else {
                    CHECK_PTR(ep, s, bufflen-1);
                    ++ep;  // advance past the vertical bar
                    p = ep; // update the start of string pointer
                }
            }
        } else {
        	++ep;
       	}
    } while (true);
}

void	    
ResourceManager::setLanguage(const char* langCode) {
	if (std::strcmp(mLanguage.c_str(), langCode) == 0) {
		// destroy cache entries that were language specific
		const char* lang = mLanguage.c_str();
		std::vector<void*>::iterator p = mFiles.begin();
		while (p != mFiles.end()) {
			FileInfo* fip = (FileInfo*)*p;
			if (fip->stringLangCache) {
				DEBUG_ONLY( OS::_DOUT("Freeing strings-%s.txt file cache (size [%d])", lang, fip->stringLangCacheSize); )
				std::free((void*)fip->stringLangCache);
				fip->stringLangCache = 0;
			}
			++p;
		}
		// remove language specific images from cache
		for (int i = 0; i < MAX_CACHED_IMAGES; i++) {
			if (mImagesCacheHasLang[i] && mImagesNamesCache[i] && mImagesCache[i]) {  
				DEBUG_ONLY( OS::_DOUT("Releasing Image [%s] [%p] from cache", 
									  mImagesNamesCache[i], mImagesCache[i]); )
				Image* img = mImagesCache[i];
				char* name = mImagesNamesCache[i];
				img->release();
				std::free( name );  // this was malloc'd in getImage()
				mImagesNamesCache[i] = 0;
			}
		}
	}
	mLanguage = langCode;
}

// get the language we are currently working in
const char* 
ResourceManager::getLanguage() {
	return mLanguage.c_str();
}

// add a new resource file to the list of files to be searched for resources
// files are searched in order from most recently added to first added
// returns refNum for file.
int     
ResourceManager::openResourceFile(const char* filename) {
    int ref = 0;
    FileInfo* fip = new FileInfo;
    fip->name = filename;
    fip->stringCache = 0;
    fip->stringLangCache = 0;
    fip->stringCacheSize = 0;
    fip->stringLangCacheSize = 0;
    std::string fullpathstr = "";
    DEBUG_PRINT( "Adding resource file [%s]", filename );
    if (!os_isAbsolutePath(filename)) {
    	DEBUG_PRINT( " App Resource Directory [%s]", OS::getApplicationResourceDirectory() );
    	fullpathstr.append(OS::getApplicationResourceDirectory());
    	fullpathstr.append("/");
    	fullpathstr.append(filename);
    } else {
    	fullpathstr = filename;
    }
    fip->fullpath = OS::makeCanonicalPath(fullpathstr.c_str());
    DEBUG_PRINT( " Canonical resource file [%s]", fip->fullpath.c_str() );
#ifndef PDG_NO_ZIP
    unzFile file = unzOpen2(fip->fullpath.c_str(), gZLibFileFuncs);
    if (file) {
    	ref = ++gLastRefNum;
    	fip->isDir = false;
    	fip->file = file;
    	fip->ref = ref;
        // add it to the list
        mFiles.insert(mFiles.begin(), (void*)fip);
		DEBUG_ONLY( OS::_DOUT("Added Resource file to list [%s]", fip->fullpath.c_str()); )
	} else {
    	fip->file = 0;
		DEBUG_ONLY( OS::_DOUT("Resource Error: failed to find file [%s]", fip->fullpath.c_str()); )
	}
#endif // PDG_NO_ZIP
	if (ref == 0) {
		// not yet found
		fip->isDir = true;
		// TODO: validate that this is really a directory that exists
    	ref = ++gLastRefNum;
    	fip->ref = ref;
        mFiles.insert(mFiles.begin(), (void*)fip);
		DEBUG_ONLY( OS::_DOUT("Added Resource directory to list [%s]", fip->fullpath.c_str()); )
	}
    return ref;
}

// optionally remove a resource file from the list. this is not necessary
// for cleanup, the cleanup is done automatically
void
ResourceManager::closeResourceFile(int refNum) {
	// TODO: remove the entry from the list
    bool found = false;
	std::vector<void*>::iterator p = mFiles.begin();
    while (!found && (p != mFiles.end())) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->ref == refNum) {
			DEBUG_ONLY( OS::_DOUT("Removing Resource [%s] from list", fip->fullpath.c_str()); )
        	mFiles.erase(p);
        	return;
    	}
    	p++;
    }
}

// load an image from the first resource file it can be found in
Image*
ResourceManager::getImage(const char* imageName) {
    Image* img = NULL;
    DEBUG_ASSERT(mFiles.size() > 0, "ResourceManager::getImage() called but no resources files open");
	// check the cache first, reuse image resources if possible
    for (int i = 0; i < MAX_CACHED_IMAGES; i++) {
        if (!mImagesNamesCache[i]) {
			break;
		} else if (std::strcmp(mImagesNamesCache[i], imageName) == 0) {
			img = mImagesCache[i];
			img->addRef();
			return img;
		}
	}
    std::vector<void*>::iterator p = mFiles.begin();
    bool found = false;
    while (!found && (p != mFiles.end())) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->isDir) {
			std::string s = fip->fullpath;
            s += '/';
            s += imageName;
			if (fileExists(s.c_str())) {
				img = Image::createImageFromFile(s.c_str());
				if (img) {
					found = true;
					if (mNumCachedImages < MAX_CACHED_IMAGES) {
						// cache the image
						img->addRef();
						mImagesCache[mNumCachedImages] = img;
						mImagesCacheHasLang[mNumCachedImages] = false;
						mImagesNamesCache[mNumCachedImages] = (char*) std::malloc( std::strlen(imageName) + 1 );
						std::strcpy(mImagesNamesCache[mNumCachedImages], imageName);
						mNumCachedImages++;
					}
				}
			}
    	}
      #ifndef PDG_NO_ZIP
        else if (fip->file) {
            if (unzLocateFile (fip->file, imageName, false) == UNZ_OK) {
                // found the file, now need to load it
                unz_file_info info;
                if (unzGetCurrentFileInfo(fip->file, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
                    // allocate space to hold the file contents
                    int len = info.uncompressed_size;
                    char* imageData = (char*) std::malloc(len);
                    if (imageData != NULL) {
                        if (unzOpenCurrentFile(fip->file) == UNZ_OK) {
                            int bytesRead = unzReadCurrentFile(fip->file, imageData, len);
                            if (bytesRead == len) {
                                // read the data, now create the Image
                                img = Image::createImageFromData(imageName, imageData, len);
								found = true;
                            } else {
 								DEBUG_ONLY( OS::_DOUT("Resource Error: failed to read image [%s], "
 									"wanted [%ld] bytes, got [%ld]", imageName, len, bytesRead); )
                            }
                            unzCloseCurrentFile(fip->file);
                        }
                        std::free(imageData);  // always free the data, the image will have copied it if necessary
                    }
                }
            }
        }
      #endif // PDG_NO_ZIP
        ++p;
    }
    return img;
}

ImageStrip*
ResourceManager::getImageStrip(const char* imageName) {
	// internally both are handled by the same underlying implementation object, ImageImpl
	return dynamic_cast<ImageStrip*>(getImage(imageName));
}

// load a sound from the first resource file it can be found in
Sound*       
ResourceManager::getSound(const char* soundName) {
    // load a WAV or some other kind of file
    Sound* snd = NULL;
    DEBUG_ASSERT(mFiles.size() > 0, "ResourceManager::getSound() called but no resources files open");
#ifndef PDG_NO_SOUND
    std::vector<void*>::iterator p = mFiles.begin();
    bool found = false;
    while (!found && (p != mFiles.end())) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->isDir) {
			std::string s = fip->fullpath + "/" + soundName;
			if (fileExists(s.c_str())) {
				snd = Sound::createSoundFromFile(s.c_str());
				if (snd) {
					found = true;
				}
			}
		}
      #ifndef PDG_NO_ZIP
        else if (fip->file) {
            if (unzLocateFile (fip->file, soundName, false) == UNZ_OK) {
                // found the file, now need to load it
                unz_file_info info;
                if (unzGetCurrentFileInfo(fip->file, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
                    // allocate space to hold the file contents
                    int len = info.uncompressed_size;
                    char* soundData = (char*) std::malloc(len);
                    if (soundData != NULL) {
                        if (unzOpenCurrentFile(fip->file) == UNZ_OK) {
                            int bytesRead = unzReadCurrentFile(fip->file, soundData, len);
                            if (bytesRead == len) {
                                // read the data, now create the Sound
								snd = Sound::createSoundFromData(soundName, soundData, len);
								found = true;
                            } else {
 								DEBUG_ONLY( OS::_DOUT("Resource Error: failed to read sound [%s], "
 									"wanted [%ld] bytes, got [%ld]", soundName, len, bytesRead); )
                            }
                            unzCloseCurrentFile(fip->file);
                        }
                        std::free(soundData);  // always free the data, the sound will have copied it if necessary
                    }
                }
            }
        }
      #endif // !PDG_NO_ZIP
        ++p;
    }
  #endif // !PDG_NO_SOUND
    return snd;
}

// load a string (or substring from a multipart string)
// returns empty string if there is no such string or string & substring
const char*
ResourceManager::getString(std::string& outStr, short id, short substring) {
    outStr.assign("");
    DEBUG_ASSERT(mFiles.size() > 0, "ResourceManager::getString() called but no resources files open");
    std::vector<void*>::iterator p = mFiles.begin();
    bool found = false;
	while (!found && (p != mFiles.end())) {
		// first check to see if the data is already in the cache
		bool checkLang = (mLanguage.length() > 0);
		bool done = false;
		char* stringsFileData = 0;
		while (!found && !done) {
			FileInfo* fip = (FileInfo*)*p;
//			DEBUG_ONLY( OS::_DOUT(" Checking %s", fip->fullpath.c_str()); )
    		// check for language specific items first, then default language items
			stringsFileData = (char*) ((checkLang) ? fip->stringLangCache : fip->stringCache);
			int len = (checkLang) ? fip->stringLangCacheSize : fip->stringCacheSize;
			if (!stringsFileData) {
				// not in the cache, need to load it
				std::string fname;
				if (checkLang) {
					fname = localizedName(STRINGS_FILENAME, mLanguage.c_str());
				} else {
					fname = STRINGS_FILENAME;
				}
//				DEBUG_ONLY( OS::_DOUT("  Looking for file %s", fname.c_str()); )
				if (fip->isDir) {
					// the current resource is a directory, try to 
					// load the strings.txt file from there
					std::string s = fip->fullpath + "/" + fname;
					std::ifstream file;
					file.open(s.c_str(), std::ios::binary);
					file.seekg(0, std::ios::end);
					len = file.tellg();
//					DEBUG_ONLY( OS::_DOUT("  Checking file [%s] size [%d]", s.c_str(), len); )
					if (len != -1) {  // fileExists
						++len;
						stringsFileData = (char*) std::malloc(len);
						if (stringsFileData) {
							file.seekg(0, std::ios::beg);
							file.read(stringsFileData, len);
							stringsFileData[len-1] = 0; // make sure it is nul terminated
						}
					}
					file.close();
				}
			  #ifndef PDG_NO_ZIP
				else if (fip->file) {
//					DEBUG_ONLY( OS::_DOUT("  Checking for file [%s] inside zip file [%s]", fname.c_str(), fip->name.c_str()); )
					if (unzLocateFile (fip->file, fname.c_str(), false) == UNZ_OK) {
						// found the file, now need to load it
						unz_file_info info;
						if (unzGetCurrentFileInfo(fip->file, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
							// allocate space to hold the file contents
							if (unzOpenCurrentFile(fip->file) == UNZ_OK) {
								len = info.uncompressed_size + 1;
								stringsFileData = (char*) std::malloc(len);
								if (stringsFileData) {
									unzReadCurrentFile(fip->file, stringsFileData, len-1);
									stringsFileData[len-1] = 0; // make sure it is nul terminated
								}
								unzCloseCurrentFile(fip->file);
							}
						}
					}
				}
			  #endif // PDG_NO_ZIP
			}
			if (stringsFileData) {
				ResourceManager::getStringFromDataBlock(stringsFileData, len, outStr, id, substring);
				if (outStr.length() != 0) {
					// we found the string in this strings.txt file, stop looking
					found = true;
				}
				if (!checkLang && fip->stringCache == 0) {
					DEBUG_ONLY( OS::_DOUT("Adding strings.txt file (size [%d]) to cache", 
						len); )
					fip->stringCache = stringsFileData;
					fip->stringCacheSize = len;
				} else if (checkLang && fip->stringLangCache == 0) {
					DEBUG_ONLY( OS::_DOUT("Adding strings-%s.txt file (size [%d]) to cache", 
						mLanguage.c_str(), len); )
					fip->stringLangCache = stringsFileData;
					fip->stringLangCacheSize = len;
				}
			}
			if (checkLang) {
				checkLang = false;
			} else {
				done = true;
			}
		}
		++p;
    }
    return outStr.c_str();
}


// find out how big a resource is
unsigned long  
ResourceManager::getResourceSize(const char* resourceName)
{
	unsigned long len = 0;
    std::vector<void*>::iterator p = mFiles.begin();
    bool found = false;
    while (!found && (p != mFiles.end())) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->isDir) {
			std::string s = fip->fullpath + "/" + resourceName;
			if (fileExists(s.c_str())) {
				len = fileSize(s.c_str());
				found = true;
			}
		}
     #ifndef PDG_NO_ZIP
		else if (fip->file) {
            if (unzLocateFile (fip->file, resourceName, false) == UNZ_OK) {
                // found the file, now need to load it
                unz_file_info info;
                if (unzGetCurrentFileInfo(fip->file, &info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
                    // allocate space to hold the file contents
                    len = info.uncompressed_size;
                    found = true;
                }
            }
        }
      #endif // PDG_NO_ZIP
        ++p;
    }
    return len;
}


// load a resource
bool    
ResourceManager::getResource(const char* resourceName, void* buffer, unsigned long bufferSize)
{	
    DEBUG_ASSERT(mFiles.size() > 0, "ResourceManager::getResource() called but no resources files open");
	bool ret = false;
    std::vector<void*>::iterator p = mFiles.begin();
    bool found = false;
    while (!found && (p != mFiles.end())) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->isDir) {
			std::string s = fip->fullpath + "/" + resourceName;
			if (fileExists(s.c_str())) {
				std::ifstream file;
				file.open(s.c_str(), std::ios::binary);
				file.seekg(0, std::ios::beg);
				file.read((char*)buffer, bufferSize);
				file.close();
				ret = true;
				found = true;
			}
		}
     #ifndef PDG_NO_ZIP
		else if (fip->file) {
            if (unzLocateFile (fip->file, resourceName, false) == UNZ_OK) {
                // found the file, now need to load it
                if (unzOpenCurrentFile(fip->file) == UNZ_OK) {
                    int bytesRead = unzReadCurrentFile(fip->file, buffer, bufferSize);
                    if ((unsigned long)bytesRead == bufferSize) {
						ret = true;
					} else {
						ret = false;
					}
					unzCloseCurrentFile(fip->file);
					found = true;
                }
            }
        }
      #endif // PDG_NO_ZIP
        ++p;
    }
    return ret;
}


// return a semicolon separated list of paths (directories and zip file names) that
// are currently open for loading resources
std::string	
ResourceManager::getResourcePaths() {
	std::string paths("");
    std::vector<void*>::iterator p = mFiles.begin();
    while (p != mFiles.end()) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (p != mFiles.begin()) {
    		paths.append(";");
    	}
    	if (std::strchr(fip->fullpath.c_str(), ' ')) {
    		// path has spaces in it, surround with quotes
    		paths.append("\"");
    		paths.append(fip->fullpath);
			if (fip->isDir) {
				paths.append("/");
			}
   			paths.append("\"");
    	} else {
    		paths.append(fip->fullpath);
			if (fip->isDir) {
				paths.append("/");
			}
    	}
//     	if (!fip->isDir() && !fileExists(fip->fullpath)) {
// 			paths.append(" <== MISSING ");
//     	}
        ++p;
    }
    std::replace( paths.begin(), paths.end(), '\\', '/');
    return paths;
}


// This is not terribly robust due to inherent count limitations and implementation choices, improve as you see fit
//
int
ResourceManager::snprintfLoc(char* dst, int maxlen, const char * fmt, ...)
{
	int result = SUCCESS;
	static const int MAX_ARGS = 10;

	//static char buf[2048];
	char* cptr[MAX_ARGS];

	std::va_list lst;
	va_start(lst, fmt);

	// Init the arg array
	for (int i=0; i<MAX_ARGS; i++) {
		cptr[i] = 0;
	}

	std::string outstr("");
	const char* cp = fmt, *sp = fmt;

	// Find all the % chars in the format str, and reposition args as necessary
	int numargs = 0;
	while (numargs < MAX_ARGS) {
		cp = std::strchr( cp, '%' );
		if (cp != 0) {
			int pos = (*++cp) - 1;
			if (std::isdigit(pos)) {
				uint8 idx = pos - 48;	// 48 is base for ASCII '0'
				if (idx < MAX_ARGS) {
					cptr[idx] = va_arg( lst, char * );
					++numargs;
				}
			}
		} else {
			break;
		}
	}
	va_end(lst);

	// Now, go back through again and assemble substrings and arguments
	sp = cp = fmt;
	for (int arg=0; arg<numargs; ++arg) {
		cp = std::strchr( cp, '%' );
		if (cp != 0) {
			int pos = (*++cp) - 1;
			if (std::isdigit(pos)) {
				uint8 idx = pos - 48;	// 48 is base for ASCII '0'
				if (idx < MAX_ARGS) {
					int len = cp - sp - 1;	// the length of the fmt substr prior to the '%'
					if (len > 0) {
						outstr.append( sp, len );	// Append the fmt str portion first..
					}
					sp = cp + 1;
					if (cptr[arg] != 0) {
						outstr.append( cptr[arg] );
					}
				}
			}
		}
	}

	if (*sp != 0) {				// if not already at end of format string
		outstr.append( sp );	// add on the remaining format chars
	}

	//int n = std::vsnprintf(buf, 2047, fmt, lst);
	//buf[n+1] = 0;

	std::strncpy( dst, outstr.c_str(), maxlen );
	return result;
}


// lifecycle
ResourceManager::ResourceManager() :
  mFiles(),
  mNumCachedImages(0)
{
    for (int i = 0; i < MAX_CACHED_IMAGES; i++) {
		mImagesNamesCache[i] = 0;
		mImagesCache[i] = 0;
		mImagesCacheHasLang[i] = false;
	}
}

ResourceManager::~ResourceManager() {
	const char* lang = mLanguage.c_str();
    // clean up cached images
    for (int i = 0; i < MAX_CACHED_IMAGES; i++) {
        if (mImagesNamesCache[i] && mImagesCache[i]) {  
            DEBUG_ONLY( OS::_DOUT("Releasing Image [%s] [%p] from cache", 
								  mImagesNamesCache[i], mImagesCache[i]); )
			Image* img = mImagesCache[i];
			char* name = mImagesNamesCache[i];
			img->release();
			std::free( name );  // this was malloc'd in getImage()
			mImagesNamesCache[i] = 0;
        }
    }
    // clean up cached string files and any files left open
    std::vector<void*>::iterator p = mFiles.begin();
    while (p != mFiles.end()) {
    	FileInfo* fip = (FileInfo*)*p;
    	if (fip->stringCache) {
            DEBUG_ONLY( OS::_DOUT("Freeing strings.txt file cache (size [%d])", fip->stringCacheSize); )
    		std::free((void*)fip->stringCache);
    	}
    	if (fip->stringLangCache) {
            DEBUG_ONLY( OS::_DOUT("Freeing strings-%s.txt file cache (size [%d])", lang, fip->stringLangCacheSize); )
    		std::free((void*)fip->stringLangCache);
    	}
#ifndef PDG_NO_ZIP
        if (fip->file) {
            unzClose(fip->file);
        }
#endif // PDG_NO_ZIP
		delete(fip);
		++p;
    }
    // clean up any strings still allocated
/*    std::vector<char*>::iterator p1 = strings.begin();
    while (p1 != strings.end()) {
        if (*p != 0) {
            std::free(*p);
            *p = 0;
        }
        ++p;
    } */
}

ResourceManager* ResourceManager::createSingletonInstance() {
	ResourceManager* resMgr = new ResourceManager();
	return resMgr;
}


} // end namespace pdg

