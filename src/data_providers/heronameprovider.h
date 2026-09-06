#ifndef HERONAMEPROVIDER_H
#define HERONAMEPROVIDER_H

#include "dataprovider.h"

class HeroNameProvider : public DataProvider
{
public:
    explicit HeroNameProvider(QObject *parent = nullptr);

    /**
     * @brief setAffix Adds a string suffix and/or prefix to the data provided
     * by a provider.
     * @param prefix A string prefix to prepend the data.
     * @param suffix A string suffix to append the data.
     */
    void setAffix(const QString &prefix, const QString &suffix);


    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint32_t *heroID;
    uint32_t previousHeroID;

    QString prefix;
    QString suffix;
};

#endif // HERONAMEPROVIDER_H
