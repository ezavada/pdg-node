// -----------------------------------------------
// image-opengl.cpp
// 
// OpenGL implementation of image functionality
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


#ifndef PDG_NO_GUI

#define _USE_MATH_DEFINES // for MSVC

#include "pdg_project.h"

#include "pdg/msvcfix.h"
#include "image-opengl.h"
#include "graphics-opengl.h"
#include "pdg/sys/os.h"
#include "internals.h"

#include <cstdlib>
#include <fstream>
#include <cmath>

#ifdef _MSC_VER
// Calculates log2 of number.  

	float Log2F(float n) {  

		// log(n)/log(2) is log2.  

		return log(n) / 0.30102999566; // log( 2 ) = 0.30102999566  

	}
	#ifndef log2f
		#define log2f Log2F
	#endif
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE GL_CLAMP
#endif

namespace pdg {
	
    GLuint gBoundTexture;   // this gets cleared by graphics_startDrawing

	void       
	ImageOpenGL::draw(const Point& loc) {
		if (mSuperImage) {
			// pass to super image
            uint8 opac = mSuperImage->getOpacity();
            mSuperImage->setOpacity(opacity);
			if (mFrameNum >= 0) {
				mSuperImage->drawFrame(loc, mFrameNum);
			} else if (mIsQuadSection) {
				Quad q = mSectionQuad;
				q.moveRight(loc.x);
				q.moveDown(loc.y);
				mSuperImage->drawSection(q, mSectionQuad);
			} else {
				mSuperImage->drawSection(mSectionRect + loc, mSectionRect);
			}
            mSuperImage->setOpacity(opac);
		} else {
			Rect r(loc, width, height);
			draw(r, fit_Fill);
		}
	}
	
	void 
	ImageOpenGL::draw(const Rect& r, FitType fitType, bool clipOverflow) {
		if (mSuperImage) {
			// pass to super image
            uint8 opac = mSuperImage->getOpacity();
            mSuperImage->setOpacity(opacity);
			if (mFrameNum >= 0) {
				mSuperImage->drawFrame(r, mFrameNum, fitType, clipOverflow);
			} else if (mIsQuadSection) {
				// TODO: make use of fitType and clipOverflow
				mSuperImage->drawSection(r, mSectionQuad);
			} else {
				// TODO: make use of fitType and clipOverflow
				mSuperImage->drawSection(r, mSectionRect);
			}
            mSuperImage->setOpacity(opac);
		} else if (fitType == fit_Fill) {
			draw( Quad(r) );
		} else if (fitType == fit_Width) {
			Rect fr(width, height);
			float wRatio = r.width() / width;
			fr.scale(wRatio);
			fr.center(r.centerPoint());
			// TODO: make clipOverflow
			draw( Quad(fr) );
		} else if (fitType == fit_Height) {
			Rect fr(width, height);
			float hRatio = r.height() / height;
			fr.scale(hRatio);
			fr.center(r.centerPoint());
			// TODO: make clipOverflow
			draw( Quad(fr) );
		} else if (fitType == fit_FillKeepProportions) {
			Rect fr(width, height);
			float wRatio = r.width() / width;
			float hRatio = r.height() / height;
			if (wRatio > hRatio) {
				fr.scale(wRatio);
			} else {
				fr.scale(hRatio);
			}
			fr.center(r.centerPoint());
			// TODO: make clipOverflow
			draw( Quad(fr) );
		} else if (fitType == fit_Inside) {
			Rect fr(width, height);
			float wRatio = r.width() / width;
			float hRatio = r.height() / height;
			if (wRatio < hRatio) {
				fr.scale(wRatio);
			} else {
				fr.scale(hRatio);
			}
			fr.center(r.centerPoint());
			draw( Quad(fr) );
		}
	}
	
