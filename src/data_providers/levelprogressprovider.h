#ifndef LEVELPROGRESSPROVIDER_H
#define LEVELPROGRESSPROVIDER_H

#include "dataprovider.h"
#include <QObject>

class LevelProgressProvider : public DataProvider
{
public:
    explicit LevelProgressProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param valueOffset The memory offset in the HeroStruct for the data source.
     */
    void init(uint32_t *xpAddress);


    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint32_t *currentXP;
    uint32_t previousXP;
};

#endif // LEVELPROGRESSPROVIDER_H
