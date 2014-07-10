// -----------------------------------------------
// pdg-lib.cpp
// 
// library entry points
//
// Written by Ed Zavada, 2009-2012
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

#include "pdg-lib.h"
#include "pdg-main.h"
#include "internals.h"

#include <iostream>
#include <cstdlib>

#define MAX_ARGS 100

bool gPDG_IsInitialized = false;
bool gPDG_Quitting = false;
bool gPDG_InRunLoop = false;
int  gPDG_argc = 0;
const char* gPDG_argv[MAX_ARGS];
bool gPDG_haveArgs = false;

void pdg_LibSaveArgs(int argc, const char* argv[]) {
    gPDG_argc = (argc < MAX_ARGS) ? argc : MAX_ARGS;
    for (int i = 0; i<MAX_ARGS; i++) {
        if (i < argc) {
            gPDG_argv[i] = argv[i];
        } else {
            gPDG_argv[i] = 0;
        }
    }
    gPDG_haveArgs = true;
}

int  pdg_LibGetArgc() {
    return gPDG_argc;
}
const char** pdg_LibGetArgv() {
    return gPDG_argv;
}


bool pdg_LibNeedsInit() {
    return !gPDG_IsInitialized;
}

void pdg_LibInit() {
    if (!gPDG_haveArgs) {
		std::cerr << "Fatal Error: pdg_LibInit() called before pdg_LibSaveArgs()";
		exit(1);
    }
	int err = pdg::main_initManagers();
	if (err) {
		std::cerr << "Fatal Error: Could not initialize PDG Managers";
		exit(err);
	}
    pdg::main_initKeyStates();
	pdg::platform_setupDirectories(gPDG_argc, gPDG_argv);
	pdg::platform_init(gPDG_argc, gPDG_argv);
	gPDG_IsInitialized = true;
	gPDG_Quitting = false;
	gPDG_InRunLoop = false;
}

void pdg_LibIdle() {
    if (!gPDG_IsInitialized) {
    	pdg_LibInit();
	}
	if (gPDG_IsInitialized && !gPDG_Quitting) {
		pdg::main_run();
		pdg::platform_pollEvents();
	}
}

void pdg_LibRun() {
    if (!gPDG_IsInitialized) {
    	pdg_LibInit();
	}
	gPDG_InRunLoop = true;
	do {
		pdg::main_run();
		pdg::platform_pollEvents();
	} while (!gPDG_Quitting);
    pdg::main_cleanup();
    pdg::platform_cleanup();
}

void pdg_LibQuit() {
	gPDG_Quitting = true;
	if (!gPDG_InRunLoop) {	// run loop calls this itself
    	pdg::main_cleanup();
    	pdg::platform_cleanup();
    }
}

bool pdg_LibIsQuitting() {
	return gPDG_Quitting;
}

