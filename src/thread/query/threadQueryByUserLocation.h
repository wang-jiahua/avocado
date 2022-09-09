#ifndef _THREAD_QUERY_BY_USER_LOCATION_H
#define _THREAD_QUERY_BY_USER_LOCATION_H

#include "../../storage/dataLocationCount.h"
#include <QSet>

class ThreadQueryByUserLocation {
public:
    static DataLocationCount queryByUserLocation(
            qint64 timeBegin,
            qint64 timeEnd,
            const QSet<int> &userIDs,
            const QSet<int> &locationIDs);
};

#endif // _THREAD_QUERY_BY_USER_LOCATION_H
