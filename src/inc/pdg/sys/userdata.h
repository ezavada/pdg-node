// -----------------------------------------------
// userdata.h
//
// a method for storing data that the user wants
// to associate with a framework object but that
// needs to be freed or managed by the framework
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


#ifndef PDG_USERDATA_H_INCLUDED
#define PDG_USERDATA_H_INCLUDED

#include "pdg_project.h"

namespace pdg {

class RefCountedObj;

typedef void (*UserFreeFuncT)(void* data);

// these define how to handle user data when the objects like timers or event data that hold them are
enum FreeDataT {
	data_DoNothing = 0,     // don't do anything with the user data
	data_Free = 1,          // call std::free() on the user data
	data_Delete = 2,        // cast to char* then call delete on the user data
	data_ArrayDelete = 3,   // call delete[] on the user data
	data_CallUserFunc = 4,	// call a userDefined function in form of void (UserFreeFunc*)(void* data)
	obj_RefCountedRelease = 5  // cast to pdg::RefCountedObj and then call obj->release()
};

// this class is used to wrap data or objects that are to be associated with a low level PDG object
class UserData {
public:
	template <class T> T* getData() { return static_cast<T*>(data); }
	void* getData() { return data;}
	void release() { delete this; }

	// makes a malloc'd copy of the data block, with an optional custom function to free or
	// release data or objects pointed to within that data. The custom free function must *NOT*
	// free the master pointer, however, that is taken care of automatically
	static UserData* makeUserDataViaCopy(void* ptr, long dataSize, UserFreeFuncT freeFunc = 0);

	// stores the pointer, and calls a custom function to free it
	// for example, this could be used for a pointer to an object Foo, and the custom free
	// function would typecast the pointer back to a Foo* then delete it.
	static UserData* makeUserDataWithCustomFree(void* ptr, UserFreeFuncT freeFunc);

	// stores the reference to the RefCountedObject and immediately calls obj->addRef() on it
	// when the UserData is to be freed, it calls obj->release()
	static UserData* makeUserDataFromRefCountedObj(RefCountedObj* obj);

	// stores the pointer, then uses one of the following methods to free it
	//   data_DoNothing -- doesn't attempt to free it, use this for static data or data freed elsewhere
	//   data_Free -- calls std::free() on the pointer
	//   data_Delete -- calls (char*) delete
	//   data_ArrayDelete -- calls (char*) delete[]
	// no other modes are allowed
	static UserData* makeUserDataFromPointer(void* ptr, FreeDataT freeHow = data_Free);

protected:
	UserData();
	~UserData();
	FreeDataT		freeHow;
	UserFreeFuncT	freeFunc;
	void*			data;
};


} // end namespace pdg


#endif // PDG_CORE_H_INCLUDED

