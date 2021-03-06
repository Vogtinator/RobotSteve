#include "world.h"

#include <iostream>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

SignedCoords operator+(const Coords& left, const SignedCoords& right)
{
    return {static_cast<int>(left.first + right.first), static_cast<int>(left.second + right.second)};
}

SignedCoords operator+(const SignedCoords& left, const Coords& right)
{
    return {static_cast<int>(left.first + right.first), static_cast<int>(left.second + right.second)};
}

World::World(unsigned int width, unsigned int length, unsigned int max_height)
    : front_obj{0}, size{width, length}, max_height{max_height}
{
    if(!resize(width, length))
        throw std::string("Invalid world size!");
}

bool World::resize(unsigned int width, unsigned int length)
{
    if(width > maximum_size.first || length > maximum_size.second
            || width < minimum_size.first || length < minimum_size.second)
        return false;

    map.resize(width);
    for(unsigned int x = 0; x < width; x++)
        map[x].resize(length);

    size.first = width;
    size.second = length;

    //If steve isn't on the resized map, place him on (0,0),
    //but reset the world if there is a cube.
    if(steve.first >= width || steve.second >= length)
    {
        steve.first = 0;
        steve.second = 0;

        if(map[0][0].has_cube)
            reset();
    }

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

bool World::inBounds(SignedCoords &coords) const
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

    steve = front;

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
    default:
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
    default:
        break;
    }

    updateFront();
}

void World::setMark(bool b)
{
    WorldObject &obj = getObject(steve);
    if(!obj.has_cube)
        obj.has_mark = b;
}

bool World::setCube(bool b)
{
    if(isWall() //Trying to place a cube into a wall or remove a cube from a wall
        || (front_obj->has_cube && b) //Tying to place a cube when there is already one present
        || (!front_obj->has_cube && !b)  //Trying to remove a non-existant cube
        || front_obj->stack_size != 0 //Trying to place a cube into a stack
        || front_obj->has_mark) //Trying to place a cube onto a mark
        return false;

    front_obj->has_cube = b;

    return true;
}

SignedCoords World::getForward() const
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
        Q_ASSERT(false);
        return {0, 0};
    }
}

unsigned int World::getStackSize()
{
    if(frontBlocked())
        return 0; //No bricks in walls or cubes.

    return getObject(front).stack_size;
}

bool World::deposit(unsigned int count)
{
    if(frontBlocked())
        return false;

    front_obj->stack_size += count;

    if(front_obj->stack_size > max_height)
    {
        front_obj->stack_size = max_height;
        return false;
    }

    return true;
}

bool World::pickup(unsigned int count)
{
    if(frontBlocked() || front_obj->stack_size < count)
        return false; //Not enough bricks or in wall/cube

    front_obj->stack_size -= count;

    return true;
}

bool World::isMarked()
{
    return getObject(steve).has_mark;
}

void World::updateFront()
{
    front = steve + getForward();

    if(inBounds(front))
        front_obj = &getObject(front);
}

void World::dumpWorld() const
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

            const WorldObject &obj = map[x][y];
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

WorldState World::getState() const
{
    return WorldState(steve, size, orientation, max_height, map);
}

