#ifndef SKILLPROVIDER_H
#define SKILLPROVIDER_H

#include "dataprovider.h"

class SkillProvider : public DataProvider
{
public:
    explicit SkillProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets internal member variables required when calling
     * "checkForUpdate".
     * @param skillSlot Which skill slot (1-8) the provider should provide data
     * for.
     */
    void init(uint8_t skillSlot);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint8_t skillSlot;
    uint8_t *skills;
    std::unique_ptr<uint8_t[]> previousSkill;

    uint8_t *skillOrder;
};

#endif // SKILLPROVIDER_H
