// -----------------------------------------------
// SCML_pdg.h
// 
// Header file for Spriter support using SCMLpp
//
// Written by Ed Zavada, 2013
// Copyright (c) 2013, Dream Rock Studios, LLC
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

#ifndef SCML_PDG_RENDERER_H_INCLUDED
#define SCML_PDG_RENDERER_H_INCLUDED

#include "pdg/sys/graphics.h"
#include "pdg/sys/resource.h"
#include "pdg/sys/os.h"

#include "SCMLpp.h"

/*! \brief Namespace for SDL_pdg renderer
*/
namespace SCML_pdg
{

/*! \brief Storage class for images, indexed by folder and file IDs.
 *
 * This is the renderer-specific storage for images.
*/
class FileSystem : public SCML::FileSystem
{
    public:
    
    /*! These ints are: Folder, File.  SCMLpp uses these two integers to uniquely identify to an image.
    */
    SCML_MAP(SCML_PAIR(int, int), pdg::Image*) images;
    
    /*! Delete all of the stored images in the destructor
    */
    virtual ~FileSystem();
    
    /*! Load images and store them so they can be accessed again by the folder/file ID combo.
    */
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename);
    
    /*! Delete all stored images
    */
    virtual void clear();
    
    /*! Get the width and height of an image
    */
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    
    /*! Get an image
    */
    pdg::Image* getImage(int folderID, int fileID) const;
    
};

/*! \brief A class to draw SCML character data.
 */
class Entity : public SCML::Entity
{
    public:
    
    /*! The entity needs to have a way to retrieve image data from the folder/file IDs that SCMLpp uses.
    */
    FileSystem* file_system;
    
    /*! The entity also needs a place to draw to.
    */
    pdg::Port* port;
    
    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);
    Entity(SCML::Data* data, const char* entityName, int animation = 0, int key = 0);
    
    FileSystem* setFileSystem(FileSystem* fs);
    pdg::Port* setPort(pdg::Port* port);
    
    /*! This converts x, y, and angle to the proper coordinate system, as necessary.
     * Usually, this just means inverting y and angle.
     */
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle);
    
    /*! Get the width and height of an image.  I don't really like this (since FileSystem already does it), but it is currently necessary.
    */
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    
    /*! The actual drawing call
     * (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise), so they must be converted,
     */
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y);
};

}


#endif // SCML_PDG_RENDERER_H_INCLUDED
