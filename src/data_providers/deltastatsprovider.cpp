#include "deltastatsprovider.h"

#include "../memoryscanner.h"
#include "../gamestructs.h"

DeltaStatsProvider::DeltaStatsProvider(QObject *parent)
    : DataProvider{parent},
      statValue(nullptr),
      previousStatValue(0),
      lastHeroID(0)
{
    this->displayTimer.setInterval(2000);
    this->displayTimer.setSingleShot(true);

    connect(&this->displayTimer, &QTimer::timeout, this, &DeltaStatsProvider::clearDisplay);
}

void DeltaStatsProvider::init(uint8_t *primaryStatAddress)
{
    this->statValue = primaryStatAddress;
}

void DeltaStatsProvider::checkForUpdate()
{
    if(this->lastHeroID != MemoryScanner::player->selectedHeroID)
    {
        this->previousStatValue = *this->statValue;
        this->lastHeroID = MemoryScanner::player->selectedHeroID;
        return;
    }

    // No change, do nothing
    if(*this->statValue == this->previousStatValue)
    {
        return;
    }

    int8_t result = int8_t(*this->statValue - this->previousStatValue);
    this->previousStatValue = *this->statValue;

    if(result < 0)
    {
        emit labelColor(QString("QLabel{color:#FF0000}"));
        emit updated(QString::number(result));
        this->displayTimer.start();
    }
    else if(result > 0)
    {
        emit labelColor(QString("QLabel{color:#00FF00}"));
        emit updated("+" + QString::number(result));
        this->displayTimer.start();
    }
}

void DeltaStatsProvider::clearDisplay()
{
    emit updated(QString(""));
}
