#ifndef _SDL_GPU_RENDERER_H__
#define _SDL_GPU_RENDERER_H__

#include "SDL_gpu.h"
#include "SCMLpp.h"

/*! \brief Namespace for SDL_gpu renderer (the example renderer)
*/
namespace SCML_SDL_gpu
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
    SCML_MAP(SCML_PAIR(int, int), GPU_Image*) images;
    
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
    GPU_Image* getImage(int folderID, int fileID) const;
    
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
    GPU_Target* screen;
    
    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);
    
    FileSystem* setFileSystem(FileSystem* fs);
    GPU_Target* setScreen(GPU_Target* scr);
    
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



#endif
