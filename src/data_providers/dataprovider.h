#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>

#include "../memoryscanner.h"

class DataProvider : public QObject
{
    Q_OBJECT
public:
    explicit DataProvider(QObject *parent = nullptr);

    virtual void checkForUpdate() = 0;

signals:
    /**
     * @brief updated signal to be triggered when a provider has detected that
     * new data is available.
     * @param value The new updated value.
     */
    void updated(const QString &value);
};

#endif // DATAPROVIDER_H
