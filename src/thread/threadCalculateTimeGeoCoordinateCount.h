#ifndef _THREAD_CALCULATE_TIME_GEOCOORDINATE_COUNT_H
#define _THREAD_CALCULATE_TIME_GEOCOORDINATE_COUNT_H

#include "thread.h"

class ThreadCalculateTimeGeoCoordinateCount : public Thread {
public:
    explicit ThreadCalculateTimeGeoCoordinateCount(QObject *parent);

private:
    void run() override;
};

#endif // _THREAD_CALCULATE_TIME_GEOCOORDINATE_COUNT_H
