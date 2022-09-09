#ifndef _GPS_RANGE_H
#define _GPS_RANGE_H

struct GPSRange {
    double latitudeBegin;
    double latitudeEnd;
    double longitudeBegin;
    double longitudeEnd;

    GPSRange(
            double _latitudeBegin,
            double _latitudeEnd,
            double _longitudeBegin,
            double _longitudeEnd)
            : latitudeBegin(_latitudeBegin), latitudeEnd(_latitudeEnd), longitudeBegin(_longitudeBegin),
              longitudeEnd(_longitudeEnd) {
    }
};

#endif // _GPS_RANGE_H