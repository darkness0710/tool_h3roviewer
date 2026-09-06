#if defined(_MSC_VER)
#define NOMINMAX
#endif

#include "luckmoraleprovider.h"
#include "../memoryscanner.h"

#include <QString>

// Item ID : Luck boost
const QMap<uint64_t, int8_t> luckItems {{0x0000002D, 1},
                                        {0x0000002E, 1},
                                        {0x0000002F, 1},
                                        {0x00000030, 1},
                                        {0x0000006C, 3},
                                        {0x00000086, 1}};

// Item ID : Morale boost
const QMap<uint64_t, int8_t> moraleItems {{0x0000002D, 1},
                                          {0x00000031, 1},
                                          {0x00000032, 1},
                                          {0x00000033, 1},
                                          {0x0000006C, 3},
                                          {0x00000086, 1}};


LuckMoraleProvider::LuckMoraleProvider(QObject *parent)
    : DataProvider{parent},
      previousStat(50),
      type(Luck),
      itemMap(&luckItems)
{

}

void LuckMoraleProvider::checkForMoralLuckItems(const uint64_t item)
{
    if(!itemMap->contains(item))
    {
        return;
    }

    for (size_t i = 0;i < this->equipedItems.size();i++)
    {
        if (this->equipedItems[i] == item)
        {
            return;
        }
        if (this->equipedItems[i] == 0)
        {
            this->equipedItems[i] = item;
            return;
        }
    }
}


void LuckMoraleProvider::checkForUpdate()
{
    equipedItems.fill(0);
    HeroStructs *hero = MemoryScanner::hero;
    checkForMoralLuckItems(hero->base.arti1);
    checkForMoralLuckItems(hero->base.arti2);
    checkForMoralLuckItems(hero->base.arti3);
    checkForMoralLuckItems(hero->base.arti4);
    checkForMoralLuckItems(hero->base.arti5);
    checkForMoralLuckItems(hero->base.ringLeft);
    checkForMoralLuckItems(hero->base.ringRight);
    checkForMoralLuckItems(hero->base.neck);
    checkForMoralLuckItems(hero->base.chest);

    int8_t totalStat = 0;
    for(size_t i = 0; i < this->equipedItems.size(); i++)
    {
        if(this->equipedItems[i] != 0)
        {
            // "itemMap" will always contain all keys which is stored in "equipedItems".
            totalStat += (*this->itemMap)[this->equipedItems[i]];
        }
    }

    totalStat += *this->temporaryBuff + *this->skillLevel;
    totalStat = std::min(static_cast<int8_t> (3), totalStat);
    totalStat = std::max(static_cast<int8_t> (-3), totalStat);

    if(totalStat != this->previousStat)
    {
        QString imageType = this->type == Morale ? "morale":"luck";

        emit updated(":/images/main/" + imageType + QString::number(totalStat) +".png");
        this->previousStat = totalStat;
    }
}

void LuckMoraleProvider::init(LuckMoraleEnum type)
{
    this->type = type;
    this->skillLevel = &MemoryScanner::hero->base.luckSkill;
    this->temporaryBuff = &MemoryScanner::hero->base.temporaryLuck;
    if (type == Morale)
    {
        this->skillLevel = &MemoryScanner::hero->base.leadershipSkill;
        this->temporaryBuff = &MemoryScanner::hero->base.temporaryMorale;
        this->itemMap = &moraleItems;
    }
}
