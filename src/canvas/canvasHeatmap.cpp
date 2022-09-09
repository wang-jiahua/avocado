// reference: https://github.com/gongjianbo/MyTestCode/tree/master/Qt/MyHeatMap

#include "canvasHeatmap.h"
#include "../configuration.h"
#include "../storage/cacheHeatmap.h"
#include "../storage/dataTimeGeoCoordinateCount.h"
#include "../storage/dataset.h"
#include "../utilities.h"
#include <QDebug>
#include <QPainter>

CanvasHeatmap::CanvasHeatmap(QWidget *parent)
        : QWidget(parent) {
    linear = QLinearGradient(QPoint(0, 0), QPoint(255, 0));
    linear.setColorAt(0.0, Qt::magenta);
    linear.setColorAt(0.2, Qt::blue);
    linear.setColorAt(0.4, Qt::cyan);
    linear.setColorAt(0.6, Qt::green);
    linear.setColorAt(0.8, Qt::yellow);
    linear.setColorAt(1.0, Qt::red);

    colorImage = QImage(256, 1, QImage::Format_ARGB32);
    (void) colorPainter.begin(&colorImage);
    colorPainter.fillRect(colorImage.rect(), linear);

    colorMap = QList<QRgb>();

    alpha = 128;
    for (int i = 0; i < 256; ++i) {
        colorMap.append((colorImage.pixel(i, 0) & 0x00FFFFFFU) | ((quint32) (alpha * i >> 8) << 24));
    }
}

void CanvasHeatmap::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.drawImage(0, 0, heatImage);
    QWidget::paintEvent(event);
}

void CanvasHeatmap::showHeatmap(
        double topLatitude,
        double leftLongitude,
        double bottomLatitude,
        double rightLongitude,
        qint64 timeStamp) {
    // no GPS information
    if (!Configuration::get(QString("GPS")).toBool()) {
        return;
    }

    qint64 key = qHash(topLatitude) * qHash(leftLongitude) * qHash(bottomLatitude) * qHash(rightLongitude) *
                 qHash(timeStamp);

    if (CacheHeatmap::cacheHeatmap.contains(key)) {
        heatImage = CacheHeatmap::cacheHeatmap[key];
        update();
        return;
    }

    int height = this->height();
    int width = this->width();

    dataImage = QImage(width, height, QImage::Format_Alpha8);
    dataImage.fill(Qt::transparent);

    heatImage = QImage(width, height, QImage::Format_ARGB32);
    heatImage.fill(Qt::transparent);

    QPainter dataPainter(&dataImage);
    dataPainter.setPen(Qt::transparent);

    double longitudeRange = rightLongitude - leftLongitude;
    if (longitudeRange < static_cast<double>(0)) {
        longitudeRange += static_cast<double>(360);
    }

    qint64 currentTimestamp = timeStamp / 604800 * 604800;

    EntryTimeGeoCoordinateCount entry = DataTimeGeoCoordinateCount::dataTimeGeoCoordinateCount.value(currentTimestamp);

    for (const QGeoCoordinate &geoCoordinate: entry.keys()) {
        double latitude = geoCoordinate.latitude();
        double longitude = geoCoordinate.longitude();

        if (rightLongitude >= leftLongitude && (longitude > rightLongitude || longitude < leftLongitude)) {
            continue;
        }

        if (rightLongitude < leftLongitude && (longitude > rightLongitude && longitude < leftLongitude)) {
            continue;
        }

        double distanceFromLeft = longitude < leftLongitude
                                  ? (longitude - leftLongitude + 360.0)
                                  : (longitude - leftLongitude);

        int x = static_cast<int>(distanceFromLeft / longitudeRange * static_cast<double>(width));
        int y = static_cast<int>((mercator(topLatitude) - mercator(latitude)) /
                                 (mercator(topLatitude) - mercator(bottomLatitude)) * static_cast<double>(height));

        const auto alpha_tmp = uchar(entry.value(geoCoordinate) / 5.0 * 255);

        QRadialGradient gradient(static_cast<qreal>(x), static_cast<qreal>(y), 10.0);
        gradient.setColorAt(0.0, QColor(0, 0, 0, alpha_tmp));
        gradient.setColorAt(1.0, QColor(0, 0, 0, 0));
        dataPainter.setBrush(gradient);
        dataPainter.drawEllipse(QPointF(static_cast<qreal>(x), static_cast<qreal>(y)), 50.0, 50.0);
    }

    for (int row = 0; row < height; ++row) {
        const uchar *line_data = dataImage.scanLine(row);
        QRgb *line_heat = reinterpret_cast<QRgb *>(heatImage.scanLine(row));
        for (int col = 0; col < width; ++col) {
            line_heat[col] = colorMap[line_data[col]];
        }
    }

    update();

    (void) CacheHeatmap::cacheHeatmap.insert(key, heatImage);
}
