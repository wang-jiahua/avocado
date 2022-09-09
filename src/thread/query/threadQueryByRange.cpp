#include "threadQueryByRange.h"
#include "../../storage/cacheQueryByRange.h"
#include "../../storage/kdtree.h"
#include <QDebug>

DataTimeCount ThreadQueryByRange::queryByRange(
        qint64 timeBegin,
        qint64 timeEnd,
        const GPSRange &range,
        qint64 step) {
    double latitudeBegin = range.latitudeBegin;
    double latitudeEnd = range.latitudeEnd;
    double longitudeBegin = range.longitudeBegin;
    double longitudeEnd = range.longitudeEnd;

    qint64 key = qHash(latitudeBegin) * qHash(latitudeEnd) * qHash(longitudeBegin) * qHash(longitudeEnd);

    if (CacheQueryByRange::cacheQueryByRange.contains(key)) {
        return (CacheQueryByRange::cacheQueryByRange[key]);
    }

    DataTimeCount result = DataTimeCount();

    QMap<int, QVariant> searchRange;
    searchRange.insert(0, timeBegin);
    searchRange.insert(1, timeEnd);
    searchRange.insert(2, latitudeBegin);
    searchRange.insert(3, latitudeEnd);
    searchRange.insert(4, longitudeBegin);
    searchRange.insert(5, longitudeEnd);

    QMap<qint64, int> tmp = KDTree::rangeSearchByTime(KDTree::root, 0, searchRange);

    QMap<qint64, int> visit;

    for (qint64 timestamp: tmp.keys()) {
        ++visit[timestamp / step * step];
    }

    for (qint64 timestamp = timeBegin / step * step; timestamp < timeEnd / step * step; timestamp += step) {
        if (visit.contains(timestamp)) {
            result.append(EntryTimeCount(timestamp, visit[timestamp]));
        } else {
            result.append(EntryTimeCount(timestamp, 0));
        }
    }

    CacheQueryByRange::cacheQueryByRange.insert(key, result);

    return result;
}