	void 
	ImageOpenGL::draw(const Quad& quad) {
		if (quad.getBounds().intersection(mPort->getDrawingArea()).empty()) {
			return;
		}
		if (mSuperImage) {
			// pass to super image
            uint8 opac = mSuperImage->getOpacity();
            mSuperImage->setOpacity(opacity);
			if (mFrameNum >= 0) {
				mSuperImage->drawFrame(quad, mFrameNum);
			} else if (mIsQuadSection) {
				mSuperImage->drawSection(quad, mSectionQuad);
			} else {
				mSuperImage->drawSection(quad, mSectionRect);
			}
            mSuperImage->setOpacity(opac);
		} else {
			bindTexture();
			PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
			port.setOpenGLModesForDrawing(opacity != 255 || mTextureFormat == GL_RGBA); 
			
			// calculate the clamping, to prevent sampling of texture pixels from outside the texture
			// when the destination is not at a precise pixel boundary.
			float clampX = mUseEdgeClamp ? 1.0f/(2.0f*(float)width) : 0.0f;
			float clampY = mUseEdgeClamp ? 1.0f/(2.0f*(float)height) : 0.0f;
			
			float tx = (float)width/(float)mBufferWidth;
			float ty = (float)height/(float)mBufferHeight;
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f (clampX, ty - clampY);
			glVertex2f( quad.points[lftBot].x, quad.points[lftBot].y );
			glTexCoord2f (clampX, clampY);
			glVertex2f( quad.points[lftTop].x, quad.points[lftTop].y );
			glTexCoord2f (tx - clampX, ty - clampY);
			glVertex2f( quad.points[rgtBot].x, quad.points[rgtBot].y );
			glTexCoord2f (tx - clampX, clampY);
			glVertex2f( quad.points[rgtTop].x, quad.points[rgtTop].y );
			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
	}
	
	void       
	ImageOpenGL::drawFrame(const Point& loc, int frame) {
		if (mSuperImage) return;  // don't do for subimage
		Rect r(loc, frameWidth, height);
		drawFrame(r, frame, fit_Fill);
	}
	
	void       
	ImageOpenGL::drawFrame(const Rect& r, int frame, FitType fitType, bool clipOverflow) {
		if (mSuperImage) return;  // don't do for subimage
		if (fitType == fit_Fill) {
			drawFrame( Quad(r), frame );
		}
	}
	
	void       
	ImageOpenGL::drawFrame(const Quad& quad, int frame) {
		if (mSuperImage) return;  // don't do for subimage
		if (quad.getBounds().intersection(mPort->getDrawingArea()).empty()) {
			return;
		}
		bindTexture();
		PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
		port.setOpenGLModesForDrawing(opacity != 255 || mTextureFormat == GL_RGBA);  // no alpha for now
		
		// calculate the clamping, to prevent sampling of texture pixels from outside the texture
		// when the destination is not at a precise pixel boundary. 
		float clampX = mUseEdgeClamp ? 1.0f/(2.0f*(float)frameWidth) : 0.0f;
		float clampY = mUseEdgeClamp ? 1.0f/(2.0f*(float)height) : 0.0f;
		
		float ty = (float)height/(float)mBufferHeight;
		float x1 = (float)(frame * frameWidth)/(float)mBufferWidth;
		float x2 = (float)((frame+1) * frameWidth)/(float)mBufferWidth;
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f (x1 + clampX, ty - clampY);
		glVertex2f( quad.points[lftBot].x, quad.points[lftBot].y );
		glTexCoord2f (x1 + clampX, clampY);
		glVertex2f( quad.points[lftTop].x, quad.points[lftTop].y );
		glTexCoord2f (x2 - clampX, ty - clampY);
		glVertex2f( quad.points[rgtBot].x, quad.points[rgtBot].y );
		glTexCoord2f (x2 - clampX, clampY);
		glVertex2f( quad.points[rgtTop].x, quad.points[rgtTop].y );
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	
	void    
	ImageOpenGL::drawTexture(const Rect& r) {
        // TODO: rewrite to use:
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // and use a single call
		if (mSuperImage) {
			// pass to super image
            uint8 opac = mSuperImage->getOpacity();
            mSuperImage->setOpacity(opacity);
			if (mFrameNum >= 0) {
				mSuperImage->drawTextureFrame(r, mFrameNum);
			} else if (mIsQuadSection) {
//				mSuperImage->drawSection(r, mSectionQuad);
			} else {
//				mSuperImage->drawSection(r, mSectionRect);
			}
            mSuperImage->setOpacity(opac);
		} else {
			if ((height == 0) || (width == 0)) return;
			Rect clipSave = mPort->getClipRect();
			Rect r2 = r.intersection( clipSave );
			r2.round();
			if (r2.empty()) return; // nothing to draw
			mPort->setClipRect(r2);
			Rect dr(width, height);
			dr.moveTo(r2.leftTop());
			int vc = 1 + r2.height() / height;
			int hc = 1 + r2.width() / width;
			for (int v = 0; v < vc; v++) {
				for (int h = 0; h < hc; h++) {
					draw(dr);
					dr.moveRight(width);			 
				}
				dr.left = r2.left;
				dr.right = r2.right;
				dr.moveDown(height);
			}
			mPort->setClipRect(clipSave);
		}
	}
	
	void    
	ImageOpenGL::drawTextureFrame(const Rect& r, int frame) {
		if (mSuperImage) return;  // don't do for subimage
		if ((height == 0) || (frameWidth == 0)) return;
		Rect clipSave = mPort->getClipRect();
		Rect r2 = r.intersection( clipSave );
		r2.round();
		if (r2.empty()) return; // nothing to draw
		mPort->setClipRect(r2);
		Rect dr(frameWidth, height);
		dr.moveTo(r2.leftTop());
		int vc = 1 + r2.height() / height;
		int hc = 1 + r2.width() / frameWidth;
		for (int v = 0; v < vc; v++) {
		    for (int h = 0; h < hc; h++) {
				drawFrame(dr, frame);
				dr.moveRight(frameWidth);			 
			}
			dr.left = r2.left;
			dr.right = r2.right;
			dr.moveDown(height);
		}
		mPort->setClipRect(clipSave);
	}

	
    //! draws image as texture wrapped around a sphere
    void	
	ImageOpenGL::drawTexturedSphere(const Point& loc, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians) {
		if (mSuperImage) {
			// pass to super image
            uint8 opac = mSuperImage->getOpacity();
            mSuperImage->setOpacity(opacity);
			if (mFrameNum >= 0) {
				mSuperImage->drawTexturedSphereFrame(loc, mFrameNum, radius, rotation, polarOffsetRadians, lightOffsetRadians);
			} else if (mIsQuadSection) {
//				mSuperImage->drawSection(r, mSectionQuad);
			} else {
//				mSuperImage->drawSection(r, mSectionRect);
			}
            mSuperImage->setOpacity(opac);
		} else {
            // decide how many slices to draw (very crude LOD)
            long slices = log2f(radius) * 4;
            if (slices < 5) slices = 5;

			bindTexture();
			PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
			port.setOpenGLModesForDrawing(false); 


            float degreesRot = rotation * 180.0 / M_PI;
            static GLfloat light_position[] = { 0, 0, -10, 1 };
            static GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_DEPTH_TEST);
 
            glPushMatrix();
            
            glTranslatef(loc.x, loc.y, 1.0f);            
            // translate light position spherical to cartesian coordinates
            GLfloat theta = lightOffsetRadians.y; // start 90° offset
            GLfloat rho = lightOffsetRadians.x;
            GLfloat r = radius * 10.0f;
            GLfloat lx = -r * sin(rho);
            GLfloat ly = -r * sin(theta) * sin(rho);
            GLfloat lz = 10.0 * cos(theta) * cos(rho);  // don't use r because we aren't scaling z axis by radius
            light_position[0] = lx;
            light_position[1] = ly;
            light_position[2] = lz;
            glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

            glScalef(-radius, radius, 1.0f);

            GLfloat xRotDeg = (polarOffsetRadians.x * 180.0 / M_PI) - 90.0;
            GLfloat yRotDeg = polarOffsetRadians.y * 180.0 / M_PI;
            glRotatef(xRotDeg, 1.0f, 0.0f, 0.0);
            glRotatef(yRotDeg, 0.0f, 1.0f, 0.0);

            glRotatef(-degreesRot, 0.0f, 0.0f, 1.0);


            GLUquadricObj* qobj = gluNewQuadric();
            gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
            gluQuadricOrientation(qobj, GLU_INSIDE);
            gluQuadricTexture(qobj, GL_TRUE);
            gluQuadricNormals(qobj, GLU_SMOOTH);
            gluSphere(qobj, 1.0f, slices, slices);
 
            glPopMatrix();
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
       }
    }
    
    //! draws single frame of multiframe image as texture wrapped around a sphere
    void	
	ImageOpenGL::drawTexturedSphereFrame(const Point& loc, int frame, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians) {
    }

	void	
	ImageOpenGL::drawSection(const Rect& r, const Rect& section) {
		if (mSuperImage) return;  // don't do for subimage
		if (r.intersection(mPort->getDrawingArea()).empty()) {
			return;
		}
		
		bindTexture();
		PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
		port.setOpenGLModesForDrawing(opacity != 255 || mTextureFormat == GL_RGBA); 
		
		// calculate the clamping, to prevent sampling of texture pixels from outside the texture
		// when the destination is not at a precise pixel boundary.
		float clampX = mUseEdgeClamp ? 1.0f/(2.0f*(float)width) : 0.0f;
		float clampY = mUseEdgeClamp ? 1.0f/(2.0f*(float)height) : 0.0f;
		
		float sx, sy, tx, ty;
		// find tex coordinates; if section is larger than image, just scale the full image to destination rect
		if (section.width() >= width) {
			sx = 0;
			tx = (float)width/(float)mBufferWidth;
		}
		else {
			sx = section.left / mBufferWidth;
			tx = section.right / mBufferWidth;
		}
		
		if (section.height() >= height) {
			sy = 0;
			ty = (float)height/(float)mBufferHeight;
		}
		else {
			sy = section.top / mBufferHeight;
			ty = section.bottom / mBufferHeight;
		}
		
		
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f (sx + clampX, ty - clampY);
		glVertex2f( r.left, r.bottom );
		glTexCoord2f (sx +clampX, sy + clampY);
		glVertex2f( r.left, r.top );
		glTexCoord2f (tx - clampX, ty - clampY);
		glVertex2f( r.right, r.bottom );
		glTexCoord2f (tx - clampX, sy +clampY);
		glVertex2f( r.right, r.top );
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	
	
	void	
	ImageOpenGL::drawSection(const Rect& r, const Quad& section) {
		if (mSuperImage) return;  // don't do for subimage
		if (r.intersection(mPort->getDrawingArea()).empty()) {
			return;
		}
		
		bindTexture();
		PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
		port.setOpenGLModesForDrawing(opacity != 255 || mTextureFormat == GL_RGBA); 
		
		// calculate the clamping, to prevent sampling of texture pixels from outside the texture
		// when the destination is not at a precise pixel boundary.
//		float clampX = mUseEdgeClamp ? 1.0f/(2.0f*(float)width) : 0.0f;
//		float clampY = mUseEdgeClamp ? 1.0f/(2.0f*(float)height) : 0.0f;
		
		float tx, ty;
		// find tex coordinates; if section is larger than image, constrain to keep section inside texture coordinates

		glBegin(GL_TRIANGLE_STRIP);
		tx = fminf(fmaxf(section.points[lftBot].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[lftBot].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( r.left, r.bottom );
		tx = fminf(fmaxf(section.points[lftTop].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[lftTop].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( r.left, r.top );
		tx = fminf(fmaxf(section.points[rgtBot].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[rgtBot].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( r.right, r.bottom );
		tx = fminf(fmaxf(section.points[rgtTop].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[rgtTop].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( r.right, r.top );
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	
	void 
	ImageOpenGL::drawSection(const Quad& q, const Rect& section) {
		if (mSuperImage) return;  // don't do for subimage
		drawSection(q, Quad(section));
	}
	
	void 
	ImageOpenGL::drawSection(const Quad& q, const Quad& section) {
		if (mSuperImage) return;  // don't do for subimage
		if (q.getBounds().intersection(mPort->getDrawingArea()).empty()) {
			return;
		}
		
		bindTexture();
		PortImpl& port = static_cast<PortImpl&>(*mPort); // get us access to our private data
		port.setOpenGLModesForDrawing(opacity != 255 || mTextureFormat == GL_RGBA); 
		
		// calculate the clamping, to prevent sampling of texture pixels from outside the texture
		// when the destination is not at a precise pixel boundary.
		//		float clampX = mUseEdgeClamp ? 1.0f/(2.0f*(float)width) : 0.0f;
		//		float clampY = mUseEdgeClamp ? 1.0f/(2.0f*(float)height) : 0.0f;
		
		float tx, ty;
		// find tex coordinates; if section is larger than image, constrain to keep section inside texture coordinates
		
		glBegin(GL_TRIANGLE_STRIP);
		tx = fminf(fmaxf(section.points[lftBot].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[lftBot].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( q.points[lftBot].x, q.points[lftBot].y );
		tx = fminf(fmaxf(section.points[lftTop].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[lftTop].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( q.points[lftTop].x, q.points[lftTop].y );
		tx = fminf(fmaxf(section.points[rgtBot].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[rgtBot].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( q.points[rgtBot].x, q.points[rgtBot].y );
		tx = fminf(fmaxf(section.points[rgtTop].x/mBufferWidth, 0.0f), 1.0f);
		ty = fminf(fmaxf(section.points[rgtTop].y/mBufferHeight, 0.0f), 1.0f);
		glTexCoord2f(tx, ty);
		glVertex2f( q.points[rgtTop].x, q.points[rgtTop].y );
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}
	

	void	
	ImageOpenGL::prepareToRasterize() {
		if (mSuperImage) return;  // don't do for subimage
		bindTexture(GL_LINEAR);
	}

	void    
	ImageOpenGL::bindTexture(uint32 mipMode) {
		if (mSuperImage) return;  // don't do for subimage
		if (mTexture == 0 && data) {
			glGenTextures(1, &mTexture);
			glBindTexture(GL_TEXTURE_2D, mTexture);
            gBoundTexture = mTexture;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipMode);  // defaults to GL_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipMode);
//			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);  // no lighting or colorization effects
			if (mUseEdgeClamp) {
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			} else {
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			}
			GLint glMaxTexDim;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);			// Get Maximum Texture Size Supported
			if (mBufferWidth > glMaxTexDim) {
				DEBUG_PRINT("IMAGE ERROR: buffer width %ld > max GL Texture Dim %ld", mBufferWidth, glMaxTexDim);
//				mBufferWidth = glMaxTexDim;
			}
			if (mBufferHeight > glMaxTexDim) {
				DEBUG_PRINT("IMAGE ERROR: buffer height %ld > max GL Texture Dim %ld", mBufferHeight, glMaxTexDim);
//				mBufferHeight = glMaxTexDim;
			}
			if (mTextureFormat == GL_RGBA) {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			} else {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			}
			glTexImage2D(GL_TEXTURE_2D, 0, mTextureFormat, mBufferWidth, mBufferHeight, 0, 
						 mTextureFormat, GL_UNSIGNED_BYTE, data);
			if (!mRetainData && !mRetainAlpha) {
				std::free(data);
				data = 0;
			}
		}
		glColor4f(1.0f,1.0f,1.0f, (float) opacity / 255.0f );
		glEnable(GL_TEXTURE_2D);
        if (gBoundTexture != mTexture) {
            glBindTexture(GL_TEXTURE_2D, mTexture);
            gBoundTexture = mTexture;
        }
	}

	ImageOpenGL::~ImageOpenGL() {
		if (mSuperImage) {
			DEBUG_ASSERT(mTexture == 0, "Error: Sub-Image has GL Texture! Should not be!!");
			mSuperImage->release();	// one fewer reference to the super image
			mSuperImage = 0;
		} else if (mTexture != 0)  {
			glDeleteTextures(1, &mTexture);
			mTexture = 0;
		}
/*		if (mTransparentMaskData) {
			std::free(mTransparentMaskData);
			mTransparentMaskData = 0;
		} */
	}


} // end namespace pdg

#endif // PDG_NO_GUI
