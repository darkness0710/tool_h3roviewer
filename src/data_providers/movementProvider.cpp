#include "movementProvider.h"

#include "../memoryscanner.h"
#include "../gamestructs.h"

#include <qdebug.h>
MovementProvider::MovementProvider(QObject *parent)
    : DataProvider{parent},
      movementPoints(nullptr),
      previousMovementPoints(-1),
      lastHeroID(0xFEFEFEFE)
{
}

void MovementProvider::init(uint32_t *address)
{
    this->movementPoints = address;
    this->movementPoints = &MemoryScanner::hero->base.movePoints;
    this->maxMovementPoints= &MemoryScanner::hero->base.maxMovePoints;
}


void MovementProvider::sendValue()
{
    QString MPString;
    if(*this->movementPoints > 10000)
    {
        MPString = QString::number(*this->movementPoints/1000) + "k / ";
    }
    else
    {
        MPString = QString::number(*this->movementPoints) + " / ";
    }

    if(*this->maxMovementPoints > 10000)
    {
        MPString.append(QString::number(*maxMovementPoints/1000) + "k");
    }
    else
    {
        MPString.append(QString::number(*maxMovementPoints));
    }
    emit updated(MPString);
}

void MovementProvider::checkForUpdate()
{
    // Changed to another hero
    if (MemoryScanner::player->selectedHeroID != this->lastHeroID)
    {
        this->previousMovementPoints = *this->movementPoints;
        this->lastHeroID = MemoryScanner::player->selectedHeroID;
        sendValue();
        return;
    }

    if(this->previousMovementPoints == *this->movementPoints &&
            this->previousMaxMovementPoints == *this->maxMovementPoints)
    {
        return;
    }
    sendValue();
    this->previousMovementPoints = *this->movementPoints;

    return;
}

