#ifndef _MAP_TIME_H
#define _MAP_TIME_H

#include <QMap>

class MapTime {
public:
    static QMap<qint64, int> mapTime;
    static bool ready;
};

#endif // _MAP_TIME_H