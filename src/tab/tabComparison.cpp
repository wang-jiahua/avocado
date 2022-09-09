#include "../mainwindow.h"
#include "../storage/dataset.h"
#include "../thread/query/threadQueryByLocation.h"
#include "../thread/query/threadQueryByUser.h"
#include "../ui_mainwindow.h"
#include <QFuture>
#include <QMessageBox>
#include <QPieSeries>
#include <QSplineSeries>
#include <QtConcurrent>

void MainWindow::on_comparisonObject_currentTextChanged(const QString &object) {
    ui->comparisonStep->setEnabled(object == "POI");
    ui->comparisonUnit->setEnabled(object == "POI");
}

void MainWindow::on_comparisonApply_clicked() {
    if (!Dataset::ready) {
        QMessageBox::warning(this, "warning", "Data loading not completed", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QStringList ID1s = ui->comparisonID1->text().split(" ", QString::SkipEmptyParts);
    QStringList ID2s = ui->comparisonID2->text().split(" ", QString::SkipEmptyParts);

    int countID1 = ID1s.size();
    int countID2 = ID2s.size();

    if (countID1 < 1) {
        QMessageBox::warning(this, "warning", "please input ID1", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if (countID1 > 1) {
        QMessageBox::warning(this, "warning", "too many IDs in ID1", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if (countID2 < 1) {
        QMessageBox::warning(this, "warning", "please input ID2", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if (countID2 > 1) {
        QMessageBox::warning(this, "warning", "too many IDs in ID2", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    if (ID1s[0] == ID2s[0]) {
        QMessageBox::warning(this, "warning", "same IDs", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    qint64 timeBegin = ui->comparisonTimeBegin->dateTime().toSecsSinceEpoch();
    qint64 timeEnd = ui->comparisonTimeEnd->dateTime().toSecsSinceEpoch();

    int ID1 = ui->comparisonID1->text().toInt();
    int ID2 = ui->comparisonID2->text().toInt();

    if (ui->comparisonObject->currentText() == "user") {
        QFuture<DataLocationCount> futureQueryByUser1 = QtConcurrent::run(ThreadQueryByUser::queryByUser, timeBegin,
                                                                          timeEnd, ID1);
        QFuture<DataLocationCount> futureQueryByUser2 = QtConcurrent::run(ThreadQueryByUser::queryByUser, timeBegin,
                                                                          timeEnd, ID2);

        auto *watcherQueryByUser1 = new QFutureWatcher<DataLocationCount>(this);
        auto *watcherQueryByUser2 = new QFutureWatcher<DataLocationCount>(this);

        connect(watcherQueryByUser1, &QFutureWatcher<DataLocationCount>::finished, this, [=]() {
            comparisonPieChart(futureQueryByUser1.result(), comparisonChartViewLeft, 0, ID1);
        });
        connect(watcherQueryByUser2, &QFutureWatcher<DataLocationCount>::finished, this, [=]() {
            comparisonPieChart(futureQueryByUser2.result(), comparisonChartViewRight, 1, ID2);
        });

        watcherQueryByUser1->setFuture(futureQueryByUser1);
        watcherQueryByUser2->setFuture(futureQueryByUser2);
    } else {
        int scale = 1;
        QString unit = ui->comparisonUnit->currentText();
        if (unit == "minute(s)") {
            scale = 60;
        } else if (unit == "hour(s)") {
            scale = 3600;
        } else if (unit == "day(s)") {
            scale = 86400;
        } else if (unit == "week(s)") {
            scale = 604800;
        }

        qint64 step = ui->comparisonStep->text().toInt() * scale;

        QFuture<DataTimeCount> futureQueryByLocation1 = QtConcurrent::run(ThreadQueryByLocation::queryByLocation,
                                                                          timeBegin, timeEnd, ID1, step);
        QFuture<DataTimeCount> futureQueryByLocation2 = QtConcurrent::run(ThreadQueryByLocation::queryByLocation,
                                                                          timeBegin, timeEnd, ID2, step);

        auto *watcherQueryByLocation1 = new QFutureWatcher<DataTimeCount>(this);
        auto *watcherQueryByLocation2 = new QFutureWatcher<DataTimeCount>(this);

        connect(watcherQueryByLocation1, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            comparisonLineChart(futureQueryByLocation1.result(), ID1);
        });
        connect(watcherQueryByLocation2, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            comparisonLineChart(futureQueryByLocation2.result(), ID2);
        });

        watcherQueryByLocation1->setFuture(futureQueryByLocation1);
        watcherQueryByLocation2->setFuture(futureQueryByLocation2);
    }
}

void MainWindow::comparisonPieChart(const DataLocationCount &data, QChartView *chartView, int position, int userID) {
    if (comparisonChartView) {
        ui->comparisonLayout->removeWidget(comparisonChartView);
        comparisonChartView->hide();
    }

    if (chartView) {
        ui->comparisonLayout->removeWidget(chartView);
        chartView->hide();
    }

    auto series = new QPieSeries();
    series->setPieSize(0.65);

    int pieCount = qMin(data.size(), 10);

    if (pieCount == 0) {
        QMessageBox::warning(this, "Warning", "No data available", QMessageBox::Ok, QMessageBox::Ok);
    } else if (pieCount < 10) {
        QMessageBox::warning(this, "Warning", "Data too sparse", QMessageBox::Ok, QMessageBox::Ok);
    }

    for (int i = 0; i < pieCount; ++i) {
        EntryLocationCount entry = data[i];
        series->append(QString::number(entry.locationID), entry.visitCount);
    }

    for (QPieSlice *slice: series->slices()) {
        slice->setLabel(slice->label() + " (" + QString::number(slice->value()) + ")");
        // slice->setLabel("POI " + slice->label() + ": " + QString::number(slice->value()) + " time(s)");
        slice->setLabelFont(QFont("Microsoft YaHei", 9));
    }

    series->setLabelsVisible(true);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setTitle("Top 10 POIs visited by user " + QString::number(userID));
    chart->setTitleFont(QFont("Microsoft YaHei", 9));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    ui->comparisonLayout->addWidget(chartView, 1, position);
}

void MainWindow::comparisonLineChart(const DataTimeCount &data, int locationID) {
    QMutexLocker locker(&mutex);

    if (comparisonChartViewLeft) {
        ui->comparisonLayout->removeWidget(comparisonChartViewLeft);
        comparisonChartViewLeft->hide();
    }
    if (comparisonChartViewRight) {
        ui->comparisonLayout->removeWidget(comparisonChartViewRight);
        comparisonChartViewRight->hide();
    }

    auto series = data.size() < 100 ? new QSplineSeries() : new QLineSeries();
    series->setName("POI " + QString::number(locationID));

    // if (data.size() < 30) {
    //     QMessageBox::warning(this, "Warning", "Data too sparse", QMessageBox::Ok, QMessageBox::Ok);
    // }

    for (EntryTimeCount entry: data) {
        series->append((double) entry.timestamp * 1000, entry.visitCount);
    }

    // already have one series
    if (comparisonChartView && comparisonChartView->chart() && comparisonChartView->chart()->series().size() == 1) {
        comparisonChartView->chart()->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);

        QLineSeries *previousSeries = (QLineSeries *) (comparisonChartView->chart()->series()).at(0);

        qreal max = 0;

        for (QPointF point: previousSeries->points()) {
            max = qMax(max, point.y());
        }

        for (QPointF point: series->points()) {
            max = qMax(max, point.y());
        }

        // if (max == 0) {
        //     QMessageBox::warning(this, "Warning", "Data too sparse", QMessageBox::Ok, QMessageBox::Ok);
        // }

        axisY->setRange(0, max);

        return;
    }

    // have two series
    if (comparisonChartView) {
        ui->comparisonLayout->removeWidget(comparisonChartView);
        comparisonChartView->hide(); //
    }

    auto *chart = new QChart();

    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle("checking-in data over time");
    chart->setTitleFont(QFont("Microsoft YaHei", 9));
    chart->addSeries(series);

    axisX = new QDateTimeAxis(chart);
    axisX->setFormat("MM-dd HH:mm");
    axisX->setLabelsFont(QFont("Microsoft YaHei", 9));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis(chart);
    axisY->setLabelFormat("%d");
    axisY->setLabelsFont(QFont("Microsoft YaHei", 9));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    qreal max = 0;

    for (QPointF point: series->points()) {
        max = qMax(max, point.y());
    }

    if (max == 0) {
        QMessageBox::warning(this, "Warning", "Data too sparse", QMessageBox::Ok, QMessageBox::Ok);
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Microsoft YaHei", 9));

    comparisonChartView = new QChartView(chart);
    comparisonChartView->setRenderHint(QPainter::Antialiasing);

    ui->comparisonLayout->addWidget(comparisonChartView);
}
