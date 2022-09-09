#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include <QGeoRectangle>
#include <QQuickItem>

void MainWindow::initTabHeatmap() {
    QSurfaceFormat format;
    format.setVersion(4, 4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    ui->heatmapQuick->setFormat(format);

    QUrl sourceHeatmap("qrc:src/map/mapHeatmap.qml");
    ui->heatmapQuick->setSource(sourceHeatmap);
    ui->heatmapQuick->setClearColor(QColor(255, 255, 255));
    ui->heatmapQuick->setResizeMode(QQuickWidget::SizeRootObjectToView);
    mapHeatmap = ui->heatmapQuick->rootObject();

    canvasHeatmap = new CanvasHeatmap(this);
    ui->heatmapLayout->addWidget(canvasHeatmap, 0, 0);
    canvasHeatmap->raise();
    canvasHeatmap->update();
    canvasHeatmap->setAttribute(Qt::WA_TransparentForMouseEvents);

    connect(
            (QObject *) mapHeatmap,
            SIGNAL(qmlSignal(double, double, double, double)),
            this,
            SLOT(on_heatmap_visibleregion_changed(double, double, double, double)));

    connect(
            ui->heatmapCurrentTime,
            &QDateTimeEdit::dateTimeChanged,
            this,
            &MainWindow::on_heatmap_time_changed);

    connect(this, &MainWindow::updateHeatmap, canvasHeatmap, &CanvasHeatmap::showHeatmap);

    connect(ui->heatmapTimeLine, &QSlider::valueChanged, [this](int val) {
        qint64 max = ui->heatmapCurrentTime->maximumDateTime().toSecsSinceEpoch();
        qint64 min = ui->heatmapCurrentTime->minimumDateTime().toSecsSinceEpoch();
        auto timestamp = (qint64) (val / 99.0 * (double) (max - min) + (double) min);
        ui->heatmapCurrentTime->setDateTime(QDateTime::fromSecsSinceEpoch(timestamp));
    });
}

void MainWindow::on_heatmap_time_changed(const QDateTime &dateTime) {
    auto rect = mapHeatmap->property("rect").value<QGeoRectangle>();
    double topLatitude = rect.topLeft().latitude();
    double leftLongitude = rect.topLeft().longitude();
    double bottomLatitude = rect.bottomRight().latitude();
    double rightLongitude = rect.bottomRight().longitude();
    emit updateHeatmap(
            topLatitude,
            leftLongitude,
            bottomLatitude,
            rightLongitude,
            dateTime.toSecsSinceEpoch());
}

void MainWindow::on_heatmap_visibleregion_changed(
        double topLatitude,
        double leftLongitude,
        double bottomLatitude,
        double rightLongitude) {
    emit updateHeatmap(
            topLatitude,
            leftLongitude,
            bottomLatitude,
            rightLongitude,
            ui->heatmapCurrentTime->dateTime().toSecsSinceEpoch());
}
