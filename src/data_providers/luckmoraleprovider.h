#ifndef LUCKMORALEPROVIDER_H
#define LUCKMORALEPROVIDER_H

#include "dataprovider.h"

#include <QObject>
#include <QMap>
#include <array>

class LuckMoraleProvider : public DataProvider
{
public:
    enum LuckMoraleEnum
    {
        Luck,
        Morale
    };
    explicit LuckMoraleProvider(QObject *parent = nullptr);
    void checkForUpdate();
    void init(LuckMoraleEnum type);
private:
    void checkForMoralLuckItems(const uint64_t item);

    std::array<uint64_t, 9> equipedItems;
    int8_t previousStat;
    uint8_t *skillLevel;
    int8_t *temporaryBuff;
    LuckMoraleEnum type;

    const QMap<uint64_t, int8_t> * itemMap;
};

#endif // LUCKMORALEPROVIDER_H
