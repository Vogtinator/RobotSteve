#include "world.h"

#include <iostream>

SignedCoords operator+(const Coords& left, const SignedCoords& right)
{
    return {static_cast<int>(left.first + right.first), static_cast<int>(left.second + right.second)};
}

SignedCoords operator+(const SignedCoords& left, const Coords& right)
{
    return {static_cast<int>(left.first + right.first), static_cast<int>(left.second + right.second)};
}

World::World(unsigned int width, unsigned int length)
    : front_obj{0}, size{width, length}
{
    if(!resize(width, length))
        throw std::string("Invalid world size!");

    reset();
}

bool World::resize(unsigned int width, unsigned int length)
{
    if(width > 100 || length > 100 || width < 3 || length < 3)
        return false;

    map.resize(width);
    for(unsigned int x = 0; x < width; x++)
        map[x].resize(length);

    if(steve.first >= width || steve.second >= length)
        steve.first = steve.second = 0;

    size.first = width;
    size.second = length;

    updateFront();

    return true;
}

void World::reset()
{
    steve.first = steve.second = 0;
    orientation = ORIENT_SOUTH;
    for(unsigned int x = 0; x < size.first; x++)
        for(unsigned int y = 0; y < size.second; y++)
            map[x][y] = {};

    updateFront();
}

bool World::inBounds(SignedCoords &coords)
{
    return coords.first >= 0 && coords.second >= 0 && static_cast<unsigned int>(coords.first) < size.first && static_cast<unsigned int>(coords.second) < size.second;
}

bool World::isWall()
{
    return !inBounds(front);
}

bool World::isCube()
{
    return front_obj->has_cube;
}

bool World::frontBlocked()
{
    return isWall() || isCube();
}

bool World::stepForward()
{
    if(frontBlocked())
        return false; //Oh no! Stepped into a wall :-(

    steve.first = front.first;
    steve.second = front.second;

    updateFront();

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

    updateFront();
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

    updateFront();
}

void World::setMark(bool b)
{
    WorldObject &obj = map[steve.first][steve.second];
    if(!obj.has_cube)
        map[steve.first][steve.second].has_mark = b;
}

bool World::setCube(bool b)
{
    if(isWall())
        return false; //Nope, no cubes in walls please.

    if((front_obj->has_cube && b) || (!front_obj->has_cube && !b) || front_obj->stack_size != 0 || front_obj->has_mark)
        return false; //Mine non-existent, place into existing cube, place into stack or onto a mark

    front_obj->has_cube = b;
    front_obj->has_mark = false;

    return true;
}

SignedCoords World::getForward()
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
    if(frontBlocked())
        return 0; //No bricks in walls or cubes.

    return map[front.first][front.second].stack_size;
}

bool World::deposit(int count)
{
    if(frontBlocked())
        return false;

    front_obj->stack_size += count;

    //TODO: Make changeable
    if(count > 10)
    {
        front_obj->stack_size = 10;
        return false;
    }

    return true;
}

bool World::pickup(int count)
{
    if(frontBlocked() || front_obj->stack_size < count)
        return false; //Not enough bricks or in wall/cube

    front_obj->stack_size -= count;

    return true;
}

bool World::isMarked()
{
    return map[steve.first][steve.second].has_mark;
}

void World::updateFront()
{
    front = steve + getForward();

    //Check whether in array bounds
    if(inBounds(front))
        front_obj = &(map[front.first][front.second]);
}

void World::dumpWorld()
{
    std::cout << " ";
    for(unsigned int width = 0; width < size.first; width++)
        std::cout << "-";
    std::cout << std::endl;

    for(unsigned int y = 0; y < size.second; y++)
    {
        std::cout << "|";
        for(unsigned int x = 0; x < size.first; x++)
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
    for(unsigned int width = 0; width < size.first; width++)
        std::cout << "-";
    std::cout << std::endl;
}

bool World::setState(WorldState &state)
{
    if(state.map.size() != state.size.first)
        return false;

    for(auto & i : state.map)
        if(i.size() != state.size.second)
            return false;

    if(steve.first >= state.size.first || steve.second >= state.size.second)
        return false;

    map = state.map;
    size = state.size;
    steve = state.steve;
    orientation = state.orientation;

    updateFront();

    return true;
}

WorldState World::getState()
{
    return {steve, size, orientation, map};
}
