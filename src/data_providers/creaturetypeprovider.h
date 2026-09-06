#ifndef CREATURETYPEPROVIDER_H
#define CREATURETYPEPROVIDER_H

#include "dataprovider.h"

class CreatureTypeProvider : public DataProvider
{
public:
    explicit CreatureTypeProvider(QObject *parent = nullptr);

    void init(uint32_t *creatureTypeAddress);

    void checkForUpdate();
private:
    uint32_t *creatureType;
    uint32_t previousCreatureType;
};

#endif // CREATURETYPEPROVIDER_H
