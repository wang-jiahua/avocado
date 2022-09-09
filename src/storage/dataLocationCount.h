#ifndef _DATA_LOCATION_COUNT_H
#define _DATA_LOCATION_COUNT_H

#include <QList>

struct EntryLocationCount {
    int locationID;
    int visitCount;

    EntryLocationCount(int _locationID, int _visitCount)
            : locationID(_locationID), visitCount(_visitCount) {
    }
};

using DataLocationCount = QList<EntryLocationCount>;

#endif // _DATA_LOCATION_COUNT_H