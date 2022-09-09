#include "threadQueryByLocation.h"
#include "../../storage/dataset.h"
#include "../../storage/hashLocation.h"
#include <QDebug>

DataTimeCount
ThreadQueryByLocation::queryByLocations(qint64 timeBegin, qint64 timeEnd, const QSet<int> &locationIDs, qint64 step) {
    DataTimeCount result = DataTimeCount();

    QMap<qint64, int> visit;
    for (int locationID: locationIDs) {
        for (int primaryKey: HashLocation::hashLocation.values(locationID)) {
            Fields fields = Dataset::dataset[primaryKey];
            if (fields.timestamp < timeBegin || fields.timestamp > timeEnd) {
                continue;
            }
            qint64 roundedTimestamp = fields.timestamp / step * step;
            ++visit[roundedTimestamp];
        }
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

    return result;
}

DataTimeCount ThreadQueryByLocation::queryByLocation(qint64 timeBegin, qint64 timeEnd, int locationID, qint64 step) {
    DataTimeCount result = DataTimeCount();

    QMap<qint64, int> visit;
    for (int primaryKey: HashLocation::hashLocation.values(locationID)) {
        Fields fields = Dataset::dataset[primaryKey];
        if (fields.timestamp < timeBegin || fields.timestamp > timeEnd) {
            continue;
        }
        qint64 roundedTimestamp = fields.timestamp / step * step;
        ++visit[roundedTimestamp];
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

    return result;
}
