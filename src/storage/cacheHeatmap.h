#ifndef _CACHE_HEATMAP_H
#define _CACHE_HEATMAP_H

#include <QImage>
#include <QMap>

class CacheHeatmap {
public:
    static QMap<qint64, QImage> cacheHeatmap;
};

#endif // _CACHE_HEATMAP_H