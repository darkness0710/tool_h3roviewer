#include "buffprovider.h"
#include "../imagesMap.h"

BuffProvider::BuffProvider(QObject *parent)
    : DataProvider{parent},
    buffType(None),
    buff(nullptr),
    previousBuff(0),
    bitMask(0)
{

}


void BuffProvider::init(const BuffType buffType)
{
    switch (buffType)
    {
    case BuffType::Stabled:
        this->bitMask = 1 << (1);
        break;
    case BuffType::Trailblazer:
        this->bitMask = 1 << (23);
        break;
    case BuffType::None:
        this->bitMask = 0;
        break;
    }

    this->buffType = buffType;
    this->buff = &MemoryScanner::hero->base.buffType;
    this->hotaBuff = &MemoryScanner::hero->hota.buffType;
}


void BuffProvider::checkForUpdate()
{
    uint32_t currentBuff = ((*this->buff) + ((*this->hotaBuff) << 16)) & this->bitMask;
    if(currentBuff == this->previousBuff)
    {
        return;
    }
    if (currentBuff == 0)
    {
        emit updated("");
    }
    else if(buffMap.contains(currentBuff))
    {
        emit updated(buffMap[currentBuff]);
    }
    else
    {
        emit updated("Unknown_buff_" + QString::number(currentBuff));
    }


    this->previousBuff = currentBuff;
}
