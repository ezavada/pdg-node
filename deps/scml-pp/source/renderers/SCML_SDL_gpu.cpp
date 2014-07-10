#include "SCML_SDL_gpu.h"
#include <cstdlib>
#include <cmath>


namespace SCML_SDL_gpu
{


FileSystem::~FileSystem()
{
    // Delete everything
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    // Load an image and store it somewhere accessible by its folder/file ID combo.
    GPU_Image* img = GPU_LoadImage(SCML_TO_CSTRING(filename));
    if(img == NULL)
        return false;
    if(!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
        printf("SCML_SDL_gpu::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
        GPU_FreeImage(img);
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    // Delete the stored images
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, GPU_Image*, item)
    {
        GPU_FreeImage(item);
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    // Return the width and height of an image (as a pair of unsigned ints)
    GPU_Image* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if(img == NULL)
        return SCML_MAKE_PAIR(0,0);
    return SCML_MAKE_PAIR(img->w, img->h);
}

GPU_Image* FileSystem::getImage(int folderID, int fileID) const
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

// Set the renderer-specific FileSystem
FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

// Set the renderer-specific render target
GPU_Target* Entity::setScreen(GPU_Target* scr)
{
    GPU_Target* old = screen;
    screen = scr;
    return old;
}




// Convert from the renderer's coordinate system to SCML's coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    // SDL_gpu uses a common left-handed CG coordinate system (+x to the right, +y down, +angle clockwise)
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
    // Convert from SCML's coordinate system to the renderer's coordinate system.
    // This is the inverse of convert_to_SCML_coords() and is usually identical.
    y = -y;
    angle = 360 - angle;
    
    // Get the image
    GPU_Image* img = file_system->getImage(folderID, fileID);
    
    // Draw centered (SDL_gpu does that by default).  Scale the image before rotation.  The rotation pivot point is at the center of the image.
    GPU_BlitTransform(img, NULL, screen, x, y, angle, scale_x, scale_y);
}





}

