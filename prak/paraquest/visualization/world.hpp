#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>
#include <vector>

#include <QGraphicsScene>
#include <QObject>

#include "creature.hpp"

class World : public QObject
{
    Q_OBJECT
public:
    World(quint16 sizeY, quint16 sizeX, quint16 scale, QGraphicsScene *scene);
    void addCreature(QString type);
    // TODO
    // some container type getCollisions();
    void populate(quint32 count);
    void simulate(quint32 ticks);

public slots:
    void updateCreatures();

private:
    std::vector<std::unique_ptr<Creature>> creatures;
    quint16 sizeX;
    quint16 sizeY;
    quint16 scale;
    QGraphicsScene *scene;
};

#endif // WORLD_HPP