#include "statisticsdatabase.h"

StatisticsDatabase::StatisticsDatabase(QObject *parent)
    : QObject{parent},
    statistics("stats")
{
    statistics.setDatabaseName();
}
