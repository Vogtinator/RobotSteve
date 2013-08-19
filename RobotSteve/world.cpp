/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

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
