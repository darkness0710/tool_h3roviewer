#ifndef BUFFPROVIDER_H
#define BUFFPROVIDER_H

#include "dataprovider.h"

class BuffProvider : public DataProvider
{
public:
    explicit BuffProvider(QObject *parent = nullptr);

    enum BuffType
    {
        None,
        Stabled,
        Trailblazer
    };

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param buffType The type of buff to check for
     */
    void init(const BuffType buffType);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    BuffType buffType;
    uint16_t * buff;
    uint8_t * hotaBuff;
    uint32_t  previousBuff;
    uint32_t bitMask;


};

#endif // BUFFPROVIDER_H
