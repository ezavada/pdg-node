// -----------------------------------------------
// tilelayer.cpp
// 
// tile functionality implementation
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

#include "pdg/sys/os.h"
#include "pdg/sys/tilelayer.h"
#include "pdg/sys/sprite.h"
#include "pdg/sys/iserializer.h"
#include "pdg/sys/ideserializer.h"

#include "spritemanager.h"
#include "image-impl.h"
#include "collisiondetection.h"

#ifndef PDG_NO_GUI
#include "include-opengl.h"
#include "image-opengl.h"
#include "graphics-opengl.h"
#endif // ! PDG_NO_GUI

#include <cstdlib>

//#define TILING_INTERNAL_DEBUG 1

#define PDG_TILE_LAYER_MAGIC_NUMBER    	0x10959843

#ifndef PDG_UNSAFE_SERIALIZATION
#define PDG_TAG_SERIALIZED_DATA
#endif

namespace pdg {

uint32 TileLayer::getSerializedSize(ISerializer* serializer) const {
	uint32 totalSize = 0;
#ifdef PDG_TAG_SERIALIZED_DATA
	totalSize += 4;  // size of request magic number
#endif
//	totalSize += 4; // size of action type (2), seat (1), and undoable (1)
	return totalSize;
}


void TileLayer::serialize(ISerializer* serializer) const {
#ifdef PDG_TAG_SERIALIZED_DATA
	serializer->serialize_4(PDG_TILE_LAYER_MAGIC_NUMBER);
#endif
}


void TileLayer::deserialize(IDeserializer* deserializer) {
#ifdef PDG_TAG_SERIALIZED_DATA
	uint32 tag = deserializer->deserialize_4();
	DEBUG_ASSERT(tag == PDG_TILE_LAYER_MAGIC_NUMBER, "OUT OF SYNC: expected tag for Tile Layer object");
#endif
}

void	
TileLayer::setWorldSize(long width, long height, bool repeatingX, bool repeatingY) {
	// save the world size and repeating flags
	mWorldWidth = width;
	mWorldHeight = height;
	mRepeatingX = repeatingX;
	mRepeatingY = repeatingY;
	
	mDataSize = mWorldWidth * mWorldHeight;
	
	// allocate a datablock for the layer
	mTileData = (uint8*) std::malloc( mDataSize );
	std::memset(mTileData, 0, mDataSize);
}

Rect
TileLayer::getWorldBounds() {
	Rect worldRect(mWorldWidth * mSrcTileWidth, mWorldHeight * mSrcTileHeight);
	return worldRect;
}

Rect
TileLayer::getWorldSize() {
	return Rect(mWorldWidth, mWorldHeight);
}
	
void
TileLayer::defineTileSet(int tileWidth, int tileHeight, Image* tiles, bool hasTransparency, bool flipTiles) {
	// save tile width and height
	mSrcTileWidth = tileWidth;
	mSrcTileHeight = tileHeight;
	mHasTransparency = hasTransparency;
  #ifndef PDG_NO_GUI
	mMipMode = GL_NEAREST;
  #endif // ! PDG_NO_GUI
	
	// calc and save number of tiles based on how many fit in image
	mSrcTileCountX = tiles->getWidth() / mSrcTileWidth;
	mSrcTileCountY = tiles->getHeight() / mSrcTileHeight;
	
	mUseFacing = !flipTiles && ((mSrcTileCountX * mSrcTileCountY) <= 64);
	
	mUseFlipping = flipTiles;
	if (!mUseFlipping) {
		mFlipHoriz = false;
		mFlipVert = false;
	} else {
		if (mSrcTileCountX <= 8) {
			mFlipHoriz = true;
		}
		if (mSrcTileCountY <= 8) {
			mFlipVert = true;
		}
		if (!mFlipVert && !mFlipHoriz) {
			mUseFlipping = false;
		}
	}

	// save ratio for drawing layer
	mTileWorldRatioX = (float)1/mSrcTileCountX;
	mTileWorldRatioY = (float)1/mSrcTileCountY;
	mPixelWorldRatioX = (float)1/tiles->getWidth();
	mPixelWorldRatioY = (float)1/tiles->getHeight();
	
	// issue debug warning if tiles don't fit evenly into image
	DEBUG_ONLY( 
	   if ( tiles->getWidth() % mSrcTileWidth != 0 ) {
		   DEBUG_PRINT("TileSet image width %d not evenly divisible by tile width %d",tiles->getWidth(), mSrcTileWidth);
	   }
	   if ( tiles->getHeight() % mSrcTileHeight != 0 ) {
		   DEBUG_PRINT("TileSet image height %d not evenly divisible by tile height %d", tiles->getHeight(), mSrcTileHeight);
	   }
	)
	
	// save image
	mTiles = tiles;
	mTiles->addRef();
	mTiles->setEdgeClamping(true);
}

void
TileLayer::loadMapData(uint8* dataPtr, long mapWidth, long mapHeight, long dstX, long dstY) {
	// copy the data into the allocated block, row by row, starting at dstX, dstY
	// and taking mapWidth as row length to copy and mapHeight as number of rows to copy
	/*
	long length = mapWidth - dstX;
	uint8* ptr = mTileData + dstX;
	for (int i=dstY; i<mapHeight; i++) {
		memcpy(ptr, dataPtr, length);
		ptr += mapWidth;
	}
	*/
	
	memcpy(mTileData, dataPtr, mapWidth*mapHeight);
}

uint8*
TileLayer::getMapData(long mapWidth, long mapHeight, long srcX, long srcY) {
	// allocate a buffer and copy the rows of data starting at dstX, dstY into it
	// taking mapWidth as row length to copy and mapHeight as number of rows to copy
	// return NIL if the copy would go outside the bounds of the currently allocated world
	
	// one byte for each tile
	uint8 * data = (uint8 *) malloc(mapWidth*mapHeight);
	memcpy(data, mTileData, mapWidth*mapHeight);
	return data;
}

uint8
TileLayer::getTileTypeAt(long x, long y, TFacing* outFacing) const {
	// look up tile at specific position and return it's value
	if (outFacing) {
		*outFacing = facing_North;
	}
	if (mRepeatingX) {
		x = (x % mWorldWidth);
		if (x < 0) {
			x += mWorldWidth;
		}
	} else if ( (x < 0) || (x >= mWorldWidth) ) return 0;
	
	if (mRepeatingY) {
		y = (y % mWorldHeight);
		if (y < 0) {
			y += mWorldHeight;
		}
	} else if ( (y < 0) || (y >= mWorldHeight) ) return 0;
	
	uint32 index = x + (y  * mWorldWidth);
	
	if (index > mDataSize) return 0;
	uint8 t = mTileData[index];
	if (outFacing) {
		if (mUseFacing) {
			*outFacing = (TFacing)(t - (t & 0x3F));
		}
		if (mUseFlipping) {
			if (mFlipVert && mFlipHoriz) {
				*outFacing = (TFacing)(t - (t & 0x3F));
			} else {
				if (mFlipHoriz) {
					*outFacing = (TFacing)((t - (t & 0x7F))>>1);
				} else {
					*outFacing = (TFacing)(t - (t & 0x7F));
				}
			}
		}
	}
	return t;
}

void
TileLayer::setTileTypeAt(long x, long y, uint8 t, TFacing facing) {
	// set tile at specific position
	// be sure not to write outside our allocated data
	if (mRepeatingX) {
		x = (x % mWorldWidth);
		if (x < 0) {
			x += mWorldWidth;
		}
	} else if ( (x < 0) || (x >= mWorldWidth) ) return;
	
	if (mRepeatingY) {
		y = (y % mWorldHeight);
		if (y < 0) {
			y += mWorldHeight;
		}
	} else if ( (y < 0) || (y >= mWorldHeight) ) return;
	
	uint32 index = x + (y  * mWorldWidth);
	
	if (index > mDataSize) return;
	if (facing == facing_Ignore || (!mUseFacing && !mUseFlipping)) {
		// no flipping or facing
		mTileData[index] = t;
	} else if (mUseFacing || (mUseFlipping && mFlipVert && mFlipHoriz)) {
		// using facing or both horizontal and vertical flipping
		mTileData[index] = (t & 0x3F) + (facing & 0xC0);
	} else if (mUseFlipping) {
		// using one direction flipping only
		if (mFlipHoriz) {
			mTileData[index] = (t & 0x7F) + ((facing & 0x40) << 1);
		} else {
			mTileData[index] = (t & 0x7F) + (facing & 0x80);
		}
	}
}

uint32    
TileLayer::checkCollision(Sprite *movingSprite, uint8 alphaThreshold, bool shortCircuit, float *outCollisionMag) const {
	// get the sprite bounding box
	Sprite* inSprite = movingSprite;
	ImageImpl* spriteImage = inSprite->mFrames[inSprite->mCurrFrame].image;
	ImageImpl* spriteMaskImage = inSprite->mFrames[inSprite->mCurrFrame].collisionMask;
	if (spriteMaskImage == 0) {
		spriteMaskImage = spriteImage;
	}
//	Rect spriteRect = inSprite->mScaledFrameBounds;
	
	RotatedRect rotated = inSprite->getFrameRotatedBounds(inSprite->mCurrFrame);
    //(spriteRect, inSprite->getRotation());
	//rotated.setCenterOffset(Point(inSprite->mFrames[inSprite->mCurrFrame].centerOffsetX * z, inSprite->mFrames[inSprite->mCurrFrame].centerOffsetY * z));

//	float z = mZoom; // adjust for tile layer zoom

	Rect spriteMaskRect = spriteMaskImage->getImageBounds();
	spriteMaskRect.moveRight(inSprite->mCurrFrame * spriteMaskImage->getWidth());
	
	const pdg::Point tileSize = getTileSize();
	const float tWidth = tileSize.x; // * z;
	const float tHeight = tileSize.y; // * z;
	// calc rect of tiles to check
	Rect tileOverlap = rotated.getBounds();
	// expand the overlap area to be on a tileSize boundary
	const int left = int(tileOverlap.left / tWidth) * int(tWidth);
	const int top = int(tileOverlap.top / tHeight) * int(tHeight);
	// output data
	uint32 totalCollisionPts = 0;
	uint32 collisionPts = 0;
	uint32* collisionPtsPtr = (shortCircuit) ? 0 : &collisionPts;
	if (outCollisionMag) {
		*outCollisionMag = 0.0f;
	}
	float collisionMag = 0.0f;
	float* collisionMagPtr = (shortCircuit) ? 0 : &collisionMag;
	// now check the tiles we intersect
	for(int l = left; l < tileOverlap.right; l += tWidth) {
		for( int t = top; t < tileOverlap.bottom; t += tHeight) {
			TileLayer::TFacing facing;
			uint8 val = getTileTypeAt(l / tWidth, t / tHeight, &facing);
			if (val == 0) continue; // val == 0 is an empty tile, so don't collide against it
			if (mUseFacing && ((val & 63) == 0)) continue; // these are also empty tiles when using facing or flipping
			// assemble parameters
			Rect checkRect(Point(l, t), tWidth, tHeight);
			int tileRow = (val / mSrcTileCountX) % mSrcTileCountY;
			int tileCol = val % mSrcTileCountX;
			
			Rect tileImageRect;
			tileImageRect.top = tileRow * mSrcTileHeight; // * mZoom;
			tileImageRect.bottom = tileImageRect.top + mSrcTileHeight; // * mZoom;
			tileImageRect.left = tileCol * mSrcTileWidth; // * mZoom;
			tileImageRect.right = tileImageRect.left + mSrcTileWidth; // * mZoom;
			
			if (true == CollisionDetection::detectPixelCollision(rotated, checkRect, spriteMaskImage, mTiles, spriteMaskRect, 
																 tileImageRect, facing, alphaThreshold, collisionPtsPtr, collisionMagPtr) ) {
				if (shortCircuit) return 1;
				totalCollisionPts += collisionPts;
				*outCollisionMag += collisionMag;
			}
		}
	}
	return totalCollisionPts;
}

//#define ADJUST(n) floor(n)
//#define ADJUST(n) roundf(n)
#define ADJUST(n) n

#ifndef PDG_NO_GUI
void 
TileLayer::drawLayer() {
	if (mHidden) return;
	// calculate number of cells that are visible
	Rect drawRect = mPort->getClipRect();
	if (drawRect.empty()) { 
		drawRect = mPort->getDrawingArea();
	}

    Point origin(-mLocation.x, -mLocation.y);
    origin *= mZoom;
    origin -= mOrigin;

	float dstTileWidth = mSrcTileWidth * mZoom;
	float dstTileHeight = mSrcTileHeight * mZoom;
	int dstTileCountX = 2 + drawRect.width() / dstTileWidth;
	int dstTileCountY = 2 + drawRect.height() / dstTileHeight;
		
	int dataXOffset = origin.x / dstTileWidth;
	int dataYOffset = origin.y / dstTileHeight;

	// do we use automatic N/S/E/W facing of tiles?
	bool useFacing = mUseFacing;
	bool flipHorizOnly = mUseFlipping &&  mFlipHoriz && !mFlipVert;
	bool flipVertOnly  = mUseFlipping && !mFlipHoriz &&  mFlipVert;
	bool flipBoth      = mUseFlipping &&  mFlipHoriz &&  mFlipVert;
	uint8 maxUnalteredVal = 0xFF;
	if (useFacing || flipBoth) {
		maxUnalteredVal = 0x3F;
	} else if (flipHorizOnly || flipVertOnly) {
		maxUnalteredVal = 0x7F;
	}
	
	float pixelXOffset = fmodf(origin.x, dstTileWidth);
	float pixelYOffset = fmodf(origin.y, dstTileHeight);
	
	rec_PixelXOffset = pixelXOffset;
	rec_PixelYOffset = pixelYOffset;
	
	static int maxVertexCount = 12000;  // this is what has been observed on the iPad
	static int maxIndexCount = 4000;	// might be able to scale this down for iPhone
	
	static GLfloat* vertexArray = 0;
	static int	  vertexArraySize = 0;
	if (!vertexArray || (vertexArraySize < maxVertexCount)) {
		if (vertexArray) {
			std::free(vertexArray);
		}
		// we have both 2 components (x & y) for each of two elements (vertex coordinates and texture coordinates)
		vertexArraySize = maxVertexCount + 400;  // 100 extra vertex elements so we don't have to keep reallocating
		vertexArray = (GLfloat*) std::malloc( vertexArraySize * sizeof(GLfloat) );
		DEBUG_PRINT("Reallocated tile vertex array to %d elements", vertexArraySize);
	}
	// check/reallocate space for those cells in index array
	static GLushort* indexArray = 0;
	static int		 indexArraySize = 0;
	if (!indexArray || (indexArraySize < maxIndexCount)) {
		if (indexArray) {
			std::free(indexArray);
		}
		indexArraySize = maxIndexCount + 200;  // 100 extra indices so we don't have to keep reallocating
		indexArray = (GLushort*)std::malloc(indexArraySize * sizeof(GLushort));
		DEBUG_PRINT("Reallocated tile index array to %d elements", indexArraySize);
	}
	
	// build vertex array
	int i = 0;
	int j = 0;

	bool haveNonEmptyTile = !mHasTransparency;  // if we have transparency, we need to look for non empty tiles
	bool haveTiledRow = false;
	uint8* dataEnd = mTileData + mWorldWidth * mWorldHeight;

    
	// specify vertex & texture coordinate positions
	int startY = -1; //(origin.y < 0) ? -1 : 0;
	int maxY = dstTileCountY; //(origin.y < 0) ? dstTileCountY - 1 : dstTileCountY;
	int vertexPtr = 0;
	for (int ty = startY; ty < maxY; ty++) {
		float y = ADJUST((float)ty * dstTileHeight - pixelYOffset);
		float y2 = ADJUST(y + dstTileHeight);
		TileLayer::TFacing facing;
		int startX = -1; //(origin.x < 0) ? -1 : 0;
		int maxX = dstTileCountX; //(origin.x < 0) ? dstTileCountX - 1 : dstTileCountX;
		float v1,h1,v2,h2,v3,h3,v4,h4;
		uint8* dataRowPtr = (uint8*)mTileData + ((ty + dataYOffset) * mWorldWidth);
		bool inEmptyRegion = false;
		bool emptyRow = mHasTransparency;  // we only look for empty rows if we have transparency
		for (int tx = startX; tx < maxX; tx++) {
			float x = ADJUST((float)tx * dstTileWidth - pixelXOffset);
			float x2 = ADJUST(x + dstTileWidth);
			bool needTopLeftOnly = false;
			uint8* dp = dataRowPtr + tx + dataXOffset;
			uint8 t = ( (dp < mTileData) || (dp > dataEnd) ) ? 0 : *dp; 
			if ((t == 0) && mHasTransparency) {
				needTopLeftOnly = (!emptyRow && !inEmptyRegion);
				inEmptyRegion = true;
				if (!needTopLeftOnly) {
					continue;
				}
			}
#ifdef TILING_INTERNAL_DEBUG
			Rect r;
			r.top = y;
			r.bottom = y2;
			r.left = x;
			r.right = x + dstTileWidth;
			mPort->frameRect(r, Color(1.0f,1.0f,1.0f,0.25f));
//            continue;
#endif
			int dx;
			int dy;
			
			if (t <= maxUnalteredVal) {
				
				dx = t % mSrcTileCountX;
				dy = t / mSrcTileCountX;
				
				v1 = (float)dy * mTileWorldRatioY;
				h1 = (float)dx * mTileWorldRatioX;
				v2 = v1;
				h2 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
				v3 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
				h3 = h1;
				v4 = v3;
				h4 = h2;
			}
			else {
				if (useFacing || flipBoth) {
					facing = (TFacing)(t & 0xC0);
					t = t & 0x3F;
				} else if (flipHorizOnly || flipVertOnly) {
					facing = (TFacing)(t & 0x80);
					t = t & 0x7f;
				}
				
				dx = t % mSrcTileCountX;
				dy = t / mSrcTileCountX;
				
				if (useFacing) {
					// facing cases
					if (facing == facing_South) {
						v1 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h1 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v2 = v1;
						h2 = (float)(dx) * mTileWorldRatioX;
						v3 = (float)(dy) * mTileWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
					}
					else if (facing == facing_East) {
						v1 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h1 = (float)dx * mTileWorldRatioX;
						v2 = (float)(dy) * mTileWorldRatioY;
						h2 = h1;
						v3 = v1;
						h3 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v4 = v2;
						h4 = h3;
					}
					else if (facing == facing_West) {
						v1 = (float)dy * mTileWorldRatioY;
						h1 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v2 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h2 = h1;
						v3 = v1;
						h3 = (float)dx * mTileWorldRatioX;
						v4 = v2;
						h4 = h3;
					}
				} else {
					// flipping cases
					if (flipHorizOnly) {
						
						v1 = (float)dy * mTileWorldRatioY;
						h1 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v2 = v1;
						h2 = (float)dx * mTileWorldRatioX;
						v3 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
						
					} else if (flipVertOnly) {
						
						v1 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h1 = (float)dx * mTileWorldRatioX;
						v2 = v1;
						h2 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v3 = (float)dy * mTileWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
						
					} else if (facing == flipped_Horizontal) {
						
						v1 = (float)dy * mTileWorldRatioY;
						h1 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v2 = v1;
						h2 = (float)dx * mTileWorldRatioX;
						v3 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
						
					} else if (facing == flipped_Vertical) {

						v1 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h1 = (float)dx * mTileWorldRatioX;
						v2 = v1;
						h2 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v3 = (float)dy * mTileWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
						
					} else {
						// flipped both ways
						v1 = (float)((dy+1) * mTileWorldRatioY) - mPixelWorldRatioY;
						h1 = (float)((dx+1) * mTileWorldRatioX) - mPixelWorldRatioX;
						v2 = v1;
						h2 = (float)dx * mTileWorldRatioX;
						v3 = (float)dy * mTileWorldRatioY;
						h3 = h1;
						v4 = v3;
						h4 = h2;
					}
				}
			}
			
			// entry for each topLeft vertex position, offset 0
			vertexArray[i++] = x;
			vertexArray[i++] = y;
	
			// entry for topLeft texture coordinate			
			vertexArray[i++] = h1;
			vertexArray[i++] = v1;
			
			if (needTopLeftOnly) {
				indexArray[j++] = vertexPtr;		// top left vertex
				indexArray[j++] = vertexPtr;		// top left vertex
				vertexPtr += 1;
			} else {
				
				if ((haveTiledRow && emptyRow) || (!emptyRow && inEmptyRegion)) {
					// start of new section after an empty region empty
					indexArray[j++] = vertexPtr;		// top left vertex
					indexArray[j++] = vertexPtr;		// top left vertex		
				}
				
				emptyRow = false;
				inEmptyRegion = false;
				
				// topRight vertex, offset 1
				vertexArray[i++] = x2;
				vertexArray[i++] = y;
				// topRight texture Coord
				vertexArray[i++] = h2;
				vertexArray[i++] = v2;
				// botRight vertex, offset 2
				vertexArray[i++] = x2;
				vertexArray[i++] = y2;
				// botRight texture Coord		
				vertexArray[i++] = h4;
				vertexArray[i++] = v4;
				// botLeft vertex, offset 3	
				vertexArray[i++] = x;
				vertexArray[i++] = y2;
				// botLeft texture Coord			
				vertexArray[i++] = h3;
				vertexArray[i++] = v3;
				indexArray[j++] = vertexPtr + 3;	// bottom left vertex
				indexArray[j++] = vertexPtr;		// top left vertex
				indexArray[j++] = vertexPtr + 2;	// bottom right vertex
				indexArray[j++] = vertexPtr + 1;	// top right vertex
				vertexPtr += 4;
			}
			if ( (i >= (vertexArraySize - 16) ) || (j >= (indexArraySize - 6))) {
				break;
			}
		}
		if ( (i >= (vertexArraySize - 16) ) || (j >= (indexArraySize - 6))) {
			break;
		}
		if (!emptyRow) {
			// end of row, wrap around
			int index = indexArray[j-2];
			indexArray[j++] = index;
			indexArray[j++] = index;
			haveTiledRow = true;
			haveNonEmptyTile = true; 
		}
	} 
	if (i > maxVertexCount) {
		maxVertexCount = i + 100;
		DEBUG_PRINT("Tile set max vertex count: %d", maxVertexCount);
	}
	DEBUG_ASSERT(vertexArraySize >= i, "MEMORY STOMP!!! wrote past end of tile vertex array");
	if (j > maxIndexCount) {
		maxIndexCount = j + 100;
		DEBUG_PRINT("Tile set max index count: %d", maxIndexCount);
	}
	DEBUG_ASSERT(indexArraySize >= j, "MEMORY STOMP!!! wrote past end of tile index array");
	
	if (haveNonEmptyTile && mTiles) {
		static_cast<ImageOpenGL*>(mTiles)->bindTexture(mMipMode);

		PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
		port.setOpenGLModesForDrawing( (static_cast<ImageOpenGL*>(mTiles)->mTextureFormat == GL_RGBA) && mHasTransparency);
		
		// define triange strip using indexed vertices
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(2, GL_FLOAT, 4*sizeof(GLfloat), vertexArray);
		glTexCoordPointer(2, GL_FLOAT, 4*sizeof(GLfloat), vertexArray+2);
		
		// draw opengl array
		if (haveNonEmptyTile) {
			glDrawElements(GL_TRIANGLE_STRIP, j, GL_UNSIGNED_SHORT, indexArray);
		}

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

	#ifdef TILING_INTERNAL_DEBUG
		Point lp = Point(vertexArray[indexArray[0]], vertexArray[indexArray[0] + 1]);
		for (i = 1; i < j; i++) {
			int ai = indexArray[i] * 4;
			Point p = Point(vertexArray[ai], vertexArray[ai + 1]);
			if (i % 4 == 2) {
				mPort->drawLine(lp, p, Color(1.0f, 0.0f, 0.0f, 0.5f) );
			}
			if (i % 4 == 0) {
				mPort->drawLine(lp, p, Color(1.0f, 0.0f, 1.0f, 0.5f) );
			}
			lp = p;
		}
		mPort->drawLine(Point(0, -mZoomedOrigin.y), Point(drawRect.width(), -mZoomedOrigin.y), Color(1.0f, 0.0f, 0.0f, 0.5f) );
		mPort->drawLine(Point(-mZoomedOrigin.x, 0), Point(-mZoomedOrigin.x, drawRect.height()), Color(1.0f, 0.0f, 0.0f, 0.5f) );
	#endif
	} // end if haveNonEmptyTile
	
	SpriteLayer::drawLayer();
}
#endif // ! PDG_NO_GUI

void 
TileLayer::animateLayer(long msElapsed) {
	SpriteLayer::animateLayer(msElapsed);
}
	
Image* TileLayer::getTileSetImage(){
	Image* image = static_cast<Image*> (mTiles);
	return image;
}

Point TileLayer::getTileSize() const {
	return Point(mSrcTileWidth, mSrcTileHeight);
}
	
#ifndef PDG_NO_GUI
TileLayer::TileLayer(Port* port) 
	: SpriteLayer(port),
	mTiles(0),
	mTileData(0),
	mDataSize(0),
	mSrcTileWidth(0),
	mSrcTileHeight(0),
	mSrcTileCountX(0),
	mSrcTileCountY(0),
	mWorldHeight(0),
	mWorldWidth(0),
	mRepeatingX(false),
	mRepeatingY(false),
	mHasTransparency(false),
	mUseFacing(false),
	mUseFlipping(false)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mTileLayerScriptObj);
