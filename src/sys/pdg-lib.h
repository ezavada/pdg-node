// -----------------------------------------------
// pdg-lib.h
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

#ifndef PDG_LIB_H_INCLUDED
#define PDG_LIB_H_INCLUDED

#include "pdg_project.h"

#if __cplusplus
extern "C" {
#endif

void pdg_LibSaveArgs(int argc, const char* argv[]);
int  pdg_LibGetArgc();
const char** pdg_LibGetArgv();
    
bool pdg_LibNeedsInit();
void pdg_LibInit();
    
void pdg_LibIdle();
void pdg_LibRun();

void pdg_LibQuit();
bool pdg_LibIsQuitting();

#if __cplusplus
}
#endif

#endif // PDG_LIB_H_INCLUDED
