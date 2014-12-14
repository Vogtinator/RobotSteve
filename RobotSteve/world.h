#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <utility>
#include <QString>
#include <QXmlStreamReader>

typedef std::pair<int,int> SignedCoords;
typedef std::pair<unsigned int, unsigned int> Size;
typedef std::pair<unsigned int, unsigned int> Coords;

struct WorldObject {
    bool has_mark = false;
    bool has_cube = false;
    unsigned int stack_size = 0;
};

enum ORIENTATION {
    ORIENT_INVALID = -1,

    ORIENT_NORTH,
    ORIENT_EAST,
    ORIENT_SOUTH,
    ORIENT_WEST
};

struct WorldState {
    WorldState() {}
    WorldState(const Coords &steve, const Size &size, const ORIENTATION &orientation, unsigned int max_height, const std::vector<std::vector<WorldObject> > &map)
        : steve{steve}, size{size}, orientation{orientation}, max_height{max_height}, map{map} {}

    Coords steve;
    Size size;
    ORIENTATION orientation;
    unsigned int max_height;
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
    World(unsigned int width, unsigned int length, unsigned int max_height);

    //These functions are invoked by the SteveInterpreter.
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
    virtual unsigned int getStackSize();
    virtual bool isMarked();

    Size getSize() const { return size; }
    ORIENTATION getOrientation() const { return orientation; }
    unsigned int getX() const { return steve.first; }
    unsigned int getY() const { return steve.second; }
    WorldObject &getObject(const Coords &pos) { return map[pos.first][pos.second]; }

    unsigned int getMaxHeight() const { return max_height; }
    virtual void setMaxHeight(unsigned int max_height);

    void dumpWorld() const;
    WorldState getState() const;
    virtual bool setState(WorldState &state);

    bool saveFile(const QString &filename) const;
    bool loadFile(const QString &filename);
    bool loadXML(const QString &xml);
    virtual bool loadXMLStream(QXmlStreamReader &file_reader);

    const Size maximum_size = {25, 25}, minimum_size = {3, 3};

protected:
    SignedCoords getForward() const;
    virtual void updateFront();
    bool inBounds(SignedCoords &coords) const;

    //Not translated to keep the world file format compatible
    const std::map<ORIENTATION,QString> orientation_str = {
        std::make_pair(ORIENT_NORTH, "north"),
        std::make_pair(ORIENT_EAST, "east"),
        std::make_pair(ORIENT_SOUTH, "south"),
        std::make_pair(ORIENT_WEST, "west")
    };

    WorldObject *front_obj;
    Size size;
    Coords steve;
    //This has to be signed; if steve is at (1,0) and looking at the wall, front is (1,-1)
    SignedCoords front;
    ORIENTATION orientation = ORIENT_SOUTH;
    std::vector<std::vector<WorldObject>> map;
    unsigned int max_height;
};

#endif // WORLD_H
