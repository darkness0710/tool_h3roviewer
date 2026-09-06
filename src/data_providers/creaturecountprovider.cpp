#include "creaturecountprovider.h"

#include "../gamestructs.h"
#include "../memoryscanner.h"

CreatureCountProvider::CreatureCountProvider(QObject *parent)
    : DataProvider{parent},
    creatureCount(nullptr),
    previousCreatureCount(0)
{

}

void CreatureCountProvider::init(int32_t * creatureCountAddress)
{
    this->creatureCount = creatureCountAddress;
}


void CreatureCountProvider::checkForUpdate()
{
    if(*this->creatureCount == this->previousCreatureCount)
    {
        return;
    }

    QString returnString(QString::number(*this->creatureCount));

    if(*this->creatureCount >= 10000)
    {
        returnString = returnString.chopped(3) + "k";
    }

    if(*this->creatureCount == 0 )
    {
        returnString  = "";
    }

    emit updated(returnString);
    this->previousCreatureCount = *this->creatureCount;
}
