#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <QtMath>

inline double mercator(double latitude) {
    double radius = latitude * M_PI / 180;
    return qLn(qTan(radius / static_cast<double>(2) + M_PI / 4));
}

#endif // _UTILITIES_H