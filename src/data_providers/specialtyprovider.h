#ifndef SPECIALTYPROVIDER_H
#define SPECIALTYPROVIDER_H

#include "dataprovider.h"

class SpecialtyProvider : public DataProvider
{
public:
    explicit SpecialtyProvider(QObject *parent = nullptr);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private:
    uint8_t *specialty;
    uint8_t previousSpecialty;
};

#endif // SPECIALTYPROVIDER_H
