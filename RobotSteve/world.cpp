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

#include <iostream>

Coords operator+(const Coords& left, const Coords& right)
{
    return {left.first + right.first, left.second + right.second};
}

World::World(int width, int length)
    : size(width, length)
{
    if(!resize(width, length))
        throw std::string("Invalid world size!");

    reset();
}

bool World::resize(int width, int length)
{
    if(width > 25 || length > 25 || width < 3 || length < 3)
        return false;

    map.resize(width);
    for(int x = 0; x < width; x++)
        map[x].resize(length);

    if(steve.first >= width || steve.second >= length)
        steve.first = steve.second = 0;

    size.first = width;
    size.second = length;

    return true;
}

void World::reset()
{
    steve.first = steve.second = 0;
    orientation = ORIENT_SOUTH;
    for(int x = 0; x < size.first; x++)
        for(int y = 0; y < size.second; y++)
            map[x][y] = {};
}

bool World::isWall()
{
    Coords then = steve + getForward();

    return then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first;
}

bool World::stepForward()
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return false; //Oh no! Stepped into a wall :-(

    steve = then;

    return true;
}

void World::turnRight()
{
    int o = static_cast<int>(orientation);
    o++;
    o %= 4;
    orientation = static_cast<ORIENTATION>(o);
}

void World::turnLeft()
{
    int o = static_cast<int>(orientation);
    o--;
    o %= 4;
    orientation = static_cast<ORIENTATION>(o);
}

void World::setMark(bool b)
{
    map[steve.first][steve.second].has_mark = b;
}

bool World::setCube(bool b)
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return false; //Nope, no cubes in walls please.

    WorldObject &obj = map[then.first][then.second];
    if((obj.has_cube && b) || (!obj.has_cube && !b))
        return false; //Mine non-existent or place into existing cube

    obj.has_cube = b;
    return true;
}

Coords World::getForward()
{
    switch(orientation)
    {
    case ORIENT_NORTH:
        return {0, -1};
    case ORIENT_SOUTH:
        return {0, +1};
    case ORIENT_EAST:
        return {+1, 0};
    case ORIENT_WEST:
        return {-1, 0};
    default:
        throw std::string("Umm, something weird happened.");
    }
}

int World::getStackSize()
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return 0; //Nope, no cubes in walls please.

    return map[then.first][then.second].stack_size;
}

bool World::isMarked()
{
    return map[steve.first][steve.second].has_mark;
}

void World::dumpWorld()
{
    std::cout << " ";
    for(int width = 0; width < size.first; width++)
        std::cout << "-";
    std::cout << std::endl;

    for(int y = 0; y < size.second; y++)
    {
        std::cout << "|";
        for(int x = 0; x < size.first; x++)
        {
            if(steve.first == x && steve.second == y)
            {
                std::cout << 'S';
                continue;
            }

            WorldObject &obj = map[x][y];
            if(obj.stack_size != 0)
            {
                std::cout << obj.stack_size;
                continue;
            }
            if(obj.has_mark)
            {
                std::cout << 'M';
                continue;
            }
            if(obj.has_cube)
            {
                std::cout << 'C';
                continue;
            }
            std::cout << " ";
        }
        std::cout << "|" << std::endl;
    }

    std::cout << " ";
    for(int width = 0; width < size.first; width++)
        std::cout << "-";
    std::cout << std::endl;
}
