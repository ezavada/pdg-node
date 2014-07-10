#ifndef _COCOS2DX_RENDERER_H__
#define _COCOS2DX_RENDERER_H__

#include "cocos2d.h"
#include "SCMLpp.h"

/*! \brief Namespace for Cocos2d-x renderer
*/
namespace SCML_cocos2dx
{
    
class FileSystem : public SCML::FileSystem
{
    public:
    
    // Folder, File
    SCML_MAP(SCML_PAIR(int, int), cocos2d::CCSprite*) images;
    
    virtual ~FileSystem();
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename);
    virtual void clear();
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    
    cocos2d::CCSprite* getImage(int folderID, int fileID) const;
    
};

class Entity : public cocos2d::CCNode, public SCML::Entity
{
    public:
    
    FileSystem* file_system;
    
    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);
    
    FileSystem* setFileSystem(FileSystem* fs);
    
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle);
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y);
};

}



#endif
