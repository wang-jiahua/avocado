#include "../mainwindow.h"
#include "../storage/dataset.h"
#include "../ui_mainwindow.h"
#include <QFuture>
#include <QGeoRectangle>
#include <QMessageBox>
#include <QQuickItem>
#include <QtConcurrent>

void MainWindow::initTabTrajectory() {
    QUrl sourceTrajectory("qrc:src/map/mapTrajectory.qml");
    ui->trajectoryQuick->setSource(sourceTrajectory);

    connect(ui->trajectoryTimeLine, &QSlider::valueChanged, [this](int val) {
        qint64 max = ui->trajectoryCurrentTime->maximumDateTime().toSecsSinceEpoch();
        qint64 min = ui->trajectoryCurrentTime->minimumDateTime().toSecsSinceEpoch();
        auto timestamp = (qint64) (val / 9828.0 * ((double) max - (double) min) + (double) min);
        ui->trajectoryCurrentTime->setDateTime(QDateTime::fromSecsSinceEpoch(timestamp));
    });

    mapTrajectory = ui->trajectoryQuick->rootObject();

    connect(
            (QObject *) mapTrajectory,
            SIGNAL(qmlSignal(double, double, double, double)),
            this,
            SLOT(on_trajectory_visibleregion_changed(double, double, double, double)));

    connect(timer, &QTimer::timeout, this, [=]() {
        ui->trajectoryTimeLine->setValue(ui->trajectoryTimeLine->value() + 20);
    });
}

void MainWindow::on_trajectory_visibleregion_changed(
        double topLatitude,
        double leftLongitude,
        double bottomLatitude,
        double rightLongitude) {
    if (dataTrajectory.frames.empty()) {
        return;
    }
    canvasTrajectory->showTrajectory(
            dataTrajectory,
            topLatitude,
            leftLongitude,
            bottomLatitude,
            rightLongitude);
}

void MainWindow::on_trajectoryApply_clicked() {
    if (!Dataset::ready) {
        QMessageBox::warning(this, "warning", "Data loading not completed", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QStringList IDs = ui->trajectoryUserID->text().split(" ", QString::SkipEmptyParts);

    int countID = IDs.size();
    if (countID < 1) {
        QMessageBox::warning(this, "warning", "please input user ID", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if (countID > 1) {
        QMessageBox::warning(this, "warning", "too many user IDs", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (canvasTrajectory
        && canvasTrajectory->animation
        && canvasTrajectory->animation->state() == QAbstractAnimation::Running) {
        canvasTrajectory->animation->stop();
    }
    if (canvasTrajectory
        && canvasTrajectory->group
        && canvasTrajectory->group->state() == QAbstractAnimation::Running) {
        canvasTrajectory->group->stop();
    }

    timer->stop();
    ui->trajectoryTimeLine->setValue(0);

    qint64 timeBegin = ui->trajectoryTimeBegin->dateTime().toSecsSinceEpoch();
    qint64 timeEnd = ui->trajectoryTimeEnd->dateTime().toSecsSinceEpoch();
    int userID = ui->trajectoryUserID->text().toInt();

    QFuture<DataTrajectory> futureQueryTrajectory = QtConcurrent::run(
            ThreadQueryTrajectory::queryTrajectory,
            timeBegin,
            timeEnd,
            userID);
    auto *watcherQueryTrajectory = new QFutureWatcher<DataTrajectory>(this);

    connect(watcherQueryTrajectory, &QFutureWatcher<DataTrajectory>::finished, this, [=]() {
        dataTrajectory = futureQueryTrajectory.result();

        const DataTrajectory &data = futureQueryTrajectory.result();

        qint64 minTimestamp = data.minTimestamp;
        qint64 maxTimestamp = data.maxTimestamp;

        ui->trajectoryCurrentTime->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(minTimestamp));
        ui->trajectoryCurrentTime->setMaximumDateTime(QDateTime::fromSecsSinceEpoch(maxTimestamp));
        ui->trajectoryCurrentTime->setDateTime(QDateTime::fromSecsSinceEpoch(minTimestamp));

        double topLatitude = data.topLatitude;
        double bottomLatitude = data.bottomLatitude;
        double leftLongitude = data.leftLongitude;
        double rightLongitude = data.rightLongitude;
        double latitudeRange = topLatitude - bottomLatitude;
        double longitudeRange = rightLongitude - leftLongitude;
        if (longitudeRange < 0) {
            longitudeRange += 360;
        }

        double margin = 0.2;

        mapTrajectory->findChild<QObject *>("map")->setProperty(
                "visibleRegion",
                QVariant::fromValue(
                        QGeoRectangle(
                                QGeoCoordinate(
                                        topLatitude + latitudeRange * margin,
                                        leftLongitude - longitudeRange * margin),
                                QGeoCoordinate(
                                        bottomLatitude - latitudeRange * margin,
                                        rightLongitude + longitudeRange * margin))));
    });

    if (canvasTrajectory != nullptr) {
        canvasTrajectory->lower();
        ui->trajectoryLayout->removeWidget(canvasTrajectory);
        canvasTrajectory->hide();
    }

    canvasTrajectory = new CanvasTrajectory(this);
    ui->trajectoryLayout->addWidget(canvasTrajectory, 0, 0);
    canvasTrajectory->raise();

    watcherQueryTrajectory->setFuture(futureQueryTrajectory);
}

void MainWindow::on_trajectoryPlay_clicked() {
    if (!Dataset::ready) {
        QMessageBox::warning(this, "warning", "Data loading not completed", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (!canvasTrajectory) {
        QMessageBox::warning(this, "warning", "User ID not specified", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (canvasTrajectory->animation->state() == QAbstractAnimation::Running) {
        canvasTrajectory->animation->stop();
    }
    if (canvasTrajectory->group->state() == QAbstractAnimation::Running) {
        canvasTrajectory->group->stop();
    }

    timer->stop();
    ui->trajectoryTimeLine->setValue(0);

    connect(canvasTrajectory->animation, &QAbstractAnimation::finished, timer, &QTimer::stop);

    for (QLabel *label: *(canvasTrajectory->labelList)) {
        label->show();
    }

    canvasTrajectory->animation->start();
    canvasTrajectory->group->start();
    timer->start(10);
}
