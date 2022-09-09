#ifndef _CACHE_QUERY_BY_RANGE_H
#define _CACHE_QUERY_BY_RANGE_H

#include "dataTimeCount.h"
#include <QMap>

class CacheQueryByRange {
public:
    static QMap<qint64, DataTimeCount> cacheQueryByRange;
};

#endif // _CACHE_QUERY_BY_RANGE_H