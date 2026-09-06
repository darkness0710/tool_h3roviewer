#ifndef DELTASTATSPROVIDER_H
#define DELTASTATSPROVIDER_H

#include "dataprovider.h"
#include <QTimer>
#include <QObject>

class DeltaStatsProvider : public DataProvider
{
    Q_OBJECT
public:
    explicit DeltaStatsProvider(QObject *parent = nullptr);

    /**
     * @brief init Sets the memory location in the hero struct where the
     * provider should read from.
     * @param primaryStatAddress The memory offset for the given stat.
     */
    void init(uint8_t *primaryStatAddress);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available.
     */
    void checkForUpdate();

signals:
    /**
     * @brief labelColor Signal to trigger a QLabel to change the font color
     * @param styleSheetColor The stylesheet to se the font color.
     * Will be either red or green.
     */
    void labelColor(const QString &styleSheetColor);

private slots:
    /**
     * @brief clearDisplay A slot to trigger the clearing of the stat change.
     */
    void clearDisplay();

private:
    QTimer displayTimer; /*< The timer to trigger clearing of the stat change */

    uint8_t *statValue;
    uint8_t previousStatValue;

    uint32_t lastHeroID;
};

#endif // DELTASTATSPROVIDER_H
