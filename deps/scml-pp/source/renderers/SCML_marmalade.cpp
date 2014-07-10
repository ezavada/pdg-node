#include "SCML_marmalade.h"
#include <cstdlib>
#include <cmath>


namespace SCML_MARMALADE
{

FileSystem::~FileSystem()
{
    // Delete everything
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    // Load an image and store it somewhere accessible by its folder/file ID combo.
    CIwTexture *img = new CIwTexture();
    img->LoadFromFile(SCML_TO_CSTRING(filename));
    img->SetMipMapping(false);
    img->Upload();

    if(img == NULL)
        return false;
    if(!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
        printf("SCML_SDL_gpu::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
        delete img;
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    // Delete the stored images
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, CIwTexture*, item)
    {
        delete item;
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    // Return the width and height of an image (as a pair of unsigned ints)
    CIwTexture* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if(img == NULL)
        return SCML_MAKE_PAIR(0,0);
    return SCML_MAKE_PAIR(img->GetWidth(), img->GetHeight());
}

CIwTexture* FileSystem::getImage(int folderID, int fileID) const
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
CIwGxSurface* Entity::setScreen(CIwGxSurface* scr)
{
    CIwGxSurface* old = screen;
    screen = scr;
    return old;
}

// Convert from the renderer's coordinate system to SCML's coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    // Marmalade uses a common left-handed CG coordinate system (+x to the right, +y down, +angle clockwise)
    y = -y;
    angle = 360.f - angle;
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

    angle = (360.f - angle) * (PI/180.f);
    
    // Get the image
    CIwTexture* img = file_system->getImage(folderID, fileID);
    
    // Draw centered .  Scale the image before rotation.  The rotation pivot point is at the center of the image.
        
    if(img == NULL)
        return;
    CIwMaterial* mat = IW_GX_ALLOC_MATERIAL();
    mat->SetTexture(img);
    mat->SetDepthWriteMode(CIwMaterial::DEPTH_WRITE_DISABLED);
    mat->SetModulateMode(CIwMaterial::MODULATE_NONE);

    IwGxSetMaterial(mat);

#ifdef USE_SUBPIXEL
    ////calculate position
    int16 xSize = img->GetWidth();
    int16 ySize = img->GetHeight();
	int16 x1 = (int16)((x - xSize/2)*8.0f*scale_x);
    int16 x2 = (int16)((x + xSize/2)*8.0f*scale_x);
    int16 y1 = (int16)((y - ySize/2)*8.0f*scale_y);
    int16 y2 = (int16)((y + ySize/2)*8.0f*scale_y);
#else
    ////calculate position
    int xSize = img->GetWidth();
    int ySize = img->GetHeight();
	float x1 = (float)((x - (xSize/2.0)*scale_x));
    float x2 = (float)((x + (xSize/2.0)*scale_x));
    float y1 = (float)((y - (ySize/2.0)*scale_y));
    float y2 = (float)((y + (ySize/2.0)*scale_y));
#endif
        static CIwFVec2 xy3[4];

        xy3[0] = CIwFVec2(x1, y1);
        xy3[1] = CIwFVec2(x1, y2);
        xy3[2] = CIwFVec2(x2, y2);
        xy3[3] = CIwFVec2(x2, y1);
            //xy3[0].x = x1, xy3[0].y = y1;
            //xy3[1].x = x1, xy3[1].y = y2;
            //xy3[2].x = x2, xy3[2].y = y2;
            //xy3[3].x = x2, xy3[3].y = y1;
#ifdef USE_SUBPIXEL
        IwGxSetVertStreamScreenSpaceSubPixel(xy3, 4);
#else
		IwGxSetVertStreamScreenSpace(xy3, 4);
#endif
        static CIwFMat2D m;
        m = CIwFMat2D::g_Identity;
        m.SetRot(angle,CIwFVec2(x,y));
        for(uint32 i = 0; i < 4; i++)
         {
            xy3[i] = m.TransformVec(xy3[i]);
         }

        static CIwFVec2 uvs[4];
        uvs[0] = CIwFVec2(0, 0);
        uvs[1] = CIwFVec2(0, 1.0);
        uvs[2] = CIwFVec2(1.0, 1.0);
        uvs[3] = CIwFVec2(1.0, 0);
        IwGxSetUVStream(uvs);

        // Draw single triangle
        IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
        IwGxFlush();
    }

}