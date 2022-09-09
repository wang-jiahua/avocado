#ifndef _THREAD_QUERY_TRAJECTORY_H
#define _THREAD_QUERY_TRAJECTORY_H

#include "../../storage/dataTrajectory.h"

class ThreadQueryTrajectory {
public:
    static DataTrajectory queryTrajectory(
            qint64 timeBegin,
            qint64 timeEnd,
            int userID);
};

#endif // _THREAD_QUERY_TRAJECTORY_H
