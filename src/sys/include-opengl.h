// -----------------------------------------------
// include_opengl.h
// 
// include open gl for various platforms and compilers
//
// Written by Ed Zavada, 2010-2012
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


#ifndef INCLUDE_OPENGL_H_INCLUDED
#define INCLUDE_OPENGL_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#ifdef PLATFORM_WIN32
  #include <windows.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
  #define glVertexColor4f glColor4f
#elif PLATFORM_MACOSX
  #ifdef PLATFORM_OPENGLES
    #include <OpenGLES/ES1/gl.h>
    #include "gles-hacks.h"
    #include "glues_quad.h"
  #else
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #define glVertexColor4f glColor4f
  #endif
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #define glVertexColor4f glColor4f
#endif




#endif
