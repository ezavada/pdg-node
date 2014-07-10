#include "main.h"
using namespace std;

int main(int argc, char* argv[])
{
    if(!init(800, 600))
        return 1;
    
    vector<string> data_files;
    data_files.push_back("samples/monster/Example.SCML");
    data_files.push_back("samples/knight/knight.scml");
    data_files.push_back("samples/hero/Hero.SCML");
    
    main_loop(data_files);
    
    quit();
    return 0;
}
