#include "primaryskillprovider.h"

PrimarySkillProvider::PrimarySkillProvider(QObject *parent)
    : DataProvider{parent},
      skillLevel(nullptr),
      previousSkillLevel(0xFF),
      maxValue(INT64_MAX),
      minValue(INT64_MIN)
{

}

void PrimarySkillProvider::init(uint8_t * primarySkillAddress)
{
    this->skillLevel = primarySkillAddress;
}


void PrimarySkillProvider::setMinMax(size_t min, size_t max)
{
    this->minValue = min;
    this->maxValue = max;
}


void PrimarySkillProvider::setAffix(QString prefix, QString suffix)
{
    this->prefix = prefix;
    this->suffix = suffix;
}

void PrimarySkillProvider::checkForUpdate()
{
    if(*this->skillLevel == this->previousSkillLevel)
    {
        return;
    }

    QString number;
    // The game treats skill lavels above 230 as the minimum possible.
    // Ex, having 250 Knowledge will result in having 1 Knowledge in game.
    if(*this->skillLevel > 230)
    {
        number = QString::number(this->minValue);
    }
    else if(*this->skillLevel >= this->maxValue)
    {
        number = QString::number(this->maxValue);
    }
    else if(*this->skillLevel <= this->minValue)
    {
        number = QString::number(this->minValue);
    }
    else
    {
        number = QString::number(*this->skillLevel);
    }
    emit updated(this->prefix + number + this->suffix);
    this->previousSkillLevel = *this->skillLevel;
}

