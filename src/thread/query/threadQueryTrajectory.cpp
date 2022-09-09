#include "threadQueryTrajectory.h"
#include "../../storage/dataset.h"
#include "../../storage/hashUser.h"
#include <QDebug>

DataTrajectory ThreadQueryTrajectory::queryTrajectory(
        qint64 timeBegin,
        qint64 timeEnd,
        int userID) {
    DataTrajectory result = DataTrajectory();

    for (int primaryKey: HashUser::hashUser.values(userID)) {
        Fields fields = Dataset::dataset[primaryKey];
        if (fields.timestamp < timeBegin || fields.timestamp > timeEnd) {
            continue;
        }
        result.frames[fields.timestamp] = QGeoCoordinate(fields.latitude, fields.longitude);

        result.minTimestamp = qMin(fields.timestamp, result.minTimestamp);
        result.maxTimestamp = qMax(fields.timestamp, result.maxTimestamp);
        result.topLatitude = qMax(fields.latitude, result.topLatitude);
        result.bottomLatitude = qMin(fields.latitude, result.bottomLatitude);
        result.leftLongitude = qMin(fields.longitude, result.leftLongitude);
        result.rightLongitude = qMax(fields.longitude, result.rightLongitude);
    }

    return result;
}
