#ifndef PRIMARYSKILLPROVIDER_H
#define PRIMARYSKILLPROVIDER_H

#include "dataprovider.h"

class PrimarySkillProvider : public DataProvider
{
public:
    explicit PrimarySkillProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param primarySkillAddress The memory offset in the HeroStruct for the data source.
     */
    void init(uint8_t *primarySkillAddress);

    /**
     * @brief setMaxMin Sets value max/min for data the provider should give.
     * @param max The maximum value the provider should be able to give
     * @param min The minimum value the provider should be able to give
     */
    void setMinMax(size_t min, size_t max);

    /**
     * @brief setAffix Adds a string suffix and/or prefix to the data provided
     * by a provider.
     * @param prefix A string prefix to prepend the data.
     * @param suffix A string suffix to append the data.
     */
    void setAffix(QString prefix, QString suffix);


    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint8_t *skillLevel;
    uint8_t previousSkillLevel;
    size_t maxValue;
    size_t minValue;
    QString prefix;
    QString suffix;
};

#endif // PRIMARYSKILLPROVIDER_H