//TODO: If canceled midway, updateFront() should be called or state not saved at all
bool World::loadXMLStream(QXmlStreamReader &file_reader)
{
    bool size_set = false, steve_set = false;
    while(!file_reader.atEnd())
    {
        if(file_reader.readNext() == QXmlStreamReader::Invalid)
            break;

        if(file_reader.isEndElement() && file_reader.name() == "world")
            break; //Finished

        if(!file_reader.isStartElement())
            continue;

        if(file_reader.name().compare("world", Qt::CaseInsensitive) == 0)
        {
            const QXmlStreamAttributes attributes = file_reader.attributes();
            QString width_str = attributes.value("width").toString();
            bool ok = false;
            unsigned int width = width_str.toUInt(&ok);
            if(width_str.isEmpty() || !ok)
                    return false;

            QString length_str = attributes.value("length").toString();
            unsigned int length = length_str.toUInt(&ok);
            if(length_str.isEmpty() || !ok)
                    return false;

            QString max_height_str = attributes.value("max_height").toString();
            unsigned int max_height = max_height_str.toUInt(&ok);
            if(max_height_str.isEmpty())
                max_height = 10;
            else if(!ok)
                return false;

            if(!resize(width, length))
                return false;

            this->max_height = max_height;

            reset();

            size_set = true;
        }
        else if(file_reader.name().compare("steve", Qt::CaseInsensitive) == 0)
        {
            const QXmlStreamAttributes attributes = file_reader.attributes();
            QString x_str = attributes.value("x").toString();
            bool ok = false;
            unsigned int x = x_str.toUInt(&ok);
            if(x_str.isEmpty() || !ok)
                    return false;

            QString y_str = attributes.value("y").toString();
            unsigned int y = y_str.toUInt(&ok);
            if(y_str.isEmpty() || !ok)
                    return false;

            QString orientation_str = attributes.value("orientation").toString();
            ORIENTATION orientation = ORIENT_INVALID;
            for(auto&& t : this->orientation_str)
            {
                if(std::get<1>(t).compare(orientation_str, Qt::CaseInsensitive) == 0)
                {
                    orientation = std::get<0>(t);
                    break;
                }
            }
            if(orientation == ORIENT_INVALID)
                    return false;

            if(x >= size.first || y >= size.second)
                return false;

            steve.first = x;
            steve.second = y;
            this->orientation = orientation;

            steve_set = true;
        }
        else if(file_reader.name().compare("stack", Qt::CaseInsensitive) == 0
                || file_reader.name().compare("cube", Qt::CaseInsensitive) == 0
                || file_reader.name().compare("mark", Qt::CaseInsensitive) == 0)
        {
            const QXmlStreamAttributes attributes = file_reader.attributes();
            QString x_str = attributes.value("x").toString();
            bool ok = false;
            unsigned int x = x_str.toUInt(&ok);
            if(x_str.isEmpty() || !ok)
                    return false;

            QString y_str = attributes.value("y").toString();
            unsigned int y = y_str.toUInt(&ok);
            if(y_str.isEmpty() || !ok)
                    return false;

            if(x >= size.first || y >= size.second)
                return false;

            WorldObject *obj = &(map[x][y]);

            if(file_reader.name().compare("stack", Qt::CaseInsensitive) == 0)
            {
                QString height_str = attributes.value("height").toString();
                unsigned int height = height_str.toUInt(&ok);
                if(height_str.isEmpty() || !ok)
                    return false;

                obj->stack_size = height;
                obj->has_cube = false;
            }
            else if(file_reader.name().compare("cube", Qt::CaseInsensitive) == 0)
            {
                obj->has_cube = true;
                obj->has_mark = false;
                obj->stack_size = 0;
            }
            else if(file_reader.name().compare("mark", Qt::CaseInsensitive) == 0)
            {
                obj->has_mark = true;
                obj->has_cube = false;
            }
        }
    }

    updateFront();

    return size_set && steve_set;
}


bool World::loadFile(const QString &filename)
{
    QFile file{filename};
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader file_reader(&file);

    return loadXMLStream(file_reader);
}

bool World::loadXML(const QString &xml)
{
    QXmlStreamReader file_reader{xml};

    return loadXMLStream(file_reader);
}

bool World::saveFile(const QString &filename) const
{
    QFile file{filename};
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    QXmlStreamWriter file_writer(&file);
    file_writer.setAutoFormatting(true);
    file_writer.writeStartDocument();

    //Write world dimensions
    file_writer.writeStartElement("world");
    file_writer.writeAttribute("width", QString("%1").arg(size.first));
    file_writer.writeAttribute("length", QString("%1").arg(size.second));
    file_writer.writeAttribute("max_height", QString("%1").arg(max_height));

    //Write steve's position and orientation
    file_writer.writeStartElement("steve");
    file_writer.writeAttribute("x", QString("%1").arg(steve.first));
    file_writer.writeAttribute("y", QString("%1").arg(steve.second));
    file_writer.writeAttribute("orientation", orientation_str.at(orientation));
    file_writer.writeEndElement();

    //Write elements
    for(unsigned int x = 0; x < size.first; x++)
        for(unsigned int y = 0; y < size.second; y++)
        {
            const WorldObject &obj = map[x][y];
            if(obj.has_mark)
            {
                file_writer.writeStartElement("mark");
                file_writer.writeAttribute("x", QString("%1").arg(x));
                file_writer.writeAttribute("y", QString("%1").arg(y));
                file_writer.writeEndElement();
            }

            if(obj.has_cube)
                file_writer.writeStartElement("cube");
            else if(obj.stack_size > 0)
            {
                file_writer.writeStartElement("stack");
                file_writer.writeAttribute("height", QString("%1").arg(obj.stack_size));
            }
            else
                continue;

            file_writer.writeAttribute("x", QString("%1").arg(x));
            file_writer.writeAttribute("y", QString("%1").arg(y));
            file_writer.writeEndElement();

        }

    file_writer.writeEndElement();

    file_writer.writeEndDocument();

    return !file_writer.hasError();
}

void World::setMaxHeight(unsigned int max_height)
{
    //Don't accept ridiculously high worlds...
    if(max_height > 100)
        return;

    this->max_height = max_height;

    for(unsigned int x = 0; x < size.first; x++)
        for(unsigned int y = 0; y < size.second; y++)
        {
            WorldObject *obj = &map[x][y];

            if(obj->stack_size > max_height)
                obj->stack_size = max_height;
        }
}
