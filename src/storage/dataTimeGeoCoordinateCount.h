#ifndef _DATA_TIME_GEOCOORDINATE_COUNT_H
#define _DATA_TIME_GEOCOORDINATE_COUNT_H

#include <QGeoCoordinate>
#include <QHash>
#include <QMap>

using EntryTimeGeoCoordinateCount = QHash<QGeoCoordinate, int>;

class DataTimeGeoCoordinateCount {
public:
    static QMap<qint64, EntryTimeGeoCoordinateCount> dataTimeGeoCoordinateCount;
};

#endif // _DATA_TIME_GEOCOORDINATE_COUNT_H
