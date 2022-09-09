#include "../configuration.h"
#include "../mainwindow.h"
#include "../storage/GPSRange.h"
#include "../storage/dataset.h"
#include "../storage/kdtree.h"
#include "../thread/query/threadQueryAll.h"
#include "../thread/query/threadQueryByLocation.h"
#include "../thread/query/threadQueryByRange.h"
#include "../thread/query/threadQueryByUser.h"
#include "../thread/query/threadQueryByUserLocation.h"
#include "../thread/query/threadQueryByUserRange.h"
#include "../ui_mainwindow.h"
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QFuture>
#include <QMessageBox>
#include <QSplineSeries>
#include <QtConcurrent>

void MainWindow::initTabPlot() {
    connect(ui->plotLatitudeBegin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &MainWindow::on_plotRange_valueChanged);
    connect(ui->plotLatitudeEnd, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &MainWindow::on_plotRange_valueChanged);
    connect(ui->plotLongitudeBegin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &MainWindow::on_plotRange_valueChanged);
    connect(ui->plotLongitudeEnd, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &MainWindow::on_plotRange_valueChanged);
}

void MainWindow::on_plotLocationIDs_textEdited(const QString &plotLocationIDs) {
    bool searchByRange = plotLocationIDs.isEmpty();

    ui->plotLatitudeBegin->setEnabled(searchByRange);
    ui->plotLatitudeEnd->setEnabled(searchByRange);
    ui->plotLongitudeBegin->setEnabled(searchByRange);
    ui->plotLongitudeEnd->setEnabled(searchByRange);
}

void MainWindow::on_plotRange_valueChanged(double value) {
    ui->plotLocationIDs->setEnabled(
            ui->plotLatitudeBegin->value() == -90.0
            && ui->plotLatitudeEnd->value() == 90.0
            && ui->plotLongitudeBegin->value() == -180
            && ui->plotLongitudeEnd->value() == 180);
}

void MainWindow::on_plotApply_clicked() {
    Dataset::mutex.lock();
    if (!Dataset::ready) {
        QMessageBox::warning(this, "warning", "Data loading not completed", QMessageBox::Ok, QMessageBox::Ok);
        Dataset::mutex.unlock();
        return;
    }
    Dataset::mutex.unlock();

    bool userIDsInputted = !(ui->plotUserIDs->text().isEmpty());
    bool locationIDsInputted = !(ui->plotLocationIDs->text().isEmpty());
    bool rangeChanged = (ui->plotLatitudeBegin->value() != -90.0)
                        || (ui->plotLatitudeEnd->value() != 90.0)
                        || (ui->plotLongitudeBegin->value() != -180.0)
                        || (ui->plotLongitudeEnd->value() != 180.0);

    int scale = 1;
    QString unit = ui->plotUnit->currentText();
    if (unit == "minute(s)") {
        scale = 60;
    } else if (unit == "hour(s)") {
        scale = 3600;
    } else if (unit == "day(s)") {
        scale = 86400;
    } else if (unit == "week(s)") {
        scale = 604800;
    }

    qint64 timeBegin = ui->plotTimeBegin->dateTime().toSecsSinceEpoch();
    qint64 timeEnd = ui->plotTimeEnd->dateTime().toSecsSinceEpoch();
    double latitudeBegin = ui->plotLatitudeBegin->value();
    double latitudeEnd = ui->plotLatitudeEnd->value();
    double longitudeBegin = ui->plotLongitudeBegin->value();
    double longitudeEnd = ui->plotLongitudeEnd->value();
    GPSRange range(latitudeBegin, latitudeEnd, longitudeBegin, longitudeEnd);
    qint64 step = ui->plotStep->text().toInt() * scale;

    QSet<int> userIDs;
    QStringList list = ui->plotUserIDs->text().split(" ", QString::SkipEmptyParts);
    for (QString &num: list) {
        userIDs.insert(num.toInt());
    }

    QSet<int> locationIDs;
    list = ui->plotLocationIDs->text().split(" ", QString::SkipEmptyParts);
    for (QString &num: list) {
        locationIDs.insert(num.toInt());
    }

    if (userIDsInputted && locationIDsInputted) {
        QFuture<DataLocationCount> futureQueryByUserLocation = QtConcurrent::run(
                ThreadQueryByUserLocation::queryByUserLocation,
                timeBegin,
                timeEnd,
                userIDs,
                locationIDs);
        auto *watcherQueryByUserLocation = new QFutureWatcher<DataLocationCount>(this);

        connect(watcherQueryByUserLocation, &QFutureWatcher<DataLocationCount>::finished, this, [=]() {
            plotBarChart(futureQueryByUserLocation.result());
        });

        watcherQueryByUserLocation->setFuture(futureQueryByUserLocation);
        return;
    }

    if (userIDsInputted && rangeChanged) {
        QFuture<DataLocationCount> futureQueryByUserRange = QtConcurrent::run(
                ThreadQueryByUserRange::queryByUserRange,
                timeBegin,
                timeEnd,
                range,
                userIDs);
        auto *watcherQueryByUserRange = new QFutureWatcher<DataLocationCount>(this);

        connect(watcherQueryByUserRange, &QFutureWatcher<DataLocationCount>::finished, this, [=]() {
            plotBarChart(futureQueryByUserRange.result());
        });

        watcherQueryByUserRange->setFuture(futureQueryByUserRange);
        return;
    }

    // only user IDs inputted
    if (userIDsInputted) {
        QFuture<DataLocationCount> futureQueryByUser = QtConcurrent::run(
                ThreadQueryByUser::queryByUsers,
                timeBegin,
                timeEnd,
                userIDs);
        auto *watcherQueryByUser = new QFutureWatcher<DataLocationCount>(this);

        connect(watcherQueryByUser, &QFutureWatcher<DataLocationCount>::finished, this, [=]() {
            plotBarChart(futureQueryByUser.result());
        });

        watcherQueryByUser->setFuture(futureQueryByUser);
        return;
    }

    // ----- user IDs not inputted -----

    // only location IDs inputted
    if (locationIDsInputted) {
        QFuture<DataTimeCount> futureQueryByLocation = QtConcurrent::run(
                ThreadQueryByLocation::queryByLocations,
                timeBegin,
                timeEnd,
                locationIDs,
                step);
        auto *watcherQueryByLocation = new QFutureWatcher<DataTimeCount>(this);

        connect(watcherQueryByLocation, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            plotLineChart(futureQueryByLocation.result());
        });

        watcherQueryByLocation->setFuture(futureQueryByLocation);
        return;
    }

    // all data when GPS information not provided
    if (!Configuration::get(QString("GPS")).toBool()) {
        QFuture<DataTimeCount> futureQueryAll = QtConcurrent::run(
                ThreadQueryAll::queryAll,
                timeBegin,
                timeEnd,
                step);

        auto *watcherQueryAll = new QFutureWatcher<DataTimeCount>(this);

        connect(watcherQueryAll, &QFutureWatcher<DataTimeCount>::started, this, [=]() {
            progressText->setText(" Querying data... ");
        });

        connect(watcherQueryAll, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            progressText->setText(" ready ");
            plotLineChart(futureQueryAll.result());
        });

        watcherQueryAll->setFuture(futureQueryAll);
        return;
    }

    // the following cases require querying KD tree

    KDTree::mutex.lock();
    if (!KDTree::ready) {
        QMessageBox::warning(this, "warning", "KD tree constructing not completed", QMessageBox::Ok, QMessageBox::Ok);
        KDTree::mutex.unlock();
        return;
    }
    KDTree::mutex.unlock();

    // only GPS range changed
    if (rangeChanged) {
        QFuture<DataTimeCount> futureQueryByRange = QtConcurrent::run(
                ThreadQueryByRange::queryByRange,
                timeBegin,
                timeEnd,
                range,
                step);

        auto *watcherQueryByRange = new QFutureWatcher<DataTimeCount>(this);

        connect(watcherQueryByRange, &QFutureWatcher<DataTimeCount>::started, this, [=]() {
            progressText->setText(" Querying data... ");
        });

        connect(watcherQueryByRange, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            progressText->setText(" ready ");
            plotLineChart(futureQueryByRange.result());
        });

        watcherQueryByRange->setFuture(futureQueryByRange);
        return;
    }

    // all data
    {
        QFuture<DataTimeCount> futureQueryAll = QtConcurrent::run(
                ThreadQueryAll::queryAll,
                timeBegin,
                timeEnd,
                step);

        auto *watcherQueryAll = new QFutureWatcher<DataTimeCount>(this);

        connect(watcherQueryAll, &QFutureWatcher<DataTimeCount>::started, this, [=]() {
            progressText->setText(" Querying data... ");
        });

        connect(watcherQueryAll, &QFutureWatcher<DataTimeCount>::finished, this, [=]() {
            progressText->setText(" ready ");
            plotLineChart(futureQueryAll.result());
        });

        watcherQueryAll->setFuture(futureQueryAll);
    }
}

