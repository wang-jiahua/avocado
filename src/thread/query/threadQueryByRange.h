#ifndef _THREAD_QUERY_BY_RANGE_H
#define _THREAD_QUERY_BY_RANGE_H

#include "../../storage/GPSRange.h"
#include "../../storage/dataTimeCount.h"

class ThreadQueryByRange {
public:
    static DataTimeCount queryByRange(
            qint64 timeBegin,
            qint64 timeEnd,
            const GPSRange &range,
            qint64 step);
};

#endif // _THREAD_QUERY_BY_RANGE_H
