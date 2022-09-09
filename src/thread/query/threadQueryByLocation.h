#ifndef _THREAD_QUERY_BY_LOCATION_H
#define _THREAD_QUERY_BY_LOCATION_H

#include "../../storage/dataTimeCount.h"
#include <QSet>

class ThreadQueryByLocation {
public:
    static DataTimeCount queryByLocations(qint64 timeBegin, qint64 timeEnd, const QSet<int> &locationIDs, qint64 step);

    static DataTimeCount queryByLocation(qint64 timeBegin, qint64 timeEnd, int locationID, qint64 step);
};

#endif // _THREAD_QUERY_BY_LOCATION_H
