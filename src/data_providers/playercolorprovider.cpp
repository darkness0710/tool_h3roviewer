#include "playercolorprovider.h"

playerColorProvider::playerColorProvider(QObject *parent)
    : DataProvider{parent},
      playerColor(nullptr),
      previousPlayerColor(0xFF)
{

}

void playerColorProvider::init(uint8_t * playerColorAddress)
{
    this->playerColor = playerColorAddress;
}

void playerColorProvider::checkForUpdate()
{
    if(*this->playerColor == this->previousPlayerColor)
    {
        return;
    }
    emit updated(QString::number(*this->playerColor));
    this->previousPlayerColor = *this->playerColor;
}
