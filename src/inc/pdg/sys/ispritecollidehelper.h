// -----------------------------------------------
// ispritecollidehelper.h
// 
// sprite collision helper functionality
//
// Written by Ed Zavada, 2012
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


#ifndef PDG_SPRITE_COLLIDE_HELPER_H_INCLUDED
#define PDG_SPRITE_COLLIDE_HELPER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/global_types.h"
#include "pdg/sys/coordinates.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

class Sprite;
class Port;

/* -----------------------------------------------------------------------------------
 * Sprite Collide Helper
 *
 * Implement this interface to handle the deciding which sprites should collide
 * with one another. For example, if you have bullet sprites fired by enemies that should 
 * collide with your player sprites but not with other enemy sprites you would implement
 * this helper and attach it to all the sprites so it can return true if it is an
 * enemy bullet about to collide with a player sprite, and false otherwise. 
 *
 * For those coding in Javascript, there is an implementation of ISpriteCollideHelper 
 * that maps a function definition to the draw call. So to create a helper:
 *
 *      var myHelper = new pdg.ISpriteCollideHelper(function(sprite, withSprite) {
 *            console.log("in my sprite collide helper for " + sprite );
 *			  if (sprite.id == BULLET_SPRITE && withSprite.id == PLAYER_SPRITE) return true;
 *            return false; // don't let collision happen
 *      });
 *		mySprite.setCollideHelper(myHelper);
 *
 * If you need something more complex, you can also use classify to create a new
 * Javascript class that derives from pdg.ISpriteCollideHelper, and it will call the 
 * allowCollision() method of your class. For example:
 *
 *      classify(pdg.ISpriteCollideHelper, 'MyCollideHelperClass', function() {
 *			def('allowCollision', function(sprite, withSprite) {
 *            console.log("MyCollideHelperClass.allowCollision(" + sprite + ")" );
 *            return true; // let collision happen (ignored for post draw)
 *			});
 *		});
 *		mySprite.setCollideHelper( new MyCollideHelperClass() );
 */

class ISpriteCollideHelper : public ISerializable {
public: 
	
	SERIALIZABLE_TAG( CLASSTAG_COLLIDE_HELPER );

	// a moving sprite has just come into contact with another sprite
	// return true to allow the collision to happen, false if they shouldn't
	// collide
    virtual bool allowCollision(Sprite* sprite, Sprite* withSprite) = 0;
    
    // returning true means Sprite should delete the helper when it removes it
    virtual bool ownedBySprite() { return true; }

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mISpriteCollideHelperScriptObj;
#endif

	ISpriteCollideHelper() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mISpriteCollideHelperScriptObj);
    	#endif
	}

    virtual ~ISpriteCollideHelper() {
				#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
					CleanupISpriteCollideHelperScriptObject(mISpriteCollideHelperScriptObj);
				#endif
			}

	// your helper will need to implement these if you are going to serialize or deserializer helpers
	virtual uint32 getSerializedSize(pdg::ISerializer* serializer) const { return 0; }
	virtual void serialize(pdg::ISerializer* serializer) const {}
	virtual void deserialize(pdg::IDeserializer* deserializer) {}

};



} // end namespace pdg

#endif // PDG_SPRITE_COLLIDE_HELPER_H_INCLUDED

