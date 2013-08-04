#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <string>

class Creature
{
public:
    Creature(std::string type, uint32_t id, uint16_t positionX, uint16_t positionY,
             uint16_t worldSizeX, uint16_t worldSizeY);

    void doAction();

    static std::string typeToString(uint16_t type);
    static int16_t stringToType(std::string type);

private:
    uint16_t type;
    uint32_t id;
    uint16_t positionX;
    uint16_t positionY;
    uint16_t worldSizeX;
    uint16_t worldSizeY;

    uint16_t strength;
    uint16_t agility;
    uint16_t intelligence;
    uint16_t sociability;
};

#endif // CREATURE_HPP
