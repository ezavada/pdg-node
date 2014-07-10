#ifndef _SFML_RENDERER_H__
#define _SFML_RENDERER_H__

#include "SFML/Graphics/Texture.hpp"
#include "SCMLpp.h"

/*! \brief Namespace for SFML renderer
*/
namespace SCML_SFML
{
    
class FileSystem : public SCML::FileSystem
{
    public:
    
    // Folder, File
    SCML_MAP(SCML_PAIR(int, int), sf::Texture*) images;
    
    virtual ~FileSystem();
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename);
    virtual void clear();
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    
    sf::Texture* getImage(int folderID, int fileID) const;
    
};

class Entity : public SCML::Entity
{
    public:
    
    FileSystem* file_system;
    sf::RenderTarget* screen;
    
    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);
    
    FileSystem* setFileSystem(FileSystem* fs);
    sf::RenderTarget* setScreen(sf::RenderTarget* scr);
    
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle);
    virtual SCML_PAIR(unsigned int, unsigned int) getImageDimensions(int folderID, int fileID) const;
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y);
};

}



#endif