#endif
}
#endif // ! PDG_NO_GUI

TileLayer::TileLayer() 
	: SpriteLayer(),
	mTiles(0),
	mTileData(0),
	mDataSize(0),
	mSrcTileWidth(0),
	mSrcTileHeight(0),
	mSrcTileCountX(0),
	mSrcTileCountY(0),
	mWorldHeight(0),
	mWorldWidth(0),
	mRepeatingX(false),
	mRepeatingY(false),
	mHasTransparency(false),
	mUseFacing(false),
	mUseFlipping(false)
{
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	INIT_SCRIPT_OBJECT(mTileLayerScriptObj);
#endif
}

TileLayer::~TileLayer()
{
	if (mTileData) {
		std::free(mTileData);
		mTileData = (uint8*)0xDEADBEEF;
	}
	if (mTiles) {
		mTiles->release();
		mTiles = (Image*)0xDEADBEEF;
	}
#ifdef PDG_COMPILING_FOR_SCRIPT_BINDINGS
	CleanupTileLayerScriptObject(mTileLayerScriptObj);
#endif
}

#ifndef PDG_NO_GUI
TileLayer* createTileLayer(Port* port) {
//	DEBUG_ASSERT(port, "must have a pdg::Port");
	// create sprite manager singleton instance if necessary
	TileLayer* layer = SpriteManager::createTileLayer(port);
	SpriteManager::getSingletonInstance()->addLayer(layer);
	return layer;
}
#endif // ! PDG_NO_GUI

TileLayer* createTileLayer() {
	// create sprite manager singleton instance if necessary
	TileLayer* layer = SpriteManager::createTileLayer();
	SpriteManager::getSingletonInstance()->addLayer(layer);
	return layer;
}


} // end namespace pdg