void MainWindow::plotLineChart(const DataTimeCount &data) {
    auto series = data.size() < 100 ? new QSplineSeries() : new QLineSeries();

    if (data.size() < 30) {
        QMessageBox::warning(this, "Warning", "Data too sparse", QMessageBox::Ok, QMessageBox::Ok);
    }

    for (EntryTimeCount entry: data) {
        series->append(entry.timestamp * 1000, entry.visitCount);
    }

    if (plotChartView) {
        ui->plotLayout->removeWidget(plotChartView);
        plotChartView->hide();
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("checking-in data over time");
    chart->setTitleFont(QFont("Microsoft YaHei", 9));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QDateTimeAxis(chart);
    axisX->setFormat("MM-dd HH:mm");
    axisX->setLabelsFont(QFont("Microsoft YaHei", 9));
    //axisX->setRange(ui->timePlotBegin->dateTime(), ui->timePlotEnd->dateTime());
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis(chart);
    //axisY->setRange(0, series->at(0).y());
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

    // axisY->setRange(0, max);

    chart->legend()->setVisible(false);
    // chart->legend()->setAlignment(Qt::AlignBottom);

    plotChartView = new QChartView(chart);
    plotChartView->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout->addWidget(plotChartView);
}

void MainWindow::plotBarChart(const DataLocationCount &data) {
    if (plotChartView) {
        ui->plotLayout->removeWidget(plotChartView);
        plotChartView->hide();
    }

    auto *set = new QBarSet("");

    QStringList categories;

    int barCount = qMin(data.size(), 10);

    if (barCount == 0) {
        QMessageBox::warning(this, "Warning", "No data available");
    } else if (barCount < 10) {
        QMessageBox::warning(this, "Warning", "Data too sparse");
    }

    for (int i = 0; i < barCount; ++i) {
        EntryLocationCount entry = data[i];
        *set << entry.visitCount;
        categories << QString::number(entry.locationID);
    }

    auto *series = new QBarSeries();
    series->append(set);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Top 10 POIs visited");
    chart->setTitleFont(QFont("Microsoft YaHei", 9));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsFont(QFont("Microsoft YaHei", 9));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, set->at(0));
    axisY->setLabelFormat("%d");
    axisY->setLabelsFont(QFont("Microsoft YaHei", 9));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    // chart->legend()->setAlignment(Qt::AlignBottom);

    plotChartView = new QChartView(chart);
    plotChartView->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout->addWidget(plotChartView);
}
