#ifndef PLAYERCOLORPROVIDER_H
#define PLAYERCOLORPROVIDER_H

#include "dataprovider.h"

class playerColorProvider : public DataProvider
{
public:
    explicit playerColorProvider(QObject *parent = nullptr);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param playerColorAddress The memory offset in the HeroStruct for the data source.
     */
    void init(uint8_t *playerColorAddress);
private:
    uint8_t *playerColor;
    uint8_t previousPlayerColor;
};

#endif // PLAYERCOLORPROVIDER_H
