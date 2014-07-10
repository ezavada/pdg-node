#include "SCML_sprig.h"
#include "SDL_image.h"
#include <cstdlib>
#include <cmath>


namespace SCML_sprig
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    SDL_Surface* img = IMG_Load(SCML_TO_CSTRING(filename));
    if(img == NULL)
        return false;
    
    // Convert to display format for optimized blitting
    SDL_Surface* tmp = img;
    img = SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(tmp);
    
    if(img == NULL)
        return false;
    
    if(!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
        printf("SCML_sprig::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
        SDL_FreeSurface(img);
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, SDL_Surface*, item)
    {
        SDL_FreeSurface(item);
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    SDL_Surface* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if(img == NULL)
        return SCML_MAKE_PAIR(0,0);
    return SCML_MAKE_PAIR(img->w, img->h);
}

SDL_Surface* FileSystem::getImage(int folderID, int fileID) const
{
    return SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
}






    
Entity::Entity()
    : SCML::Entity()
{}

Entity::Entity(SCML::Data* data, int entity, int animation, int key)
    : SCML::Entity(data, entity, animation, key)
{}

FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

SDL_Surface* Entity::setScreen(SDL_Surface* scr)
{
    SDL_Surface* old = screen;
    screen = scr;
    return old;
}






void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    y = -y;
    angle = 360 - angle;
}

SCML_PAIR(unsigned int, unsigned int) Entity::getImageDimensions(int folderID, int fileID) const
{
    return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
    y = -y;
    angle = 360 - angle;
    
    SDL_Surface* img = file_system->getImage(folderID, fileID);
    
    SPG_TransformX(img, screen, angle, scale_x, scale_y, img->w/2, img->h/2, x, y, SPG_TBLEND);
}





}

