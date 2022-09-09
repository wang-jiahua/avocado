#ifndef _THREAD_QUERY_ALL_H
#define _THREAD_QUERY_ALL_H

#include "../../storage/dataTimeCount.h"

class ThreadQueryAll {
public:
    static DataTimeCount queryAll(
            qint64 timeBegin,
            qint64 timeEnd,
            qint64 step);
};

#endif // _THREAD_QUERY_ALL_H
