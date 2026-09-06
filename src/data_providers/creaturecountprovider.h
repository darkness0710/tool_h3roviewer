#ifndef CREATURECOUNTPROVIDER_H
#define CREATURECOUNTPROVIDER_H

#include "dataprovider.h"

class CreatureCountProvider : public DataProvider
{
public:

    explicit CreatureCountProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param creatureCountAddress The memory address in the HeroStruct for the data source.
     */
    void init(int32_t *creatureCountAddress);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    int32_t *creatureCount;
    int32_t previousCreatureCount;
};

#endif // CREATURECOUNTPROVIDER_H
