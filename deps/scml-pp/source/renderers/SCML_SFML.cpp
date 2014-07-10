#include "SCML_SFML.h"
#include "SFML/Graphics.hpp"
#include <cstdlib>
#include <cmath>


namespace SCML_SFML
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    sf::Texture* img = new sf::Texture;
    img->loadFromFile(filename);
    if(img == NULL)
        return false;
    
    if(!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
        printf("SCML_SFML::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
        delete img;
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, sf::Texture*, item)
    {
        delete item;
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    sf::Texture* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if(img == NULL)
        return SCML_MAKE_PAIR(0,0);
    return SCML_MAKE_PAIR(img->getSize().x, img->getSize().y);
}

sf::Texture* FileSystem::getImage(int folderID, int fileID) const
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

sf::RenderTarget* Entity::setScreen(sf::RenderTarget* scr)
{
    sf::RenderTarget* old = screen;
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
    
    sf::Texture* img = file_system->getImage(folderID, fileID);
    if(img == NULL)
        return;
    
    sf::Sprite sprite(*img);
    sprite.setOrigin(img->getSize().x/2, img->getSize().y/2);
    sprite.setScale(scale_x, scale_y);
    sprite.setRotation(angle);
    sprite.setPosition(x, y);
    
    screen->draw(sprite);
}





}

