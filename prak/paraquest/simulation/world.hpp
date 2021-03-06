#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>
#include <vector>
#include <string>

#include "creature.hpp"

class World
{
public:
    World(uint16_t size_x, uint16_t sizeY, uint16_t comm_size, uint16_t comm_rank, char* processor_name);
    std::vector<std::vector<std::unique_ptr<Creature>>::iterator> getCreaturesAt(uint16_t x, uint16_t y);
    void addCreature(std::string species);
    void populate(uint32_t creature_count, uint32_t obstacle_count);
    void simulate(uint32_t ticks);
    void move();
    void sync();
    void collide();
    void dumpSettings(uint32_t max_tick);
    void dumpState(uint32_t tick, uint32_t max_tick);

private:
    // TODO maybe ust std::map<uint16_t id, std::unique_ptr<Creature>> instead to deal with id duplicates
    // when putting things back together during visualization?
    std::vector<std::unique_ptr<Creature>> creatures;
    uint16_t sizeX;
    uint16_t sizeY;
    uint16_t commSize;
    uint16_t commRank;
    uint16_t chunkSize;
    uint16_t offset;
    char* processorName;
    uint32_t currentId; // ever-increasing number that uniquely identifies a creature
};

#endif // WORLD_HPP
