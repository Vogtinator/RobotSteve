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

Coords operator-(const Coords& left, const Coords& right)
{
    return {left.first - right.first, left.second - right.second};
}

Coords operator*(const Coords& left, const float o)
{
    return {left.first * o, left.second * o};
}

Coords operator/(const Coords& left, const float o)
{
    return {left.first / o, left.second / o};
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
    if(width > 100 || length > 100 || width < 3 || length < 3)
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
            then.second < 0 || then.second >= size.first || map[then.first][then.second].has_cube)
        return false; //Oh no! Stepped into a wall :-(

    steve = then;

    return true;
}

void World::turnRight(int quarters)
{
    quarters %= 4;
    if(quarters > 1)
        turnRight(quarters - 1);

    switch(orientation)
    {
    case ORIENT_NORTH:
        orientation = ORIENT_EAST;
        break;
    case ORIENT_EAST:
        orientation = ORIENT_SOUTH;
        break;
    case ORIENT_SOUTH:
        orientation = ORIENT_WEST;
        break;
    case ORIENT_WEST:
        orientation = ORIENT_NORTH;
        break;
    }
}

void World::turnLeft(int quarters)
{
    quarters %= 4;
    if(quarters > 1)
        turnLeft(quarters - 1);

    switch(orientation)
    {
    case ORIENT_NORTH:
        orientation = ORIENT_WEST;
        break;
    case ORIENT_EAST:
        orientation = ORIENT_NORTH;
        break;
    case ORIENT_SOUTH:
        orientation = ORIENT_EAST;
        break;
    case ORIENT_WEST:
        orientation = ORIENT_SOUTH;
        break;
    }
}

void World::setMark(bool b)
{
    WorldObject &obj = map[steve.first][steve.second];
    if(!obj.has_cube)
        map[steve.first][steve.second].has_mark = b;
}

bool World::setCube(bool b)
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return false; //Nope, no cubes in walls please.

    WorldObject &obj = map[then.first][then.second];
    if((obj.has_cube && b) || (!obj.has_cube && !b) || obj.stack_size != 0 || obj.has_mark)
        return false; //Mine non-existent, place into existing cube, place into stack or onto a mark

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
        //throw std::string("Umm, something weird happened.");
        return {0, 0};
    }
}

int World::getStackSize()
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return 0;

    WorldObject &obj = map[then.first][then.second];
    if(obj.has_cube)
        return 0; //Nope, no bricks in cubes, please.

    return map[then.first][then.second].stack_size;
}

bool World::deposit(int count)
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return false; //Nope, no more bricks in walls please.

    WorldObject &obj = map[then.first][then.second];
    if(obj.has_cube)
        return false; //Nope, no bricks in cubes, please.

    obj.stack_size += count;
    if(count > 10)
    {
        obj.stack_size = 10;
        return false;
    }

    return true;
}

bool World::pickup(int count)
{
    Coords then = steve + getForward();

    if(then.first < 0 || then.first >= size.first ||
            then.second < 0 || then.second >= size.first)
        return false; //Nope, no more bricks in walls please.

    WorldObject &obj = map[then.first][then.second];
    if(obj.stack_size < count || obj.has_cube)
        return false; //No more bricks to pick up

    obj.stack_size -= count;

    return true;
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
