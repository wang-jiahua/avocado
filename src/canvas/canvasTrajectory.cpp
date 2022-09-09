#include "canvasTrajectory.h"
#include "../configuration.h"
#include "../storage/dataset.h"
#include "../thread/query/threadQueryTrajectory.h"
#include "../utilities.h"
#include <QDebug>
#include <QList>
#include <QPainter>
#include <QParallelAnimationGroup>

CanvasTrajectory::CanvasTrajectory(QWidget *parent)
        : QWidget(parent), currentLocation(new QLabel(this)), group(new QParallelAnimationGroup(this)),
          labelList(new QList<QLabel *>()), animationList(new QList<QPropertyAnimation *>()) {
}

void CanvasTrajectory::showTrajectory(
        const DataTrajectory &data,
        double topLatitude,
        double leftLongitude,
        double bottomLatitude,
        double rightLongitude) {
    // no GPS information
    if (!Configuration::get(QString("GPS")).toBool()) {
        return;
    }

    currentLocation->setPixmap(QPixmap(":/img/circle.png"));
    currentLocation->setScaledContents(true);
    currentLocation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    currentLocation->resize(0, 0);

    //currentLocation->raise();

    int duration = 10000;

    animation = new QPropertyAnimation(currentLocation, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::InOutExpo);

    qint64 minTimestamp = data.minTimestamp;
    qint64 maxTimestamp = data.maxTimestamp;

    qint64 timestampRange = maxTimestamp - minTimestamp;
    double latitudeRange = topLatitude - bottomLatitude;
    double longitudeRange = rightLongitude - leftLongitude;
    if (longitudeRange < static_cast<double>(0)) {
        longitudeRange += static_cast<double>(360);
    }

    group->clear();
    labelList->clear();
    animationList->clear();

    for (qint64 timestamp: data.frames.keys()) {
        double latitude = data.frames[timestamp].latitude();
        double longitude = data.frames[timestamp].longitude();

        if (rightLongitude >= leftLongitude && (longitude > rightLongitude || longitude < leftLongitude)) {
            continue;
        }

        if (rightLongitude < leftLongitude && (longitude > rightLongitude && longitude < leftLongitude)) {
            continue;
        }

        double distanceFromLeft = longitude < leftLongitude
                                  ? (longitude - leftLongitude + 360)
                                  : (longitude - leftLongitude);

        int x = distanceFromLeft / longitudeRange * width();
        int y = (mercator(topLatitude) - mercator(latitude)) /
                (mercator(topLatitude) - mercator(bottomLatitude)) * height();

        animation->setKeyValueAt(
                (double) (timestamp - minTimestamp) / (double) timestampRange,
                QRect(x - 15, y - 15, 30, 30)
        );

        if (timestamp == minTimestamp) {
            startX = x;
            startY = y;
            startTimestamp = timestamp;
            continue;
        }

        labelList->append(new QLabel(this));

        labelList->last()->setPixmap(QPixmap(
                (x - startX) / (y - startY) > 0 ? ":/img/line_topleft.png"
                                                : ":/img/line_topright.png"));

        labelList->last()->resize(0, 0);
        labelList->last()->setScaledContents(true);
        labelList->last()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

        animationList->append(new QPropertyAnimation(labelList->last(), "geometry"));
        animationList->last()->setDuration(10000.0);

        animationList->last()->setStartValue(QRect(static_cast<int>(startX), static_cast<int>(startY), 0, 0));

        animationList->last()->setKeyValueAt(
                (double) (startTimestamp - minTimestamp) / (double) timestampRange,
                QRect(static_cast<int>(startX), static_cast<int>(startY), 0, 0));

        int left = x >= startX ? static_cast<int>(startX) : static_cast<int>(x);
        int top = y >= startY ? static_cast<int>(startY) : static_cast<int>(y);

        animationList->last()->setKeyValueAt(
                (double) (timestamp - minTimestamp) / (double) timestampRange,
                QRect(left, top, static_cast<int>(abs(x - startX)), static_cast<int>(abs(y - startY))));

        animationList->last()->setEndValue(
                QRect(left, top, static_cast<int>(abs(x - startX)), static_cast<int>(abs(y - startY))));

        animationList->last()->setEasingCurve(QEasingCurve::InOutExpo);

        group->addAnimation(animationList->last());

        startX = x;
        startY = y;
        startTimestamp = timestamp;
    }
}
