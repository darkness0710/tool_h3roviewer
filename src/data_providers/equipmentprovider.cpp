#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif


#include "equipmentprovider.h"
#include "../imagesMap.h"

constexpr char SLOT_BLOCKED_IMG[] = ":/images/items/Artifact_Blocked.png";
constexpr char NONE[] = "";

constexpr uint32_t SCROLL_ID = 0x00000001;


EquipmentProvider::EquipmentProvider() :

    equipment(nullptr),
    previousEquipment(0xFFFFFFFF),
    useBlocked(false),
    blocked(nullptr),
    previousBlocked(0xFF),
    arties{nullptr,nullptr,nullptr,nullptr,nullptr},
    previousArties{0xFF,0xFF,0xFF,0xFF,0xFF},
    rightRing(nullptr),
    previousRightRing(0xFF)
{

}
QString EquipmentProvider::mapValueToItem(const uint32_t *itemValue)
{
    uint32_t itemID = *itemValue;

    if (*(itemValue+1) == 0 && itemID == 0)
    {
        return QString(NONE);
    }

    if (itemID == SCROLL_ID)
    {
        if(scrollMap.contains(*(itemValue+1)))
        {
            return scrollMap[*(itemValue+1)];
        }
    }
    if (itemMap.contains(itemID))
    {
        return itemMap[itemID];
    }
    else if (itemID == NO_ITEM)
    {
        return QString(NONE);
    }
    else if(itemID == 0)
    {
        return QString(NONE);
    }
    return QString("Item id: " + QString::number(itemID));
}

void EquipmentProvider::handleEquipment()
{
    // If useBlocked is set, use it as well to check if we have to do something.
    if(*this->equipment == this->previousEquipment &&
            (this->useBlocked == 0 ||
             (this->useBlocked != 0 && *this->blocked == this->previousBlocked)))
    {
        return;
    }

    QString itemInSlot("");
    if(*this->equipment != NO_ITEM)
    {
        itemInSlot = mapValueToItem(this->equipment);
    }
    else if(this->useBlocked)
    {
        // Check if slot is blocked by combi arti
        if(*this->blocked)
        {
            itemInSlot = QString(SLOT_BLOCKED_IMG);
        }
    }
    emit updated(itemInSlot);
    if(this->useBlocked)
    {
        this->previousBlocked = *this->blocked;
    }
    this->previousEquipment = *this->equipment;
}

void EquipmentProvider::handleLeftRing()
{
    if(this->useBlocked == 0)
    {
        return;
    }

    if (*this->equipment == previousEquipment
            && *this->blocked == previousBlocked &&
            *this->rightRing == this->previousRightRing)
    {
        return;
    }

    QString itemInSlot("");

    if (*this->equipment != NO_ITEM)
    {
        itemInSlot = mapValueToItem(this->equipment);
    }
    // Check if slot is blocked by combi arti
    else if((*this->blocked == 1 && *this->rightRing != NO_ITEM)
            || *this->blocked == 2)
    {
        itemInSlot = QString(SLOT_BLOCKED_IMG);
    }
    emit updated(itemInSlot);
    this->previousBlocked = *this->blocked;
    this->previousEquipment = *this->equipment;
    this->previousRightRing = *this->rightRing;
}

void EquipmentProvider::handleArti()
{
    std::array <uint32_t, 5> arti;

    bool anyArtiUpdated = false;
    for (size_t i = 0; i < this->arties.size(); i++)
    {
        arti[i] = *this->arties[i];
        // Scrolls are a special case, all scrolls has the same ID but
        // uses the second uint32 to identify which scroll.
        if(arti[i] == SCROLL_ID)
        {
            arti[i] = *(this->equipment + 1);
        }

        if(arti[i] != this->previousArties[i])
        {
            anyArtiUpdated = true;
            break;
        }
    }

    if (*this->blocked == previousBlocked &&
            anyArtiUpdated == false)
    {
        return;
    }

    uint8_t blockedArtiSlots = 0;
    QString itemInSlot("");

    for(ssize_t i = this->arties.size() - 1; i >= 0; i--)
    {
        uint32_t *otherArti = this->arties[i];

        if(*otherArti == NO_ITEM)
        {
            blockedArtiSlots++;
            if (this->equipment == otherArti &&
                    *this->blocked >= blockedArtiSlots)
            {
                itemInSlot = QString(SLOT_BLOCKED_IMG);
                break;
            }
        }
    }
    if(*this->equipment != NO_ITEM)
    {
        itemInSlot = mapValueToItem(this->equipment);
    }
    emit updated(itemInSlot);
    this->previousEquipment = *this->equipment;
    this->previousBlocked = *this->blocked;
    for ( size_t i = 0; i < arties.size(); i++)
    {
        this->previousArties[i] = arti[i];
    }
}

void EquipmentProvider::init(const imageType type,
                             uint32_t * equipmentSlotAddress,
                             uint8_t * blockSlotAddress)
{

    this->type = type;
    if(blockSlotAddress)
    {
        this->useBlocked = true;
    }

    this->equipment = equipmentSlotAddress;
    this->blocked = blockSlotAddress;
    this->rightRing = &MemoryScanner::hero->base.ringRight;
    this->arties[0] = &MemoryScanner::hero->base.arti1;
    this->arties[1] = &MemoryScanner::hero->base.arti2;
    this->arties[2] = &MemoryScanner::hero->base.arti3;
    this->arties[3] = &MemoryScanner::hero->base.arti4;
    this->arties[4] = &MemoryScanner::hero->base.arti5;

}


void EquipmentProvider::checkForUpdate()
{
    switch(this->type)
    {
    case EquipmentProvider::Equipment:
        handleEquipment();
        break;
    case EquipmentProvider::LeftRing:
        handleLeftRing();
        break;
    case EquipmentProvider::Artifact:
        handleArti();
        break;
    }
}
