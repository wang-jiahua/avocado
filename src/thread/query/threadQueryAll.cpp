#include "threadQueryAll.h"
#include "../../configuration.h"
#include "../../storage/cacheQueryAll.h"
#include "../../storage/kdtree.h"
#include "../../storage/mapTime.h"
#include <QDebug>

DataTimeCount ThreadQueryAll::queryAll(
        qint64 timeBegin,
        qint64 timeEnd,
        qint64 step) {
    qint64 key = qHash(timeBegin) * qHash(timeEnd) * qHash(step);

    if (CacheQueryAll::cacheQueryAll.contains(key)) {
        return (CacheQueryAll::cacheQueryAll[key]);
    }

    DataTimeCount result = DataTimeCount();

    QMap<qint64, int> tmp;

    if (Configuration::get(QString("GPS")).toBool()) {
        QMap<int, QVariant> searchRange;

        searchRange.insert(0, timeBegin);
        searchRange.insert(1, timeEnd);
        searchRange.insert(2, -90);
        searchRange.insert(3, 90);
        searchRange.insert(4, -180);
        searchRange.insert(5, 180);

        tmp = KDTree::rangeSearchByTime(KDTree::root, 0, searchRange);
    } else {
        QMap<qint64, int>::const_iterator i = MapTime::mapTime.find(timeBegin);

        if (i.key() != timeBegin) {
            i = MapTime::mapTime.begin();
        }

        while (i != MapTime::mapTime.end() && i.key() < timeEnd) {
            tmp.insert(i.key(), i.value());
            ++i;
        }
    }

    QMap<qint64, int> visit;

    for (int timestamp: tmp.keys()) {
        ++visit[timestamp / step * step];
    }

    qint64 roundedTimeBegin = timeBegin / step * step;
    qint64 roundedTimeEnd = timeEnd / step * step;

    for (qint64 timestamp = roundedTimeBegin; timestamp < roundedTimeEnd; timestamp += step) {
        if (visit.contains(timestamp)) {
            result.append(EntryTimeCount(timestamp, visit[timestamp]));
        } else {
            result.append(EntryTimeCount(timestamp, 0));
        }
    }

    CacheQueryAll::cacheQueryAll.insert(key, result);

    return result;
}
