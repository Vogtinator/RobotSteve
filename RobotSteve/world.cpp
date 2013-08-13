#include "world.h"

World::World(int width, int length)
    : width(width), length(length)
{
    if(!resize(width, length))
        throw "Invalid world size!";
}

bool World::resize(int width, int length)
{
    if(width > 25 || length > 25 || width < 3 || length < 3)
        return false;

    map.resize(width);
    for(auto i : map)
        i.resize(length);

    if(x >= width || y >= length)
        x = y = 0;

    return true;
}
