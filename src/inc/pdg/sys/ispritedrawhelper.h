// -----------------------------------------------
// ispritedrawhelper.h
// 
// sprite drawing helper functionality
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


#ifndef PDG_SPRITE_DRAW_HELPER_H_INCLUDED
#define PDG_SPRITE_DRAW_HELPER_H_INCLUDED

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
 * Sprite Draw Helper
 *
 * Implement this interface to do extra drawing stuff for a specific Sprite
 * then start using it by calling the sprite's setDrawHelper() method.
 * You can only have one Sprite Draw Helper attached to a particular sprite
 * for normal drawing (before the sprite would normally draw) and one for 
 * post draw (after the sprite has been drawn)
 *
 * For those coding in Javascript, there is an implementation of ISpriteDrawHelper 
 * that maps a function definition to the draw call. So to create a helper:
 *
 *      var myHelper = new pdg.ISpriteDrawHelper(function(sprite, port) {
 *            console.log("in my sprite draw helper for " + sprite );
 *			  var r = sprite.getLayer().layerToPortRect(sprite.getFrameRotatedBounds());
 *            port.fillRect(r, "black");
 *            port.frameRect(r, "white");
 *            return false; // don't let sprite draw itself (ignored for post draw)
 *      });
 *		mySprite.setDrawHelper(myHelper);
 *
 * If you need something more complex, you can also use classify to create a new
 * Javascript class that derives from pdg.ISpriteDrawHelper, and it will call the 
 * draw() method of your class. For example:
 *
 *      classify(pdg.ISpriteDrawHelper, 'MyDrawHelperClass', function() {
 *			def('draw', function(sprite, port) {
 *            console.log("MyDrawHelperClass.draw(" + sprite + ")" );
 *            return true; // let sprite draw itself (ignored for post draw)
 *			});
 *		});
 *		mySprite.setDrawHelper( new MyDrawHelperClass() );
 */

class ISpriteDrawHelper : public ISerializable {
public: 
	
	SERIALIZABLE_TAG( CLASSTAG_DRAW_HELPER );

	// sprite is the Sprite object for which the helper is being invoked
	// port is the Port into which is should be drawn
    virtual bool draw(Sprite* sprite, Port* port) = 0;
    
    // returning true means Sprite should delete the helper when it removes it
    virtual bool ownedBySprite() { return true; }

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mISpriteDrawHelperScriptObj;
#endif

	ISpriteDrawHelper() {
    	#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
    		INIT_SCRIPT_OBJECT(mISpriteDrawHelperScriptObj);
    	#endif
	}

    virtual ~ISpriteDrawHelper() {
				#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
					CleanupISpriteDrawHelperScriptObject(mISpriteDrawHelperScriptObj);
				#endif
			}

	// your helper will need to implement these if you are going to serialize or deserializer helpers
	virtual uint32 getSerializedSize(pdg::ISerializer* serializer) const { return 0; }
	virtual void serialize(pdg::ISerializer* serializer) const {}
	virtual void deserialize(pdg::IDeserializer* deserializer) {}

};



} // end namespace pdg

#endif // PDG_SPRITE_DRAW_HELPER_H_INCLUDED

