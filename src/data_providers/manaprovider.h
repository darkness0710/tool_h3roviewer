#ifndef MANAPROVIDER_H
#define MANAPROVIDER_H

#include "dataprovider.h"

class ManaProvider : public DataProvider
{
public:
    explicit ManaProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param manaAddress The memory offset in the HeroStruct for the data source.
     */
    void init(uint16_t *manaAddress);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint16_t *manaPoints;
    uint16_t previousManaPoints;
};

#endif // MANAPROVIDER_H
