/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>

typedef std::pair<int,int> Coords;

struct WorldObject {
    bool has_mark = false;
    bool has_cube = false;
    int stack_size = 0;
};

enum ORIENTATION {
    ORIENT_NORTH,
    ORIENT_EAST,
    ORIENT_SOUTH,
    ORIENT_WEST
};

Coords operator+(const Coords& left, const Coords& right);
Coords operator-(const Coords& left, const Coords& right);
Coords operator*(const Coords& left, const float o);
Coords operator/(const Coords& left, const float o);

//Called World although it also contains Steve..
class World
{
    friend class GLView;
public:
    World(int width, int length);

    virtual void reset();
    virtual bool resize(int width, int length);
    virtual bool stepForward();
    virtual void turnRight(int quarters);
    virtual void turnLeft(int quarters);
    virtual void setMark(bool b);
    virtual bool setCube(bool b);
    virtual bool pickup(int count);
    virtual bool deposit(int count);
    virtual bool isWall();
    virtual int getStackSize();
    virtual bool isMarked();

    ORIENTATION getOrientation() { return orientation; }
    int getX() { return steve.first; }
    int getY() { return steve.second; }
    void dumpWorld();

protected:
    Coords getForward();

    Coords size;
    Coords steve;
    ORIENTATION orientation = ORIENT_SOUTH;
    std::vector<std::vector<WorldObject> > map;
};

#endif // WORLD_H
