// -----------------------------------------------
// userdata.cpp
// 
// Implementation of core application/system interface
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


#include "pdg_project.h"

#include "pdg/msvcfix.h"  // fix non-standard MSVC

#include "pdg/sys/userdata.h"
#include "pdg/sys/os.h"
#include "pdg/sys/refcounted.h"

#include <cstdlib>

#ifdef LEAK_AND_EXCEPTION_CHECKS
#include "..\LeakCheck\LeakCheck.h"
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define THIS_FILE __FILE__
#endif


namespace pdg {


UserData* UserData::makeUserDataViaCopy(void* ptr, long dataSize, UserFreeFuncT freeFunc) {
	UserData* userData = new UserData();
	userData->data = std::malloc(dataSize);
	std::memcpy(userData->data, ptr, dataSize);
	userData->freeFunc = freeFunc;
	userData->freeHow = data_Free;
	return userData;
}

UserData* UserData::makeUserDataWithCustomFree(void* ptr, UserFreeFuncT freeFunc) {
	UserData* userData = new UserData();
	userData->data = ptr;
	userData->freeFunc = freeFunc;
	userData->freeHow = data_CallUserFunc;
	return userData;
}

UserData* UserData::makeUserDataFromRefCountedObj(RefCountedObj* obj)  {
	UserData* userData = new UserData();
	userData->data = (void*) obj;
	obj->addRef();
	userData->freeFunc = NULL;
	userData->freeHow = obj_RefCountedRelease;
	return userData;
}

UserData* UserData::makeUserDataFromPointer(void* ptr, FreeDataT freeHow) {
	if (freeHow >= data_CallUserFunc) {
		DEBUG_ONLY( pdg::OS::_DOUT("Illegal mode [%d] for UserData::makeUserDataFromPointer()", freeHow) );
		return 0;
	}
	UserData* userData = new UserData();
	userData->data = ptr;
	userData->freeFunc = NULL;
	userData->freeHow = freeHow;
	return userData;
}

UserData::UserData() {
	freeHow = data_DoNothing;
    freeFunc = NULL;
    data = 0;
}

UserData::~UserData() {
	if (freeHow != data_DoNothing) {
		if (freeFunc != NULL) {
			freeFunc( data );
		}
        if (freeHow == data_Free) {
            std::free( data );
        } else if (freeHow == data_Delete) {
            delete (char*) data;
        } else if (freeHow == data_ArrayDelete) {
            delete[] (char*) data;
        } else if (freeHow == obj_RefCountedRelease) {
			RefCountedObj* obj = static_cast<RefCountedObj*>( data );
			obj->release();
		}
	}
}
	


} // end namespace pdg

