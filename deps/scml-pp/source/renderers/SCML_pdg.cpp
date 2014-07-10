// -----------------------------------------------
// SCML_pdg.cpp
// 
// Implementation file for Spriter support using SCMLpp
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

#define _USE_MATH_DEFINES // for MSVC

#include "SCML_pdg.h"
#include <cstdlib>
#include <cmath>

namespace SCML_pdg
{


FileSystem::~FileSystem()
{
    // Delete everything
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    // Load an image and store it somewhere accessible by its folder/file ID combo.
    pdg::Image* img = pdg::ResourceManager::instance().getImage(SCML_TO_CSTRING(filename));
    if (img == NULL) {
        DEBUG_ONLY( pdg::OS::_DOUT( "SCML_pdg::FileSystem failed to load image: %s\n", SCML_TO_CSTRING(filename)); )
        return false;
    }
    if (!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img)) {
        DEBUG_ONLY( 
        	pdg::OS::_DOUT( "SCML_pdg::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", 
        		SCML_TO_CSTRING(filename), folderID, fileID); 
        )
		img->release();
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    // Delete the stored images
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, pdg::Image*, item)
    {
        item->release();
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    // Return the width and height of an image (as a pair of unsigned ints)
    pdg::Image* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if (img == NULL) {
        return SCML_MAKE_PAIR(0,0);
    }
    return SCML_MAKE_PAIR(img->getWidth(), img->getHeight());
}

pdg::Image* FileSystem::getImage(int folderID, int fileID) const
{
    // Return an image
    return SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
}






// Pass the initialization on to the base class, SCML::Entity.
Entity::Entity()
    : SCML::Entity()
{}

Entity::Entity(SCML::Data* data, int entity, int animation, int key)
    : SCML::Entity(data, entity, animation, key)
{}

Entity::Entity(SCML::Data* data, const char* entityName, int animation, int key)
    : SCML::Entity(data, entityName, animation, key)
{}

// Set the renderer-specific FileSystem
FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

// Set the renderer-specific render target
pdg::Port* Entity::setPort(pdg::Port* scr)
{
    pdg::Port* old = port;
    port = scr;
    return old;
}



// Convert from the renderer's coordinate system to SCML's coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    // PDG uses a common left-handed CG coordinate system (+x to the right, +y down, +angle clockwise)
    y = -y;
    angle = 360 - angle;
}

SCML_PAIR(unsigned int, unsigned int) Entity::getImageDimensions(int folderID, int fileID) const
{
    // Let the FileSystem do the work
    return file_system->getImageDimensions(folderID, fileID);
}

// The actual rendering call.
// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
  #ifndef PDG_NO_GUI
    // Convert from SCML's coordinate system to the renderer's coordinate system.
    // This is the inverse of convert_to_SCML_coords() and is usually identical.
    y = -y;
    angle = 360 - angle;
    
    // Get the image
    pdg::Image* img = file_system->getImage(folderID, fileID);
    if (!img) return;
    img->setOpacity(128);
    
    // Draw centered.  Scale the image before rotation.  The rotation pivot point is at the center of the image.

	pdg::Point p(x, y);
	pdg::Rect r = img->getImageBounds();
 	r.horzScale(scale_x);
 	r.vertScale(scale_y);
	r.center(p);
	pdg::RotatedRect rr(r, (M_PI/180.0f)*angle);

	port->drawImage(img, rr);

	port->frameRect(rr, pdg::Color(0.0f, 0.0f, 0.0f, 0.5f));
	
  #endif // !PDG_NO_GUI
}



} // end namespace SCML_pdg

