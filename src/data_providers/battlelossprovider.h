#ifndef BATTLELOSSPROVIDER_H
#define BATTLELOSSPROVIDER_H

#include "dataprovider.h"
#include <QTimer>

class BattleLossProvider : public DataProvider
{
public:
    explicit BattleLossProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets the memory location in the hero struct where the
     * provider should read from.
     * @param creatureCountAddress The memory address for the creature count for
     * the given slot creature slot.
     */
    void init(int32_t *creatureCountAddress);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available.
     */
    void checkForUpdate();

private slots:
    /**
     * @brief clearDisplay A slot to trigger the clearing of the battle loss
     * text.
     */
    void clearDisplay();

private:
    int32_t *creatureCount;
    int32_t preBattleCreatureCount; /*< Always has the previous creature count */
    uint32_t lastHeroID;
    QTimer displayTimer; /*< Timer to trigger when the battle loss display should clear */
};

#endif // BATTLELOSSPROVIDER_H
