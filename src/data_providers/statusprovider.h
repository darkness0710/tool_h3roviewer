#ifndef STATUSPROVIDER_H
#define STATUSPROVIDER_H

enum StatusState
{
    NOT_IN_ACTIVE_GAME = 0,
    NEW_DAY = 1,
    HERO = 3,
    CASTLE = 4,
    MAP_INTERACTION = 6
};

#include "dataprovider.h"
#include <QObject>
#include <QTimer>

class StatusProvider : public DataProvider
{
public:
    explicit StatusProvider(QObject *parent = nullptr);

    /**
     * @brief checkForUpdate Compares current frame data with previous frame
     * data to figure out if new data is available
     */
    void checkForUpdate();

private slots:
    void setToNormal();


private:
    uint8_t *interactionDisplayID;
    uint8_t previousInteractionDisplayID;
    uint16_t *interactionID;
    uint16_t previousInteractionID;
    QTimer maxNewDayDuration;
};

#endif // STATUSPROVIDER_H
