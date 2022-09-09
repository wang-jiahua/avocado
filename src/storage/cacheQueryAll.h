#ifndef _CACHE_QUERY_ALL_H
#define _CACHE_QUERY_ALL_H

#include "dataTimeCount.h"
#include <QMap>

class CacheQueryAll {
public:
    static QMap<qint64, DataTimeCount> cacheQueryAll;
};

#endif // _CACHE_QUERY_ALL_H