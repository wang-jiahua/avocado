#ifndef _THREAD_QUERY_BY_USER_H
#define _THREAD_QUERY_BY_USER_H

#include "../../storage/dataLocationCount.h"
#include <QSet>

class ThreadQueryByUser {
public:
    static DataLocationCount queryByUsers(qint64 timeBegin, qint64 timeEnd, const QSet<int> &userIDs);

    static DataLocationCount queryByUser(qint64 timeBegin, qint64 timeEnd, int userID);
};

#endif // _THREAD_QUERY_BY_USER_H
