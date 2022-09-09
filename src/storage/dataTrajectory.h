#ifndef _DATA_TRAJECTORY_H
#define _DATA_TRAJECTORY_H

#include <QGeoCoordinate>
#include <QMap>

struct DataTrajectory {
    QMap<qint64, QGeoCoordinate> frames;
    qint64 minTimestamp;
    qint64 maxTimestamp;
    double topLatitude;
    double bottomLatitude;
    double leftLongitude;
    double rightLongitude;

    explicit DataTrajectory(
            const QMap<qint64, QGeoCoordinate> &_frames = QMap<qint64, QGeoCoordinate>(),
            qint64 _minTimestamp = LLONG_MAX,
            qint64 _maxTimestamp = 0,
            double _topLatitude = -91,
            double _bottomLatitude = 91,
            double _leftLongitude = 181,
            double _rightLongitude = -181)
            : frames(_frames), minTimestamp(_minTimestamp), maxTimestamp(_maxTimestamp), topLatitude(_topLatitude),
              bottomLatitude(_bottomLatitude), leftLongitude(_leftLongitude), rightLongitude(_rightLongitude) {
    }
};

#endif // _DATA_TRAJECTORY_H