#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "canvas/canvasHeatmap.h"
#include "canvas/canvasTrajectory.h"
#include "filter.h"
#include "storage/dataLocationCount.h"
#include "storage/dataTimeCount.h"
#include <QChartView>
#include <QDateTimeAxis>
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QMutex>
#include <QProgressBar>
#include <QQuickItem>
#include <QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

signals:

    void updateHeatmap(
            double topLatitude,
            double leftLongitude,
            double bottomLatitude,
            double rightLongitude,
            qint64 timestamp);

private slots:

    void on_filter_accepted();

    void on_plotLocationIDs_textEdited(const QString &);

    void on_plotRange_valueChanged(double plotLatitudeBegin);

    void on_plotApply_clicked();

    void on_comparisonObject_currentTextChanged(const QString &object);

    void on_comparisonApply_clicked();

    void on_trajectoryApply_clicked();

    void on_trajectoryPlay_clicked();

    void on_trajectory_visibleregion_changed(
            double topLatitude,
            double leftLongitude,
            double bottomLatitude,
            double rightLongitude);

    void on_heatmap_time_changed(const QDateTime &dateTime);

    void on_heatmap_visibleregion_changed(
            double topLatitude,
            double leftLongitude,
            double bottomLatitude,
            double rightLongitude);

private:
    Ui::MainWindow *ui;
    Filter *filter = nullptr;
    QProgressBar *progressBar = nullptr;
    QLabel *progressText = nullptr;

    QChartView *plotChartView = nullptr;
    QChartView *comparisonChartViewLeft = nullptr;
    QChartView *comparisonChartViewRight = nullptr;
    QChartView *comparisonChartView = nullptr;
    QDateTimeAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;
    QMutex mutex;

    CanvasTrajectory *canvasTrajectory = nullptr;
    CanvasHeatmap *canvasHeatmap = nullptr;
    QObject *mapHeatmap = nullptr;
    QQuickItem *mapTrajectory = nullptr;
    QTimer *timer = nullptr;
    DataTrajectory dataTrajectory;

    void initTabPlot();

    void initTabTrajectory();

    void initTabHeatmap();

    void plotLineChart(const DataTimeCount &data);

    void plotBarChart(const DataLocationCount &data);

    void comparisonPieChart(const DataLocationCount &data1, QChartView *chartView, int position, int userID);

    void comparisonLineChart(const DataTimeCount &data, int locationID);
};

#endif // _MAINWINDOW_H
