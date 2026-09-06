#include "creaturetypeprovider.h"

#include "../imagesMap.h"

#define NONE ""

CreatureTypeProvider::CreatureTypeProvider(QObject *parent)
    : DataProvider{parent},
      creatureType(nullptr),
      previousCreatureType(0xFFFFFFFF)
{

}

void CreatureTypeProvider::init(uint32_t * creatureTypeAddress)
{
    this->creatureType = creatureTypeAddress;
}


void CreatureTypeProvider::checkForUpdate()
{
    if(*this->creatureType == this->previousCreatureType)
    {
        return;
    }

    if (monsterMap.contains(*this->creatureType))
    {
        emit updated(monsterMap[*this->creatureType]);
    }
    else if(*this->creatureType == NO_CREATURE)
    {
        emit updated(NONE);
    }
    else
    {
        emit updated("Creature id: " + QString::number(*this->creatureType));
    }
    this->previousCreatureType = *this->creatureType;
}
