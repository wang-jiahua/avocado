#include "threadQueryByUser.h"
#include "../../storage/dataset.h"
#include "../../storage/hashUser.h"
#include <QDebug>
#include <QtAlgorithms>

DataLocationCount ThreadQueryByUser::queryByUsers(qint64 timeBegin, qint64 timeEnd, const QSet<int> &userIDs) {
    DataLocationCount result = DataLocationCount();

    QMap<int, int> visit; // locationID => visitCount
    for (int userID: userIDs) {
        for (int primaryKey: HashUser::hashUser.values(userID)) {
            const Fields &fields = Dataset::dataset[primaryKey];
            if (fields.timestamp < timeBegin || fields.timestamp > timeEnd) {
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

DataLocationCount ThreadQueryByUser::queryByUser(qint64 timeBegin, qint64 timeEnd, int userID) {
    DataLocationCount result = DataLocationCount();

    QMap<int, int> visit; // locationID => visitCount
    for (int primaryKey: HashUser::hashUser.values(userID)) {
        const Fields &fields = Dataset::dataset[primaryKey];
        if (fields.timestamp < timeBegin || fields.timestamp > timeEnd) {
            continue;
        }
        ++visit[fields.locationID];
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
