#ifndef _CANVAS_HEATMAP_H
#define _CANVAS_HEATMAP_H

#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

class CanvasHeatmap : public QWidget {
Q_OBJECT

public slots:

    void showHeatmap(
            double topLatitude,
            double leftLongitude,
            double bottomLatitude,
            double rightLongitude,
            qint64 timeStamp);

public:
    explicit CanvasHeatmap(QWidget *parent);

private:
    void paintEvent(QPaintEvent *event) override;

    QImage dataImage;
    QImage heatImage;
    QImage colorImage;

    QLinearGradient linear;
    QList<QRgb> colorMap;
    quint32 alpha;

    QPainter colorPainter;
};

#endif // _CANVAS_HEATMAP_H
