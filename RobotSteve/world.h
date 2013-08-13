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
