#include "statusprovider.h"
#include "../memoryscanner.h"


StatusProvider::StatusProvider(QObject *parent)
    : DataProvider{parent},
      interactionDisplayID(nullptr),
      previousInteractionDisplayID(0),
      interactionID(nullptr),
      previousInteractionID(0)
{
    this->interactionDisplayID = &MemoryScanner::gameState->status.interactionDisplayID;
    this->interactionID = &MemoryScanner::gameState->status.interactionID;
    this->maxNewDayDuration.setInterval(4000);
    this->maxNewDayDuration.setSingleShot(true);
    connect(&this->maxNewDayDuration, &QTimer::timeout, this, &StatusProvider::setToNormal);
}

void StatusProvider::checkForUpdate()
{
    if (*this->interactionDisplayID == StatusState::MAP_INTERACTION &&
            (this->previousInteractionDisplayID != *this->interactionDisplayID ||
             this->previousInteractionID        != *this->interactionID))
    {
        emit updated(QString::number(StatusState::MAP_INTERACTION));
    }
    if (*this->interactionDisplayID == StatusState::NEW_DAY &&
            *this->interactionDisplayID != this->previousInteractionDisplayID)
    {
        this->maxNewDayDuration.start();
        emit updated(QString::number(StatusState::NEW_DAY));
    }
    if ((*this->interactionDisplayID == StatusState::HERO ||
            *this->interactionDisplayID == StatusState::CASTLE) &&
            this->previousInteractionDisplayID != *this->interactionDisplayID)
    {
        emit updated(QString::number(StatusState::HERO));
    }

    if(*this->interactionDisplayID == StatusState::NOT_IN_ACTIVE_GAME &&
       this->previousInteractionDisplayID != *this->interactionDisplayID)
    {
        emit updated(QString::number(StatusState::HERO));
    }

    this->previousInteractionDisplayID = *this->interactionDisplayID;
    this->previousInteractionID = *this->interactionID;
}

void StatusProvider::setToNormal()
{
    if(*this->interactionDisplayID == StatusState::NEW_DAY)
    {
        emit updated(QString::number(StatusState::HERO));
    }
}
