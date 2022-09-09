#include "threadConstructTimeTree.h"
#include "../storage/dataset.h"
#include "../storage/mapTime.h"
#include <QDebug>
#include <QMutex>
#include <QTime>

ThreadConstructTimeTree::ThreadConstructTimeTree(QObject *parent)
        : Thread(parent) {
}

void ThreadConstructTimeTree::run() {
    int oldTime = QTime::currentTime().msecsSinceStartOfDay();

    int cnt = 0;
    int MAX_CNT = Dataset::dataset.size();

    // qDebug() << "MAX_CNT: " << MAX_CNT;

    for (int primaryKey: Dataset::dataset.keys()) {
        const Fields &fields = Dataset::dataset[primaryKey];
        qint64 timestamp = fields.timestamp;
        ++MapTime::mapTime[timestamp];
        ++cnt;
        emit changed((int) ((double) cnt / MAX_CNT * 1000));
    }

    QMutex mutex;
    mutex.lock();

    MapTime::ready = true;

    mutex.unlock();

    emit ready();

    int newTime = QTime::currentTime().msecsSinceStartOfDay();

    qDebug() << "constructing time tree costs" << (newTime - oldTime) / 1000.0 << "s";
}
