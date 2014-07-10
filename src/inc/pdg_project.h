// -----------------------------------------------
// pdg_project.h
// 
// Lets individual projects have custom blocks
// of code and build settings that don't affect other projects
//
// Written by Ed Zavada, 2004-2012
// Copyright (c) 2004-2012, Dream Rock Studios, LLC
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

#ifndef PDG_PROJECT_H_INCLUDED
#define PDG_PROJECT_H_INCLUDED

// to use this file, make a copy of it in your
// source directory such that it will be included
// instead of the original in framework/inc
// In your copy, customize the PDG_PROJECT define
// to your own project, and add any additional defines

// add additional author names to the global copy
// in framework/inc
#define PDG_PROJECT_NONE     	0
#define PDG_PROJECT_CATAN    	1
#define PDG_PROJECT_NODE_ADDON	2
#define PDG_PROJECT_NODE_APP	3
#define PDG_PROJECT_IOS_APP		4
#define PDG_PROJECT_IOS_JS_APP	5

// change this in your local copy or in your build settings to a valid project
#ifndef PDG_PROJECT
	#define PDG_PROJECT PDG_PROJECT_NONE
#endif

// in your local copy, add additional custom build 
// defines, etc.. here


#if (PDG_PROJECT == PDG_PROJECT_IOS_APP) || (PDG_PROJECT == PDG_PROJECT_IOS_JS_APP)
    #ifndef PDG_PROJECT_IOS_APP_SETTINGS_DEFINED
        #define PDG_PROJECT_IOS_APP_SETTINGS_DEFINED

        #define PDG_USE_CHIPMUNK_PHYSICS
        #define PDG_SCML_SUPPORT
        #define PDG_NO_64BIT
        #define PDG_NO_NETWORK		// TODO: remove when networking fixed
        #ifndef PLATFORM_IOS
            #define PLATFORM_IOS 1
        #endif
        #ifndef PDG_INTERNAL_LIB
            #define PDG_INTERNAL_LIB	// TODO: this shouldn't be necessary for a C++ build
        #endif
		
		#if (PDG_PROJECT == PDG_PROJECT_IOS_JS_APP)
			#define PDG_COMPILING_FOR_JAVASCRIPT
		#endif
    #endif
    #define PROJECT_IOS_APP_ONLY(what) what
#else
	#define PROJECT_IOS_APP_ONLY(what)
#endif

// TODO: use this rather that build flags
#if PDG_PROJECT == PDG_PROJECT_NODE_APP
// 	#define PDG_STANDALONE_NODE_APP
// 	#define PDG_LIBRARY
// 	#define PDG_COMPILING_FOR_JAVASCRIPT
// 	#define PDG_NO_SLEEP		// Node.js handles sleeping for us
// 	#define PDG_INTERNAL_LIB	// C++ PDG isn't exposed
// 	#define PDG_NO_NETWORK		// using Node.js's network stuff
	#define PROJECT_NODE_APP_ONLY(what) what
#else
	#define PROJECT_NODE_APP_ONLY(what)
#endif

#if defined( PDG_COMPILING_FOR_JAVASCRIPT ) || defined( PDG_COMPILING_FOR_RUBY )
	#define PDG_COMPILING_FOR_SCRIPT_BINDINGS
#endif

#endif // PDG_PROJECT_H_INCLUDED
