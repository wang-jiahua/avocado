#include "threadLoadData.h"
#include "../configuration.h"
#include "../storage/cacheQueryAll.h"
#include "../storage/cacheQueryByRange.h"
#include "../storage/dataset.h"
#include "../storage/hashLocation.h"
#include "../storage/hashUser.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QTime>

ThreadLoadData::ThreadLoadData(QObject *parent)
        : Thread(parent) {
}

void ThreadLoadData::run() {
    int oldTime = QTime::currentTime().msecsSinceStartOfDay();

    // clear previous data and cache
    Dataset::dataset.clear();
    HashUser::hashUser.clear();
    HashLocation::hashLocation.clear();
    CacheQueryAll::cacheQueryAll.clear();
    CacheQueryByRange::cacheQueryByRange.clear();

    // data loading begins
    QFile file(Configuration::get(QString("path")).toString());
    if (!file.open(QIODevice::ReadOnly)) {
        emit message("Can not open file");
        return;
    }

    QString fileLine;
    QStringList list;
    int primaryKey = 0;

    QTextStream in(&file);

    while (!in.atEnd()) {
        fileLine = in.readLine();
        list = fileLine.split(",", QString::SkipEmptyParts);

        int userId = list[0].toInt();
        int locationId = list[1].toInt();
        qint64 timestamp = QDateTime::fromString(list[2], Qt::ISODate).toSecsSinceEpoch();

        if (Configuration::get(QString("GPS")).toBool()) {
            double latitude = list[3].toDouble();
            double longitude = list[4].toDouble();

            Dataset::dataset.insert(primaryKey, Fields(userId, locationId, timestamp, latitude, longitude));
        } else {
            Dataset::dataset.insert(primaryKey, Fields(userId, locationId, timestamp));
        }

        HashUser::hashUser.insert(userId, primaryKey);
        HashLocation::hashLocation.insert(locationId, primaryKey);

        //qDebug() << Dataset::dataset[primaryKey].userID;

        primaryKey++;

        emit changed((int) (primaryKey / 1502536.0 * 1000));
    }

    file.close();

    Dataset::mutex.lock();
    Dataset::ready = true;
    Dataset::mutex.unlock();

    emit ready();

    int newTime = QTime::currentTime().msecsSinceStartOfDay();

    qDebug() << "loading data costs" << (newTime - oldTime) / 1000.0 << "s";
}
