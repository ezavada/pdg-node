// -----------------------------------------------
// singleton.h
// 
// utility template for implementing the singleton pattern
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


#ifndef PDG_SINGLETON_H_INCLUDED
#define PDG_SINGLETON_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/platform.h"

#include <cassert>

namespace pdg {

// singleton template

template <class T >
class Singleton {
public:
    static T*  getSingletonInstance() {
            if (sInstance == 0) {   // if we haven't created the singleton, do it now
                sInstance = T::createSingletonInstance();
            }
            assert(sInstance != 0); // make sure creation worked
            return sInstance; 
        } 
    // check to see if the instance has been created
    inline static bool hasInstance() {
        return sInstance != 0;
    }
    // Use this when you know the instance has already been created
    inline static T&  instance() {
			return *sInstance;
        }
    static void setInstance(T* i) {
            sInstance = i;
        }
    Singleton() { sInstance = static_cast<T*>(this); }  // assign the static instance variable
    virtual ~Singleton() { sInstance = 0; }  // clear static instance variable
    static T* createSingletonInstance();  // implement this to create the instance
private:
    static T* sInstance;
};
template <class T> T* Singleton<T>::sInstance = 0; // initialize the static instance variable

} // end namespace pdg


#endif // PDG_SINGLETON_H_INCLUDED
