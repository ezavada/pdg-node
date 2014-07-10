// -----------------------------------------------
// tilelayer.h
// 
// sprite functionality
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


#ifndef PDG_TILELAYER_H_INCLUDED
#define PDG_TILELAYER_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/spritelayer.h"

#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
#include "pdg_script_bindings.h"
#endif

namespace pdg {

class SpriteManager;
class Image;

// -----------------------------------------------------------------------------------
// Tile Layer
// Efficiently handles large tile datasets, useful for backgrounds, terrains, etc...
// -----------------------------------------------------------------------------------

class TileLayer : public SpriteLayer {
friend class SpriteManager;
public:
	
	SERIALIZABLE_TAG( CLASSTAG_TILE_LAYER )
	SERIALIZABLE_METHODS()

	enum TFacing {
		facing_North = 0,
		facing_East = 64,
		facing_South = 128,
		facing_West = 192,
		facing_Ignore = 256,
		
		flipped_None = 0,
		flipped_Horizontal = 64,
		flipped_Vertical = 128,
		flipped_Both = 192,
		flipped_Ignore = 256
	};
	
	virtual void	setWorldSize(long width, long height, bool repeatingX = false, bool repeatingY = false);
	virtual Rect	getWorldSize();
	virtual Rect	getWorldBounds();  // size * tile size (no zoom)
	virtual void	defineTileSet(int tileWidth, int tileHeight, Image* tiles, bool hasTransparency = true, bool flipTiles = false);
	virtual void    loadMapData(uint8* dataPtr, long mapWidth = 0, long mapHeight = 0, long dstX = 0, long dstY = 0);
	virtual uint8*	getMapData(long mapWidth = 0, long mapHeight = 0, long srcX = 0, long srcY = 0);
	virtual Image*	getTileSetImage();
	virtual Point	getTileSize() const;
	
	virtual uint8   getTileTypeAt(long x, long y, TFacing* outFacing = 0) const;
	virtual void	setTileTypeAt(long x, long y, uint8 t, TFacing facing = facing_Ignore);
	
	// checks to see if the movingSprite collides with any features of the spriteLayer.  
	//	Determines the number of pixels that overlap based on the specified alphaThreshold
	virtual uint32    checkCollision(Sprite *movingSprite, uint8 alphaThreshold = 128, bool shortCircuit = true, float *outCollisionMag = 0) const;
	
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	SCRIPT_OBJECT_REF mTileLayerScriptObj;
#endif

#ifndef PDG_INTERNAL_LIB
protected:
#endif
/// @cond C++
#ifndef PDG_NO_GUI
	TileLayer(Port* port);
#endif // ! PDG_NO_GUI
	TileLayer();
	virtual ~TileLayer();

	// these need to be implemented to override SpriteLayer
#ifndef PDG_NO_GUI
	virtual void drawLayer();
#endif // ! PDG_NO_GUI
	virtual void animateLayer(long msElapsed);

	Image*  mTiles;
	uint8*  mTileData;
	uint32	mDataSize;
	int		mSrcTileWidth;
	int		mSrcTileHeight;
	int		mSrcTileCountX;
	int		mSrcTileCountY;
	long	mWorldHeight;
	long	mWorldWidth;
	bool	mRepeatingX;
	bool	mRepeatingY;
	bool	mHasTransparency;
	bool    mUseFacing;
	bool	mUseFlipping;
	bool	mFlipHoriz;
	bool	mFlipVert;
	uint32  mMipMode;
	float	mTileWorldRatioX;
	float	mTileWorldRatioY;
	float	mPixelWorldRatioX;
	float	mPixelWorldRatioY;
	
	// DEBUG STUFF
public:
	float rec_PixelXOffset;
	float rec_PixelYOffset;

/// @endcond
};

// create a Sprite Layer object
// will create own event manager and timer manager if none given
#ifndef PDG_NO_GUI
TileLayer* createTileLayer(Port* port);
#endif // ! PDG_NO_GUI	
TileLayer* createTileLayer();


} // end namespace pdg

#endif // PDG_TILELAYER_H_INCLUDED
