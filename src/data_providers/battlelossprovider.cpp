#include "battlelossprovider.h"

#include "../memoryscanner.h"
#include "../gamestructs.h"

BattleLossProvider::BattleLossProvider(QObject *parent)
    : DataProvider{parent},
      creatureCount(nullptr),
      preBattleCreatureCount(-1),
      lastHeroID(0xFEFEFEFE)
{
    this->displayTimer.setInterval(3000);
    this->displayTimer.setSingleShot(true);

    connect(&this->displayTimer, &QTimer::timeout, this, &BattleLossProvider::clearDisplay);
}

void BattleLossProvider::init(int32_t *creatureCountAddress)
{
    this->creatureCount = creatureCountAddress;
}

void BattleLossProvider::checkForUpdate()
{
    // Changed to another hero
    if (MemoryScanner::player->selectedHeroID != this->lastHeroID)
    {
        this->preBattleCreatureCount = *this->creatureCount;
        this->lastHeroID = MemoryScanner::player->selectedHeroID;
        return;
    }

    // The frame after we left battle. The battle pointer will be true even
    // when the battle result is showing.
    if(MemoryScanner::gameState->isOnBattleResult)
    {
        int32_t result = *this->creatureCount - this->preBattleCreatureCount;
        if(result < 0)
        {
            emit updated(QString::number(result));
            this->displayTimer.start();
        }
    }

    // The battle pointer is != 0 in battles, this includes auto battle.
    // After rejecting an auto battle, there is a very short time (a few frames)
    // where this pointer goes back to 0, we must catch that one. This requires
    // this application to run at game speed of faster ( >60Hz)
    if(MemoryScanner::gameState->status.isInBattlePointer == 0)
    {
        this->preBattleCreatureCount = *creatureCount;
    }

    return;
}


void BattleLossProvider::clearDisplay()
{
    if(MemoryScanner::gameState->isOnBattleResult)
    {
        this->displayTimer.start();
    }
    else
    {
        emit updated(QString(""));
    }
}
