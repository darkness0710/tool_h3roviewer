#ifndef MOVEMENTPROVIDER_H
#define MOVEMENTPROVIDER_H

#include "dataprovider.h"

class MovementProvider : public DataProvider
{
public:
    explicit MovementProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets the memory location in the hero struct where the
     * provider should read from.
     * @param address The memory address to read.
     */
    void init(uint32_t *address);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available.
     */
    void checkForUpdate();


private:
    void sendValue();
    uint32_t *movementPoints;
    uint32_t *maxMovementPoints;
    uint32_t previousMovementPoints;
    uint32_t previousMaxMovementPoints;
    uint32_t lastHeroID;

};

#endif // MOVEMENTPROVIDER_H
