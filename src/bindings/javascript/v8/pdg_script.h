// -----------------------------------------------
// pdg_script.h
// 
// All inclusive include for JavaScript bindings
//
// Written by Ed Zavada, 2012-2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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


#ifndef PDG_SCRIPT_H_INCLUDED
#define PDG_SCRIPT_H_INCLUDED

#ifndef PDG_BUILDING_INTERFACE_FILES
#include "pdg_project.h"
#endif

// don't do anything unless we are actually targetting Javascript
#ifdef PDG_COMPILING_FOR_JAVASCRIPT

#include "v8/pdg_script_bindings.h"  // must include first

#include "v8/javascript_object_wrap.h"
#include "v8/pdg_v8_support.h"
#include "v8/pdg_js_macros.h"

#include "memblock.h"

#endif // PDG_COMPILING_FOR_JAVASCRIPT

#endif // PDG_SCRIPT_H_INCLUDED
