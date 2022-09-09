#include "threadCalculateTimeGeoCoordinateCount.h"
#include "../storage/dataTimeGeoCoordinateCount.h"
#include "../storage/dataset.h"
#include <QDebug>

ThreadCalculateTimeGeoCoordinateCount::ThreadCalculateTimeGeoCoordinateCount(QObject *parent)
        : Thread(parent) {
}

void ThreadCalculateTimeGeoCoordinateCount::run() {
    qint64 step = 604800;

    for (int primaryKey: Dataset::dataset.keys()) {
        Fields fields = Dataset::dataset[primaryKey];
        QGeoCoordinate geoCoordinate = QGeoCoordinate(fields.latitude, fields.longitude);
        QMap<qint64, EntryTimeGeoCoordinateCount> &data = DataTimeGeoCoordinateCount::dataTimeGeoCoordinateCount;
        qint64 rounded = fields.timestamp / step * step;

        if (!data.contains(rounded)) {
            data.insert(rounded, EntryTimeGeoCoordinateCount());
        }
        ++data[rounded][geoCoordinate];
    }
}
