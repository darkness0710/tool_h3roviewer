#ifndef EQUIPMENTPROVIDER_H
#define EQUIPMENTPROVIDER_H

#include <QObject>

#include "dataprovider.h"
#include "../gamestructs.h"
#include "../memoryscanner.h"


class EquipmentProvider : public DataProvider
{
public:
    enum imageType
    {
        Equipment,
        LeftRing,
        Artifact
    };

    EquipmentProvider();

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param type Which type of data this provider should convert for
     * @param equipmentSlotAddress The memory address in the HeroStruct for the data source.
     * @param blockSlotAddress The memory address in the HeroStruct where the
     * "slot blocked" is stored.
     */
    void init(const imageType type,
              uint32_t *equipmentSlotAddress,
              uint8_t *blockSlotAddress);


    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    /**
     * @brief mapValueToItem Converts an item ID to a resource image path.
     * @param itemValue The item id.
     * @return A resource path to the matching image of the id.
     */
    QString mapValueToItem(const uint32_t *itemValue);

    /**
     * @brief handleEquipment Checks general equipments for changes. If a change
     * has occured, triggers the "updated" signal
     */
    void handleEquipment();

    /**
     * @brief handleLeftRing Checks the special left ring slot for changes.
     * If a change has occured, triggers the "updated" signal
     */
    void handleLeftRing();

    /**
     * @brief handleArti Checks the arti slots for changes. If a change
     * has occured, triggers the "updated" signal
     */
    void handleArti();

    uint32_t *equipment;
    uint32_t previousEquipment;

    bool useBlocked;
    uint8_t *blocked;
    uint8_t previousBlocked;

    std::array <uint32_t*, 5> arties;
    std::array <uint32_t, 5> previousArties;

    uint32_t *rightRing;
    uint32_t previousRightRing;

    imageType type;
};

#endif // EQUIPMENTPROVIDER_H
