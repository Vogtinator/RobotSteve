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

struct WorldObject {
    bool has_mark = false;
    bool has_cube = false;
    bool stone_count = 0;
};

class World
{
    friend class GLView;
public:
    World(int width, int length);
    bool resize(int width, int length);
    bool stepForward();
    void turnRight();
    void turnLeft();
    bool isWall();
    void setMark();
    bool setCube();

private:
    int width, length;
    int x, y;
    std::vector<std::vector<WorldObject> > map;
};

#endif // WORLD_H
