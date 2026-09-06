#include "heronameprovider.h"

#include "../imagesMap.h"

HeroNameProvider::HeroNameProvider(QObject *parent)
    : DataProvider{parent},
      heroID(nullptr),
      previousHeroID(0xFFFFFFFF)
{
    this->heroID = &MemoryScanner::player->selectedHeroID;
}


void HeroNameProvider::setAffix(const QString &prefix, const QString &suffix)
{
    this->prefix = prefix;
    this->suffix = suffix;
}

void HeroNameProvider::checkForUpdate()
{

    if((*this->heroID == NO_HERO || *this->heroID == this->previousHeroID))
    {
        return;
    }

    if (*this->heroID < heroMap.size())
    {
        emit updated(this->prefix + heroMap[*this->heroID] + this->suffix);
    }
    else
    {
        emit updated("Hero id: " + QString::number(*this->heroID));
    }
    this->previousHeroID = *this->heroID;
}
