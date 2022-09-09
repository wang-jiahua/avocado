#include "threadQueryByUserRange.h"
#include "../../storage/dataset.h"
#include "../../storage/hashUser.h"
#include <QDebug>
#include <QtAlgorithms>

DataLocationCount ThreadQueryByUserRange::queryByUserRange(
        qint64 timeBegin,
        qint64 timeEnd,
        const GPSRange &range,
        const QSet<int> &userIDs) {
    double latitudeBegin = range.latitudeBegin;
    double latitudeEnd = range.latitudeEnd;
    double longitudeBegin = range.longitudeBegin;
    double longitudeEnd = range.longitudeEnd;

    DataLocationCount result = DataLocationCount();

    QMap<int, int> visit; // locationID => visitCount
    for (int userID: userIDs) {
        for (int primaryKey: HashUser::hashUser.values(userID)) {
            const Fields &fields = Dataset::dataset[primaryKey];
            if (fields.timestamp < timeBegin || fields.timestamp > timeEnd
                || fields.latitude < latitudeBegin || fields.latitude > latitudeEnd
                || fields.longitude < longitudeBegin || fields.longitude > longitudeEnd) {
                continue;
            }
            ++visit[fields.locationID];
        }
    }

    // sort the map by value in descending order

    for (int locationID: visit.keys()) {
        result.append(EntryLocationCount(locationID, visit[locationID]));
    }

    std::sort(result.begin(), result.end(),
              [](const EntryLocationCount &a, const EntryLocationCount &b) {
                  return a.visitCount > b.visitCount;
              });

    return result;
}
