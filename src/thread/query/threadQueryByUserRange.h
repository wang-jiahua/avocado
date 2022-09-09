#ifndef _THREAD_QUERY_BY_USER_RANGE_H
#define _THREAD_QUERY_BY_USER_RANGE_H

#include "../../storage/GPSRange.h"
#include "../../storage/dataLocationCount.h"
#include <QSet>

class ThreadQueryByUserRange {
public:
    static DataLocationCount queryByUserRange(
            qint64 timeBegin,
            qint64 timeEnd,
            const GPSRange &range,
            const QSet<int> &userIDs);
};

#endif // _THREAD_QUERY_BY_USER_RANGE_H
