#include "SCML_SDL_gpu.h"
#include <vector>
#include <list>
#include <cmath>

using namespace std;
using namespace SCML_SDL_gpu;

GPU_Target* screen = NULL;
Uint8* keystates = NULL;


void main_loop(vector<string>& data_files)
{
    size_t data_file_index = 0;
    SCML::Data data(data_files[data_file_index]);
    data.log();
    
    FileSystem fs;
    fs.load(&data);
    printf("Loaded %zu images.\n", fs.images.size());
    
    list<Entity*> entities;
    for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
    {
        Entity* entity = new Entity(&data, e->first);
        entity->setFileSystem(&fs);
        entity->setScreen(screen);
        entities.push_back(entity);
    }
    printf("Loaded %zu entities.\n", entities.size());
    
    float x = 400.0f;
    float y = 300.0f;
    float angle = 0.0f;
    float scale = 1.0f;
    bool flipped = false;
    bool drawObjectPositions = false;
    bool drawBones = false;
    
    bool paused = false;
    
    bool done = false;
    SDL_Event event;
    int dt_ms = 0;
    Uint32 framestart = SDL_GetTicks();
    
    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                else if(event.key.keysym.sym == SDLK_SPACE)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->startAnimation(rand()%data.getNumAnimations((*e)->entity));
                    }
                }
                else if(event.key.keysym.sym == SDLK_d)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->update(1);
                    }
                }
                else if(event.key.keysym.sym == SDLK_f)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->update(5);
                    }
                }
                else if(event.key.keysym.sym == SDLK_g)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->update(50);
                    }
                }
                else if(event.key.keysym.sym == SDLK_h)
                {
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        (*e)->update(100);
                    }
                }
                else if(event.key.keysym.sym == SDLK_p)
                {
                    paused = !paused;
                }
                else if(event.key.keysym.sym == SDLK_BACKSPACE)
                {
                    flipped = !flipped;
                }
                else if(event.key.keysym.sym == SDLK_o)
                {
                    drawObjectPositions = !drawObjectPositions;
                }
                else if(event.key.keysym.sym == SDLK_b)
                {
                    drawBones = !drawBones;
                }
                else if(event.key.keysym.sym == SDLK_RETURN)
                {
                    // Destroy all of our data
                    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
                    {
                        delete (*e);
                    }
                    entities.clear();
                    
                    fs.clear();
                    data.clear();
                    
                    // Get next data file
                    data_file_index++;
                    if(data_file_index >= data_files.size())
                        data_file_index = 0;
                    
                    // Load new data
                    data.load(data_files[data_file_index]);
                    data.log();
                    
                    fs.load(&data);
                    printf("Loaded %zu images.\n", fs.images.size());
                    
                    for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
                    {
                        Entity* entity = new Entity(&data, e->first);
                        entity->setFileSystem(&fs);
                        entity->setScreen(screen);
                        entities.push_back(entity);
                    }
                    printf("Loaded %zu entities.\n", entities.size());
                }
            }
        }
        
        if(keystates[SDLK_UP])
            y -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_DOWN])
            y += 100*dt_ms/1000.0f;
        if(keystates[SDLK_LEFT])
            x -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_RIGHT])
            x += 100*dt_ms/1000.0f;
        if(keystates[SDLK_MINUS])
            scale -= 0.2f*dt_ms/1000.0f;
        else if(keystates[SDLK_EQUALS] || keystates[SDLK_PLUS])
            scale += 0.2f*dt_ms/1000.0f;
        if(keystates[SDLK_COMMA])
            angle -= 100*dt_ms/1000.0f;
        else if(keystates[SDLK_PERIOD])
            angle += 100*dt_ms/1000.0f;
        
        if(!paused)
        {
            for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
            {
                (*e)->update(dt_ms);
            }
        }
        
        GPU_ClearRGBA(screen, 255, 255, 255, 255);
        
        
        for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
        {
            (*e)->draw(x, y, angle, (flipped? -scale : scale), scale);
        }
        
        if(drawBones)
        {
            for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
            {
                int numBones = (*e)->getNumBones();
                for(int i = 0; i < numBones; i++)
                {
                    SCML::Transform transform;
                    if((*e)->getBoneTransform(transform, i))
                    {
                        SDL_Color blue = {0, 0, 255, 255};
                        GPU_Line(screen, transform.x, transform.y, transform.x + 50*cos(transform.angle*M_PI/180), transform.y + 50*sin(transform.angle*M_PI/180), blue);
                    }
                }
            }
        }
        
        if(drawObjectPositions)
        {
            for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
            {
                int numObjects = (*e)->getNumObjects();
                for(int i = 0; i < numObjects; i++)
                {
                    SCML::Transform transform;
                    if((*e)->getObjectTransform(transform, i))
                    {
                        SDL_Color red = {255, 0, 0, 255};
                        GPU_CircleFilled(screen, transform.x, transform.y, 5*transform.scale_x*transform.scale_y, red);
                    }
                }
            }
        }
        
        GPU_Flip(GPU_GetContextTarget());
        SDL_Delay(10);
        
        Uint32 framefinish = SDL_GetTicks();
        dt_ms = framefinish - framestart;
        framestart = framefinish;
    }
    
    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        delete (*e);
    }
    entities.clear();
    
    data.clear();
}




bool init(unsigned int w, unsigned int h)
{
    SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center"));
    
    screen = GPU_Init(800, 600, SDL_SWSURFACE | SDL_DOUBLEBUF);

    if(screen == NULL)
    {
        printf("Couldn't set video mode %dx%d: %s\n", 800, 600, SDL_GetError());
		return false;
    }

    SDL_WM_SetCaption("SCML demo", NULL);
    
    keystates = SDL_GetKeyState(NULL);
    
    return true;
}

void quit()
{
    GPU_Quit();
    SDL_Quit();
}


