#ifndef _DATA_TIME_COUNT_H
#define _DATA_TIME_COUNT_H

#include <QList>

struct EntryTimeCount {
    qint64 timestamp;
    int visitCount;

    EntryTimeCount(qint64 _timestamp, int _visitCount)
            : timestamp(_timestamp), visitCount(_visitCount) {
    }
};

using DataTimeCount = QList<EntryTimeCount>;

#endif // _DATA_TIME_COUNT_H
