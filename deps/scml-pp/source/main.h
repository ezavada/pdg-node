#ifndef _RENDERER_H__
#define _RENDERER_H__

#include "SCMLpp.h"
#include <vector>
#include <string>

bool init(unsigned int w, unsigned int h);
void quit();

void main_loop(std::vector<std::string>& data_files);




#endif
