// -----------------------------------------------
// image-opengl.h
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


#ifndef PDG_IMAGE_OPEN_GL_H_INCLUDED
#define PDG_IMAGE_OPEN_GL_H_INCLUDED

#include "pdg_project.h"

#include "pdg/sys/image.h"
#include "pdg/sys/color.h"

#include "image-impl.h"

#include "include-opengl.h"

namespace pdg {

	class ImageOpenGL : public ImageImpl {
	public:

		static pdg::ISerializable* CreateInstance() { return new ImageOpenGL(); }

		virtual void    draw(const Point& loc);
		virtual void    draw(const Rect& r, FitType fitType = fit_Fill, bool clipOverflow = false);
		virtual void    draw(const Quad& quad);
		virtual void    drawFrame(const Point& loc, int frame);
		virtual void    drawFrame(const Rect& r, int frame, FitType fitType = fit_Fill, bool clipOverflow = false);
		virtual void    drawFrame(const Quad& quad, int frame);
		virtual void    drawTexture(const Rect& r);
		virtual void    drawTextureFrame(const Rect& r, int frame);
		virtual void	drawSection(const Rect& r, const Rect& section);
		virtual void	drawSection(const Quad& r, const Rect& section);
		virtual void	drawSection(const Rect& r, const Quad& section);
		virtual void	drawSection(const Quad& r, const Quad& section);
        virtual void    drawTexturedSphere(const Point& loc, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians);
        virtual void    drawTexturedSphereFrame(const Point& loc, int frame, float radius, float rotation, const Offset& polarOffsetRadians, const Offset& lightOffsetRadians);

		virtual void	prepareToRasterize();
		
		void	bindTexture(uint32 mipMode = GL_LINEAR);
		
		ImageOpenGL() 
 		     : ImageImpl(),  mTexture(0)
		{
		}

		ImageOpenGL(Port* port) 
 		     : ImageImpl(),  mTexture(0)
		{
			setPort(port); 
		}

		virtual ~ImageOpenGL();

		GLuint   mTexture;
	};

} // end namespace pdg



#endif // PDG_IMAGE_OPEN_GL_H_INCLUDED
