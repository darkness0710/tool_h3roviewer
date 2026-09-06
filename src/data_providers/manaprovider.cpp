#include "manaprovider.h"

ManaProvider::ManaProvider(QObject *parent)
    : DataProvider{parent},
      manaPoints(nullptr),
      previousManaPoints(0)
{

}

void ManaProvider::init(uint16_t * manaAddress)
{
    this->manaPoints = manaAddress;
}

void ManaProvider::checkForUpdate()
{
    if(*this->manaPoints == this->previousManaPoints)
    {
        return;
    }
    emit updated(QString::number(*this->manaPoints));
    previousManaPoints = *this->manaPoints;
}

