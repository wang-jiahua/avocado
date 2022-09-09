#ifndef _CANVAS_TRAJECTORY_H
#define _CANVAS_TRAJECTORY_H

#include "../thread/query/threadQueryTrajectory.h"
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

class CanvasTrajectory : public QWidget {
Q_OBJECT

public slots:

public:
    explicit CanvasTrajectory(QWidget *parent);

    void showTrajectory(
            const DataTrajectory &data,
            double topLatitude,
            double leftLongitude,
            double bottomLatitude,
            double rightLongitude);

    QPropertyAnimation *animation = nullptr;
    QParallelAnimationGroup *group = nullptr;
    QList<QLabel *> *labelList = nullptr;

private:
    QList<QPropertyAnimation *> *animationList = nullptr;
    QLabel *currentLocation = nullptr;
    QLabel *line = nullptr;

    double startX{};
    double startY{};

    qint64 startTimestamp{};
};

#endif // _CANVAS_TRAJECTORY_H
