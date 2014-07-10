#include "SCML_cocos2dx.h"
#include <cstdlib>
#include <cmath>

using namespace cocos2d;

namespace SCML_cocos2dx
{


FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
	CCSprite* img = CCSprite::create(SCML_TO_CSTRING(filename));
	
    if(img == NULL)
        return false;
    
    if(!SCML_MAP_INSERT(images, SCML_MAKE_PAIR(folderID, fileID), img))
    {
        printf("SCML_cocos2dx::FileSystem failed to load image: Loading %s duplicates a folder/file id (%d/%d)\n", SCML_TO_CSTRING(filename), folderID, fileID);
        return false;
    }
    else
		img->retain();
    return true;
}

void FileSystem::clear()
{
    typedef SCML_PAIR(int,int) pair_type;
    SCML_BEGIN_MAP_FOREACH_CONST(images, pair_type, CCSprite*, item)
    {
        item->release();
    }
    SCML_END_MAP_FOREACH_CONST;
    images.clear();
}

SCML_PAIR(unsigned int, unsigned int) FileSystem::getImageDimensions(int folderID, int fileID) const
{
    CCSprite* img = SCML_MAP_FIND(images, SCML_MAKE_PAIR(folderID, fileID));
    if(img == NULL)
        return SCML_MAKE_PAIR(0,0);
    return SCML_MAKE_PAIR(img->boundingBox().size.width, img->boundingBox().size.height);
}

CCSprite* FileSystem::getImage(int folderID, int fileID) const
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






void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    //y = -y;
    angle = 360 - angle;
}

SCML_PAIR(unsigned int, unsigned int) Entity::getImageDimensions(int folderID, int fileID) const
{
    return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
    //y = -y;
    angle = 360 - angle;
    
    CCSprite* img = file_system->getImage(folderID, fileID);
    if(img == NULL)
        return;

	x += getPositionX();
	y += getPositionY();

	img->setPosition(ccp(x,y));
	img->setRotation(angle);
	img->setScaleX(scale_x);
	img->setScaleY(scale_y);

	img->visit();
}




}

