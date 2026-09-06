#ifndef STATISTICSDATABASE_H
#define STATISTICSDATABASE_H

#include <QObject>
#include <QSqlDatabase>

class StatisticsDatabase : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase statistics;

public:
    explicit StatisticsDatabase(QObject *parent = nullptr);

signals:

};

#endif // STATISTICSDATABASE_H
