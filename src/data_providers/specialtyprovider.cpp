#include "specialtyprovider.h"

#include "../imagesMap.h"

SpecialtyProvider::SpecialtyProvider(QObject *parent)
    : DataProvider{parent},
      specialty(nullptr),
      previousSpecialty(0xFF)
{
    this->specialty = &MemoryScanner::hero->base.heroSpecialty;
}

void SpecialtyProvider::checkForUpdate()
{
    if(*this->specialty == this->previousSpecialty)
    {
        return;
    }

    if (*this->specialty < specialityMap.size())
    {
        emit updated(specialityMap[*this->specialty]);
    }
    else
    {
        emit updated("Specialty id: " + QString::number(*this->specialty));
    }
    previousSpecialty = *this->specialty;
}
