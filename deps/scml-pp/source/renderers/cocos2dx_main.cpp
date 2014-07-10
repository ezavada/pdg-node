#include "SCML_cocos2dx.h"
#include <vector>
#include <list>

#include "CCDirector.h"

using namespace std;
using namespace cocos2d;
using namespace SCML_cocos2dx;
USING_NS_CC;




class Test : public cocos2d::CCLayerColor
{
public:
    
    size_t data_file_index;
    SCML::Data data;
    FileSystem fs;
    list<Entity*> entities;
    
    float x;
    float y;
    float angle;
    float scale;

    virtual bool init();

    static cocos2d::CCScene* scene();
    void menuCloseCallback(CCObject* pSender);
    CREATE_FUNC(Test);

	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
	void update(float dt);
	void draw();
};


CCScene* Test::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        Test *layer = Test::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Test::init()
{
    bool bRet = false;
    do 
    {
        // super init first
        CC_BREAK_IF(! CCLayer::init());

        // Set up scene
        initWithColor(ccc4(255,255,255,255));
        
        data_file_index = 0;
        //SCML::Data data(data_files[data_file_index]);
        data.load("samples/monster/Example.SCML");
        data.log();
        
        fs.load(&data);
        printf("Loaded %zu images.\n", fs.images.size());
        
        x = 400.0f;
        y = 300.0f;
        angle = 0.0f;
        scale = 1.0f;
        
        scheduleUpdate();
        
        int i = 0;
        for(map<int, SCML::Data::Entity*>::iterator e = data.entities.begin(); e != data.entities.end(); e++)
        {
            Entity* entity = new Entity(&data, e->first);
            entity->setFileSystem(&fs);
            entity->autorelease();
            entity->setPosition(ccp(x, y));
            //entity->scheduleUpdate();
            entities.push_back(entity);
            this->addChild(entity, 0, 100+i);
            i++;
        }
        printf("Loaded %zu entities.\n", entities.size());


		CCDirector* pDirector = CCDirector::sharedDirector();
		pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);

        bRet = true;

    } while (0);

    return bRet;
}

void Test::update(float dt)
{
    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        Entity* entity = *e;
        
        entity->setPosition(ccp(x, y));
        entity->setRotation(angle);
        entity->setScale(scale);
        entity->SCML::Entity::update(dt * 1000.0f);
    }
}

void Test::draw()
{
    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        Entity* entity = *e;
        entity->SCML::Entity::draw(entity->getPositionX(), entity->getPositionY(), entity->getRotation(), entity->getScale(), entity->getScale());
    }
}

void Test::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}


bool Test::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    x = touch->getLocation().x;
    y = touch->getLocation().y;
    printf("x,y: (%.2f,%.2f)\n", x, y);
    
    // Begin new animation
    for(list<Entity*>::iterator e = entities.begin(); e != entities.end(); e++)
    {
        (*e)->startAnimation(rand()%data.getNumAnimations((*e)->entity));
    }
    
    // Load new entities
    /*if(false)
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
    }*/

    return true;
}




class  AppDelegate : private cocos2d::CCApplication
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual bool applicationDidFinishLaunching();

    virtual void applicationDidEnterBackground();

    virtual void applicationWillEnterForeground();
};

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();

    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
    
    TargetPlatform target = getTargetPlatform();
    
    if (target == kTargetIpad)
    {
        // ipad
        
        CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        
        // don't enable retina because we don't have ipad hd resource
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(960, 640, kResolutionNoBorder);
    }
    else if (target == kTargetIphone)
    {
        // iphone
        
        // try to enable retina on device (Interface does not even exist on Linux!)
        if (true)//true == CCDirector::sharedDirector()->enableRetinaDisplay(true))
        {
            // iphone hd
            CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        }
        else 
        {
            CCFileUtils::sharedFileUtils()->setResourceDirectory("iphone");
        }
    }
    else 
    {
        // android, windows, blackberry, linux or mac
        // use 960*640 resources as design resolution size
        CCFileUtils::sharedFileUtils()->setResourceDirectory("iphonehd");
        //CCEGLView::sharedOpenGLView()->setDesignResolutionSize(1600, 1200, kResolutionNoBorder);
        CCEGLView::sharedOpenGLView()->setDesignResolutionSize(800, 600, kResolutionNoBorder);
    }

    // turn on display FPS
    //pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = Test::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
}




void main_loop(vector<string>& data_files)
{
    CCApplication::sharedApplication()->run();
}





bool init(unsigned int w, unsigned int h)
{
    
    // get application path
    /*int length;
    // 500 is enough?
    char fullpath[500];
    length = readlink("/proc/self/exe", fullpath, sizeof(fullpath));
    fullpath[length] = '\0';

    std::string resourcePath = fullpath;
    resourcePath = resourcePath.substr(0, resourcePath.find_last_of("/"));
    resourcePath += "/../../../Resources/";*/
    
    
    (void)new AppDelegate;  // Cocos2d-x stores this secretly...
    //CCApplication::sharedApplication()->setResourceRootPath(resourcePath.c_str());
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setFrameSize(800, 600);
    
    return true;
}

void quit()
{
    delete CCApplication::sharedApplication();
}


