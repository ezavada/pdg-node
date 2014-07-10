// -----------------------------------------------
// ianimationhelper.h
// 
// animation helper functionality
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


#ifndef PDG_ANIMATION_HELPER_H_INCLUDED
#define PDG_ANIMATION_HELPER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/serializable.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

class Animated;

/* -----------------------------------------------------------------------------------
 * Animation Helper
 *
 * Implement this interface to do extra animation stuff for a specific Animated object
 * then add by calling the object's addAnimationHelper() method
 * You can have multiple Animation Helpers attached to the same object.
 *
 * For those coding in Javascript, there is an implementation of IAnimationHelper 
 * that maps a function definition to the animate call. So to create a helper:
 *
 *      var myHelper = new pdg.IAnimationHelper(function(what, msElapsed) {
 *            console.log("in my animation helper for " + what + " after " + msElapsed + "ms" );
 *            return true; // not done, keep helping
 *      });
 *		myAnimatedObj.addAnimationHelper(myHelper);
 *
 * If you need something more complex, you can also use classify to create a new
 * Javascript class that derives from pdg.IAnimationHelper, and it will call the 
 * animated() method of your class. For example:
 *
 *      classify(pdg.IAnimationHelper, 'MyAnimationHelperClass', function() {
 *			def('animate', function(what, msElapsed) {
 *            console.log("MyAnimationHelper.animate(" + what + ", " + msElapsed + "ms)" );
 *            return false; // all done, delete the helper
 *			});
 *		});
 *		myAnimatedObj.addAnimationHelper( new MyAnimationHelperClass() );
 */

class IAnimationHelper : public ISerializable {
public: 
	
	SERIALIZABLE_TAG( CLASSTAG_ANIM_HELPER );

	// what is the Animated object for which normal animation has just completed
	// msElapsed is time (milliseconds) since last call to animate
	// return true if this helper should continue to be used, false
	// if it should be removed from the helper list
    virtual bool animate(Animated* what, uint32 msElapsed) = 0;
    
    // returning true means Animated should delete the helper when it removes it
    // from the helper list
    virtual bool ownedByAnimated() { return true; }

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mIAnimationHelperScriptObj;
#endif

	IAnimationHelper() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mIAnimationHelperScriptObj);
    	#endif
	}

    virtual ~IAnimationHelper() {
				#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
					CleanupIAnimationHelperScriptObject(mIAnimationHelperScriptObj);
				#endif
			}

	// your helper will need to implement these if you are going to serialize or deserializer helpers
	virtual uint32 getSerializedSize(pdg::ISerializer* serializer) const { return 0; }
	virtual void serialize(pdg::ISerializer* serializer) const {}
	virtual void deserialize(pdg::IDeserializer* deserializer) {}

};



} // end namespace pdg

#endif // PDG_ANIMATION_HELPER_H_INCLUDED

