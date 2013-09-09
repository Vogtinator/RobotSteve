#ifndef WORLD_H
#define WORLD_H

#include <QString>
#include <vector>
#include <map>
#include <utility>

typedef std::pair<int,int> SignedCoords;
typedef std::pair<unsigned int, unsigned int> Size;
typedef std::pair<unsigned int, unsigned int> Coords;

struct WorldObject {
    bool has_mark = false;
    bool has_cube = false;
    unsigned int stack_size = 0;
};

enum ORIENTATION {
    ORIENT_NORTH,
    ORIENT_EAST,
    ORIENT_SOUTH,
    ORIENT_WEST
};

struct WorldState {
    WorldState() {}
    WorldState(Coords &steve, Size &size, ORIENTATION &orientation, std::vector<std::vector<WorldObject> > &map)
        : steve{steve}, size{size}, orientation{orientation}, map{map} {}

    Coords steve;
    Size size;
    ORIENTATION orientation;
    std::vector<std::vector<WorldObject> > map;
};

Coords operator+(const Coords& left, const Coords& right);
Coords operator-(const Coords& left, const Coords& right);
Coords operator*(const Coords& left, const float o);
Coords operator/(const Coords& left, const float o);

//Called World although it also contains Steve..
class World
{
public:
    World(unsigned int width, unsigned int length);

    virtual void reset();
    virtual bool resize(unsigned int width, unsigned int length);
    virtual bool stepForward();
    virtual void turnRight(int quarters);
    virtual void turnLeft(int quarters);
    virtual void setMark(bool b);
    virtual bool setCube(bool b);
    virtual bool pickup(unsigned int count);
    virtual bool deposit(unsigned int count);
    virtual bool isWall();
    virtual bool isCube();
    virtual bool frontBlocked();
    virtual int getStackSize();
    virtual bool isMarked();

    ORIENTATION getOrientation() { return orientation; }
    int getX() { return steve.first; }
    int getY() { return steve.second; }
    void dumpWorld();
    WorldState getState();
    virtual bool setState(WorldState &state);
    bool saveFile(const QString &filename);
    virtual bool loadFile(const QString &filename);

protected:
    SignedCoords getForward();
    void updateFront();
    bool inBounds(SignedCoords &coords);

    const std::map<ORIENTATION,QString> orientation_str = {
        std::make_pair(ORIENT_NORTH, "north"),
        std::make_pair(ORIENT_EAST, "east"),
        std::make_pair(ORIENT_SOUTH, "south"),
        std::make_pair(ORIENT_WEST, "west")
    };

    WorldObject *front_obj;
    Size size;
    Coords steve;
    SignedCoords front;
    ORIENTATION orientation = ORIENT_SOUTH;
    std::vector<std::vector<WorldObject> > map;
};

#endif // WORLD_H
