#ifndef _DATASET_H
#define _DATASET_H

#include <QHash>
#include <QMutex>

class Fields {
public:
    int userID;
    int locationID;
    qint64 timestamp;
    double latitude;
    double longitude;

    explicit Fields(int _userID = 0,
                    int _locationID = 0,
                    qint64 _timestamp = 0,
                    double _latitude = 0,
                    double _longitude = 0)
            : userID(_userID), locationID(_locationID), timestamp(_timestamp), latitude(_latitude),
              longitude(_longitude) {}
};

class Dataset {
public:
    static QHash<int, Fields> dataset;
    static QMutex mutex;
    static bool ready;
};

#endif // _DATASET_H
